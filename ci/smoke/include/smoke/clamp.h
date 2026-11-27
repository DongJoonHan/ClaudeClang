/**
 * @file    clamp.h
 * @brief   값을 범위로 제한하는 함수의 공개 인터페이스 (툴체인 검증용 참조 코드, C17).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef SMOKE_CLAMP_H
#define SMOKE_CLAMP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   반환값을 호출자가 무시하면 컴파일 경고(-Werror로 오류)가 되게 하는 속성.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define SMOKE_NODISCARD __attribute__((warn_unused_result))

/**
 * @brief   함수 실행 결과 상태.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    SMOKE_OK = 0,        /**< 정상 실행 */
    SMOKE_ERR_NULL_ARG,  /**< 널 포인터 인자 */
    SMOKE_ERR_RANGE      /**< 인자 범위 오류 */
} smoke_status_t;

/**
 * @brief   값을 [lo, hi] 범위로 제한한다.
 * @param   value  제한할 값
 * @param   lo     범위 하한
 * @param   hi     범위 상한 (lo 이상이어야 함)
 * @param   out    결과를 돌려받는 출력 인자
 * @return  SMOKE_OK, out이 널이면 SMOKE_ERR_NULL_ARG, lo가 hi보다 크면 SMOKE_ERR_RANGE
 * @satisfies REQ-F-001
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
SMOKE_NODISCARD smoke_status_t smoke_clamp(int value, int lo, int hi, int* out);

#ifdef __cplusplus
}
#endif

#endif /* SMOKE_CLAMP_H */
