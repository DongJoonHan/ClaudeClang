/**
 * @file    input_validation.h
 * @brief   원시 입력 묶음 검증 모듈의 공개 인터페이스 (ARCH-I-001, DD-I-001).
 * @satisfies ARCH-C-001, DD-M-001
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_INPUT_VALIDATION_H
#define ECL_INPUT_VALIDATION_H

#include "ecl/ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   원시 입력 묶음의 형식·범위·신선도를 검사해 필드별 유효성이 표시된 검증 입력을 만든다.
 * @param   raw           원시 입력 묶음
 * @param   eval_time_s   평가 시각 [s]
 * @param   out           검증 입력을 돌려받는 출력 인자
 * @return  ECL_OK, raw 또는 out 이 널이면 ECL_ERR_NULL_ARG (이때 out 은 바뀌지 않는다)
 * @satisfies REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004, REQ-F-005, REQ-S-003, REQ-S-008
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
ECL_NODISCARD ecl_status_t ecl_input_validate(const ecl_raw_snapshot_t* raw, double eval_time_s,
                                              ecl_validated_t* out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_INPUT_VALIDATION_H */
