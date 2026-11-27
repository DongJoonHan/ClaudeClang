/**
 * @file    ecl_test_support.hpp
 * @brief   시험 공용 도우미 (시험 전용 헤더, 제품 코드가 아니다).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_TEST_SUPPORT_HPP
#define ECL_TEST_SUPPORT_HPP

#include "ecl/ecl_types.h"

// 필드 값을 만드는 짧은 도우미
static inline ecl_raw_num_t Num(double value)
{
    return ecl_raw_num_t{1, value};
}

static inline ecl_raw_int_t Int(int value)
{
    return ecl_raw_int_t{1, value};
}

static inline ecl_raw_num_t MissingNum()
{
    return ecl_raw_num_t{0, 0.0};
}

static inline ecl_raw_int_t MissingInt()
{
    return ecl_raw_int_t{0, 0};
}

// 모든 필드가 유효한 원시 입력 묶음: 속도 10 km/h, 기어 D, 시각 1.0 s, 충돌 없음, 점화 켜짐
static inline ecl_raw_snapshot_t ValidRaw()
{
    ecl_raw_snapshot_t raw{};
    raw.speed_kph = Num(10.0);
    raw.gear = Int(ECL_GEAR_D);
    raw.timestamp_s = Num(1.0);
    raw.crash_status = Int(ECL_CRASH_NONE);
    raw.rear_left_risk = Int(0);
    raw.rear_right_risk = Int(0);
    raw.fire = Int(0);
    raw.overtemp = Int(0);
    raw.adult_present = Int(0);
    raw.isofix_left = Int(0);
    raw.isofix_right = Int(0);
    raw.ignition_on = Int(1);
    raw.sensor_fault = Int(0);
    return raw;
}

// 유효한 원시 명령: 좌 잠금, 물리 버튼
static inline ecl_raw_command_t ValidCommand()
{
    ecl_raw_command_t cmd{};
    cmd.side = Int(ECL_SIDE_LEFT);
    cmd.action = Int(ECL_ACTION_LOCK);
    cmd.source = Int(ECL_SRC_PHYSICAL_BUTTON);
    return cmd;
}

// 모든 필드가 유효한 검증 입력 (다른 모듈의 결과에 의존하지 않고 직접 만든다)
static inline ecl_validated_t ValidValidated()
{
    ecl_validated_t v{};
    const auto setValid = [](ecl_val_int_t& field) {
        field.valid = 1;
        field.value = 0;
    };
    setValid(v.gear);
    setValid(v.crash_status);
    setValid(v.rear_left_risk);
    setValid(v.rear_right_risk);
    setValid(v.fire);
    setValid(v.overtemp);
    setValid(v.adult_present);
    setValid(v.isofix_left);
    setValid(v.isofix_right);
    setValid(v.ignition_on);
    setValid(v.sensor_fault);
    v.speed_kph = ecl_val_num_t{1, 10.0};
    v.timestamp_s = ecl_val_num_t{1, 1.0};
    v.ignition_on.value = 1;
    v.stale = 0;
    v.all_valid = 1;
    return v;
}

#endif  // ECL_TEST_SUPPORT_HPP
