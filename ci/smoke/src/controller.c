/**
 * @file    controller.c
 * @brief   제어기 구현 (툴체인 검증용 참조 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "smoke/controller.h"

#include <stddef.h>

smoke_status_t smoke_controller_init(smoke_controller_t* controller, int low, int high)
{
    smoke_status_t result = SMOKE_OK;

    if (controller == NULL)
    {
        result = SMOKE_ERR_NULL_ARG;
    }
    else
    {
        controller->low = low;
        controller->high = high;
    }

    return result;
}

smoke_status_t smoke_controller_step(const smoke_controller_t* controller,
                                     const smoke_sensor_t* sensor, int* out_command)
{
    smoke_status_t result = SMOKE_OK;
    int raw = 0;

    if ((controller == NULL) || (sensor == NULL) || (sensor->read == NULL) ||
        (out_command == NULL))
    {
        result = SMOKE_ERR_NULL_ARG;
    }
    else
    {
        result = sensor->read(sensor->context, &raw);
        if (result == SMOKE_OK)
        {
            result = smoke_clamp(raw, controller->low, controller->high, out_command);
        }
    }

    return result;
}
