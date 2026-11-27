/**
 * @file    status_eval.h
 * @brief   상태 판정 모듈의 공개 인터페이스 (ARCH-I-003, DD-I-003).
 * @satisfies ARCH-C-003, DD-M-003
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_STATUS_EVAL_H
#define ECL_STATUS_EVAL_H

#include "ecl/ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   검증 입력으로부터 상태(FAULT, DEGRADED, OFF, ACTIVE)와 경고코드를 정한다.
 * @param   in   검증 입력
 * @param   out  상태 판정 결과를 돌려받는 출력 인자
 * @return  ECL_OK, 인자가 널이면 ECL_ERR_NULL_ARG
 * @satisfies REQ-F-014, REQ-S-003, REQ-S-009
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
ECL_NODISCARD ecl_status_t ecl_status_evaluate(const ecl_validated_t* in, ecl_status_result_t* out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_STATUS_EVAL_H */
