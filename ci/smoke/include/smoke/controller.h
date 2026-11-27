/**
 * @file    controller.h
 * @brief   센서 값을 읽어 범위로 제한하는 제어기의 공개 인터페이스 (툴체인 검증용 참조 코드, C17).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef SMOKE_CONTROLLER_H
#define SMOKE_CONTROLLER_H

#include "smoke/clamp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   센서 읽기 함수 포인터 형식. 제어기는 이 형식으로만 센서와 통신한다.
 * @param   context    센서 구현이 쓰는 불투명 문맥
 * @param   out_value  읽은 값을 돌려받는 출력 인자
 * @return  SMOKE_OK 또는 오류 상태
 * @satisfies ARCH-I-001
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef smoke_status_t (*smoke_sensor_read_fn)(void* context, int* out_value);

/**
 * @brief   센서 인터페이스(함수 포인터 + 문맥). 구현은 제어기 밖에서 주입한다.
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
    smoke_sensor_read_fn read;  /**< 센서 읽기 함수 */
    void* context;              /**< read에 전달할 문맥 */
} smoke_sensor_t;

/**
 * @brief   센서 값을 [low, high] 범위로 제한한 명령을 만드는 제어기 상태.
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
    int low;   /**< 명령 하한 */
    int high;  /**< 명령 상한 */
} smoke_controller_t;

/**
 * @brief   제어기를 초기화한다.
 * @param   controller  초기화할 제어기
 * @param   low         명령 하한
 * @param   high        명령 상한
 * @return  SMOKE_OK, controller가 널이면 SMOKE_ERR_NULL_ARG
 * @satisfies REQ-F-002
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
SMOKE_NODISCARD smoke_status_t smoke_controller_init(smoke_controller_t* controller, int low,
                                                     int high);

/**
 * @brief   센서를 한 번 읽어 범위로 제한한 명령을 만든다.
 * @param   controller   초기화된 제어기
 * @param   sensor       센서 인터페이스
 * @param   out_command  명령을 돌려받는 출력 인자
 * @return  SMOKE_OK, 인자가 널이면 SMOKE_ERR_NULL_ARG, 센서 오류는 그대로 전달
 * @satisfies REQ-F-002, ARCH-I-002
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
SMOKE_NODISCARD smoke_status_t smoke_controller_step(const smoke_controller_t* controller,
                                                     const smoke_sensor_t* sensor,
                                                     int* out_command);

#ifdef __cplusplus
}
#endif

#endif /* SMOKE_CONTROLLER_H */
