/**
 * @file    command_intake.c
 * @brief   운전자 명령 접수 모듈 구현 (DD-M-002).
 * @satisfies ARCH-C-002, REQ-F-006, REQ-F-007, REQ-F-008, REQ-F-009, REQ-F-018, REQ-F-019
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "ecl/command_intake.h"

#include <stddef.h>

static const int k_side_code_max = 2;
static const int k_action_code_max = 1;
static const int k_source_code_max = 3;

static int code_in_range(const ecl_raw_int_t* code, int lo, int hi)
{
    return (code->present != 0) && (code->value >= lo) && (code->value <= hi);
}

static int command_is_valid(const ecl_raw_command_t* cmd)
{
    return code_in_range(&cmd->side, 0, k_side_code_max) &&
           code_in_range(&cmd->action, 0, k_action_code_max) &&
           code_in_range(&cmd->source, 0, k_source_code_max);
}

static ecl_command_result_t take_first(const ecl_raw_command_t* cmd, double request_time_s)
{
    ecl_command_result_t result = {0};

    if (command_is_valid(cmd))
    {
        result.accepted = 1;
        result.side = (ecl_side_t)cmd->side.value;
        result.action = (ecl_action_t)cmd->action.value;
        result.source = (ecl_source_t)cmd->source.value;
        result.request_time_s = request_time_s;
        result.note = ECL_REASON_NONE;
    }
    else
    {
        result.note = ECL_REASON_CMD_REJECTED_INVALID;
    }

    return result;
}

static ecl_command_result_t mark_extra(ecl_command_result_t result, size_t count)
{
    ecl_command_result_t marked = result;

    if (count > 1U)
    {
        marked.extra_rejected = count - 1U;
        if (result.accepted != 0)
        {
            marked.note = ECL_REASON_CMD_REJECTED_EXTRA;
        }
    }

    return marked;
}

ecl_status_t ecl_command_intake(const ecl_raw_command_t* cmds, size_t count, double request_time_s,
                                ecl_command_result_t* out)
{
    ecl_status_t result = ECL_OK;
    ecl_command_result_t taken = {0};

    if ((out == NULL) || ((cmds == NULL) && (count > 0U)))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        if (count > 0U)
        {
            taken = mark_extra(take_first(cmds, request_time_s), count);
        }
        *out = taken;
    }

    return result;
}
