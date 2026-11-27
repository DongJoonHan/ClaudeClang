/**
 * @file    controller.cpp
 * @brief   Controller 구현 (툴체인 검증용 참조 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "smoke/controller.hpp"

namespace smoke
{

Controller::Controller(int low, int high) : low_(low), high_(high)
{
}

Status Controller::step(ISensor* sensor, int* out_command) const
{
    Status result = Status::Ok;
    int raw = 0;

    if ((sensor == nullptr) || (out_command == nullptr))
    {
        result = Status::ErrNullArg;
    }
    else
    {
        result = sensor->read(&raw);
        if (result == Status::Ok)
        {
            result = clamp(raw, low_, high_, out_command);
        }
    }

    return result;
}

}  // namespace smoke
