/**
 * @file    input_validation_test.cpp
 * @brief   input_validation 단위 테스트 (MC/DC 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>
#include "ecl/input_validation.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      UT-input-001
 * @brief     [목적] raw 가 널이면 ECL_ERR_NULL_ARG 를 반환하고 out 을 바꾸지 않는다.
 * @satisfies DD-M-001, REQ-S-008
 * @technique MC/DC(조건 raw == NULL 의 독립 영향)
 * @input     raw=nullptr, out=센티널
 * @procedure 1) out.all_valid=7 로 둔다. 2) ecl_input_validate(nullptr, ...) 를 호출한다.
 * @expected  반환값 == ECL_ERR_NULL_ARG, out.all_valid == 7
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, ReturnsErrNullArgWhenRawIsNull)
{
    ecl_validated_t out{};
    out.all_valid = 7;
    EXPECT_EQ(ecl_input_validate(nullptr, 1.1, &out), ECL_ERR_NULL_ARG);
    EXPECT_EQ(out.all_valid, 7);
}

/**
 * @test      UT-input-002
 * @brief     [목적] out 이 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-001, REQ-S-008
 * @technique MC/DC(조건 out == NULL 의 독립 영향)
 * @input     raw=유효, out=nullptr
 * @procedure 1) ecl_input_validate(&raw, 1.1, nullptr) 를 호출한다.
 * @expected  반환값 == ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, ReturnsErrNullArgWhenOutIsNull)
{
    const ecl_raw_snapshot_t raw = ValidRaw();
    EXPECT_EQ(ecl_input_validate(&raw, 1.1, nullptr), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-input-003
 * @brief     [목적] 모든 필드가 정상이면 모든 필드가 유효하고 값이 복사된다.
 * @satisfies REQ-F-001~005
 * @technique 동치 클래스(유효 입력)
 * @input     ValidRaw(), eval=1.1
 * @procedure 1) 검증한다.
 * @expected  ECL_OK, all_valid == 1, stale == 0, speed == 10.0, gear == D, ignition == 1
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, AcceptsValidSnapshotAndCopiesValues)
{
    const ecl_raw_snapshot_t raw = ValidRaw();
    ecl_validated_t out{};
    ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
    EXPECT_EQ(out.all_valid, 1);
    EXPECT_EQ(out.stale, 0);
    EXPECT_EQ(out.speed_kph.valid, 1);
    EXPECT_DOUBLE_EQ(out.speed_kph.value, 10.0);
    EXPECT_EQ(out.gear.value, ECL_GEAR_D);
    EXPECT_EQ(out.ignition_on.value, 1);
}

/**
 * @test      UT-input-004
 * @brief     [목적] 속도 0.0 과 300.0 은 유효하다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석
 * @input     speed ∈ {0.0, 300.0}
 * @procedure 1) 각 값으로 검증한다.
 * @expected  speed_kph.valid == 1, all_valid == 1
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, SpeedBoundariesAreInclusive)
{
    for (const double speed : {0.0, 300.0})
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = Num(speed);
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.speed_kph.valid, 1);
        EXPECT_EQ(out.all_valid, 1);
    }
}

/**
 * @test      UT-input-005
 * @brief     [목적] 범위 밖·NaN·무한대·누락 속도는 INVALID 이다.
 * @satisfies REQ-F-001
 * @technique 경계값 분석(-0.1, 300.1) + 동치 클래스(NaN, inf, 누락)
 * @input     speed ∈ {-0.1, 300.1, NaN, +inf, 누락}
 * @procedure 1) 각 입력으로 검증한다.
 * @expected  speed_kph.valid == 0, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, SpeedOutsideRangeOrNonFiniteIsInvalid)
{
    const ecl_raw_num_t cases[] = {Num(-0.1), Num(300.1), Num(__builtin_nan("")),
                                    Num(__builtin_inf()), MissingNum()};
    for (const ecl_raw_num_t& speed : cases)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.speed_kph = speed;
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.speed_kph.valid, 0);
        EXPECT_EQ(out.all_valid, 0);
        EXPECT_EQ(out.stale, 0);
    }
}

/**
 * @test      UT-input-006
 * @brief     [목적] 기어 코드 0~3 은 유효하다.
 * @satisfies REQ-F-002
 * @technique 경계값 분석
 * @input     gear ∈ {0, 3}
 * @procedure 1) 각 값으로 검증한다.
 * @expected  gear.valid == 1, 값 보존
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, GearCodesZeroToThreeAreValid)
{
    for (const int gear : {0, 3})
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.gear = Int(gear);
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.gear.valid, 1);
        EXPECT_EQ(out.gear.value, gear);
    }
}

/**
 * @test      UT-input-007
 * @brief     [목적] 범위 밖·누락 기어는 INVALID 이다.
 * @satisfies REQ-F-002
 * @technique 경계값 분석
 * @input     gear ∈ {-1, 4, 누락}
 * @procedure 1) 각 입력으로 검증한다.
 * @expected  gear.valid == 0, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, GearOutOfRangeOrMissingIsInvalid)
{
    const ecl_raw_int_t cases[] = {Int(-1), Int(4), MissingInt()};
    for (const ecl_raw_int_t& gear : cases)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.gear = gear;
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.gear.valid, 0);
        EXPECT_EQ(out.all_valid, 0);
    }
}

/**
 * @test      UT-input-008
 * @brief     [목적] 충돌 코드 0~2 는 유효하고 -1, 3, 누락은 INVALID 이다.
 * @satisfies REQ-F-004
 * @technique 경계값 분석
 * @input     crash ∈ {0, 1, 2, -1, 3, 누락}
 * @procedure 1) 각 입력으로 검증한다.
 * @expected  0~2: valid == 1 / 그 외: valid == 0, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, CrashStatusCodesAreValidated)
{
    for (const int code : {0, 1, 2})
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = Int(code);
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.crash_status.valid, 1);
        EXPECT_EQ(out.crash_status.value, code);
    }
    const ecl_raw_int_t bad[] = {Int(-1), Int(3), MissingInt()};
    for (const ecl_raw_int_t& crash : bad)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.crash_status = crash;
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.crash_status.valid, 0);
        EXPECT_EQ(out.all_valid, 0);
    }
}

/**
 * @test      UT-input-009
 * @brief     [목적] 아홉 불리언 필드는 0, 1 만 유효하고 2, -1, 누락은 INVALID 이다.
 * @satisfies REQ-F-005
 * @technique 경계값 분석
 * @input     9개 불리언 필드 각각 ∈ {0, 1, 2, -1, 누락}
 * @procedure 1) 필드마다 값을 바꿔 검증한다.
 * @expected  0/1: valid == 1 / 그 외: valid == 0, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, BooleanFieldsAcceptOnlyZeroAndOne)
{
    struct Field
    {
        ecl_raw_int_t ecl_raw_snapshot_t::*raw;
        ecl_val_int_t ecl_validated_t::*validated;
    };
    const Field fields[] = {
        {&ecl_raw_snapshot_t::rear_left_risk, &ecl_validated_t::rear_left_risk},
        {&ecl_raw_snapshot_t::rear_right_risk, &ecl_validated_t::rear_right_risk},
        {&ecl_raw_snapshot_t::fire, &ecl_validated_t::fire},
        {&ecl_raw_snapshot_t::overtemp, &ecl_validated_t::overtemp},
        {&ecl_raw_snapshot_t::adult_present, &ecl_validated_t::adult_present},
        {&ecl_raw_snapshot_t::isofix_left, &ecl_validated_t::isofix_left},
        {&ecl_raw_snapshot_t::isofix_right, &ecl_validated_t::isofix_right},
        {&ecl_raw_snapshot_t::ignition_on, &ecl_validated_t::ignition_on},
        {&ecl_raw_snapshot_t::sensor_fault, &ecl_validated_t::sensor_fault},
    };
    for (const Field& field : fields)
    {
        for (const int good : {0, 1})
        {
            ecl_raw_snapshot_t raw = ValidRaw();
            raw.*(field.raw) = Int(good);
            ecl_validated_t out{};
            ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
            EXPECT_EQ((out.*(field.validated)).valid, 1);
            EXPECT_EQ((out.*(field.validated)).value, good);
        }
        const ecl_raw_int_t bad[] = {Int(2), Int(-1), MissingInt()};
        for (const ecl_raw_int_t& value : bad)
        {
            ecl_raw_snapshot_t raw = ValidRaw();
            raw.*(field.raw) = value;
            ecl_validated_t out{};
            ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
            EXPECT_EQ((out.*(field.validated)).valid, 0);
            EXPECT_EQ(out.all_valid, 0);
        }
    }
}

/**
 * @test      UT-input-010
 * @brief     [목적] timestamp 가 누락·NaN·무한대이면 모든 필드가 INVALID 이고 stale 은 아니다.
 * @satisfies REQ-F-003, REQ-S-008
 * @technique 동치 클래스(누락, NaN, inf)
 * @input     timestamp ∈ {누락, NaN, +inf}
 * @procedure 1) 각 입력으로 검증한다.
 * @expected  모든 필드 valid == 0, stale == 0, all_valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, InvalidTimestampInvalidatesAllFieldsWithoutStale)
{
    const ecl_raw_num_t cases[] = {MissingNum(), Num(__builtin_nan("")), Num(__builtin_inf())};
    for (const ecl_raw_num_t& timestamp : cases)
    {
        ecl_raw_snapshot_t raw = ValidRaw();
        raw.timestamp_s = timestamp;
        ecl_validated_t out{};
        ASSERT_EQ(ecl_input_validate(&raw, 1.1, &out), ECL_OK);
        EXPECT_EQ(out.timestamp_s.valid, 0);
        EXPECT_EQ(out.speed_kph.valid, 0);
        EXPECT_EQ(out.sensor_fault.valid, 0);
        EXPECT_EQ(out.stale, 0);
        EXPECT_EQ(out.all_valid, 0);
    }
}

/**
 * @test      UT-input-011
 * @brief     [목적] 나이가 정확히 200 ms 이면 신선하다 (초과가 아니다).
 * @satisfies REQ-S-003
 * @technique 경계값 분석(나이 == 0.2 s)
 * @input     timestamp=0.0, eval=0.2
 * @procedure 1) 검증한다.
 * @expected  stale == 0, all_valid == 1
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, SnapshotExactlyAtLimitIsFresh)
{
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.timestamp_s = Num(0.0);
    ecl_validated_t out{};
    ASSERT_EQ(ecl_input_validate(&raw, 0.2, &out), ECL_OK);
    EXPECT_EQ(out.stale, 0);
    EXPECT_EQ(out.all_valid, 1);
}

/**
 * @test      UT-input-012
 * @brief     [목적] 나이가 200 ms 를 넘으면 stale 이고 모든 필드가 INVALID 이다.
 * @satisfies REQ-S-003, REQ-S-008
 * @technique 경계값 분석(나이 > 0.2 s)
 * @input     timestamp=0.0, eval=0.25
 * @procedure 1) 검증한다.
 * @expected  stale == 1, all_valid == 0, 모든 필드 valid == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, SnapshotOlderThanLimitIsStaleAndInvalidatesAll)
{
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.timestamp_s = Num(0.0);
    ecl_validated_t out{};
    ASSERT_EQ(ecl_input_validate(&raw, 0.25, &out), ECL_OK);
    EXPECT_EQ(out.stale, 1);
    EXPECT_EQ(out.all_valid, 0);
    EXPECT_EQ(out.speed_kph.valid, 0);
    EXPECT_EQ(out.timestamp_s.valid, 0);
    EXPECT_EQ(out.ignition_on.valid, 0);
}

/**
 * @test      UT-input-013
 * @brief     [목적] timestamp 가 평가 시각보다 미래이면 신선하다 (가정 A-05).
 * @satisfies REQ-S-003
 * @technique 동치 클래스(음수 나이)
 * @input     timestamp=2.0, eval=1.0
 * @procedure 1) 검증한다.
 * @expected  stale == 0, all_valid == 1
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, FutureTimestampIsTreatedAsFresh)
{
    ecl_raw_snapshot_t raw = ValidRaw();
    raw.timestamp_s = Num(2.0);
    ecl_validated_t out{};
    ASSERT_EQ(ecl_input_validate(&raw, 1.0, &out), ECL_OK);
    EXPECT_EQ(out.stale, 0);
    EXPECT_EQ(out.all_valid, 1);
}

/**
 * @test      UT-input-014
 * @brief     [목적] 원시 입력 형식에는 영상·음성·식별자 필드가 없고 허용된 13개 필드만 있다.
 * @satisfies REQ-Q-002, REQ-Q-004
 * @technique 검사(구조 크기)
 * @input     sizeof(ecl_raw_snapshot_t)
 * @procedure 1) 크기를 기대값과 비교한다.
 * @expected  크기 == 수치 2개 + 정수 코드 11개의 합 (추가 필드 없음)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(InputValidationTest, RawSnapshotHasOnlyPermittedFields)
{
    const size_t expected = (2U * sizeof(ecl_raw_num_t)) + (11U * sizeof(ecl_raw_int_t));
    EXPECT_EQ(sizeof(ecl_raw_snapshot_t), expected);
}

}  // namespace
