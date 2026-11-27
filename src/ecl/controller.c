/**
 * @file    controller.c
 * @brief   제어기 모듈 구현 (DD-M-006).
 * @satisfies ARCH-C-006, REQ-F-013, REQ-F-015, REQ-F-016, REQ-Q-001
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "ecl/controller.h"

#include <stddef.h>

#include "ecl/command_intake.h"
#include "ecl/input_validation.h"
#include "ecl/rule_engine.h"
#include "ecl/status_eval.h"

ecl_status_t ecl_controller_init(ecl_controller_t* ctl)
{
    ecl_status_t result = ECL_OK;

    if (ctl == NULL)
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        ctl->confirmed.left = ECL_OUT_RELEASE;
        ctl->confirmed.right = ECL_OUT_RELEASE;
        ctl->confirmed.priority_reason = ECL_PRIO_HOLD;
        ctl->confirmed.reason_code = ECL_REASON_NONE;
        ctl->confirmed.command_note = ECL_REASON_NONE;
    }

    return result;
}

/* 운전자 명령의 요청 시각은 입력 묶음의 timestamp 이다 (REQ-F-008). 무효이면 0.0 이다. */
static double request_time_of(const ecl_validated_t* validated)
{
    double time_s = 0.0;

    if (validated->timestamp_s.valid != 0)
    {
        time_s = validated->timestamp_s.value;
    }

    return time_s;
}

/* 하위 단계는 부수효과가 없는 계산이므로 모두 실행한 뒤 오류를 합산한다.
 * 이 모듈의 하위 호출이 돌려줄 수 있는 오류는 ECL_ERR_NULL_ARG 뿐이다 (명령 배열 널 + 개수 > 0). */
static ecl_status_t run_stages(const ecl_controller_t* ctl, const ecl_raw_snapshot_t* raw,
                               const ecl_raw_command_t* cmds, size_t count, double eval_time_s,
                               ecl_cycle_result_t* staged)
{
    ecl_validated_t validated = {0};
    ecl_command_result_t command = {0};
    ecl_status_result_t status = {ECL_STATE_ACTIVE, 0U};
    unsigned errors = 0U;

    errors |= (unsigned)ecl_input_validate(raw, eval_time_s, &validated);
    errors |= (unsigned)ecl_command_intake(cmds, count, request_time_of(&validated), &command);
    errors |= (unsigned)ecl_status_evaluate(&validated, &status);
    errors |= (unsigned)ecl_rule_decide(&validated, &command, &ctl->confirmed, &staged->decision);
    staged->state = status.state;
    staged->warnings = status.warnings;

    return (errors == 0U) ? ECL_OK : ECL_ERR_NULL_ARG;
}

ecl_status_t ecl_controller_step(ecl_controller_t* ctl, const ecl_raw_snapshot_t* raw,
                                 const ecl_raw_command_t* cmds, size_t count, double eval_time_s,
                                 ecl_cycle_result_t* out)
{
    ecl_status_t result = ECL_OK;
    ecl_cycle_result_t staged = {{ECL_OUT_RELEASE, ECL_OUT_RELEASE, ECL_PRIO_HOLD, ECL_REASON_NONE,
                                  ECL_REASON_NONE},
                                 ECL_STATE_ACTIVE, 0U};

    if ((ctl == NULL) || (raw == NULL) || (out == NULL))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        result = run_stages(ctl, raw, cmds, count, eval_time_s, &staged);
        if (result == ECL_OK)
        {
            ctl->confirmed = staged.decision;
            *out = staged;
        }
    }

    return result;
}
