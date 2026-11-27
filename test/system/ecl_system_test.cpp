/**
 * @file    ecl_system_test.cpp
 * @brief   시스템 테스트: controller 인터페이스(ARCH-I-006)에서 요구사항을 실제 컴포넌트로 검증한다 (이터레이션 1 실행분).
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
 * @test      ST-F-001
 * @brief     [목적] 정상 입력(유효 클래스 대표값)이 ACTIVE 로 처리되고 이전 확정 출력이 유지됨을 확인한다.
 * @satisfies REQ-F-001, REQ-F-002, REQ-F-004, REQ-F-005, REQ-F-016, REQ-F-011
 * @technique 동치 분할
 * @input     {"vehicle_speed_kph": "10.0", "gear": "D", "crash_status": "NONE", "불리언 9종": "ignition_on=TRUE, 나머지 FALSE", "timestamp": "1.0 s, 평가 시각 1.01 s"}
 * @procedure 이전 확정 출력을 좌 LOCK, 우 RELEASE 로 만든다 / 정상 입력으로 1주기를 실행한다 / 상태와 출력을 관측한다
 * @expected  상태 ACTIVE, 경고 없음, 좌 LOCK, 우 RELEASE (규칙 없음 → HOLD, 이유 NONE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_001)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.warnings, 0U);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-002
 * @brief     [목적] 속도 하한 경계값 0.0 km/h 가 유효임을 확인한다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석
 * @input     {"vehicle_speed_kph": "0.0"}
 * @procedure 속도만 바꿔 1주기를 실행한다
 * @expected  상태 ACTIVE, 경고 없음
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_002)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(0.0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.warnings, 0U);
}

/**
 * @test      ST-F-003
 * @brief     [목적] 속도 상한 경계값 300.0 km/h 가 유효임을 확인한다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석
 * @input     {"vehicle_speed_kph": "300.0"}
 * @procedure 속도만 바꿔 1주기를 실행한다
 * @expected  상태 ACTIVE, 경고 없음
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_003)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(300.0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.warnings, 0U);
}

/**
 * @test      ST-F-004
 * @brief     [목적] 속도 하한 미만(-0.1) 는 INVALID 이며 제어 평가에 쓰이지 않음을 확인한다.
 * @satisfies REQ-F-001, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"vehicle_speed_kph": "-0.1"}
 * @procedure 속도만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_004)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(-0.1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-005
 * @brief     [목적] 속도 상한 초과(300.1) 는 INVALID 이며 제어 평가에 쓰이지 않음을 확인한다.
 * @satisfies REQ-F-001, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"vehicle_speed_kph": "300.1"}
 * @procedure 속도만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_005)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(300.1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-006
 * @brief     [목적] 속도 누락(누락) 는 INVALID 이며 제어 평가에 쓰이지 않음을 확인한다.
 * @satisfies REQ-F-001, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"vehicle_speed_kph": "누락"}
 * @procedure 속도만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_006)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = MissingNum();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-007
 * @brief     [목적] 속도 NaN(NaN) 는 INVALID 이며 제어 평가에 쓰이지 않음을 확인한다.
 * @satisfies REQ-F-001, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"vehicle_speed_kph": "NaN"}
 * @procedure 속도만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_007)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(__builtin_nan(""));
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-008
 * @brief     [목적] 기어 코드 0(P) 가 유효임을 확인한다.
 * @satisfies REQ-F-002
 * @technique 경계값 분석
 * @input     {"gear": "P"}
 * @procedure 기어만 바꿔 1주기를 실행한다
 * @expected  상태 ACTIVE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_008)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.gear = Int(0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
}

/**
 * @test      ST-F-009
 * @brief     [목적] 기어 코드 3(R) 가 유효임을 확인한다.
 * @satisfies REQ-F-002
 * @technique 경계값 분석
 * @input     {"gear": "R"}
 * @procedure 기어만 바꿔 1주기를 실행한다
 * @expected  상태 ACTIVE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_009)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.gear = Int(3);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
}

/**
 * @test      ST-F-010
 * @brief     [목적] 기어 범위 밖 는 INVALID 이다.
 * @satisfies REQ-F-002, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"gear": "미정 코드 4"}
 * @procedure 기어만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_010)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.gear = Int(4);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-011
 * @brief     [목적] 기어 누락 는 INVALID 이다.
 * @satisfies REQ-F-002, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"gear": "누락"}
 * @procedure 기어만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_011)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.gear = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-012
 * @brief     [목적] timestamp 누락 이면 입력 묶음 전체가 INVALID 이다 (신선도 판단 불가).
 * @satisfies REQ-F-003, REQ-S-003, REQ-S-008
 * @technique 동치 분할
 * @input     {"source_timestamp_s": "누락"}
 * @procedure timestamp 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID(stale 아님), 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_012)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.timestamp_s = MissingNum();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-013
 * @brief     [목적] timestamp NaN 이면 입력 묶음 전체가 INVALID 이다 (신선도 판단 불가).
 * @satisfies REQ-F-003, REQ-S-003, REQ-S-008
 * @technique 동치 분할
 * @input     {"source_timestamp_s": "NaN"}
 * @procedure timestamp 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID(stale 아님), 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_013)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.timestamp_s = Num(__builtin_nan(""));
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-014
 * @brief     [목적] 충돌 상태 NONE 이 유효하고 다른 영향이 없음을 확인한다 (PENDING/CONFIRMED 는 REQ-F-029, REQ-S-001 케이스).
 * @satisfies REQ-F-004
 * @technique 동치 분할
 * @input     {"crash_status": "NONE"}
 * @procedure 충돌 상태 NONE 으로 1주기를 실행한다
 * @expected  상태 ACTIVE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_014)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
}

/**
 * @test      ST-F-015
 * @brief     [목적] 충돌 상태 범위 밖 는 INVALID 이다.
 * @satisfies REQ-F-004, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"crash_status": "미정 코드 3"}
 * @procedure 충돌 상태만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_015)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(3);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-016
 * @brief     [목적] 충돌 상태 누락 는 INVALID 이다.
 * @satisfies REQ-F-004, REQ-S-008
 * @technique 동치 분할, 경계값 분석
 * @input     {"crash_status": "누락"}
 * @procedure 충돌 상태만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_016)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-017
 * @brief     [목적] rear_left_risk 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"rear_left_risk": "2"}
 * @procedure rear_left_risk 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_017)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_left_risk = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-018
 * @brief     [목적] rear_left_risk 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"rear_left_risk": "누락"}
 * @procedure rear_left_risk 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_018)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_left_risk = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-019
 * @brief     [목적] rear_right_risk 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"rear_right_risk": "2"}
 * @procedure rear_right_risk 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_019)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_right_risk = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-020
 * @brief     [목적] rear_right_risk 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"rear_right_risk": "누락"}
 * @procedure rear_right_risk 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_020)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_right_risk = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-021
 * @brief     [목적] fire_detected 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"fire_detected": "2"}
 * @procedure fire_detected 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_021)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.fire = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-022
 * @brief     [목적] fire_detected 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"fire_detected": "누락"}
 * @procedure fire_detected 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_022)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.fire = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-023
 * @brief     [목적] overtemperature_detected 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"overtemperature_detected": "2"}
 * @procedure overtemperature_detected 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_023)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.overtemp = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-024
 * @brief     [목적] overtemperature_detected 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"overtemperature_detected": "누락"}
 * @procedure overtemperature_detected 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_024)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.overtemp = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-025
 * @brief     [목적] adult_present 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"adult_present": "2"}
 * @procedure adult_present 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_025)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.adult_present = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-026
 * @brief     [목적] adult_present 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"adult_present": "누락"}
 * @procedure adult_present 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_026)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.adult_present = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-027
 * @brief     [목적] isofix_left 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"isofix_left": "2"}
 * @procedure isofix_left 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_027)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.isofix_left = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-028
 * @brief     [목적] isofix_left 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"isofix_left": "누락"}
 * @procedure isofix_left 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_028)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.isofix_left = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-029
 * @brief     [목적] isofix_right 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"isofix_right": "2"}
 * @procedure isofix_right 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_029)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.isofix_right = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-030
 * @brief     [목적] isofix_right 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"isofix_right": "누락"}
 * @procedure isofix_right 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_030)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.isofix_right = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-031
 * @brief     [목적] ignition_on 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"ignition_on": "2"}
 * @procedure ignition_on 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_031)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.ignition_on = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-032
 * @brief     [목적] ignition_on 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"ignition_on": "누락"}
 * @procedure ignition_on 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_032)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.ignition_on = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-033
 * @brief     [목적] sensor_fault 불리언 아님(2) 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"sensor_fault": "2"}
 * @procedure sensor_fault 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_033)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.sensor_fault = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-034
 * @brief     [목적] sensor_fault 누락 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.
 * @satisfies REQ-F-005, REQ-S-008
 * @technique 동치 분할
 * @input     {"sensor_fault": "누락"}
 * @procedure sensor_fault 만 무효로 바꿔 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_034)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.sensor_fault = MissingInt();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-035
 * @brief     [목적] 운전자 명령의 side 가 범위 밖 3 이면 명령을 거절하고 평가에 쓰지 않는다.
 * @satisfies REQ-F-006, REQ-F-007
 * @technique 동치 분할, 경계값 분석
 * @input     {"command.side": "3", "나머지 명령 필드": "유효"}
 * @procedure 명령 1건과 함께 1주기를 실행한다 / 명령 거절 이유와 출력을 관측한다
 * @expected  command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_035)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[1] = {[&] { ecl_raw_command_t c = ValidCommand(); c.side = Int(3); return c; }()};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-036
 * @brief     [목적] 운전자 명령의 side 가 누락 이면 명령을 거절하고 평가에 쓰지 않는다.
 * @satisfies REQ-F-006, REQ-F-007
 * @technique 동치 분할, 경계값 분석
 * @input     {"command.side": "누락", "나머지 명령 필드": "유효"}
 * @procedure 명령 1건과 함께 1주기를 실행한다 / 명령 거절 이유와 출력을 관측한다
 * @expected  command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_036)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[1] = {[&] { ecl_raw_command_t c = ValidCommand(); c.side = MissingInt(); return c; }()};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-037
 * @brief     [목적] 운전자 명령의 action 가 범위 밖 2 이면 명령을 거절하고 평가에 쓰지 않는다.
 * @satisfies REQ-F-018, REQ-F-007
 * @technique 동치 분할, 경계값 분석
 * @input     {"command.action": "2", "나머지 명령 필드": "유효"}
 * @procedure 명령 1건과 함께 1주기를 실행한다 / 명령 거절 이유와 출력을 관측한다
 * @expected  command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_037)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[1] = {[&] { ecl_raw_command_t c = ValidCommand(); c.action = Int(2); return c; }()};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-038
 * @brief     [목적] 운전자 명령의 action 가 누락 이면 명령을 거절하고 평가에 쓰지 않는다.
 * @satisfies REQ-F-018, REQ-F-007
 * @technique 동치 분할, 경계값 분석
 * @input     {"command.action": "누락", "나머지 명령 필드": "유효"}
 * @procedure 명령 1건과 함께 1주기를 실행한다 / 명령 거절 이유와 출력을 관측한다
 * @expected  command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_038)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[1] = {[&] { ecl_raw_command_t c = ValidCommand(); c.action = MissingInt(); return c; }()};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-039
 * @brief     [목적] 운전자 명령의 source 가 범위 밖 4 이면 명령을 거절하고 평가에 쓰지 않는다.
 * @satisfies REQ-F-019, REQ-F-007
 * @technique 동치 분할, 경계값 분석
 * @input     {"command.source": "4", "나머지 명령 필드": "유효"}
 * @procedure 명령 1건과 함께 1주기를 실행한다 / 명령 거절 이유와 출력을 관측한다
 * @expected  command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_039)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[1] = {[&] { ecl_raw_command_t c = ValidCommand(); c.source = Int(4); return c; }()};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-040
 * @brief     [목적] 운전자 명령의 source 가 누락 이면 명령을 거절하고 평가에 쓰지 않는다.
 * @satisfies REQ-F-019, REQ-F-007
 * @technique 동치 분할, 경계값 분석
 * @input     {"command.source": "누락", "나머지 명령 필드": "유효"}
 * @procedure 명령 1건과 함께 1주기를 실행한다 / 명령 거절 이유와 출력을 관측한다
 * @expected  command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_040)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[1] = {[&] { ecl_raw_command_t c = ValidCommand(); c.source = MissingInt(); return c; }()};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-041
 * @brief     [목적] 한 평가주기에 유효한 명령 2건이 오면 첫 1건만 평가에 쓰고 나머지는 거절 이유로 기록한다.
 * @satisfies REQ-F-009, REQ-F-007
 * @technique 동치 분할, WoW 검증
 * @input     {"commands": "좌 잠금(물리 버튼), 우 해제(AVN)"}
 * @procedure 명령 2건과 함께 1주기를 실행한다
 * @expected  command_note = CMD_REJECTED_EXTRA
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_041)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const ecl_raw_command_t cmds[2] = {Cmd(ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON), Cmd(ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, ECL_SRC_AVN)};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 2, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_EXTRA);
}

/**
 * @test      ST-F-043
 * @brief     [목적] sensor_fault 가 유효한 TRUE 이면 첫 주기에 FAULT, 경고, 출력 유지, 규칙 1 이 적용된다.
 * @satisfies REQ-F-014, REQ-S-004, REQ-S-009, REQ-F-010, REQ-F-011
 * @technique 동치 분할
 * @input     {"sensor_fault": "TRUE", "이전 확정 출력": "좌 LOCK, 우 RELEASE"}
 * @procedure sensor_fault 를 TRUE 로 1주기를 실행한다
 * @expected  상태 FAULT, 경고 SENSOR_FAULT, 좌 LOCK, 우 RELEASE, priority SENSOR_FAULT, reason SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_043)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.sensor_fault = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
    EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_SENSOR_FAULT);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-044
 * @brief     [목적] sensor_fault 가 유효한 TRUE 이고 다른 필드(속도)가 INVALID 이어도 규칙 1 은 자신의 입력이 유효하므로 적용된다.
 * @satisfies REQ-F-010, REQ-S-004, REQ-S-008
 * @technique 동치 분할, WoW 검증
 * @input     {"sensor_fault": "TRUE", "vehicle_speed_kph": "500 (범위 밖)"}
 * @procedure 두 값을 바꿔 1주기를 실행한다
 * @expected  상태 FAULT, 경고 SENSOR_FAULT|INPUT_INVALID, 출력 유지, priority SENSOR_FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_044)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.sensor_fault = Int(1);
        raw.speed_kph = Num(500.0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
    EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT | ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-045
 * @brief     [목적] sensor_fault 입력이 INVALID 이면 규칙 1 은 적용되지 않고 FAULT 가 아니다.
 * @satisfies REQ-F-010, REQ-F-014
 * @technique 동치 분할
 * @input     {"sensor_fault": "2 (불리언 아님)"}
 * @procedure sensor_fault 만 무효로 1주기를 실행한다
 * @expected  상태 DEGRADED (FAULT 아님), priority HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_045)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.sensor_fault = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-046
 * @brief     [목적] stale 입력(나이 0.25 s)은 DEGRADED 이고 INPUT_STALE 경고, 이유 INPUT_STALE, 출력 유지이다.
 * @satisfies REQ-F-014, REQ-S-003
 * @technique 경계값 분석
 * @input     {"timestamp": "1.0 s", "평가 시각": "1.25 s"}
 * @procedure 평가 시각만 1.25 로 1주기를 실행한다
 * @expected  상태 DEGRADED, 경고 INPUT_STALE, 이유 INPUT_STALE, 출력 유지
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_046)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.25, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    EXPECT_EQ(out.warnings, ECL_WARN_INPUT_STALE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_STALE);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-047
 * @brief     [목적] 점화가 꺼지면(유효) 상태가 OFF 이다 (이전 출력이 RELEASE 인 경우로 출력과 분리해 확인).
 * @satisfies REQ-F-014
 * @technique 동치 분할
 * @input     {"ignition_on": "FALSE"}
 * @procedure 이전 출력 좌/우 RELEASE 에서 ignition_on=FALSE 로 1주기를 실행한다
 * @expected  상태 OFF, 경고 없음
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_047)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.ignition_on = Int(0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_OFF);
    EXPECT_EQ(out.warnings, 0U);
}

/**
 * @test      ST-F-048
 * @brief     [목적] 상태 우선순서: sensor_fault(FAULT) > 입력 오류(DEGRADED) > OFF.
 * @satisfies REQ-F-014, REQ-S-009
 * @technique 상태 전이
 * @input     {"조합": "점화 FALSE + 속도 500 → DEGRADED, 점화 FALSE + sensor_fault TRUE → FAULT"}
 * @procedure 점화 FALSE 와 속도 500 으로 1주기 / 점화 FALSE 와 sensor_fault TRUE 로 1주기
 * @expected  첫째 DEGRADED, 둘째 FAULT
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_048)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.ignition_on = Int(0);
        raw.speed_kph = Num(500.0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
        EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.ignition_on = Int(0);
        raw.sensor_fault = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_FAULT);
}

/**
 * @test      ST-F-049
 * @brief     [목적] 오류가 해소되면 다음 평가주기에 자동으로 ACTIVE 로 복귀한다 (DEGRADED 와 FAULT 모두).
 * @satisfies REQ-F-014, REQ-F-010
 * @technique 상태 전이
 * @input     {"주기 1": "속도 500", "주기 2": "정상", "주기 3": "sensor_fault TRUE", "주기 4": "정상"}
 * @procedure 4주기를 순서대로 실행한다
 * @expected  주기 1 DEGRADED, 2 ACTIVE(이유 NONE), 3 FAULT, 4 ACTIVE(이유 NONE)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_049)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(500.0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
        EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
        EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.sensor_fault = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.03, &out), ECL_OK);
        EXPECT_EQ(out.state, ECL_STATE_FAULT);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.04, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-050
 * @brief     [목적] 전원 투입 직후 첫 평가주기의 출력은 좌/우 RELEASE 이고 값이 LOCK 또는 RELEASE 로 공개된다.
 * @satisfies REQ-F-015, REQ-F-016
 * @technique 동치 분할
 * @input     {"입력": "정상"}
 * @procedure 제어기를 초기화한 직후 1주기를 실행한다
 * @expected  좌 RELEASE, 우 RELEASE, 상태 ACTIVE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_050)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
}

/**
 * @test      ST-F-051
 * @brief     [목적] 10 ms 평가주기로 100주기를 실행하고, 오류 입력이 같은 주기에 상태에 반영됨(지연 없음)을 확인한다.
 * @satisfies REQ-F-013, REQ-F-016
 * @technique 상태 전이, WoW 검증
 * @input     {"평가 시각": "1.00 + 0.01 k s (k=0..99)", "입력": "홀수 k 에서 속도 500"}
 * @procedure 100주기를 실행하며 매 주기 상태를 관측한다
 * @expected  짝수 k: ACTIVE, 홀수 k: DEGRADED (입력 오류가 같은 주기에 반영)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_051)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    for (int k = 0; k < 100; ++k)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const double now = 1.0 + (0.01 * k);
        raw.timestamp_s = Num(now);
        if ((k % 2) == 1)
        {
            raw.speed_kph = Num(500.0);
        }
        ecl_cycle_result_t out{};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, now, &out), ECL_OK);
        EXPECT_EQ(out.state, ((k % 2) == 1) ? ECL_STATE_DEGRADED : ECL_STATE_ACTIVE) << "k=" << k;
    }
}

/**
 * @test      ST-F-052
 * @brief     [목적] 좌/우 출력이 독립적으로 결정됨을 확인한다 (한쪽 규칙이 반대쪽 출력을 바꾸지 않음).
 * @satisfies REQ-F-017
 * @technique 동치 분할
 * @input     {"접근위험(좌)": "TRUE", "이전 확정 출력": "좌 RELEASE, 우 LOCK"}
 * @procedure 좌만 접근위험 TRUE 로 1주기를 실행한다
 * @expected  좌 LOCK(규칙 3), 우 LOCK 유지(규칙 없음)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_052)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_left_risk = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_APPROACH);
}

/**
 * @test      ST-NF-001
 * @brief     [목적] 충돌 CONFIRMED 가 유효하면 300 ms 이내(10 ms 주기 30주기 이내)에 좌/우 RELEASE 가 되고 CONFIRMED 인 동안 유지된다.
 * @satisfies REQ-S-001, REQ-S-005
 * @technique 경계값 분석, 성능 측정
 * @input     {"crash_status": "CONFIRMED (t0 부터 3 s 유지)", "이전 확정 출력": "좌 LOCK, 우 LOCK"}
 * @procedure 이전 출력을 좌/우 LOCK 으로 한다 / t0 에 CONFIRMED 를 주입하고 10 ms 주기로 300 주기를 실행한다 / RELEASE 가 된 첫 시각과 이후 출력을 관측한다
 * @expected  RELEASE 가 t0 부터 300 ms 이내(k ≤ 30)에 되고, 이후 300주기 동안 계속 RELEASE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_NF_001)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    int firstRelease = -1;
    for (int k = 0; k < 300; ++k)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        const double now = 1.0 + (k / 100.0);
        raw.timestamp_s = Num(now);
        raw.crash_status = Int(ECL_CRASH_CONFIRMED);
        ecl_cycle_result_t out{};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, now, &out), ECL_OK);
        const bool released = (out.decision.left == ECL_OUT_RELEASE) && (out.decision.right == ECL_OUT_RELEASE);
        if ((firstRelease < 0) && released)
        {
            firstRelease = k;
        }
        if (firstRelease >= 0)
        {
            EXPECT_TRUE(released) << "k=" << k;
        }
    }
    EXPECT_GE(firstRelease, 0);
    EXPECT_LE(firstRelease, 30);
}

/**
 * @test      ST-F-053
 * @brief     [목적] 충돌 전이 에서 RELEASE 가 된다.
 * @satisfies REQ-S-001
 * @technique 상태 전이
 * @input     {"crash_status": "직전 상태 NONE → CONFIRMED 전이"}
 * @procedure 이전 출력을 좌/우 LOCK 으로 하고 전이를 입력한다 / 다음 평가주기 출력을 관측한다
 * @expected  좌/우 RELEASE, priority CRASH, reason CRASH_CONFIRMED
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_053)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(0);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_CRASH);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_CRASH_CONFIRMED);
}

/**
 * @test      ST-F-054
 * @brief     [목적] 충돌 PENDING 경유 에서 RELEASE 가 된다.
 * @satisfies REQ-S-001
 * @technique 상태 전이
 * @input     {"crash_status": "PENDING → CONFIRMED 전이"}
 * @procedure 이전 출력을 좌/우 LOCK 으로 하고 전이를 입력한다 / 다음 평가주기 출력을 관측한다
 * @expected  좌/우 RELEASE, priority CRASH, reason CRASH_CONFIRMED
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_054)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_CRASH);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_CRASH_CONFIRMED);
}

/**
 * @test      ST-F-055
 * @brief     [목적] 충돌 PENDING 은 출력을 바꾸지 않고 이유 CRASH_PENDING 만 기록한다.
 * @satisfies REQ-F-029
 * @technique 동치 분할
 * @input     {"crash_status": "PENDING", "이전 확정 출력": "좌 LOCK, 우 RELEASE"}
 * @procedure PENDING 으로 1주기를 실행한다
 * @expected  좌 LOCK, 우 RELEASE 유지, priority HOLD, reason CRASH_PENDING, 상태 ACTIVE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_055)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.state, ECL_STATE_ACTIVE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_CRASH_PENDING);
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
}

/**
 * @test      ST-F-056
 * @brief     [목적] 좌 접근위험 TRUE 이면 좌 LOCK, 좌 해제 요청 억제, 억제 이유 기록, 우는 영향 없음.
 * @satisfies REQ-S-002, REQ-S-006, REQ-S-007, REQ-F-017
 * @technique 동치 분할
 * @input     {"rear_left_risk": "TRUE", "명령": "좌 해제(AVN), 정지 상태", "이전 확정 출력": "좌/우 RELEASE"}
 * @procedure 명령과 함께 1주기를 실행한다
 * @expected  좌 LOCK, 우 RELEASE, 이유 APPROACH_RISK_HOLD, priority APPROACH
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_056)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(0.0);
        raw.rear_left_risk = Int(1);
        const ecl_raw_command_t cmds[1] = {Cmd(ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN)};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_APPROACH);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      ST-F-057
 * @brief     [목적] 우 접근위험 TRUE 이면 우만 LOCK 이고 우 해제 요청이 억제되며 좌는 영향 없다.
 * @satisfies REQ-S-002, REQ-S-006, REQ-F-017
 * @technique 동치 분할
 * @input     {"rear_right_risk": "TRUE", "명령": "우 해제(음성), 정지 상태", "이전 확정 출력": "좌 LOCK, 우 LOCK"}
 * @procedure 명령과 함께 1주기를 실행한다
 * @expected  좌 LOCK(유지), 우 LOCK, 이유 APPROACH_RISK_HOLD
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_057)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ctl.confirmed.left = ECL_OUT_LOCK;
    ctl.confirmed.right = ECL_OUT_LOCK;
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(0.0);
        raw.rear_right_risk = Int(1);
        const ecl_raw_command_t cmds[1] = {Cmd(ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, ECL_SRC_VOICE)};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, cmds, 1, 1.01, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.right, ECL_OUT_LOCK);
    EXPECT_EQ(out.decision.reason_code, ECL_REASON_APPROACH_RISK_HOLD);
}

/**
 * @test      ST-F-058
 * @brief     [목적] 순위 1~3 규칙의 단계별 우선순위: 접근위험만 → LOCK, +충돌 → RELEASE, +sensor_fault → 확정 출력 유지.
 * @satisfies REQ-F-012, REQ-F-020, REQ-S-001, REQ-S-002, REQ-S-004
 * @technique 상태 전이
 * @input     {"주기 1": "좌 접근위험", "주기 2": "+충돌 CONFIRMED", "주기 3": "+sensor_fault, 충돌 NONE"}
 * @procedure 3주기를 같은 제어기에서 실행한다
 * @expected  주기 1 좌 LOCK(APPROACH), 주기 2 좌/우 RELEASE(CRASH), 주기 3 좌/우 RELEASE 유지(SENSOR_FAULT)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_F_058)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_left_risk = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
        EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);
        EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_APPROACH);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_left_risk = Int(1);
        raw.crash_status = Int(2);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.02, &out), ECL_OK);
        EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
        EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_CRASH);
    }
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.rear_left_risk = Int(1);
        raw.sensor_fault = Int(1);
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.03, &out), ECL_OK);
    }
    EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);
    EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);
}

/**
 * @test      ST-NF-002
 * @brief     [목적] 입력이 200 ms 를 넘게 갱신되지 않으면 그 시점부터 100 ms 이내(≤ 10 주기)에 DEGRADED 가 된다. 신선한 동안은 ACTIVE 이다.
 * @satisfies REQ-S-003, REQ-F-014
 * @technique 경계값 분석, 성능 측정
 * @input     {"source_timestamp_s": "1.0 s 고정", "평가 시각": "1.00 + 0.01 k s (k=0..40)"}
 * @procedure 동일 timestamp 로 10 ms 주기 41주기를 실행한다 / 처음 DEGRADED 가 된 k 를 구한다
 * @expected  k ≤ 19 에서 ACTIVE, 처음 DEGRADED 는 k=21 이상 k=30 이하 (나이 > 0.2 s 후 100 ms 이내), 그 이후 계속 DEGRADED
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_NF_002)
{
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    const ecl_raw_snapshot_t raw = ValidRaw();
    int firstDegraded = -1;
    for (int k = 0; k <= 40; ++k)
    {
        ecl_cycle_result_t out{};
        ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.0 + (k / 100.0), &out), ECL_OK);
        if ((k <= 19) && (out.state != ECL_STATE_ACTIVE))
        {
            ADD_FAILURE() << "k=" << k << " 에서 ACTIVE 여야 한다";
        }
        if ((firstDegraded < 0) && (out.state == ECL_STATE_DEGRADED))
        {
            firstDegraded = k;
        }
        if (firstDegraded >= 0)
        {
            EXPECT_EQ(out.state, ECL_STATE_DEGRADED) << "k=" << k;
        }
    }
    EXPECT_GE(firstDegraded, 21);
    EXPECT_LE(firstDegraded, 30);
}

/**
 * @test      ST-NF-003
 * @brief     [목적] 평가주기 한 번의 평균 처리 시간이 3 ms 미만이다 (호스트, 계측 빌드, 10000주기 평균).
 * @satisfies REQ-Q-003
 * @technique 성능 측정
 * @input     {"반복": "10000주기", "입력": "정상"}
 * @procedure 10000주기를 실행해 평균 처리 시간을 측정한다
 * @expected  평균 < 3 ms
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_NF_003)
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
 * @test      ST-NF-005
 * @brief     [목적] 원시 입력 형식에 영상·음성·식별자 필드가 없고 성인 존재는 불리언 하나뿐이다.
 * @satisfies REQ-Q-002, REQ-Q-004
 * @technique 요구사항 분석
 * @input     {"검사": "ecl_raw_snapshot_t 구조 크기와 필드"}
 * @procedure 구조 크기를 허용 필드(수치 2개, 정수 코드 11개) 합과 비교한다 / adult_present 가 정수 코드 불리언 필드임을 확인한다
 * @expected  크기가 같고 adult_present 는 0/1 만 유효
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(EclSystemTest, ST_NF_005)
{
    const size_t expected = (2U * sizeof(ecl_raw_num_t)) + (11U * sizeof(ecl_raw_int_t));
    EXPECT_EQ(sizeof(ecl_raw_snapshot_t), expected);
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.adult_present = Int(2);
    ecl_controller_t ctl{};
    ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
    EXPECT_EQ(out.state, ECL_STATE_DEGRADED);
}

}  // namespace
