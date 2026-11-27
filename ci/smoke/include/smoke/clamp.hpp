/**
 * @file    clamp.hpp
 * @brief   값을 범위로 제한하는 함수의 공개 인터페이스 (툴체인 검증용 참조 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef SMOKE_CLAMP_HPP
#define SMOKE_CLAMP_HPP

namespace smoke
{

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
enum class Status
{
    Ok = 0,      ///< 정상 실행
    ErrNullArg,  ///< 널 포인터 인자
    ErrRange     ///< 인자 범위 오류
};

/**
 * @brief   값을 [lo, hi] 범위로 제한한다.
 * @param   value  제한할 값
 * @param   lo     범위 하한
 * @param   hi     범위 상한 (lo 이상이어야 함)
 * @param   out    결과를 돌려받는 출력 인자
 * @return  Status::Ok, out이 널이면 Status::ErrNullArg, lo가 hi보다 크면 Status::ErrRange
 * @satisfies REQ-F-001
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
[[nodiscard]] Status clamp(int value, int lo, int hi, int* out);

}  // namespace smoke

#endif  // SMOKE_CLAMP_HPP
