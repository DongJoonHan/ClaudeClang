/**
 * @file    controller.h
 * @brief   평가주기를 조정하는 제어기 모듈의 공개 인터페이스 (ARCH-I-006, DD-I-006).
 * @satisfies ARCH-C-006, DD-M-006
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_CONTROLLER_H
#define ECL_CONTROLLER_H

#include "ecl/ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   제어기 상태: 직전 평가주기의 확정 결정을 보관한다.
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
    ecl_decision_t confirmed;  /**< 직전 평가주기의 확정 결정 */
} ecl_controller_t;

/**
 * @brief   제어기를 초기화한다. 좌/우 출력은 RELEASE 이다.
 * @param   ctl  초기화할 제어기
 * @return  ECL_OK, ctl 이 널이면 ECL_ERR_NULL_ARG
 * @satisfies REQ-F-015
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
ECL_NODISCARD ecl_status_t ecl_controller_init(ecl_controller_t* ctl);

/**
 * @brief   평가주기를 한 번 실행한다: 검증, 명령 접수, 상태 판정, 규칙 평가 순서.
 * @param   ctl          초기화된 제어기
 * @param   raw          원시 입력 묶음
 * @param   cmds         원시 명령 배열 (count 가 0 이면 널 가능)
 * @param   count        명령 수
 * @param   eval_time_s  평가 시각 [s]
 * @param   out          공개 결과를 돌려받는 출력 인자
 * @return  ECL_OK 또는 하위 모듈이 돌려준 오류 상태. 오류 시 확정 결정과 out 은 바뀌지 않는다.
 * @satisfies REQ-F-013, REQ-F-016, REQ-Q-001
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
ECL_NODISCARD ecl_status_t ecl_controller_step(ecl_controller_t* ctl, const ecl_raw_snapshot_t* raw,
                                               const ecl_raw_command_t* cmds, size_t count,
                                               double eval_time_s, ecl_cycle_result_t* out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_CONTROLLER_H */
