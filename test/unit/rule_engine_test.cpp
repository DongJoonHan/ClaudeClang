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
    ecl_rule_memory_t memory{};
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    EXPECT_EQ(ecl_rule_decide(nullptr, &cmd, &confirmed, &memory, &out), ECL_ERR_NULL_ARG);
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
    ecl_rule_memory_t memory{};
    const ecl_validated_t in = ValidValidated();
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    EXPECT_EQ(ecl_rule_decide(&in, nullptr, &confirmed, &memory, &out), ECL_ERR_NULL_ARG);
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
    ecl_rule_memory_t memory{};
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    ecl_decision_t out{};
    EXPECT_EQ(ecl_rule_decide(&in, &cmd, nullptr, &memory, &out), ECL_ERR_NULL_ARG);
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
    ecl_rule_memory_t memory{};
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    EXPECT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, nullptr), ECL_ERR_NULL_ARG);
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
    ecl_rule_memory_t memory{};
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_RELEASE;
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.all_valid = 0;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    confirmed.left = ECL_OUT_LOCK;
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.all_valid = 0;
    in.stale = 1;
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.sensor_fault.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    confirmed.left = ECL_OUT_LOCK;
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t invalid = ValidValidated();
    invalid.sensor_fault.valid = 0;
    invalid.sensor_fault.value = 1;
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&invalid, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    const ecl_validated_t falseFault = ValidValidated();
    ASSERT_EQ(ecl_rule_decide(&falseFault, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    const ecl_validated_t in = ValidValidated();
    const ecl_decision_t confirmed{};
    for (const ecl_reason_t note : {ECL_REASON_CMD_REJECTED_INVALID, ECL_REASON_CMD_REJECTED_EXTRA})
    {
        ecl_command_result_t cmd{};
        cmd.note = note;
        ecl_decision_t out{};
        ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.crash_status.value = ECL_CRASH_CONFIRMED;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ecl_validated_t none = ValidValidated();
    none.crash_status.value = ECL_CRASH_NONE;
    ASSERT_EQ(ecl_rule_decide(&none, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    ecl_validated_t invalid = ValidValidated();
    invalid.crash_status.valid = 0;
    invalid.crash_status.value = ECL_CRASH_CONFIRMED;
    invalid.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&invalid, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.rear_right_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.valid = 0;
    in.rear_left_risk.value = 1;
    in.rear_right_risk.valid = 0;
    in.rear_right_risk.value = 1;
    in.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    const ecl_validated_t falseRisk = ValidValidated();
    ASSERT_EQ(ecl_rule_decide(&falseRisk, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.sensor_fault.value = 1;
    in.crash_status.value = ECL_CRASH_CONFIRMED;
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    ecl_validated_t in = ValidValidated();
    in.crash_status.value = ECL_CRASH_CONFIRMED;
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
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
    ecl_rule_memory_t memory{};
    const ecl_command_result_t cmd{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t pending = ValidValidated();
    pending.crash_status.value = ECL_CRASH_PENDING;
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&pending, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.reason_code, ECL_REASON_CRASH_PENDING);
    pending.all_valid = 0;
    pending.stale = 1;
    ASSERT_EQ(ecl_rule_decide(&pending, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_INPUT_STALE);
}

/**
 * @test      UT-rule-019
 * @brief     [목적] 화재·과온·성인 탑승 각각이 유효한 TRUE 이면 좌/우를 RELEASE 하고 입력별 이유를 기록한다.
 * @satisfies REQ-F-025, REQ-F-037
 * @technique MC/DC(규칙 4 의 세 입력 독립 영향)
 * @input     fire / overtemp / adult 각각 {1,1}, 확정 좌/우 LOCK
 * @procedure 1) 각 입력으로 호출한다.
 * @expected  좌/우 RELEASE, priority EMERGENCY_RELEASE, reason FIRE/OVERTEMP/ADULT_PRESENT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, EmergencyInputsReleaseBothDoors)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    struct Case
    {
        ecl_val_int_t ecl_validated_t::*field;
        ecl_reason_t reason;
    };
    const Case cases[] = {{&ecl_validated_t::fire, ECL_REASON_FIRE},
                          {&ecl_validated_t::overtemp, ECL_REASON_OVERTEMP},
                          {&ecl_validated_t::adult_present, ECL_REASON_ADULT_PRESENT}};
    for (const Case& c : cases)
    {
        ecl_validated_t in = ValidValidated();
        (in.*(c.field)).value = 1;
        ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
        EXPECT_EQ(out.left, ECL_OUT_RELEASE);
        EXPECT_EQ(out.right, ECL_OUT_RELEASE);
        EXPECT_EQ(out.priority_reason, ECL_PRIO_EMERGENCY_RELEASE);
        EXPECT_EQ(out.reason_code, c.reason);
    }
}

/**
 * @test      UT-rule-020
 * @brief     [목적] 여러 신호가 동시이면 FIRE > OVERTEMP > ADULT_PRESENT 이고, INVALID 신호는 다른 유효 신호의 적용을 막지 않는다.
 * @satisfies REQ-F-025
 * @technique 우선순위 + MC/DC(valid 독립 영향)
 * @input     fire+overtemp+adult, overtemp+adult, fire INVALID + adult TRUE, fire INVALID(값 1) 단독
 * @procedure 1) 각각 호출한다.
 * @expected  FIRE, OVERTEMP, ADULT_PRESENT(RELEASE), HOLD(확정 출력 유지)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, EmergencyReasonPrecedenceAndInvalidSignals)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    ecl_validated_t in = ValidValidated();
    in.fire.value = 1;
    in.overtemp.value = 1;
    in.adult_present.value = 1;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_FIRE);
    in.fire.value = 0;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_OVERTEMP);
    in.fire.valid = 0;
    in.fire.value = 1;
    in.overtemp.value = 0;
    in.all_valid = 0;
    confirmed.left = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_ADULT_PRESENT);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    in.adult_present.value = 0;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
}

/**
 * @test      UT-rule-021
 * @brief     [목적] 유효한 ignition_on FALSE 이면 좌/우 RELEASE, 켜짐이거나 INVALID 이면 효과 없음.
 * @satisfies REQ-F-027
 * @technique MC/DC(규칙 5 의 valid, value 독립 영향)
 * @input     {1,0}, {1,1}, {0,0}
 * @procedure 1) 각각 호출한다.
 * @expected  {1,0}: RELEASE, IGNITION_OFF / 나머지: HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, IgnitionOffReleasesBothDoorsOnlyWhenValidFalse)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ecl_validated_t off = ValidValidated();
    off.ignition_on.value = 0;
    ASSERT_EQ(ecl_rule_decide(&off, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_IGNITION_OFF);
    EXPECT_EQ(out.reason_code, ECL_REASON_IGNITION_OFF);
    const ecl_validated_t on = ValidValidated();
    ASSERT_EQ(ecl_rule_decide(&on, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    ecl_validated_t invalid = ValidValidated();
    invalid.ignition_on.valid = 0;
    invalid.ignition_on.value = 0;
    invalid.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&invalid, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
}

/**
 * @test      UT-rule-022
 * @brief     [목적] ISOFIX 가 유효한 TRUE 인 쪽 도어만 LOCK 하고 반대쪽 출력은 유지하며, 이유는 연결된 쪽을 나타낸다.
 * @satisfies REQ-F-026, REQ-F-017
 * @technique MC/DC(규칙 6 좌·우 독립 영향) + 도어 독립
 * @input     left only, right only, both, INVALID
 * @procedure 1) 각각 호출한다.
 * @expected  left: 좌 LOCK, 우 유지, ISOFIX_LEFT / right: 우 LOCK, 좌 유지, ISOFIX_RIGHT / both: 좌/우 LOCK, ISOFIX_LEFT / INVALID: HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, IsofixLocksOnlyTheConnectedSide)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    ecl_validated_t in = ValidValidated();
    in.isofix_left.value = 1;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.reason_code, ECL_REASON_ISOFIX_LEFT);
    in.isofix_left.value = 0;
    in.isofix_right.value = 1;
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_RELEASE;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.reason_code, ECL_REASON_ISOFIX_RIGHT);
    in.isofix_left.value = 1;
    confirmed.left = ECL_OUT_RELEASE;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.reason_code, ECL_REASON_ISOFIX_LEFT);
    in.isofix_left.valid = 0;
    in.isofix_right.valid = 0;
    in.all_valid = 0;
    confirmed.left = ECL_OUT_RELEASE;
    confirmed.right = ECL_OUT_RELEASE;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
}

/**
 * @test      UT-rule-023
 * @brief     [목적] 유효한 속도가 3.0 km/h 이상이면 좌/우 LOCK, 미만이거나 INVALID 이면 효과 없음.
 * @satisfies REQ-F-022
 * @technique 경계값 분석(2.9, 3.0, 300) + MC/DC(valid, 값 독립 영향)
 * @input     speed ∈ {2.9, 3.0, 300.0, INVALID(값 50)}, 확정 좌/우 RELEASE
 * @procedure 1) 각 속도로 호출한다.
 * @expected  3.0, 300.0: LOCK, AUTO_LOCK_MOVING / 2.9, INVALID: HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, AutoLockStartsAtThreeKilometersPerHour)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    for (const double speed : {3.0, 300.0})
    {
        ecl_validated_t in = ValidValidated();
        in.speed_kph.value = speed;
        ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
        EXPECT_EQ(out.left, ECL_OUT_LOCK);
        EXPECT_EQ(out.right, ECL_OUT_LOCK);
        EXPECT_EQ(out.priority_reason, ECL_PRIO_AUTO_LOCK);
        EXPECT_EQ(out.reason_code, ECL_REASON_AUTO_LOCK_MOVING);
    }
    ecl_validated_t slow = ValidValidated();
    slow.speed_kph.value = 2.9;
    ASSERT_EQ(ecl_rule_decide(&slow, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    ecl_validated_t invalid = ValidValidated();
    invalid.speed_kph.valid = 0;
    invalid.speed_kph.value = 50.0;
    invalid.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&invalid, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
}

/**
 * @test      UT-rule-024
 * @brief     [목적] 정지·정상 입력에서 채택된 명령은 side 가 가리키는 도어에만 action 을 적용한다.
 * @satisfies REQ-F-021, REQ-F-034
 * @technique 동치 클래스 전수(side 3 x action 2)
 * @input     side ∈ {LEFT, RIGHT, ALL}, action ∈ {LOCK, UNLOCK}, 확정 좌 RELEASE 우 LOCK 또는 반대
 * @procedure 1) 조합마다 호출한다.
 * @expected  대상 도어만 action 결과, 반대쪽은 확정 출력 유지, priority DRIVER_CMD, reason DRIVER_CMD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, DriverCommandAppliesActionToTargetSideOnly)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_validated_t in = ValidValidated();
    for (const ecl_side_t side : {ECL_SIDE_LEFT, ECL_SIDE_RIGHT, ECL_SIDE_ALL})
    {
        for (const ecl_action_t action : {ECL_ACTION_LOCK, ECL_ACTION_UNLOCK})
        {
            const ecl_output_t wanted = (action == ECL_ACTION_LOCK) ? ECL_OUT_LOCK : ECL_OUT_RELEASE;
            const ecl_output_t other = (action == ECL_ACTION_LOCK) ? ECL_OUT_RELEASE : ECL_OUT_LOCK;
            confirmed.left = other;
            confirmed.right = other;
            const ecl_command_result_t cmd = AcceptedCommand(side, action, 1.0);
            ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
            EXPECT_EQ(out.left, (side == ECL_SIDE_RIGHT) ? other : wanted);
            EXPECT_EQ(out.right, (side == ECL_SIDE_LEFT) ? other : wanted);
            EXPECT_EQ(out.priority_reason, ECL_PRIO_DRIVER_CMD);
            EXPECT_EQ(out.reason_code, ECL_REASON_DRIVER_CMD);
        }
    }
}

/**
 * @test      UT-rule-025
 * @brief     [목적] 명령이 채택되지 않았거나 입력이 일부 INVALID 이거나 이동 중이면 규칙 8 은 적용되지 않는다.
 * @satisfies REQ-F-021, REQ-F-035
 * @technique MC/DC(규칙 8 의 accepted, all_valid, 정지 독립 영향)
 * @input     미채택 / all_valid=0 / 속도 5(이동) / 속도 INVALID(all_valid=1 로 구성)
 * @procedure 1) 각각 호출한다.
 * @expected  모두 HOLD, 확정 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, DriverCommandRequiresAcceptedAllValidStationary)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    confirmed.left = ECL_OUT_LOCK;
    const ecl_command_result_t unlock = AcceptedCommand(ECL_SIDE_ALL, ECL_ACTION_UNLOCK, 1.0);
    ecl_command_result_t notAccepted = unlock;
    notAccepted.accepted = 0;
    const ecl_validated_t good = ValidValidated();
    ASSERT_EQ(ecl_rule_decide(&good, &notAccepted, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    ecl_validated_t notAllValid = ValidValidated();
    notAllValid.all_valid = 0;
    ASSERT_EQ(ecl_rule_decide(&notAllValid, &unlock, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    ecl_validated_t moving = ValidValidated();
    moving.speed_kph.value = 5.0;
    moving.speed_kph.valid = 1;
    moving.stale = 0;
    ASSERT_EQ(ecl_rule_decide(&moving, &unlock, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_AUTO_LOCK);
    ecl_validated_t noSpeed = ValidValidated();
    noSpeed.speed_kph.valid = 0;
    ASSERT_EQ(ecl_rule_decide(&noSpeed, &unlock, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_HOLD);
}

/**
 * @test      UT-rule-026
 * @brief     [목적] 이동 중에 채택된 명령은 command_note 가 CMD_REJECTED_MOVING 이고 그 외에는 접수 결과의 note 를 쓴다.
 * @satisfies REQ-F-035
 * @technique MC/DC(accepted, 이동 독립 영향)
 * @input     채택+이동, 채택+정지(note EXTRA), 미채택+이동(note INVALID)
 * @procedure 1) 각각 호출한다.
 * @expected  MOVING / EXTRA / INVALID
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, MovingAcceptedCommandIsNotedAsRejected)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t moving = ValidValidated();
    moving.speed_kph.value = 10.0;
    ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 1.0);
    ASSERT_EQ(ecl_rule_decide(&moving, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.command_note, ECL_REASON_CMD_REJECTED_MOVING);
    const ecl_validated_t still = ValidValidated();
    cmd.note = ECL_REASON_CMD_REJECTED_EXTRA;
    ASSERT_EQ(ecl_rule_decide(&still, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.command_note, ECL_REASON_CMD_REJECTED_EXTRA);
    ecl_command_result_t rejected{};
    rejected.note = ECL_REASON_CMD_REJECTED_INVALID;
    ASSERT_EQ(ecl_rule_decide(&moving, &rejected, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.command_note, ECL_REASON_CMD_REJECTED_INVALID);
}

/**
 * @test      UT-rule-027
 * @brief     [목적] 접근위험이 TRUE 인 도어의 해제 명령은 억제되고(LOCK, APPROACH_RISK_HOLD) 첫 요청 시각이 기억된다.
 * @satisfies REQ-F-023, REQ-S-006
 * @technique 동치 클래스(첫 억제)
 * @input     좌 접근위험, 좌 해제 명령 t=100.0
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, reason APPROACH_RISK_HOLD, memory.left.has_first == 1, first_time_s == 100.0, override == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, ApproachRiskOutranksDriverUnlockAndRecordsFirstRequest)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    const ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 100.0);
    confirmed.left = ECL_OUT_RELEASE;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_APPROACH);
    EXPECT_EQ(out.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
    EXPECT_EQ(memory.left.has_first, 1);
    EXPECT_DOUBLE_EQ(memory.left.first_time_s, 100.0);
    EXPECT_EQ(memory.left.override_active, 0);
    EXPECT_EQ(memory.right.has_first, 0);
}

/**
 * @test      UT-rule-028
 * @brief     [목적] 첫 억제 후 10초 이하에 같은 도어 해제가 다시 오면 override 가 되어 RELEASE, 이유 OVERRIDE 이다.
 * @satisfies REQ-F-023, REQ-F-036
 * @technique 경계값 분석(+5.0 s, +10.0 s)
 * @input     t=100.0 첫 요청, 이후 105.0 또는 110.0
 * @procedure 1) 첫 요청 2) 두 번째 요청
 * @expected  두 번째: 좌 RELEASE, priority DRIVER_CMD, reason OVERRIDE, memory.left.override == 1
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, SecondUnlockWithinWindowActivatesOverride)
{
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    for (const double second : {105.0, 110.0})
    {
        ecl_rule_memory_t local{};
        ecl_validated_t in = ValidValidated();
        in.rear_left_risk.value = 1;
        ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 100.0);
        ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &local, &out), ECL_OK);
        cmd.request_time_s = second;
        confirmed.left = ECL_OUT_LOCK;
        ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &local, &out), ECL_OK);
        EXPECT_EQ(out.left, ECL_OUT_RELEASE);
        EXPECT_EQ(out.priority_reason, ECL_PRIO_DRIVER_CMD);
        EXPECT_EQ(out.reason_code, ECL_REASON_OVERRIDE);
        EXPECT_EQ(local.left.override_active, 1);
    }
}

/**
 * @test      UT-rule-029
 * @brief     [목적] 10초를 넘긴 요청은 새 첫 요청이고, 같은 시각이거나 과거의 요청은 override 하지 않는다.
 * @satisfies REQ-F-023
 * @technique 경계값 분석(+10.5 s, +0.0 s, -1.0 s)
 * @input     t=100.0 첫 요청 후 110.5 / 100.0 / 99.0
 * @procedure 1) 첫 요청 2) 두 번째 요청
 * @expected  override == 0, 110.5 이면 first_time_s 갱신, 좌 LOCK 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, UnlockAfterWindowStartsNewFirstRequestAndSameTimeDoesNothing)
{
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    for (const double second : {110.5, 100.0, 99.0})
    {
        ecl_rule_memory_t local{};
        ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 100.0);
        ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &local, &out), ECL_OK);
        cmd.request_time_s = second;
        ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &local, &out), ECL_OK);
        EXPECT_EQ(local.left.override_active, 0);
        EXPECT_EQ(out.left, ECL_OUT_LOCK);
        EXPECT_EQ(out.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
        EXPECT_DOUBLE_EQ(local.left.first_time_s, (second > 110.0) ? 110.5 : 100.0);
    }
    ecl_rule_memory_t chain{};
    ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 100.0);
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &chain, &out), ECL_OK);
    cmd.request_time_s = 120.0;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &chain, &out), ECL_OK);
    cmd.request_time_s = 125.0;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &chain, &out), ECL_OK);
    EXPECT_EQ(chain.left.override_active, 1);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
}

/**
 * @test      UT-rule-030
 * @brief     [목적] 접근위험이 유효한 FALSE 가 되면 기억과 override 가 지워지고, 다시 TRUE 가 되면 규칙 3 이 다시 적용된다.
 * @satisfies REQ-F-023, REQ-F-036
 * @technique 상태 전이
 * @input     override 후 risk FALSE, 이후 risk TRUE
 * @procedure 1) override 2) risk FALSE 3) risk TRUE
 * @expected  FALSE: memory 0 / TRUE: 좌 LOCK, APPROACH_RISK_HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, RiskFalseClearsMemoryAndRiskReappearsSuppressesAgain)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 100.0);
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
    cmd.request_time_s = 101.0;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
    ASSERT_EQ(memory.left.override_active, 1);
    in.rear_left_risk.value = 0;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(memory.left.override_active, 0);
    EXPECT_EQ(memory.left.has_first, 0);
    EXPECT_DOUBLE_EQ(memory.left.first_time_s, 0.0);
    in.rear_left_risk.value = 1;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      UT-rule-031
 * @brief     [목적] 잠금 명령, INVALID 접근위험, 다른 쪽 도어를 가리키는 명령은 해당 도어의 기억을 바꾸지 않는다.
 * @satisfies REQ-F-023
 * @technique MC/DC(update_door 의 risk TRUE, requested 독립 영향; 명령 대상 독립 영향)
 * @input     lock 명령 / risk INVALID + unlock / 우 해제 명령(좌 risk TRUE)
 * @procedure 1) 각각 호출한다.
 * @expected  좌 기억 변화 없음 (has_first == 0)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, MemoryIgnoresNonRequestsInvalidRiskAndOtherDoor)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    const ecl_command_result_t lock = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_LOCK, 100.0);
    ASSERT_EQ(ecl_rule_decide(&in, &lock, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(memory.left.has_first, 0);
    ecl_validated_t invalid = ValidValidated();
    invalid.rear_left_risk.valid = 0;
    invalid.rear_left_risk.value = 1;
    invalid.all_valid = 0;
    const ecl_command_result_t unlockLeft = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 100.0);
    ASSERT_EQ(ecl_rule_decide(&invalid, &unlockLeft, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(memory.left.has_first, 0);
    const ecl_command_result_t unlockRight = AcceptedCommand(ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, 100.0);
    ASSERT_EQ(ecl_rule_decide(&in, &unlockRight, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(memory.left.has_first, 0);
    EXPECT_EQ(memory.right.has_first, 0);
}

/**
 * @test      UT-rule-032
 * @brief     [목적] side=ALL 해제는 접근위험 양쪽 도어를 함께 override 하고, 이유는 override 상태의 대상 도어가 있을 때만 OVERRIDE 이다.
 * @satisfies REQ-F-023, REQ-F-036, REQ-F-037
 * @technique MC/DC(driver_reason 의 action, 좌·우 override 독립 영향)
 * @input     양쪽 risk TRUE, ALL 해제 두 번 / override 좌만이고 우 대상 / 잠금 명령
 * @procedure 1) 각각 호출한다.
 * @expected  OVERRIDE 또는 DRIVER_CMD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, SideAllUnlockOverridesBothDoorsAndReasonFollowsOverriddenDoor)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    in.rear_right_risk.value = 1;
    ecl_command_result_t cmd = AcceptedCommand(ECL_SIDE_ALL, ECL_ACTION_UNLOCK, 100.0);
    confirmed.left = ECL_OUT_LOCK;
    confirmed.right = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_LOCK);
    cmd.request_time_s = 103.0;
    ASSERT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.reason_code, ECL_REASON_OVERRIDE);
    ecl_validated_t leftRisk = ValidValidated();
    leftRisk.rear_left_risk.value = 1;
    ecl_validated_t rightRisk = ValidValidated();
    rightRisk.rear_right_risk.value = 1;
    ecl_rule_memory_t leftOnly{};
    leftOnly.left.override_active = 1;
    const ecl_command_result_t rightUnlock = AcceptedCommand(ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, 1.0);
    ASSERT_EQ(ecl_rule_decide(&leftRisk, &rightUnlock, &confirmed, &leftOnly, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_DRIVER_CMD);
    const ecl_command_result_t leftUnlock = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, 1.0);
    ASSERT_EQ(ecl_rule_decide(&leftRisk, &leftUnlock, &confirmed, &leftOnly, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_OVERRIDE);
    ecl_rule_memory_t rightOnly{};
    rightOnly.right.override_active = 1;
    ASSERT_EQ(ecl_rule_decide(&rightRisk, &rightUnlock, &confirmed, &rightOnly, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_OVERRIDE);
    ASSERT_EQ(ecl_rule_decide(&rightRisk, &leftUnlock, &confirmed, &rightOnly, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_DRIVER_CMD);
    const ecl_command_result_t lockLeft = AcceptedCommand(ECL_SIDE_LEFT, ECL_ACTION_LOCK, 1.0);
    ASSERT_EQ(ecl_rule_decide(&leftRisk, &lockLeft, &confirmed, &leftOnly, &out), ECL_OK);
    EXPECT_EQ(out.reason_code, ECL_REASON_DRIVER_CMD);
}

/**
 * @test      UT-rule-033
 * @brief     [목적] override 는 규칙 3 만 해제하며 이동 중에는 규칙 7 이 다시 LOCK 한다.
 * @satisfies REQ-F-036, REQ-F-022
 * @technique WoW 검증(override 후 이동 시작)
 * @input     좌 override 중 + 속도 5
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, priority AUTO_LOCK
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, OverrideDoesNotDisableHigherOrLowerRankRules)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    in.speed_kph.value = 5.0;
    memory.left.override_active = 1;
    confirmed.left = ECL_OUT_RELEASE;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_AUTO_LOCK);
}

/**
 * @test      UT-rule-034
 * @brief     [목적] 좌는 접근위험(규칙 3), 우는 화재 해제(규칙 4)처럼 도어마다 다른 규칙이 결정할 수 있고 결정 이유는 최상위 규칙이다.
 * @satisfies REQ-F-017, REQ-F-012
 * @technique 도어 독립 + 우선순위
 * @input     좌 접근위험 + 화재, 확정 좌 RELEASE, 우 LOCK
 * @procedure 1) 호출한다.
 * @expected  좌 LOCK, 우 RELEASE, priority APPROACH
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(RuleEngineTest, DoorsCanBeDecidedByDifferentRules)
{
    ecl_rule_memory_t memory{};
    ecl_decision_t confirmed{};
    ecl_decision_t out{};
    const ecl_command_result_t none{};
    ecl_validated_t in = ValidValidated();
    in.rear_left_risk.value = 1;
    in.fire.value = 1;
    confirmed.right = ECL_OUT_LOCK;
    ASSERT_EQ(ecl_rule_decide(&in, &none, &confirmed, &memory, &out), ECL_OK);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.priority_reason, ECL_PRIO_APPROACH);
    EXPECT_EQ(out.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      UT-rule-035
 * @brief     [목적] memory 가 널이면 ECL_ERR_NULL_ARG 를 반환하고 out 을 바꾸지 않는다.
 * @satisfies DD-M-004
 * @technique MC/DC(조건 memory == NULL 의 독립 영향)
 * @input     memory=nullptr
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
TEST(RuleEngineTest, ReturnsErrNullArgWhenMemoryIsNull)
{
    const ecl_validated_t in = ValidValidated();
    const ecl_command_result_t cmd{};
    const ecl_decision_t confirmed{};
    ecl_decision_t out{};
    out.left = ECL_OUT_LOCK;
    EXPECT_EQ(ecl_rule_decide(&in, &cmd, &confirmed, nullptr, &out), ECL_ERR_NULL_ARG);
    EXPECT_EQ(out.left, ECL_OUT_LOCK);
}

}  // namespace
