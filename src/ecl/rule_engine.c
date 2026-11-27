/**
 * @file    rule_engine.c
 * @brief   규칙 평가 모듈 구현 (DD-M-004). 이터레이션 2: 규칙 1(SENSOR_FAULT), 2(CRASH), 3(APPROACH)과 HOLD.
 * @satisfies ARCH-C-004, REQ-F-010, REQ-F-011, REQ-F-012, REQ-F-017, REQ-F-020, REQ-F-028, REQ-F-029, REQ-F-037, REQ-S-001, REQ-S-002, REQ-S-004, REQ-S-005, REQ-S-006, REQ-S-007
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
 * @brief   규칙 함수 형식: 입력과 확정 결정으로 효과를 계산해 돌려준다 (모듈 내부 형식).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef rule_effect_t (*rule_fn)(const ecl_validated_t* in, const ecl_decision_t* confirmed);

/* 필드가 유효하고 값이 기대값과 같은가 */
static int field_equals(const ecl_val_int_t* field, int expected)
{
    return (field->valid != 0) && (field->value == expected);
}

/* 규칙 1: sensor_fault 가 유효한 TRUE 이면 좌/우 확정 출력을 유지한다 (REQ-S-004). */
static rule_effect_t rule_sensor_fault(const ecl_validated_t* in, const ecl_decision_t* confirmed)
{
    rule_effect_t effect = {0};

    if (field_equals(&in->sensor_fault, 1))
    {
        effect.applies_left = 1;
        effect.applies_right = 1;
        effect.left = confirmed->left;
        effect.right = confirmed->right;
        effect.priority = ECL_PRIO_SENSOR_FAULT;
        effect.reason = ECL_REASON_SENSOR_FAULT;
    }

    return effect;
}

/* 규칙 2: 유효한 충돌 확정이면 좌/우를 RELEASE 로 한다 (REQ-S-001, REQ-S-005). */
static rule_effect_t rule_crash(const ecl_validated_t* in, const ecl_decision_t* confirmed)
{
    rule_effect_t effect = {0};

    (void)confirmed;
    if (field_equals(&in->crash_status, ECL_CRASH_CONFIRMED))
    {
        effect.applies_left = 1;
        effect.applies_right = 1;
        effect.left = ECL_OUT_RELEASE;
        effect.right = ECL_OUT_RELEASE;
        effect.priority = ECL_PRIO_CRASH;
        effect.reason = ECL_REASON_CRASH_CONFIRMED;
    }

    return effect;
}

/* 규칙 3: 접근위험이 유효한 TRUE 인 도어를 LOCK 으로 하고 해제 요청을 억제한다 (REQ-S-002, S-006, S-007). */
static rule_effect_t rule_approach(const ecl_validated_t* in, const ecl_decision_t* confirmed)
{
    rule_effect_t effect = {0};

    (void)confirmed;
    effect.applies_left = field_equals(&in->rear_left_risk, 1) ? 1 : 0;
    effect.applies_right = field_equals(&in->rear_right_risk, 1) ? 1 : 0;
    effect.left = ECL_OUT_LOCK;
    effect.right = ECL_OUT_LOCK;
    effect.priority = ECL_PRIO_APPROACH;
    effect.reason = ECL_REASON_APPROACH_RISK_HOLD;

    return effect;
}

/* 순위 순 규칙표. 이터레이션 3 에서 규칙 4~8 을 이 표에 추가한다. */
static const rule_fn k_rules[] = {rule_sensor_fault, rule_crash, rule_approach};
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
static int find_rule(const ecl_validated_t* in, const ecl_decision_t* confirmed, int door,
                     rule_effect_t* effect)
{
    int found = 0;
    size_t index = 0U;

    while (index < k_rule_count)
    {
        const rule_effect_t candidate = k_rules[index](in, confirmed);
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

static ecl_decision_t decide(const ecl_validated_t* in, const ecl_decision_t* confirmed)
{
    ecl_decision_t decision = {confirmed->left, confirmed->right, ECL_PRIO_HOLD, hold_reason(in),
                               ECL_REASON_NONE};
    rule_effect_t left = {0};
    rule_effect_t right = {0};
    rule_effect_t any = {0};

    if (find_rule(in, confirmed, k_door_left, &left) != 0)
    {
        decision.left = left.left;
    }
    if (find_rule(in, confirmed, k_door_right, &right) != 0)
    {
        decision.right = right.right;
    }
    if (find_rule(in, confirmed, k_door_any, &any) != 0)
    {
        decision.priority_reason = any.priority;
        decision.reason_code = any.reason;
    }

    return decision;
}

ecl_status_t ecl_rule_decide(const ecl_validated_t* in, const ecl_command_result_t* cmd,
                             const ecl_decision_t* confirmed, ecl_decision_t* out)
{
    ecl_status_t result = ECL_OK;

    if ((in == NULL) || (cmd == NULL) || (confirmed == NULL) || (out == NULL))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        *out = decide(in, confirmed);
        out->command_note = cmd->note;
    }

    return result;
}
