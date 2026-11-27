/**
 * @file    clamp_test.cpp
 * @brief   clamp 단위 테스트 (MC/DC 100% 대상).
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
 * @test      UT-clamp-001
 * @brief     [목적] out이 널이면 ErrNullArg를 반환한다.
 * @satisfies REQ-F-001
 * @technique 동치 클래스(무효 입력)
 * @input     value=1, lo=0, hi=10, out=nullptr
 * @procedure 1) out=nullptr로 clamp()를 호출한다.
 * @expected  반환값 == Status::ErrNullArg
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ClampTest, ReturnsErrNullArgWhenOutIsNull)
{
    EXPECT_EQ(smoke::clamp(1, 0, 10, nullptr), smoke::Status::ErrNullArg);
}

/**
 * @test      UT-clamp-002
 * @brief     [목적] 하한이 상한보다 크면 ErrRange를 반환하고 출력을 바꾸지 않는다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석(lo == hi + 1)
 * @input     value=5, lo=6, hi=5, out=센티널(-99)
 * @procedure 1) out을 -99로 초기화한다. 2) clamp()를 호출한다.
 * @expected  반환값 == Status::ErrRange, out == -99
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ClampTest, ReturnsErrRangeWhenLowIsGreaterThanHigh)
{
    int out = -99;
    EXPECT_EQ(smoke::clamp(5, 6, 5, &out), smoke::Status::ErrRange);
    EXPECT_EQ(out, -99);
}

/**
 * @test      UT-clamp-003
 * @brief     [목적] 하한 미만의 값은 하한으로 제한된다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석(lo - 1)
 * @input     value=-1, lo=0, hi=10
 * @procedure 1) clamp()를 호출한다.
 * @expected  반환값 == Status::Ok, out == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ClampTest, ClampsToLowWhenBelowRange)
{
    int out = 0;
    EXPECT_EQ(smoke::clamp(-1, 0, 10, &out), smoke::Status::Ok);
    EXPECT_EQ(out, 0);
}

/**
 * @test      UT-clamp-004
 * @brief     [목적] 상한 초과의 값은 상한으로 제한된다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석(hi + 1)
 * @input     value=11, lo=0, hi=10
 * @procedure 1) clamp()를 호출한다.
 * @expected  반환값 == Status::Ok, out == 10
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ClampTest, ClampsToHighWhenAboveRange)
{
    int out = 0;
    EXPECT_EQ(smoke::clamp(11, 0, 10, &out), smoke::Status::Ok);
    EXPECT_EQ(out, 10);
}

/**
 * @test      UT-clamp-005
 * @brief     [목적] 범위 경계값(lo, hi)과 범위 안의 값은 그대로 반환된다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석(lo, hi) + 동치 클래스(유효 범위 안)
 * @input     value ∈ {0, 5, 10}, lo=0, hi=10
 * @procedure 1) 각 값으로 clamp()를 호출한다.
 * @expected  반환값 == Status::Ok, out == value
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ClampTest, KeepsValueWhenInsideRangeIncludingBounds)
{
    for (const int value : {0, 5, 10})
    {
        int out = -1;
        EXPECT_EQ(smoke::clamp(value, 0, 10, &out), smoke::Status::Ok);
        EXPECT_EQ(out, value);
    }
}

}  // namespace
