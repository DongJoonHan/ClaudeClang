/**
 * @file    rule_engine.h
 * @brief   규칙 평가 모듈의 공개 인터페이스 (ARCH-I-004, DD-I-004). 이터레이션 3 은 규칙 1~8 과 HOLD, override 를 구현한다.
 * @satisfies ARCH-C-004, DD-M-004
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_RULE_ENGINE_H
#define ECL_RULE_ENGINE_H

#include "ecl/ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   규칙표 순위에 따라 도어별 출력과 이유코드를 결정한다.
 * @param   in         검증 입력
 * @param   cmd        운전자 명령 채택 결과
 * @param   confirmed  직전 평가주기의 확정 결정
 * @param   memory     규칙 엔진 기억(입력: 이전 상태, 출력: 이번 주기 반영 상태)
 * @param   out        새 결정을 돌려받는 출력 인자
 * @return  ECL_OK, 인자가 널이면 ECL_ERR_NULL_ARG (이때 memory 와 out 은 바뀌지 않는다)
 * @satisfies REQ-F-010, REQ-F-011, REQ-F-012, REQ-F-017, REQ-F-020, REQ-F-021, REQ-F-022, REQ-F-023, REQ-F-025, REQ-F-026, REQ-F-027, REQ-F-028, REQ-F-029, REQ-F-034, REQ-F-035, REQ-F-036, REQ-F-037, REQ-S-001, REQ-S-002, REQ-S-004, REQ-S-005, REQ-S-006, REQ-S-007
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
ECL_NODISCARD ecl_status_t ecl_rule_decide(const ecl_validated_t* in, const ecl_command_result_t* cmd,
                                           const ecl_decision_t* confirmed, ecl_rule_memory_t* memory,
                                           ecl_decision_t* out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_RULE_ENGINE_H */
