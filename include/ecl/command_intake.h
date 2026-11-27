/**
 * @file    command_intake.h
 * @brief   운전자 명령 접수 모듈의 공개 인터페이스 (ARCH-I-002, DD-I-002).
 * @satisfies ARCH-C-002, DD-M-002
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_COMMAND_INTAKE_H
#define ECL_COMMAND_INTAKE_H

#include "ecl/ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   원시 운전자 명령에서 첫 유효 명령 1건을 채택하고 거절 이유를 만든다.
 * @param   cmds             원시 명령 배열 (count 가 0 이면 널 가능)
 * @param   count            이번 평가주기에 들어온 명령 수
 * @param   request_time_s   명령의 요청 시각 [s]
 * @param   out              채택 결과를 돌려받는 출력 인자
 * @return  ECL_OK, out 이 널이거나 count 가 0 초과인데 cmds 가 널이면 ECL_ERR_NULL_ARG
 * @satisfies REQ-F-006, REQ-F-007, REQ-F-008, REQ-F-009, REQ-F-018, REQ-F-019
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
ECL_NODISCARD ecl_status_t ecl_command_intake(const ecl_raw_command_t* cmds, size_t count,
                                              double request_time_s, ecl_command_result_t* out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_COMMAND_INTAKE_H */
