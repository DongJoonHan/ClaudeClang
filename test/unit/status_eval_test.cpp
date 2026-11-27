/**
 * @file    status_eval_test.cpp
 * @brief   status_eval 단위 테스트 (MC/DC 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>
#include "ecl/status_eval.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      UT-status-001
 * @brief     [목적] in 이 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-003
 * @technique MC/DC(조건 in == NULL 의 독립 영향)
 * @input     in=nullptr
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
TEST(StatusEvalTest, ReturnsErrNullArgWhenInIsNull)
{
    ecl_status_result_t out{};
    EXPECT_EQ(ecl_status_evaluate(nullptr, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-status-002
 * @brief     [목적] out 이 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-003
 * @technique MC/DC(조건 out == NULL 의 독립 영향)
 * @input     out=nullptr
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
TEST(StatusEvalTest, ReturnsErrNullArgWhenOutIsNull)
{
    const ecl_validated_t in = ValidValidated();
    EXPECT_EQ(ecl_status_evaluate(&in, nullptr), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-status-003
 * @brief     [목적] 모든 입력이 유효하고 점화가 켜져 있으면 ACTIVE 이고 경고가 없다.
 * @satisfies REQ-F-014
 * @technique 동치 클래스(정상)
 * @input     ValidValidated()
 * @procedure 1) 호출한다.
 * @expected  state == ACTIVE, warnings == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, ActiveWhenAllValidAndIgnitionOn)
{
    const ecl_validated_t in = ValidValidated();
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&in, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.warnings, 0U);
}

/**
 * @test      UT-status-004
 * @brief     [목적] 점화가 꺼져 있으면(유효) OFF 이다.
 * @satisfies REQ-F-014, REQ-F-027
 * @technique MC/DC(조건 ignition_on.value == 0 의 독립 영향)
 * @input     ignition_on=0
 * @procedure 1) 호출한다.
 * @expected  state == OFF, warnings == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, OffWhenIgnitionIsOff)
{
    ecl_validated_t in = ValidValidated();
    in.ignition_on.value = 0;
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&in, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_OFF);
    EXPECT_EQ(out.warnings, 0U);
}

/**
 * @test      UT-status-005
 * @brief     [목적] sensor_fault 가 유효한 TRUE 이면 FAULT 이고 SENSOR_FAULT 경고가 있다.
 * @satisfies REQ-S-009, REQ-S-004
 * @technique MC/DC(is_true 의 valid, value 독립 영향)
 * @input     sensor_fault={valid=1,value=1}
 * @procedure 1) 호출한다.
 * @expected  state == FAULT, warnings == SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, FaultWhenSensorFaultIsValidTrue)
{
    ecl_validated_t in = ValidValidated();
    in.sensor_fault.value = 1;
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&in, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
    EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT);
}

/**
 * @test      UT-status-006
 * @brief     [목적] sensor_fault 가 INVALID(값 1)이거나 FALSE 이면 FAULT 가 아니다.
 * @satisfies REQ-S-009
 * @technique MC/DC(is_true 의 valid, value 독립 영향)
 * @input     {valid=0,value=1}, {valid=1,value=0}
 * @procedure 1) 각각 호출한다.
 * @expected  첫째: DEGRADED(all_valid=0 가정), 둘째: ACTIVE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, InvalidOrFalseSensorFaultDoesNotCauseFault)
{
    ecl_validated_t invalid = ValidValidated();
    invalid.sensor_fault.valid = 0;
    invalid.sensor_fault.value = 1;
    invalid.all_valid = 0;
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&invalid, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    const ecl_validated_t falseFault = ValidValidated();
    ASSERT_EQ(ecl_status_evaluate(&falseFault, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
}

/**
 * @test      UT-status-007
 * @brief     [목적] 필수 입력이 INVALID 이면 DEGRADED 이고 INPUT_INVALID 경고이다.
 * @satisfies REQ-F-014, REQ-S-003
 * @technique MC/DC(조건 all_valid == 0, stale != 0 의 독립 영향)
 * @input     all_valid=0, stale=0
 * @procedure 1) 호출한다.
 * @expected  state == DEGRADED, warnings == INPUT_INVALID
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, DegradedWithInvalidWarningWhenNotAllValid)
{
    ecl_validated_t in = ValidValidated();
    in.all_valid = 0;
    in.stale = 0;
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&in, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
}

/**
 * @test      UT-status-008
 * @brief     [목적] 입력이 stale 이면 DEGRADED 이고 INPUT_STALE 경고이다.
 * @satisfies REQ-S-003
 * @technique MC/DC(조건 stale != 0 의 독립 영향)
 * @input     all_valid=0, stale=1
 * @procedure 1) 호출한다.
 * @expected  state == DEGRADED, warnings == INPUT_STALE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, DegradedWithStaleWarningWhenStale)
{
    ecl_validated_t in = ValidValidated();
    in.all_valid = 0;
    in.stale = 1;
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&in, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_STALE);
}

/**
 * @test      UT-status-009
 * @brief     [목적] FAULT 와 stale 이 겹치면 상태는 FAULT 이고 경고는 두 비트가 모두 켜진다.
 * @satisfies REQ-S-009, REQ-S-003
 * @technique 조합(FAULT + 입력 오류)
 * @input     sensor_fault 유효 TRUE, all_valid=0, stale=1
 * @procedure 1) 호출한다.
 * @expected  state == FAULT, warnings == SENSOR_FAULT|INPUT_STALE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(StatusEvalTest, FaultKeepsStateAndAddsInputWarning)
{
    ecl_validated_t in = ValidValidated();
    in.sensor_fault.value = 1;
    in.all_valid = 0;
    in.stale = 1;
    ecl_status_result_t out{};
    ASSERT_EQ(ecl_status_evaluate(&in, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
    EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT | ECL_WARN_INPUT_STALE);
}

}  // namespace
