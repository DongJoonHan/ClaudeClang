/**
 * @file    ecl_rules_tdd_test.cpp
 * @brief   이터레이션 2 TDD 시험: 규칙 2(충돌 확정), 규칙 3(접근위험), PENDING (단위 테스트와 분리).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>
#include "ecl/controller.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      TDD-ecl-101
 * @brief     [목적] 충돌 확정이면 좌/우가 RELEASE 이다 (이터레이션 2 첫 사이클).
 * @satisfies REQ-S-001, REQ-F-012
 * @technique TDD red-green-refactor
 * @input     crash=CONFIRMED, 이전 좌/우 LOCK
 * @procedure 1) step
 * @expected  좌/우 RELEASE, priority CRASH, reason CRASH_CONFIRMED
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclRulesTddTest, CrashConfirmedReleasesBothDoors)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.crash_status = Int(2);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_CRASH);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_CRASH_CONFIRMED);
}

/**
 * @test      TDD-ecl-102
 * @brief     [목적] 좌 접근위험이면 좌만 LOCK 이고 우는 그대로이다.
 * @satisfies REQ-S-002, REQ-F-017
 * @technique TDD red-green-refactor
 * @input     rear_left_risk=1, 이전 좌/우 RELEASE
 * @procedure 1) step
 * @expected  좌 LOCK, 우 RELEASE, reason APPROACH_RISK_HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclRulesTddTest, LeftApproachRiskLocksOnlyLeft)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);

    ecl_raw_snapshot_t raw = ValidRaw();
    raw.rear_left_risk = Int(1);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_APPROACH);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      TDD-ecl-103
 * @brief     [목적] 접근위험 도어에 대한 해제 요청이 억제된다.
 * @satisfies REQ-S-006, REQ-S-007
 * @technique TDD red-green-refactor
 * @input     rear_right_risk=1, 우 해제 명령, 이전 우 LOCK
 * @procedure 1) step
 * @expected  우 LOCK 유지, reason APPROACH_RISK_HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclRulesTddTest, ApproachRiskSuppressesUnlockRequest)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.rear_right_risk = Int(1);
    const ecl_raw_command_t cmd = Cmd(ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      TDD-ecl-104
 * @brief     [목적] 충돌 PENDING 은 출력을 바꾸지 않고 CRASH_PENDING 만 기록한다.
 * @satisfies REQ-F-029
 * @technique TDD red-green-refactor
 * @input     crash=PENDING, 이전 좌 LOCK
 * @procedure 1) step
 * @expected  좌 LOCK 유지, priority HOLD, reason CRASH_PENDING
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclRulesTddTest, CrashPendingOnlyRecordsReason)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.crash_status = Int(1);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_CRASH_PENDING);
}

}  // namespace
