/**
 * @file    ecl_controller_integration_test.cpp
 * @brief   controller 와 하위 모듈을 실제 인터페이스로 연결하는 통합 테스트 (함수 커버리지 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>

#include <cstring>
#include <vector>
#include "ecl/controller.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      IT-ecl-001
 * @brief     [목적] 정상 입력은 검증, 상태 판정, 규칙 평가를 거쳐 ACTIVE 와 확정 출력으로 공개된다.
 * @satisfies ARCH-I-001, ARCH-I-003, ARCH-I-004, ARCH-I-006, REQ-F-013, REQ-F-016
 * @technique 데이터 흐름 전 구간
 * @input     ValidRaw() 3주기(1.01, 1.02, 1.03 s)
 * @procedure 1) init 2) step 을 3회 호출한다.
 * @expected  매 주기 ECL_OK, ACTIVE, 좌/우 RELEASE, 경고 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, ValidInputsFlowToActiveState)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    for (int i = 0; i < 3; ++i)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.timestamp_s = Num(1.0 + (0.01 * i));
        ecl_cycle_result_t out{};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.0 + (0.01 * i) + 0.005, &out), ECL_OK);
        EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
        EXPECT_EQ(out.warnings, 0U);
        EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
        EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    }
}

/**
 * @test      IT-ecl-002
 * @brief     [목적] 범위 밖 입력은 DEGRADED 로 출력 유지, 다음 정상 입력에서 ACTIVE 로 자동 복귀한다.
 * @satisfies ARCH-I-001, ARCH-I-003, ARCH-I-004, REQ-F-010, REQ-F-011, REQ-F-014
 * @technique 상태 전이(정상 → 오류 → 정상)
 * @input     주기 1: 정상, 주기 2: speed=500, 주기 3: 정상
 * @procedure 1) 세 주기를 실행한다.
 * @expected  주기 2: DEGRADED, INPUT_INVALID 경고, 이유 INPUT_INVALID, 출력 유지 / 주기 3: ACTIVE, 이유 NONE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, InvalidInputDegradesAndRecoversAutomatically)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    raw.speed_kph = Num(500.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    raw.speed_kph = Num(2.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.03, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);
}

/**
 * @test      IT-ecl-003
 * @brief     [목적] 200 ms 넘게 갱신되지 않은 입력은 DEGRADED, INPUT_STALE 경고와 출력 유지로 공개된다.
 * @satisfies ARCH-I-001, ARCH-I-003, ARCH-I-004, REQ-S-003
 * @technique 경계값 분석(0.2 s 초과)
 * @input     timestamp=1.0, eval=1.25
 * @procedure 1) step
 * @expected  DEGRADED, INPUT_STALE 경고, 이유 INPUT_STALE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, StaleInputDegradesWithStaleWarning)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.25, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_STALE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_STALE);
}

/**
 * @test      IT-ecl-004
 * @brief     [목적] sensor_fault 는 FAULT 상태, 경고, 규칙 1 의 출력 유지로 함께 공개된다.
 * @satisfies ARCH-I-003, ARCH-I-004, REQ-S-004, REQ-S-009
 * @technique 오류 전파
 * @input     sensor_fault=1, 확정 우 LOCK
 * @procedure 1) step
 * @expected  FAULT, SENSOR_FAULT 경고, 우 LOCK 유지, priority SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, SensorFaultFlowsToFaultStateAndHold)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.sensor_fault = Int(1);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
    EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);
}

/**
 * @test      IT-ecl-005
 * @brief     [목적] 점화 꺼짐은 OFF 상태로 공개된다.
 * @satisfies ARCH-I-003, REQ-F-014
 * @technique 동치 클래스(점화 꺼짐)
 * @input     ignition_on=0
 * @procedure 1) step
 * @expected  OFF, 경고 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, IgnitionOffFlowsToOffState)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.ignition_on = Int(0);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_OFF);
    EXPECT_EQ(out.warnings, 0U);
}

/**
 * @test      IT-ecl-006
 * @brief     [목적] 명령 거절 이유가 접수 모듈에서 규칙 평가를 거쳐 공개 결정까지 전달된다.
 * @satisfies ARCH-I-002, ARCH-I-004, ARCH-I-006, REQ-F-007, REQ-F-009
 * @technique 오류 전파
 * @input     주기 1: 무효 명령 1건, 주기 2: 유효 명령 2건, 주기 3: 명령 없음
 * @procedure 1) 세 주기를 실행한다.
 * @expected  command_note: CMD_REJECTED_INVALID, CMD_REJECTED_EXTRA, NONE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, CommandRejectionsReachThePublishedDecision)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    ecl_raw_command_t bad = ValidCommand();
    bad.source = Int(9);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &bad, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    const ecl_raw_command_t two[2] = {ValidCommand(), ValidCommand()};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, two, 2, 1.02, &out), ECL_OK);
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_EXTRA);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.03, &out), ECL_OK);
    EXPECT_EQ(out.decision.command_note, ECL_REASON_NONE);
}

/**
 * @test      IT-ecl-007
 * @brief     [목적] 같은 초기 상태와 같은 입력 순서는 같은 공개 결과를 만든다.
 * @satisfies ARCH-I-006, REQ-Q-001
 * @technique 결정론 재생(두 번)
 * @input     정상, 범위 밖, stale, sensor_fault, 점화 꺼짐 5주기
 * @procedure 1) 두 제어기에 같은 순서를 넣는다.
 * @expected  모든 주기에서 두 결과의 모든 필드가 같다
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, SameInputSequenceGivesSameResults)
{
    struct Step
    {
        ecl_raw_snapshot_t raw;
        double evalTime;
    };
    std::vector<Step> sequence;
    sequence.push_back({ValidRaw(), 1.01});
    ecl_raw_snapshot_t outOfRange = ValidRaw();
    outOfRange.speed_kph = Num(301.0);
    sequence.push_back({outOfRange, 1.02});
    sequence.push_back({ValidRaw(), 1.50});
    ecl_raw_snapshot_t fault = ValidRaw();
    fault.sensor_fault = Int(1);
    sequence.push_back({fault, 1.03});
    ecl_raw_snapshot_t off = ValidRaw();
    off.ignition_on = Int(0);
    sequence.push_back({off, 1.04});
    ecl_controller_t first{};
    ecl_controller_t second{};
    ASSERT_EQ(ecl_controller_init(&first), ECL_OK);
    ASSERT_EQ(ecl_controller_init(&second), ECL_OK);
    for (const Step& step : sequence)
    {
        ecl_cycle_result_t a{};
        ecl_cycle_result_t b{};
        ASSERT_EQ(ecl_controller_step(&first, &step.raw, nullptr, 0, step.evalTime, &a), ECL_OK);
        ASSERT_EQ(ecl_controller_step(&second, &step.raw, nullptr, 0, step.evalTime, &b), ECL_OK);
        EXPECT_EQ(a.decision.left, b.decision.left);
        EXPECT_EQ(a.decision.right, b.decision.right);
        EXPECT_EQ(a.decision.priority_reason, b.decision.priority_reason);
        EXPECT_EQ(a.decision.reason_code, b.decision.reason_code);
        EXPECT_EQ(a.decision.command_note, b.decision.command_note);
        EXPECT_EQ(a.state, b.state);
        EXPECT_EQ(a.warnings, b.warnings);
    }
}

/**
 * @test      IT-ecl-008
 * @brief     [목적] 충돌 확정이 모듈을 거쳐 RELEASE 로 공개되고 확정인 동안 유지되며 해소 후에도 확정 출력이 유지된다.
 * @satisfies ARCH-I-001, ARCH-I-004, ARCH-I-006, REQ-S-001, REQ-S-005
 * @technique 상태 전이(NONE → CONFIRMED → NONE)
 * @input     이전 좌/우 LOCK, 5주기 CONFIRMED 후 NONE
 * @procedure 1) 확정 출력을 LOCK 으로 둔다. 2) CONFIRMED 5주기 3) NONE 1주기
 * @expected  CONFIRMED 구간 모든 주기 좌/우 RELEASE(reason CRASH_CONFIRMED), 이후 RELEASE 유지(HOLD, NONE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, CrashConfirmedReleasesAndHoldsThenKeepsReleased)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    raw.crash_status = Int(ECL_CRASH_CONFIRMED);
    for (int k = 0; k < 5; ++k)
    {
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01 + (0.01 * k), &out), ECL_OK);
        EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
        EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
        EXPECT_EQ(out.decision.reason_code, ECL_REASON_CRASH_CONFIRMED);
    }
    raw.crash_status = Int(ECL_CRASH_NONE);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.06, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);
}

/**
 * @test      IT-ecl-009
 * @brief     [목적] 접근위험 도어의 해제 요청은 주기마다 억제되고 반대쪽 도어는 영향이 없다.
 * @satisfies ARCH-I-002, ARCH-I-004, ARCH-I-006, REQ-S-002, REQ-S-006, REQ-S-007, REQ-F-017
 * @technique 도어 독립 + 반복 요청
 * @input     우 접근위험 TRUE, 이전 좌/우 LOCK, 매 주기 좌 해제 명령과 우 해제 명령 번갈아
 * @procedure 1) 3주기 실행
 * @expected  우는 항상 LOCK(reason APPROACH_RISK_HOLD), 좌는 해제 명령이 온 주기(k=1)에 RELEASE (규칙 8) 되고 이후 유지, k=0 은 LOCK 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, ApproachRiskSuppressesUnlockRequestsPerDoor)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.rear_right_risk = Int(1);
    for (int k = 0; k < 3; ++k)
    {
        const ecl_raw_command_t cmd = Cmd((k % 2) == 0 ? ECL_SIDE_RIGHT : ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
        ecl_cycle_result_t out{};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, &cmd, 1, 1.01 + (0.01 * k), &out), ECL_OK);
        EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
        EXPECT_EQ(out.decision.left, (k >= 1) ? ECL_OUT_RELEASE : ECL_OUT_LOCK);
        EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_APPROACH);
        EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
    }
}

/**
 * @test      IT-ecl-010
 * @brief     [목적] sensor_fault > 충돌 확정 > 접근위험 우선순위가 모듈 연결에서 지켜진다.
 * @satisfies ARCH-I-004, ARCH-I-006, REQ-F-012, REQ-F-020
 * @technique 우선순위 단계별(3 > 2 > 1)
 * @input     접근위험만, 접근위험+충돌, 접근위험+충돌+sensor_fault
 * @procedure 1) 같은 제어기에서 3주기 실행
 * @expected  주기 1: 좌 LOCK(APPROACH), 주기 2: 좌/우 RELEASE(CRASH), 주기 3: 직전 출력(RELEASE) 유지(SENSOR_FAULT)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, RulePriorityFlowsThroughComponents)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    raw.rear_left_risk = Int(1);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_APPROACH);
    raw.crash_status = Int(ECL_CRASH_CONFIRMED);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_CRASH);
    raw.sensor_fault = Int(1);
    raw.crash_status = Int(ECL_CRASH_NONE);
    raw.rear_left_risk = Int(1);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.03, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);
}

/**
 * @test      IT-ecl-011
 * @brief     [목적] 규칙 2·3 이 포함된 입력 순서도 재생 결과가 같다.
 * @satisfies ARCH-I-006, REQ-Q-001
 * @technique 결정론 재생(두 번)
 * @input     충돌 확정/접근위험/PENDING 혼합 6주기
 * @procedure 1) 두 제어기에 같은 순서
 * @expected  모든 주기 결과 필드가 같다
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, SameSequenceWithRulesGivesSameResults)
{
    struct Step
    {
        int crash;
        int risk;
    };
    const Step steps[] = {{0, 0}, {1, 1}, {2, 1}, {0, 1}, {0, 0}, {2, 0}};
    ecl_controller_t first{};
    ecl_controller_t second{};
    ASSERT_EQ(ecl_controller_init(&first), ECL_OK);
    ASSERT_EQ(ecl_controller_init(&second), ECL_OK);
    double now = 1.0;
    for (const Step& step : steps)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.timestamp_s = Num(now);
        raw.crash_status = Int(step.crash);
        raw.rear_right_risk = Int(step.risk);
        ecl_cycle_result_t a{};
        ecl_cycle_result_t b{};
        ASSERT_EQ(ecl_controller_step(&first, &raw, nullptr, 0, now, &a), ECL_OK);
        ASSERT_EQ(ecl_controller_step(&second, &raw, nullptr, 0, now, &b), ECL_OK);
        EXPECT_EQ(a.decision.left, b.decision.left);
        EXPECT_EQ(a.decision.right, b.decision.right);
        EXPECT_EQ(a.decision.priority_reason, b.decision.priority_reason);
        EXPECT_EQ(a.decision.reason_code, b.decision.reason_code);
        EXPECT_EQ(a.state, b.state);
        now += 0.01;
    }
}

/**
 * @test      IT-ecl-012
 * @brief     [목적] 정지 중 운전자 명령(좌 잠금, 전체 해제)이 접수·규칙 평가를 거쳐 출력으로 공개되고 이후 주기에 유지된다.
 * @satisfies ARCH-I-002, ARCH-I-004, ARCH-I-006, REQ-F-021, REQ-F-034
 * @technique 데이터 흐름 + 상태 유지
 * @input     정지, 주기 1 좌 잠금(물리 버튼), 주기 2 명령 없음, 주기 3 전체 해제(모바일 앱)
 * @procedure 1) 세 주기 실행
 * @expected  주기 1: 좌 LOCK/우 RELEASE, 주기 2: 유지, 주기 3: 좌/우 RELEASE (reason DRIVER_CMD)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, DriverCommandsFlowThroughStationaryCycles)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_cycle_result_t out{};
    const ecl_raw_command_t lockLeft = Cmd(ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &lockLeft, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_DRIVER_CMD);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    const ecl_raw_command_t unlockAll = Cmd(ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_MOBILE_APP);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &unlockAll, 1, 1.03, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      IT-ecl-013
 * @brief     [목적] 접근위험 억제 → 10초 내 재입력 override → 접근위험 해소로 override 종료가 제어기를 거쳐 공개된다.
 * @satisfies ARCH-I-004, ARCH-I-006, REQ-F-023, REQ-F-036
 * @technique 상태 전이(억제 → override → 해소 → 재발)
 * @input     좌 접근위험, 해제 명령 t=100.0, 105.0, 이후 위험 해소, 재발
 * @procedure 1) 다섯 주기 실행
 * @expected  t=100 좌 LOCK(APPROACH_RISK_HOLD), t=105 좌 RELEASE(OVERRIDE), 위험 해소 후 RELEASE 유지, 재발 시 LOCK(APPROACH_RISK_HOLD)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, OverrideFlowsThroughControllerAndClearsWhenRiskEnds)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_command_t unlock = Cmd(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ecl_cycle_result_t out{};
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.rear_left_risk = Int(1);
    raw.timestamp_s = Num(100.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &unlock, 1, 100.0, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
    raw.timestamp_s = Num(105.0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &unlock, 1, 105.0, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_OVERRIDE);
    raw.timestamp_s = Num(106.0);
    raw.rear_left_risk = Int(0);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 106.0, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    raw.timestamp_s = Num(107.0);
    raw.rear_left_risk = Int(1);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 107.0, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      IT-ecl-014
 * @brief     [목적] 이동 시작 자동 잠금과 명령 거절 이유, 화재·점화 꺼짐 해제의 순위가 제어기를 거쳐 공개된다.
 * @satisfies ARCH-I-004, ARCH-I-006, REQ-F-022, REQ-F-025, REQ-F-027, REQ-F-035
 * @technique 우선순위 단계별(7 → 4 → 5)
 * @input     속도 5 + 해제 명령, 속도 5 + 화재, 속도 0 + 점화 꺼짐 + ISOFIX 좌
 * @procedure 1) 세 주기 실행
 * @expected  1: 좌/우 LOCK + CMD_REJECTED_MOVING, 2: 좌/우 RELEASE(FIRE), 3: 좌/우 RELEASE(IGNITION_OFF, 상태 OFF)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclControllerIntegrationTest, MovingStartLocksAndRejectsCommandThenEmergencyAndIgnitionReleaseByRank)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.speed_kph = Num(5.0);
    const ecl_raw_command_t unlock = Cmd(ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_VOICE);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, &unlock, 1, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_MOVING);
    raw.fire = Int(1);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_FIRE);
    raw.fire = Int(0);
    raw.speed_kph = Num(0.0);
    raw.ignition_on = Int(0);
    raw.isofix_left = Int(1);
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.03, &out), ECL_OK);
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_IGNITION_OFF);
    EXPECT_EQ(out.state, ECL_STATE_OFF);
}

}  // namespace
