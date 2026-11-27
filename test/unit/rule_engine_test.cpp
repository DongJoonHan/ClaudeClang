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

}  // namespace
