/**
 * @file    rule_engine.c
 * @brief   규칙 평가 모듈 구현 (DD-M-004). 이터레이션 3: 규칙 1~8, HOLD, override.
 * @satisfies ARCH-C-004, REQ-F-010, REQ-F-011, REQ-F-012, REQ-F-017, REQ-F-020, REQ-F-021, REQ-F-022, REQ-F-023, REQ-F-025, REQ-F-026, REQ-F-027, REQ-F-028, REQ-F-029, REQ-F-034, REQ-F-035, REQ-F-036, REQ-F-037, REQ-S-001, REQ-S-002, REQ-S-004, REQ-S-005, REQ-S-006, REQ-S-007
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "ecl/rule_engine.h"

#include <stddef.h>

static const int k_door_left = 0;
static const int k_door_right = 1;
static const int k_door_any = 2;
static const double k_moving_speed_kph = 3.0;
static const double k_override_window_s = 10.0;

/**
 * @brief   규칙 하나가 낸 효과 (모듈 내부 형식). applies_* 가 0 이면 그 도어에는 효과가 없다.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int applies_left;         /**< 1 이면 좌 도어에 효과가 있음 */
    int applies_right;        /**< 1 이면 우 도어에 효과가 있음 */
    ecl_output_t left;        /**< 좌 도어 출력 (applies_left 가 1 일 때) */
    ecl_output_t right;       /**< 우 도어 출력 (applies_right 가 1 일 때) */
    ecl_priority_t priority;  /**< 효과를 낸 규칙 */
    ecl_reason_t reason;      /**< 이유코드 */
} rule_effect_t;

/**
 * @brief   규칙 평가에 필요한 입력 묶음 (모듈 내부 형식).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    const ecl_validated_t* in;        /**< 검증 입력 */
    const ecl_command_result_t* cmd;  /**< 운전자 명령 채택 결과 */
    const ecl_decision_t* confirmed;  /**< 직전 확정 결정 */
    const ecl_rule_memory_t* memory;  /**< 이번 주기가 반영된 규칙 엔진 기억 */
} rule_context_t;

/**
 * @brief   규칙 함수 형식: 문맥으로 효과를 계산해 돌려준다 (모듈 내부 형식).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef rule_effect_t (*rule_fn)(const rule_context_t* ctx);

/* 필드가 유효하고 값이 기대값과 같은가 */
static int field_equals(const ecl_val_int_t* field, int expected)
{
    return (field->valid != 0) && (field->value == expected);
}

static int is_moving(const ecl_validated_t* in)
{
    return (in->speed_kph.valid != 0) && (in->speed_kph.value >= k_moving_speed_kph);
}

static int is_stationary(const ecl_validated_t* in)
{
    return (in->speed_kph.valid != 0) && (in->speed_kph.value < k_moving_speed_kph);
}

static rule_effect_t both_doors(ecl_output_t output, ecl_priority_t priority, ecl_reason_t reason)
{
    rule_effect_t effect = {0};

    effect.applies_left = 1;
    effect.applies_right = 1;
    effect.left = output;
    effect.right = output;
    effect.priority = priority;
    effect.reason = reason;

    return effect;
}

/* 규칙 1: sensor_fault 가 유효한 TRUE 이면 좌/우 확정 출력을 유지한다 (REQ-S-004). */
static rule_effect_t rule_sensor_fault(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    if (field_equals(&ctx->in->sensor_fault, 1))
    {
        effect = both_doors(ECL_OUT_RELEASE, ECL_PRIO_SENSOR_FAULT, ECL_REASON_SENSOR_FAULT);
        effect.left = ctx->confirmed->left;
        effect.right = ctx->confirmed->right;
    }

    return effect;
}

/* 규칙 2: 유효한 충돌 확정이면 좌/우를 RELEASE 로 한다 (REQ-S-001, REQ-S-005). */
static rule_effect_t rule_crash(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    if (field_equals(&ctx->in->crash_status, ECL_CRASH_CONFIRMED))
    {
        effect = both_doors(ECL_OUT_RELEASE, ECL_PRIO_CRASH, ECL_REASON_CRASH_CONFIRMED);
    }

    return effect;
}

