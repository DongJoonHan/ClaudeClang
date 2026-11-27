/**
 * @file    rule_engine_test.cpp
 * @brief   rule_engine 단위 테스트 (MC/DC 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>
#include "ecl/rule_engine.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      UT-rule-001
 * @brief     [목적] in 인자가 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-004
 * @technique MC/DC(널 조건 4개의 독립 영향)
 * @input     InIsNull
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
TEST(RuleEngineTest, ReturnsErrNullArgWhenInIsNull)
{
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    EXPECT_EQ(ecl_rule_decide(nullptr, &cmd, &confirmed, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-rule-002
 * @brief     [목적] cmd 인자가 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-004
 * @technique MC/DC(널 조건 4개의 독립 영향)
 * @input     CmdIsNull
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
TEST(RuleEngineTest, ReturnsErrNullArgWhenCmdIsNull)
{
    const ecl_validated_t in = ValidValidated();
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    EXPECT_EQ(ecl_rule_decide(&in, nullptr, &confirmed, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-rule-003
 * @brief     [목적] confirmed 인자가 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-004
 * @technique MC/DC(널 조건 4개의 독립 영향)
 * @input     ConfirmedIsNull
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
TEST(RuleEngineTest, ReturnsErrNullArgWhenConfirmedIsNull)
{
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    ecl_decision_t out{};
    EXPECT_EQ(ecl_rule_decide(&in, &cmd, nullptr, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-rule-004
 * @brief     [목적] out 인자가 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-004
 * @technique MC/DC(널 조건 4개의 독립 영향)
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
TEST(RuleEngineTest, ReturnsErrNullArgWhenOutIsNull)
{
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    EXPECT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, nullptr), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-rule-005
 * @brief     [목적] 적용되는 규칙이 없고 입력이 정상이면 확정 출력을 유지하고 HOLD/NONE 이다.
 * @satisfies REQ-F-011, REQ-F-012, REQ-F-020
 * @technique 동치 클래스(규칙 없음)
 * @input     확정 출력 좌 LOCK, 우 RELEASE, 정상 입력
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, 우 RELEASE, priority HOLD, reason NONE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, HoldsConfirmedOutputsWhenNoRuleApplies)
{
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_RELEASE;
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.reason_code, ECL_REASON_NONE);
}

/**
 * @test      UT-rule-006
 * @brief     [목적] 입력이 INVALID 이면 HOLD 이유는 INPUT_INVALID 이다.
 * @satisfies REQ-F-010, REQ-F-011
 * @technique MC/DC(조건 all_valid == 0, stale != 0 의 독립 영향)
 * @input     all_valid=0, stale=0
 * @procedure 1) 호출한다.
 * @expected  priority HOLD, reason INPUT_INVALID, 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, HoldReasonIsInputInvalidWhenNotAllValid)
{
    ecl_validated_t in = ValidValidated();
    in.all_valid = 0;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    confirmed.left = ECL_OUT_LOCK;
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.reason_code, ECL_REASON_INPUT_INVALID);
}

/**
 * @test      UT-rule-007
 * @brief     [목적] 입력이 stale 이면 HOLD 이유는 INPUT_STALE 이다.
 * @satisfies REQ-S-003, REQ-F-011
 * @technique MC/DC(조건 stale != 0 의 독립 영향)
 * @input     all_valid=0, stale=1
 * @procedure 1) 호출한다.
 * @expected  reason INPUT_STALE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, HoldReasonIsInputStaleWhenStale)
{
    ecl_validated_t in = ValidValidated();
    in.all_valid = 0;
    in.stale = 1;
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_INPUT_STALE);
}

/**
 * @test      UT-rule-008
 * @brief     [목적] sensor_fault 가 유효한 TRUE 이면 확정 출력을 유지하고 SENSOR_FAULT 규칙이 이유이다.
 * @satisfies REQ-S-004, REQ-F-012
 * @technique MC/DC(규칙 1 의 valid, value 독립 영향)
 * @input     sensor_fault={1,1}, 확정 좌 LOCK, 우 RELEASE
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, 우 RELEASE, priority SENSOR_FAULT, reason SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, SensorFaultKeepsConfirmedOutputs)
{
    ecl_validated_t in = ValidValidated();
    in.sensor_fault.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    confirmed.left = ECL_OUT_LOCK;
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_SENSOR_FAULT);
    EXPECT_EQ(out.reason_code, ECL_REASON_SENSOR_FAULT);
}

/**
 * @test      UT-rule-009
 * @brief     [목적] sensor_fault 가 INVALID 이거나 FALSE 이면 규칙 1 은 적용되지 않는다.
 * @satisfies REQ-F-010
 * @technique MC/DC(규칙 1 의 valid, value 독립 영향)
 * @input     {valid=0,value=1}, {valid=1,value=0}
 * @procedure 1) 각각 호출한다.
 * @expected  priority HOLD (규칙 1 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, InvalidOrFalseSensorFaultDoesNotApplyRuleOne)
{
    ecl_validated_t invalid = ValidValidated();
    invalid.sensor_fault.valid = 0;
    invalid.sensor_fault.value = 1;
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&invalid, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    const ecl_validated_t falseFault = ValidValidated();
    ASSERT_EQ(ecl_rule_decide(&falseFault, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
}

/**
 * @test      UT-rule-010
 * @brief     [목적] 결정의 command_note 는 명령 채택 결과의 note 와 같다.
 * @satisfies REQ-F-007, REQ-F-009
 * @technique 동치 클래스(note 값)
 * @input     note ∈ {CMD_REJECTED_INVALID, CMD_REJECTED_EXTRA}
 * @procedure 1) 각 note 로 호출한다.
 * @expected  out.command_note == note
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, CommandNoteIsCopiedFromCommandResult)
{
    const ecl_validated_t in = ValidValidated();
    const ecl_decision_t confirmed{};
    for (const ecl_reason_t note : {ECL_REASON_CMD_REJECTED_INVALID, ECL_REASON_CMD_REJECTED_EXTRA})
    {
        ecl_command_result_t cmd{};
        cmd.note = note;
        ecl_decision_t out{};
        ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
        EXPECT_EQ(out.command_note, note);
    }
}

/**
 * @test      UT-rule-011
 * @brief     [목적] 유효한 충돌 확정이면 좌/우를 RELEASE 로 하고 규칙 2 가 이유이다.
 * @satisfies REQ-S-001, REQ-S-005, REQ-F-012
 * @technique MC/DC(규칙 2 의 valid, value 독립 영향)
 * @input     crash={1,2}, 확정 좌/우 LOCK
 * @procedure 1) 호출한다.
 * @expected  좌/우 RELEASE, priority CRASH, reason CRASH_CONFIRMED
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, CrashConfirmedReleasesBothDoors)
{
    ecl_validated_t in = ValidValidated();
    in.crash_status.value = ECL_CRASH_CONFIRMED;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_CRASH);
    EXPECT_EQ(out.reason_code, ECL_REASON_CRASH_CONFIRMED);
}

/**
 * @test      UT-rule-012
 * @brief     [목적] 충돌이 NONE 이거나 값이 2 인데 INVALID 이면 규칙 2 는 적용되지 않는다.
 * @satisfies REQ-S-001
 * @technique MC/DC(규칙 2 의 valid, value 독립 영향)
 * @input     {valid=1,value=0}, {valid=0,value=2}
 * @procedure 1) 각각 호출한다.
 * @expected  확정 출력 LOCK 유지, priority HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, CrashNotConfirmedOrInvalidDoesNotApplyRuleTwo)
{
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ecl_validated_t none = ValidValidated();
    none.crash_status.value = ECL_CRASH_NONE;
    ASSERT_EQ(ecl_rule_decide(&none, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    ecl_validated_t invalid = ValidValidated();
    invalid.crash_status.valid = 0;
    invalid.crash_status.value = ECL_CRASH_CONFIRMED;
    invalid.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&invalid, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.reason_code, ECL_REASON_INPUT_INVALID);
}

/**
 * @test      UT-rule-013
 * @brief     [목적] 좌 접근위험만 TRUE 이면 좌만 LOCK 이고 우는 확정 출력을 유지한다.
 * @satisfies REQ-S-002, REQ-S-006, REQ-S-007, REQ-F-017
 * @technique MC/DC(규칙 3 좌 조건의 독립 영향) + 도어 독립
 * @input     rear_left_risk={1,1}, 확정 좌/우 RELEASE
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, 우 RELEASE, priority APPROACH, reason APPROACH_RISK_HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, LeftApproachRiskLocksOnlyLeftDoor)
{
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_APPROACH);
    EXPECT_EQ(out.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      UT-rule-014
 * @brief     [목적] 우 접근위험만 TRUE 이면 우만 LOCK 이고 좌는 확정 출력을 유지한다.
 * @satisfies REQ-S-002, REQ-S-006, REQ-F-017
 * @technique MC/DC(규칙 3 우 조건의 독립 영향) + 도어 독립
 * @input     rear_right_risk={1,1}, 확정 좌 LOCK
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK(유지), 우 LOCK, priority APPROACH
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, RightApproachRiskLocksOnlyRightDoor)
{
    ecl_validated_t in = ValidValidated();
    in.rear_right_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_APPROACH);
}

/**
 * @test      UT-rule-015
 * @brief     [목적] 접근위험이 INVALID(값 1)이거나 FALSE 이면 LOCK 하지 않는다.
 * @satisfies REQ-S-002, REQ-F-010
 * @technique MC/DC(규칙 3 의 valid, value 독립 영향)
 * @input     {valid=0,value=1}, {valid=1,value=0} 좌/우
 * @procedure 1) 호출한다.
 * @expected  확정 출력 RELEASE 유지, priority HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, InvalidOrFalseApproachRiskDoesNotLock)
{
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.valid = 0;
    in.rear_left_risk.value = 1;
    in.rear_right_risk.valid = 0;
    in.rear_right_risk.value = 1;
    in.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    const ecl_validated_t falseRisk = ValidValidated();
    ASSERT_EQ(ecl_rule_decide(&falseRisk, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
}

/**
 * @test      UT-rule-016
 * @brief     [목적] sensor_fault 는 충돌 확정·접근위험보다 순위가 높아 확정 출력을 유지한다.
 * @satisfies REQ-F-012, REQ-F-020, REQ-S-004
 * @technique 우선순위 규칙(순위 1 > 2, 3)
 * @input     sensor_fault, crash 확정, 좌 접근위험 모두 TRUE, 확정 좌 LOCK, 우 RELEASE
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, 우 RELEASE, priority SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, SensorFaultOutranksCrashAndApproach)
{
    ecl_validated_t in = ValidValidated();
    in.sensor_fault.value = 1;
    in.crash_status.value = ECL_CRASH_CONFIRMED;
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_SENSOR_FAULT);
    EXPECT_EQ(out.reason_code, ECL_REASON_SENSOR_FAULT);
}

/**
 * @test      UT-rule-017
 * @brief     [목적] 충돌 확정은 접근위험(LOCK)보다 순위가 높아 좌/우 모두 RELEASE 이다.
 * @satisfies REQ-F-012, REQ-F-020, REQ-S-001
 * @technique 우선순위 규칙(순위 2 > 3)
 * @input     crash 확정 + 좌 접근위험, 확정 좌/우 LOCK
 * @procedure 1) 호출한다.
 * @expected  좌/우 RELEASE, priority CRASH
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, CrashOutranksApproachRisk)
{
    ecl_validated_t in = ValidValidated();
    in.crash_status.value = ECL_CRASH_CONFIRMED;
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_CRASH);
}

/**
 * @test      UT-rule-018
 * @brief     [목적] 충돌 PENDING 은 출력을 바꾸지 않고 이유 CRASH_PENDING 만 기록한다. 입력 오류 이유가 우선한다.
 * @satisfies REQ-F-029
 * @technique MC/DC(HOLD 이유의 all_valid, PENDING 독립 영향)
 * @input     crash={1,1} 정상, 그리고 crash={1,1}+stale
 * @procedure 1) 각각 호출한다.
 * @expected  정상: priority HOLD, reason CRASH_PENDING, 출력 유지 / stale: reason INPUT_STALE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, PendingCrashOnlyRecordsPendingReason)
{
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t pending = ValidValidated();
    pending.crash_status.value = ECL_CRASH_PENDING;
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&pending, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.reason_code, ECL_REASON_CRASH_PENDING);
    pending.all_valid = 0;
    pending.stale = 1;
    ASSERT_EQ(ecl_rule_decide(&pending, &cmd, &confirmed, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_INPUT_STALE);
}

}  // namespace
