/**
 * @file    ecl_qm_rules_tdd_test.cpp
 * @brief   이터레이션 3 TDD 시험: 규칙 4~8(QM)과 override (단위 테스트와 분리).
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
 * @test      TDD-ecl-301
 * @brief     [목적] 화재 TRUE 이면 좌/우 RELEASE, 이유 FIRE.
 * @satisfies REQ-F-025
 * @technique TDD red-green-refactor
 * @input     fire=1, 이전 좌/우 LOCK
 * @procedure 1) step
 * @expected  좌/우 RELEASE, priority EMERGENCY_RELEASE, reason FIRE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, FireReleasesBothDoors)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.fire = Int(1);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_EMERGENCY_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_FIRE);
}

/**
 * @test      TDD-ecl-302
 * @brief     [목적] 점화 꺼짐이면 좌/우 RELEASE, 이유 IGNITION_OFF, 상태 OFF.
 * @satisfies REQ-F-027
 * @technique TDD red-green-refactor
 * @input     ignition_on=0, 이전 좌/우 LOCK
 * @procedure 1) step
 * @expected  좌/우 RELEASE, state OFF, reason IGNITION_OFF
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, IgnitionOffReleasesBothDoors)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.ignition_on = Int(0);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.state, ECL_STATE_OFF);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_IGNITION_OFF);
}

/**
 * @test      TDD-ecl-303
 * @brief     [목적] 좌 ISOFIX 이면 좌만 LOCK 이다.
 * @satisfies REQ-F-026
 * @technique TDD red-green-refactor
 * @input     isofix_left=1, 이전 좌/우 RELEASE
 * @procedure 1) step
 * @expected  좌 LOCK, 우 RELEASE, reason ISOFIX_LEFT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, IsofixLeftLocksOnlyLeft)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);

    ecl_raw_snapshot_t raw = ValidRaw();
    raw.isofix_left = Int(1);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_ISOFIX_LEFT);
}

/**
 * @test      TDD-ecl-304
 * @brief     [목적] 속도 3.0 km/h 이상이면 좌/우 LOCK.
 * @satisfies REQ-F-022
 * @technique TDD red-green-refactor
 * @input     speed=3.0, 이전 좌/우 RELEASE
 * @procedure 1) step
 * @expected  좌/우 LOCK, reason AUTO_LOCK_MOVING
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, MovingStartLocksBothDoors)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);

    ecl_raw_snapshot_t raw = ValidRaw();
    raw.speed_kph = Num(3.0);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_AUTO_LOCK_MOVING);
}

/**
 * @test      TDD-ecl-305
 * @brief     [목적] 정지 중 좌 해제 명령은 좌만 RELEASE 한다.
 * @satisfies REQ-F-021
 * @technique TDD red-green-refactor
 * @input     speed=0, 좌 해제 명령, 이전 좌/우 LOCK
 * @procedure 1) step
 * @expected  좌 RELEASE, 우 LOCK, reason DRIVER_CMD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, StationaryUnlockCommandReleasesTargetDoor)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.speed_kph = Num(0.0);
    const ecl_raw_command_t cmds[1] = {Cmd(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN)};
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_DRIVER_CMD);
}

/**
 * @test      TDD-ecl-306
 * @brief     [목적] 이동 중 명령은 적용되지 않고 CMD_REJECTED_MOVING 으로 기록된다.
 * @satisfies REQ-F-035
 * @technique TDD red-green-refactor
 * @input     speed=10, 좌 해제 명령
 * @procedure 1) step
 * @expected  좌 LOCK(자동 잠금), command_note CMD_REJECTED_MOVING
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, MovingCommandIsNotApplied)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);

    ecl_raw_snapshot_t raw = ValidRaw();
    raw.speed_kph = Num(10.0);
    const ecl_raw_command_t cmds[1] = {Cmd(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN)};
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_MOVING);
}

/**
 * @test      TDD-ecl-307
 * @brief     [목적] 접근위험 억제 후 10초 이내 같은 해제 재입력은 override 되어 RELEASE, 이유 OVERRIDE.
 * @satisfies REQ-F-023, REQ-F-036
 * @technique TDD red-green-refactor
 * @input     좌 접근위험, 해제 명령 timestamp 100.0 과 105.0, 이전 좌 LOCK
 * @procedure 1) 첫 요청 step 2) 두 번째 요청 step
 * @expected  첫째: 좌 LOCK(APPROACH_RISK_HOLD), 둘째: 좌 RELEASE, reason OVERRIDE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclQmRulesTddTest, RepeatedUnlockWithinTenSecondsOverridesApproachRisk)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    const ecl_raw_command_t cmd = Cmd(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ecl_cycle_result_t out{};
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.speed_kph = Num(0.0);
    raw.rear_left_risk = Int(1);
    raw.timestamp_s = Num(100.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 100.0, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
    raw.timestamp_s = Num(105.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 105.0, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_OVERRIDE);
}

}  // namespace
