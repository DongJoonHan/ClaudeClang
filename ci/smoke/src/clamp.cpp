/**
 * @file    clamp.cpp
 * @brief   clamp 함수 구현 (툴체인 검증용 참조 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "smoke/clamp.hpp"

namespace smoke
{

Status clamp(int value, int lo, int hi, int* out)
{
    Status result = Status::Ok;

    if (out == nullptr)
    {
        result = Status::ErrNullArg;
    }
    else if (lo > hi)
    {
        result = Status::ErrRange;
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

}  // namespace smoke
