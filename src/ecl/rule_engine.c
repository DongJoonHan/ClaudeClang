/**
 * @file    rule_engine.c
 * @brief   규칙 평가 모듈 구현 (DD-M-004). 이터레이션 1: 규칙 1(SENSOR_FAULT)과 HOLD.
 * @satisfies ARCH-C-004, REQ-F-010, REQ-F-011, REQ-F-012, REQ-F-020, REQ-S-004
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

/**
 * @brief   규칙 하나가 낸 효과 (모듈 내부 형식).
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
    ecl_output_t left;        /**< 좌 도어 출력 */
    ecl_output_t right;       /**< 우 도어 출력 */
    ecl_priority_t priority;  /**< 효과를 낸 규칙 */
    ecl_reason_t reason;      /**< 이유코드 */
} rule_effect_t;

/**
 * @brief   규칙 함수 형식: 적용되면 effect 를 채우고 1, 아니면 0 을 반환한다 (모듈 내부 형식).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef int (*rule_fn)(const ecl_validated_t* in, const ecl_decision_t* confirmed,
                       rule_effect_t* effect);

/* 규칙 1: sensor_fault 가 유효한 TRUE 이면 확정 출력을 유지한다 (REQ-S-004). */
static int rule_sensor_fault(const ecl_validated_t* in, const ecl_decision_t* confirmed,
                             rule_effect_t* effect)
{
    int applies = 0;

    if ((in->sensor_fault.valid != 0) && (in->sensor_fault.value == 1))
    {
        effect->left = confirmed->left;
        effect->right = confirmed->right;
        effect->priority = ECL_PRIO_SENSOR_FAULT;
        effect->reason = ECL_REASON_SENSOR_FAULT;
        applies = 1;
    }

    return applies;
}

/* 순위 순 규칙표. 이터레이션 2~3 에서 규칙 2~8 을 이 표에 추가한다. */
static const rule_fn k_rules[] = {rule_sensor_fault};
static const size_t k_rule_count = sizeof(k_rules) / sizeof(k_rules[0]);

static ecl_reason_t hold_reason(const ecl_validated_t* in)
{
    ecl_reason_t reason = ECL_REASON_NONE;

    if (in->all_valid == 0)
    {
        reason = (in->stale != 0) ? ECL_REASON_INPUT_STALE : ECL_REASON_INPUT_INVALID;
    }

    return reason;
}

static int first_applicable_rule(const ecl_validated_t* in, const ecl_decision_t* confirmed,
                                 rule_effect_t* effect)
{
    int found = 0;
    size_t index = 0U;

    while (index < k_rule_count)
    {
        found = k_rules[index](in, confirmed, effect);
        index = (found != 0) ? k_rule_count : (index + 1U);
    }

    return found;
}

static ecl_decision_t hold_decision(const ecl_validated_t* in, const ecl_decision_t* confirmed)
{
    ecl_decision_t decision = {confirmed->left, confirmed->right, ECL_PRIO_HOLD,
                               hold_reason(in), ECL_REASON_NONE};

    return decision;
}

ecl_status_t ecl_rule_decide(const ecl_validated_t* in, const ecl_command_result_t* cmd,
                             const ecl_decision_t* confirmed, ecl_decision_t* out)
{
    ecl_status_t result = ECL_OK;
    rule_effect_t effect = {ECL_OUT_RELEASE, ECL_OUT_RELEASE, ECL_PRIO_HOLD, ECL_REASON_NONE};

    if ((in == NULL) || (cmd == NULL) || (confirmed == NULL) || (out == NULL))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        if (first_applicable_rule(in, confirmed, &effect) != 0)
        {
            out->left = effect.left;
            out->right = effect.right;
            out->priority_reason = effect.priority;
            out->reason_code = effect.reason;
        }
        else
        {
            *out = hold_decision(in, confirmed);
        }
        out->command_note = cmd->note;
    }

    return result;
}
