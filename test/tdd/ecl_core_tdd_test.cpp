/**
 * @file    ecl_core_tdd_test.cpp
 * @brief   구현 중 TDD 로 작성한 ecl 코어 시험 (단위 테스트와 분리, 별도 결과·커버리지).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>
#include "ecl/command_intake.h"
#include "ecl/controller.h"
#include "ecl/input_validation.h"
#include "ecl/rule_engine.h"
#include "ecl/status_eval.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      TDD-ecl-001
 * @brief     [목적] 모든 필드가 정상인 입력은 all_valid 이다 (첫 사이클).
 * @satisfies REQ-F-001, REQ-F-005
 * @technique TDD red-green-refactor
 * @input     ValidRaw(), eval=1.1
 * @procedure 1) ecl_input_validate() 를 호출한다.
 * @expected  반환값 == ECL_OK, all_valid == 1, stale == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclCoreTddTest, ValidSnapshotIsAllValid)
{
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_validated_t out{};
    EXPECT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
    EXPECT_EQ(out.all_valid, 1);
    EXPECT_EQ(out.stale, 0);
}

/**
 * @test      TDD-ecl-002
 * @brief     [목적] 300.0 을 넘는 속도는 INVALID 이고 all_valid 가 아니다.
 * @satisfies REQ-F-001
 * @technique TDD red-green-refactor
 * @input     speed=300.1
 * @procedure 1) 속도를 300.1 로 바꿔 검증한다.
 * @expected  speed_kph.valid == 0, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclCoreTddTest, SpeedAboveMaximumIsInvalid)
{
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.speed_kph = Num(300.1);
    ecl_validated_t out{};
    EXPECT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
    EXPECT_EQ(out.speed_kph.valid, 0);
    EXPECT_EQ(out.all_valid, 0);
}

/**
 * @test      TDD-ecl-003
 * @brief     [목적] 200 ms 를 넘게 오래된 입력은 stale 이다.
 * @satisfies REQ-S-003
 * @technique TDD red-green-refactor
 * @input     timestamp=1.0, eval=1.3
 * @procedure 1) 평가 시각 1.3 으로 검증한다.
 * @expected  stale == 1, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclCoreTddTest, OldSnapshotIsStale)
{
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_validated_t out{};
    EXPECT_EQ(ecl_input_validate(&raw, 1.3, &out), ECL_OK);
    EXPECT_EQ(out.stale, 1);
    EXPECT_EQ(out.all_valid, 0);
}

/**
 * @test      TDD-ecl-004
 * @brief     [목적] sensor_fault 가 TRUE 이면 상태는 FAULT 이다.
 * @satisfies REQ-S-009, REQ-F-014
 * @technique TDD red-green-refactor
 * @input     sensor_fault=1
 * @procedure 1) 검증 후 상태를 판정한다.
 * @expected  state == ECL_STATE_FAULT, 경고에 SENSOR_FAULT 비트
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclCoreTddTest, SensorFaultYieldsFaultState)
{
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.sensor_fault = Int(1);
    ecl_validated_t validated{};
    ASSERT_EQ(ecl_input_validate(&raw, 1.1, &validated), ECL_OK);
    ecl_status_result_t status{};
    EXPECT_EQ(ecl_status_evaluate(&validated, &status), ECL_OK);
    EXPECT_EQ(status.state, ECL_STATE_FAULT);
    EXPECT_NE(status.warnings & ECL_WARN_SENSOR_FAULT, 0u);
}

/**
 * @test      TDD-ecl-005
 * @brief     [목적] 초기화한 제어기의 확정 출력은 좌/우 RELEASE 이다.
 * @satisfies REQ-F-015
 * @technique TDD red-green-refactor
 * @input     init 직후
 * @procedure 1) ecl_controller_init() 을 호출한다.
 * @expected  confirmed.left == RELEASE, confirmed.right == RELEASE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclCoreTddTest, ControllerStartsReleased)
{
    ecl_controller_t ctl{};
    EXPECT_EQ(ecl_controller_init(&ctl), ECL_OK);
    EXPECT_EQ(ctl.confirmed.left, ECL_OUT_RELEASE);
    EXPECT_EQ(ctl.confirmed.right, ECL_OUT_RELEASE);
}

/**
 * @test      TDD-ecl-006
 * @brief     [목적] 정상 입력 한 주기를 실행하면 출력과 상태가 공개된다.
 * @satisfies REQ-F-013, REQ-F-016
 * @technique TDD red-green-refactor
 * @input     ValidRaw(), 명령 없음
 * @procedure 1) init 2) step 을 호출한다.
 * @expected  ECL_OK, 좌/우 RELEASE, 상태 ACTIVE, 이유 NONE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclCoreTddTest, ControllerStepPublishesOutputsAndState)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    EXPECT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);
}

}  // namespace
