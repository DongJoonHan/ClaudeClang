/**
 * @file    clamp_tdd_test.cpp
 * @brief   구현 중 TDD로 작성한 clamp 시험 (단위 테스트와 분리, 별도 결과·커버리지).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>

#include "smoke/clamp.hpp"

namespace
{

/**
 * @test      TDD-clamp-001
 * @brief     [목적] 범위 안의 값은 변하지 않는다 (TDD 첫 사이클).
 * @satisfies REQ-F-001
 * @technique TDD red-green-refactor
 * @input     value=5, lo=0, hi=10
 * @procedure 1) clamp()를 호출한다.
 * @expected  반환값 == Status::Ok, out == 5
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ClampTddTest, KeepsValueInsideRange)
{
    int out = 0;
    EXPECT_EQ(smoke::clamp(5, 0, 10, &out), smoke::Status::Ok);
    EXPECT_EQ(out, 5);
}

}  // namespace