/* 도어 하나가 규칙 3 의 대상인가: 접근위험이 유효한 TRUE 이고 override 상태가 아니다. */
static int approach_applies(const ecl_val_int_t* risk, const ecl_door_memory_t* door_memory)
{
    return field_equals(risk, 1) && (door_memory->override_active == 0);
}

/* 규칙 3: 접근위험 도어를 LOCK 으로 하고 해제 요청을 억제한다 (REQ-S-002, S-006, S-007). */
static rule_effect_t rule_approach(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    effect.applies_left = approach_applies(&ctx->in->rear_left_risk, &ctx->memory->left) ? 1 : 0;
    effect.applies_right = approach_applies(&ctx->in->rear_right_risk, &ctx->memory->right) ? 1 : 0;
    effect.left = ECL_OUT_LOCK;
    effect.right = ECL_OUT_LOCK;
    effect.priority = ECL_PRIO_APPROACH;
    effect.reason = ECL_REASON_APPROACH_RISK_HOLD;

    return effect;
}

/* 규칙 4: 화재·과온·성인 탑승 중 하나가 유효한 TRUE 이면 좌/우를 RELEASE 로 한다 (REQ-F-025). */
static rule_effect_t rule_emergency_release(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};
    ecl_reason_t reason = ECL_REASON_NONE;

    if (field_equals(&ctx->in->fire, 1))
    {
        reason = ECL_REASON_FIRE;
    }
    else if (field_equals(&ctx->in->overtemp, 1))
    {
        reason = ECL_REASON_OVERTEMP;
    }
    else if (field_equals(&ctx->in->adult_present, 1))
    {
        reason = ECL_REASON_ADULT_PRESENT;
    }
    else
    {
        reason = ECL_REASON_NONE;
    }
    if (reason != ECL_REASON_NONE)
    {
        effect = both_doors(ECL_OUT_RELEASE, ECL_PRIO_EMERGENCY_RELEASE, reason);
    }

    return effect;
}

/* 규칙 5: 유효한 ignition_on FALSE 이면 좌/우를 RELEASE 로 한다 (REQ-F-027). */
static rule_effect_t rule_ignition_off(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    if (field_equals(&ctx->in->ignition_on, 0))
    {
        effect = both_doors(ECL_OUT_RELEASE, ECL_PRIO_IGNITION_OFF, ECL_REASON_IGNITION_OFF);
    }

    return effect;
}

/* 규칙 6: 유효한 ISOFIX TRUE 인 쪽 도어를 LOCK 으로 한다 (REQ-F-026). 반대쪽에는 효과가 없다. */
static rule_effect_t rule_isofix(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    effect.applies_left = field_equals(&ctx->in->isofix_left, 1) ? 1 : 0;
    effect.applies_right = field_equals(&ctx->in->isofix_right, 1) ? 1 : 0;
    effect.left = ECL_OUT_LOCK;
    effect.right = ECL_OUT_LOCK;
    effect.priority = ECL_PRIO_ISOFIX;
    effect.reason = (effect.applies_left != 0) ? ECL_REASON_ISOFIX_LEFT : ECL_REASON_ISOFIX_RIGHT;

    return effect;
}

/* 규칙 7: 이동 중(유효한 속도 3.0 km/h 이상)이면 좌/우를 LOCK 으로 한다 (REQ-F-022). */
static rule_effect_t rule_auto_lock(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    if (is_moving(ctx->in) != 0)
    {
        effect = both_doors(ECL_OUT_LOCK, ECL_PRIO_AUTO_LOCK, ECL_REASON_AUTO_LOCK_MOVING);
    }

    return effect;
}

/* 명령이 해당 쪽 도어를 가리키는가 (side 가 ALL 이거나 같은 쪽) */
static int command_targets(const ecl_command_result_t* cmd, ecl_side_t side)
{
    return (cmd->side == ECL_SIDE_ALL) || (cmd->side == side);
}

