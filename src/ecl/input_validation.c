/**
 * @file    input_validation.c
 * @brief   원시 입력 묶음 검증 모듈 구현 (DD-M-001).
 * @satisfies ARCH-C-001, REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004, REQ-F-005, REQ-S-003, REQ-S-008
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include "ecl/input_validation.h"

#include <stddef.h>

static const int k_gear_code_max = 3;
static const int k_crash_code_max = 2;
static const int k_field_count = 13;

static int is_finite_number(double value)
{
    return (value - value) == 0.0;
}

static int num_in_range(const ecl_raw_num_t* raw, double lo, double hi)
{
    return (raw->present != 0) && is_finite_number(raw->value) && (raw->value >= lo) &&
           (raw->value <= hi);
}

static int int_in_range(const ecl_raw_int_t* raw, int lo, int hi)
{
    return (raw->present != 0) && (raw->value >= lo) && (raw->value <= hi);
}

static ecl_val_num_t to_num(const ecl_raw_num_t* raw, double lo, double hi)
{
    ecl_val_num_t result = {0, 0.0};

    if (num_in_range(raw, lo, hi))
    {
        result.valid = 1;
        result.value = raw->value;
    }

    return result;
}

static ecl_val_num_t to_finite_num(const ecl_raw_num_t* raw)
{
    ecl_val_num_t result = {0, 0.0};

    if ((raw->present != 0) && is_finite_number(raw->value))
    {
        result.valid = 1;
        result.value = raw->value;
    }

    return result;
}

static ecl_val_int_t to_int(const ecl_raw_int_t* raw, int lo, int hi)
{
    ecl_val_int_t result = {0, 0};

    if (int_in_range(raw, lo, hi))
    {
        result.valid = 1;
        result.value = raw->value;
    }

    return result;
}

static ecl_validated_t validate_fields(const ecl_raw_snapshot_t* raw)
{
    ecl_validated_t v = {0};

    v.speed_kph = to_num(&raw->speed_kph, ECL_SPEED_MIN_KPH, ECL_SPEED_MAX_KPH);
    v.gear = to_int(&raw->gear, 0, k_gear_code_max);
    v.timestamp_s = to_finite_num(&raw->timestamp_s);
    v.crash_status = to_int(&raw->crash_status, 0, k_crash_code_max);
    v.rear_left_risk = to_int(&raw->rear_left_risk, 0, 1);
    v.rear_right_risk = to_int(&raw->rear_right_risk, 0, 1);
    v.fire = to_int(&raw->fire, 0, 1);
    v.overtemp = to_int(&raw->overtemp, 0, 1);
    v.adult_present = to_int(&raw->adult_present, 0, 1);
    v.isofix_left = to_int(&raw->isofix_left, 0, 1);
    v.isofix_right = to_int(&raw->isofix_right, 0, 1);
    v.ignition_on = to_int(&raw->ignition_on, 0, 1);
    v.sensor_fault = to_int(&raw->sensor_fault, 0, 1);

    return v;
}

static int count_valid_fields(const ecl_validated_t* v)
{
    return v->speed_kph.valid + v->gear.valid + v->timestamp_s.valid + v->crash_status.valid +
           v->rear_left_risk.valid + v->rear_right_risk.valid + v->fire.valid +
           v->overtemp.valid + v->adult_present.valid + v->isofix_left.valid +
           v->isofix_right.valid + v->ignition_on.valid + v->sensor_fault.valid;
}

static ecl_validated_t invalidate_all(int stale)
{
    ecl_validated_t result = {0};

    result.stale = stale;

    return result;
}

static ecl_validated_t apply_freshness(ecl_validated_t v, double eval_time_s)
{
    ecl_validated_t result = v;

    if (v.timestamp_s.valid == 0)
    {
        result = invalidate_all(0);
    }
    else if ((eval_time_s - v.timestamp_s.value) > ECL_STALE_LIMIT_S)
    {
        result = invalidate_all(1);
    }
    else
    {
        result.all_valid = (count_valid_fields(&v) == k_field_count) ? 1 : 0;
    }

    return result;
}

ecl_status_t ecl_input_validate(const ecl_raw_snapshot_t* raw, double eval_time_s,
                                ecl_validated_t* out)
{
    ecl_status_t result = ECL_OK;

    if ((raw == NULL) || (out == NULL))
    {
        result = ECL_ERR_NULL_ARG;
    }
    else
    {
        *out = apply_freshness(validate_fields(raw), eval_time_s);
    }

    return result;
}
