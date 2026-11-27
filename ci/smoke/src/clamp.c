/**
 * @file    clamp.c
 * @brief   smoke_clamp 구현 (툴체인 검증용 참조 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "smoke/clamp.h"

#include <stddef.h>

smoke_status_t smoke_clamp(int value, int lo, int hi, int* out)
{
    smoke_status_t result = SMOKE_OK;

    if (out == NULL)
    {
        result = SMOKE_ERR_NULL_ARG;
    }
    else if (lo > hi)
    {
        result = SMOKE_ERR_RANGE;
    }
    else if (value < lo)
    {
        *out = lo;
    }
    else if (value > hi)
    {
        *out = hi;
    }
    else
    {
        *out = value;
    }

    return result;
}