/* 규칙 8 의 이유: 해제 명령이 override 상태의 도어를 가리키면 OVERRIDE, 아니면 DRIVER_CMD */
static ecl_reason_t driver_reason(const rule_context_t* ctx, int left, int right)
{
    ecl_reason_t reason = ECL_REASON_DRIVER_CMD;
    const int left_override = (left != 0) && (ctx->memory->left.override_active != 0);
    const int right_override = (right != 0) && (ctx->memory->right.override_active != 0);

    if ((ctx->cmd->action == ECL_ACTION_UNLOCK) && ((left_override != 0) || (right_override != 0)))
    {
        reason = ECL_REASON_OVERRIDE;
    }

    return reason;
}

/* 규칙 8: 모든 필수 입력이 유효하고 정지이며 유효한 명령이 있으면 대상 도어에 동작을 적용한다 (REQ-F-021). */
static rule_effect_t rule_driver_command(const rule_context_t* ctx)
{
    rule_effect_t effect = {0};

    if ((ctx->cmd->accepted != 0) && (ctx->in->all_valid != 0) && (is_stationary(ctx->in) != 0))
    {
        const ecl_output_t output =
            (ctx->cmd->action == ECL_ACTION_LOCK) ? ECL_OUT_LOCK : ECL_OUT_RELEASE;

        effect.applies_left = command_targets(ctx->cmd, ECL_SIDE_LEFT) ? 1 : 0;
        effect.applies_right = command_targets(ctx->cmd, ECL_SIDE_RIGHT) ? 1 : 0;
        effect.left = output;
        effect.right = output;
        effect.priority = ECL_PRIO_DRIVER_CMD;
        effect.reason = driver_reason(ctx, effect.applies_left, effect.applies_right);
    }

    return effect;
}

/* 순위 순 규칙표 (SRS 1.3절). */
static const rule_fn k_rules[] = {rule_sensor_fault, rule_crash,         rule_approach,
                                  rule_emergency_release, rule_ignition_off, rule_isofix,
                                  rule_auto_lock,    rule_driver_command};
static const size_t k_rule_count = sizeof(k_rules) / sizeof(k_rules[0]);

/* 규칙이 해당 도어(또는 any 이면 어느 한 도어)에 효과를 내는가 */
static int applies_to(const rule_effect_t* effect, int door)
{
    int result = 0;

    if (door == k_door_left)
    {
        result = effect->applies_left != 0;
    }
    else if (door == k_door_right)
    {
        result = effect->applies_right != 0;
    }
    else
    {
        result = (effect->applies_left != 0) || (effect->applies_right != 0);
    }

    return result;
}

/* 순위 순으로 훑어 해당 도어에 처음 효과를 내는 규칙을 찾는다. 찾으면 1 */
static int find_rule(const rule_context_t* ctx, int door, rule_effect_t* effect)
{
    int found = 0;
    size_t index = 0U;

    while (index < k_rule_count)
    {
        const rule_effect_t candidate = k_rules[index](ctx);
        if (applies_to(&candidate, door) != 0)
        {
            *effect = candidate;
            found = 1;
            index = k_rule_count;
        }
        else
        {
            index++;
        }
    }

    return found;
}

static ecl_reason_t hold_reason(const ecl_validated_t* in)
{
    ecl_reason_t reason = ECL_REASON_NONE;

    if (in->all_valid == 0)
    {
        reason = (in->stale != 0) ? ECL_REASON_INPUT_STALE : ECL_REASON_INPUT_INVALID;
    }
    else if (in->crash_status.value == ECL_CRASH_PENDING)
    {
        /* all_valid 이면 crash_status 는 유효하므로 값만 확인한다. */
        reason = ECL_REASON_CRASH_PENDING;
    }
    else
    {
        reason = ECL_REASON_NONE;
    }

    return reason;
}

static ecl_decision_t decide(const rule_context_t* ctx)
{
    ecl_decision_t decision = {ctx->confirmed->left, ctx->confirmed->right, ECL_PRIO_HOLD,
                               hold_reason(ctx->in), ECL_REASON_NONE};
    rule_effect_t left = {0};
    rule_effect_t right = {0};
    rule_effect_t any = {0};

    if (find_rule(ctx, k_door_left, &left) != 0)
    {
        decision.left = left.left;
    }
    if (find_rule(ctx, k_door_right, &right) != 0)
    {
        decision.right = right.right;
    }
    if (find_rule(ctx, k_door_any, &any) != 0)
    {
        decision.priority_reason = any.priority;
        decision.reason_code = any.reason;
    }

    return decision;
}

