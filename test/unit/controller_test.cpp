/**
 * @file    controller_test.cpp
 * @brief   controller 단위 테스트 (MC/DC 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>

#include <chrono>
#include "ecl/controller.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      UT-ctl-001
 * @brief     [목적] init 의 ctl 이 널이면 ECL_ERR_NULL_ARG 이다.
 * @satisfies DD-M-006
 * @technique MC/DC(조건 ctl == NULL 의 독립 영향)
 * @input     ctl=nullptr
 * @procedure 1) 호출한다.
 * @expected  ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, InitReturnsErrNullArgWhenCtlIsNull)
{
    EXPECT_EQ(ecl_controller_init(nullptr), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-ctl-002
 * @brief     [목적] init 은 좌/우 RELEASE, HOLD, NONE 으로 초기화한다.
 * @satisfies REQ-F-015
 * @technique 동치 클래스(초기 상태)
 * @input     센티널로 채운 제어기
 * @procedure 1) 센티널로 채운다. 2) init 한다.
 * @expected  좌/우 RELEASE, priority HOLD, reason NONE, note NONE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, InitSetsReleasedHoldDecision)
{
    ecl_controller_t ctl{};
    ctl.confirmed = ecl_decision_t{ECL_OUT_LOCK, ECL_OUT_LOCK, ECL_PRIO_CRASH, ECL_REASON_FIRE, ECL_REASON_OVERRIDE};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    EXPECT_EQ(ctl.confirmed.left, ECL_OUT_RELEASE);
    EXPECT_EQ(ctl.confirmed.right, ECL_OUT_RELEASE);
    EXPECT_EQ(ctl.confirmed.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(ctl.confirmed.reason_code, ECL_REASON_NONE);
    EXPECT_EQ(ctl.confirmed.command_note, ECL_REASON_NONE);
}

/**
 * @test      UT-ctl-003
 * @brief     [목적] step 의 ctl 인자가 널이면 ECL_ERR_NULL_ARG 이다.
 * @satisfies DD-M-006
 * @technique MC/DC(널 조건 3개의 독립 영향)
 * @input     CtlIsNull
 * @procedure 1) 호출한다.
 * @expected  ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, StepReturnsErrNullArgWhenCtlIsNull)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    EXPECT_EQ(ecl_controller_step(nullptr, &raw, nullptr, 0, 1.0, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-ctl-004
 * @brief     [목적] step 의 raw 인자가 널이면 ECL_ERR_NULL_ARG 이다.
 * @satisfies DD-M-006
 * @technique MC/DC(널 조건 3개의 독립 영향)
 * @input     RawIsNull
 * @procedure 1) 호출한다.
 * @expected  ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, StepReturnsErrNullArgWhenRawIsNull)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    EXPECT_EQ(ecl_controller_step(&ctl, nullptr, nullptr, 0, 1.0, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-ctl-005
 * @brief     [목적] step 의 out 인자가 널이면 ECL_ERR_NULL_ARG 이다.
 * @satisfies DD-M-006
 * @technique MC/DC(널 조건 3개의 독립 영향)
 * @input     OutIsNull
 * @procedure 1) 호출한다.
 * @expected  ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, StepReturnsErrNullArgWhenOutIsNull)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    EXPECT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.0, nullptr), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-ctl-006
 * @brief     [목적] 정상 입력 한 주기는 ACTIVE, 좌/우 RELEASE, 이유 NONE 을 공개한다.
 * @satisfies REQ-F-013, REQ-F-016
 * @technique 동치 클래스(정상)
 * @input     ValidRaw(), 명령 없음
 * @procedure 1) init 2) step
 * @expected  ECL_OK, ACTIVE, 좌/우 RELEASE, warnings 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, StepPublishesActiveResultForValidInput)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.warnings, 0U);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
}

/**
 * @test      UT-ctl-007
 * @brief     [목적] timestamp 가 무효이면 DEGRADED 로 확정 출력을 유지한다 (요청 시각은 0.0).
 * @satisfies REQ-F-003, REQ-F-010, REQ-F-011, REQ-F-014
 * @technique MC/DC(request_time_of 의 timestamp.valid 독립 영향)
 * @input     timestamp 누락, 확정 좌 LOCK, 명령 1건
 * @procedure 1) 확정 좌를 LOCK 으로 둔다. 2) step
 * @expected  DEGRADED, 좌 LOCK 유지, reason INPUT_INVALID, warnings INPUT_INVALID
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, StepWithInvalidTimestampHoldsOutputsAndIsDegraded)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.timestamp_s = MissingNum();
    const ecl_raw_command_t cmd = ValidCommand();
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
}

/**
 * @test      UT-ctl-008
 * @brief     [목적] cmds 가 널이고 count 가 0 초과이면 오류를 반환하고 확정 결정과 out 을 바꾸지 않는다.
 * @satisfies DD-M-006
 * @technique MC/DC(오류 합산 결정의 독립 영향)
 * @input     cmds=nullptr, count=1
 * @procedure 1) 확정 좌를 LOCK 으로 둔다. 2) step
 * @expected  ECL_ERR_NULL_ARG, confirmed.left == LOCK, out.state == 센티널
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, StepWithNullCommandsAndPositiveCountFailsWithoutChangingState)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    out.state = ECL_STATE_OFF;
    EXPECT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 1, 1.01, &out), ECL_ERR_NULL_ARG);
    EXPECT_EQ(ctl.confirmed.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.state, ECL_STATE_OFF);
}

/**
 * @test      UT-ctl-009
 * @brief     [목적] sensor_fault 이면 직전 확정 출력을 유지하고 FAULT, 경고를 공개한다.
 * @satisfies REQ-S-004, REQ-S-009
 * @technique 동치 클래스(센서 고장)
 * @input     sensor_fault=1, 확정 좌 LOCK
 * @procedure 1) 확정 좌를 LOCK 으로 둔다. 2) step
 * @expected  FAULT, 좌 LOCK, 우 RELEASE, reason SENSOR_FAULT, warnings SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, SensorFaultKeepsPreviousOutputsAndReportsFault)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.sensor_fault = Int(1);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
    EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_SENSOR_FAULT);
}

/**
 * @test      UT-ctl-010
 * @brief     [목적] 한 주기에 명령이 2건이면 결정의 command_note 에 CMD_REJECTED_EXTRA 가 공개된다.
 * @satisfies REQ-F-009
 * @technique 동치 클래스(추가 명령)
 * @input     명령 2건
 * @procedure 1) step
 * @expected  decision.command_note == CMD_REJECTED_EXTRA
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, ExtraCommandIsReportedAsCommandNote)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    const ecl_raw_command_t cmds[2] = {ValidCommand(), ValidCommand()};
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 2, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_EXTRA);
}

/**
 * @test      UT-ctl-011
 * @brief     [목적] 평가주기 한 번의 평균 처리 시간은 3 ms 미만이다 (호스트, 계측 빌드).
 * @satisfies REQ-Q-003
 * @technique 성능 측정(평균)
 * @input     10000 회 반복
 * @procedure 1) 10000 회 step 을 실행하고 평균 시간을 잰다.
 * @expected  평균 < 3 ms
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, OneCycleIsFasterThanThreeMilliseconds)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    const int cycles = 10000;
    const auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < cycles; ++i)
    {
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    const auto elapsed = std::chrono::steady_clock::now() - begin;
    const double averageMs = std::chrono::duration<double, std::milli>(elapsed).count() / cycles;
    EXPECT_LT(averageMs, 3.0);
}

/**
 * @test      UT-ctl-012
 * @brief     [목적] 규칙 엔진 기억이 주기 사이에 유지되어 override 가 되고, 오류 주기에는 기억이 갱신되지 않는다.
 * @satisfies REQ-F-023, REQ-F-036, DD-M-006
 * @technique 상태 전이 + 오류 경로
 * @input     좌 접근위험, 해제 명령 t=100.0, 오류 주기, 해제 명령 t=105.0
 * @procedure 1) 첫 요청 2) cmds=nullptr,count=1 오류 3) 두 번째 요청
 * @expected  2단계: ECL_ERR_NULL_ARG 이고 기억 변화 없음, 3단계: 좌 RELEASE, reason OVERRIDE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, OverrideMemoryPersistsAcrossCyclesAndIsNotCommittedOnError)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    EXPECT_EQ(ctl.memory.left.has_first, 0);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.rear_left_risk = Int(1);
    const ecl_raw_command_t cmd = Cmd(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ecl_cycle_result_t out{};
    raw.timestamp_s = Num(100.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 100.0, &out), ECL_OK);
    EXPECT_EQ(ctl.memory.left.has_first, 1);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    raw.timestamp_s = Num(103.0);
    EXPECT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 1, 103.0, &out), ECL_ERR_NULL_ARG);
    EXPECT_EQ(ctl.memory.left.override_active, 0);
    raw.timestamp_s = Num(105.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 105.0, &out), ECL_OK);
    EXPECT_EQ(ctl.memory.left.override_active, 1);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_OVERRIDE);
}

}  // namespace
