/**
 * @file    status_eval.c
 * @brief   상태 판정 모듈 구현 (DD-M-003).
 * @satisfies ARCH-C-003, REQ-F-014, REQ-S-003, REQ-S-009
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "ecl/status_eval.h"

#include <stddef.h>

static int is_true(const ecl_val_int_t* field)
{
    return (field->valid != 0) && (field->value == 1);
}

static unsigned invalid_warning(const ecl_validated_t* in)
{
    unsigned warning = 0U;

    if (in->all_valid == 0)
    {
        warning = (in->stale != 0) ? ECL_WARN_INPUT_STALE : ECL_WARN_INPUT_INVALID;
    }

    return warning;
}

static ecl_state_t pick_state(const ecl_validated_t* in)
{
    ecl_state_t state = ECL_STATE_ACTIVE;

    if (is_true(&in->sensor_fault))
    {
        state = ECL_STATE_FAULT;
    }
    else if (in->all_valid == 0)
    {
        state = ECL_STATE_DEGRADED;
    }
    else if (in->ignition_on.value == 0)
    {
        /* all_valid 이면 ignition_on 은 유효하므로 값만 확인한다. */
        state = ECL_STATE_OFF;
    }
    else
    {
        state = ECL_STATE_ACTIVE;
    }

    return state;
}

ecl_status_t ecl_status_evaluate(const ecl_validated_t* in, ecl_status_result_t* out)
{
    ecl_status_t result = ECL_OK;

    if ((in == NULL) || (out == NULL))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        out->state = pick_state(in);
        out->warnings = invalid_warning(in);
        if (is_true(&in->sensor_fault))
        {
            out->warnings |= ECL_WARN_SENSOR_FAULT;
        }
    }

    return result;
}