/* 해제 요청이 이번 주기에 있는가: 채택된 해제 명령, 모든 입력 유효, 정지 (REQ-F-021 의 적용 조건과 같다) */
static int unlock_request_active(const ecl_validated_t* in, const ecl_command_result_t* cmd)
{
    return (cmd->accepted != 0) && (cmd->action == ECL_ACTION_UNLOCK) && (in->all_valid != 0) &&
           (is_stationary(in) != 0);
}

/* 억제 중인 도어에 해제 요청이 왔을 때: 첫 요청이면 시각을 기억하고, 이후 10초 이하이면 override,
 * 10초를 넘기면 새 첫 요청으로 본다. 같은 시각이거나 과거이면 변화 없음 (REQ-F-023). */
static ecl_door_memory_t on_unlock_request(ecl_door_memory_t memory, double request_time_s)
{
    ecl_door_memory_t next = memory;

    if (memory.has_first == 0)
    {
        next.has_first = 1;
        next.first_time_s = request_time_s;
    }
    else
    {
        const double elapsed_s = request_time_s - memory.first_time_s;
        if (elapsed_s > k_override_window_s)
        {
            next.first_time_s = request_time_s;
        }
        else if (elapsed_s > 0.0)
        {
            next.override_active = 1;
        }
        else
        {
            next = memory;
        }
    }

    return next;
}

/* 도어 하나의 기억을 갱신한다: 접근위험이 유효한 FALSE 이면 지우고, TRUE 이면서 요청이 있으면 처리한다. */
static ecl_door_memory_t update_door(ecl_door_memory_t memory, const ecl_val_int_t* risk,
                                     int requested, double request_time_s)
{
    ecl_door_memory_t next = memory;

    if (field_equals(risk, 0))
    {
        next.has_first = 0;
        next.first_time_s = 0.0;
        next.override_active = 0;
    }
    else if (field_equals(risk, 1) && (requested != 0))
    {
        next = on_unlock_request(memory, request_time_s);
    }
    else
    {
        next = memory;
    }

    return next;
}

static ecl_rule_memory_t next_memory(const ecl_validated_t* in, const ecl_command_result_t* cmd,
                                     ecl_rule_memory_t memory)
{
    ecl_rule_memory_t next = memory;
    const int active = unlock_request_active(in, cmd);

    next.left = update_door(memory.left, &in->rear_left_risk,
                            (active != 0) && command_targets(cmd, ECL_SIDE_LEFT),
                            cmd->request_time_s);
    next.right = update_door(memory.right, &in->rear_right_risk,
                             (active != 0) && command_targets(cmd, ECL_SIDE_RIGHT),
                             cmd->request_time_s);

    return next;
}

/* 명령 이유: 이동 중에 들어온 채택된 명령은 CMD_REJECTED_MOVING, 그 외는 접수 결과를 쓴다 (REQ-F-035). */
static ecl_reason_t command_note(const ecl_validated_t* in, const ecl_command_result_t* cmd)
{
    ecl_reason_t note = cmd->note;

    if ((cmd->accepted != 0) && (is_moving(in) != 0))
    {
        note = ECL_REASON_CMD_REJECTED_MOVING;
    }

    return note;
}

ecl_status_t ecl_rule_decide(const ecl_validated_t* in, const ecl_command_result_t* cmd,
                             const ecl_decision_t* confirmed, ecl_rule_memory_t* memory,
                             ecl_decision_t* out)
{
    ecl_status_t result = ECL_OK;

    if ((in == NULL) || (cmd == NULL) || (confirmed == NULL) || (memory == NULL) || (out == NULL))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        const ecl_rule_memory_t next = next_memory(in, cmd, *memory);
        const rule_context_t ctx = {in, cmd, confirmed, &next};

        *out = decide(&ctx);
        out->command_note = command_note(in, cmd);
        *memory = next;
    }

    return result;
}
