/**
 * @file    ecl_types.h
 * @brief   전자식 차일드락 제어 SW 컴포넌트가 공유하는 데이터 형식 (ARCH-I-009, DD-M-009).
 * @satisfies REQ-C-001, REQ-C-005
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef ECL_ECL_TYPES_H
#define ECL_ECL_TYPES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   반환값을 호출자가 무시하면 컴파일 오류(-Werror)가 되게 하는 속성.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_NODISCARD __attribute__((warn_unused_result))

/**
 * @brief   평가 주기 한 번에서 신선하다고 보는 최대 나이 [s] (OEM-SR-003, 200 ms).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_STALE_LIMIT_S 0.2

/**
 * @brief   vehicle_speed_kph 의 하한 [km/h] (OEM-IF-001).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_SPEED_MIN_KPH 0.0

/**
 * @brief   vehicle_speed_kph 의 상한 [km/h] (OEM-IF-001).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_SPEED_MAX_KPH 300.0

/**
 * @brief   함수 실행 결과 상태.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_OK = 0,        /**< 정상 실행 */
    ECL_ERR_NULL_ARG,  /**< 널 포인터 인자 */
    ECL_ERR_RANGE      /**< 인자 범위 오류 */
} ecl_status_t;

/**
 * @brief   원시 수치 입력 (누락 여부와 값).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int present;   /**< 0 이면 입력이 누락됨 */
    double value;  /**< 수치 값 (present 가 0 이면 의미 없음) */
} ecl_raw_num_t;

/**
 * @brief   원시 정수 코드 입력 (열거형·불리언의 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int present;  /**< 0 이면 입력이 누락됨 */
    int value;    /**< 정수 코드 (불리언은 0 또는 1 이 유효) */
} ecl_raw_int_t;

/**
 * @brief   기어 코드 (OEM-IF-001).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_GEAR_P = 0,  /**< 주차 */
    ECL_GEAR_N,      /**< 중립 */
    ECL_GEAR_D,      /**< 주행 */
    ECL_GEAR_R       /**< 후진 */
} ecl_gear_t;

/**
 * @brief   충돌 상태 코드 (OEM-IF-002).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_CRASH_NONE = 0,  /**< 충돌 없음 */
    ECL_CRASH_PENDING,   /**< 충돌 판정 중 */
    ECL_CRASH_CONFIRMED  /**< 충돌 확정 */
} ecl_crash_t;

/**
 * @brief   운전자 명령의 대상 (OEM-IF-004).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_SIDE_LEFT = 0,  /**< 좌 */
    ECL_SIDE_RIGHT,     /**< 우 */
    ECL_SIDE_ALL        /**< 좌우 모두 */
} ecl_side_t;

/**
 * @brief   운전자 명령의 동작 (OEM-IF-004).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_ACTION_LOCK = 0,  /**< 잠금 */
    ECL_ACTION_UNLOCK     /**< 해제 */
} ecl_action_t;

/**
 * @brief   운전자 명령의 입력 경로 (OEM-IF-004).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_SRC_PHYSICAL_BUTTON = 0,  /**< 물리 버튼 */
    ECL_SRC_AVN,                  /**< AVN */
    ECL_SRC_VOICE,                /**< 음성 */
    ECL_SRC_MOBILE_APP            /**< 모바일 앱 */
} ecl_source_t;

/**
 * @brief   원시 입력 묶음 (OEM-IF-001~003, 007~009). 영상·음성·식별자 필드는 없다 (REQ-Q-002).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_raw_num_t speed_kph;        /**< 차속 [km/h] */
    ecl_raw_int_t gear;             /**< 기어 코드 (ecl_gear_t) */
    ecl_raw_num_t timestamp_s;      /**< 입력 생성 시각 [s] (source_timestamp_s) */
    ecl_raw_int_t crash_status;     /**< 충돌 상태 코드 (ecl_crash_t) */
    ecl_raw_int_t rear_left_risk;   /**< 좌 후측방 접근위험 (0/1) */
    ecl_raw_int_t rear_right_risk;  /**< 우 후측방 접근위험 (0/1) */
    ecl_raw_int_t fire;             /**< 화재 감지 (0/1) */
    ecl_raw_int_t overtemp;         /**< 과온 감지 (0/1) */
    ecl_raw_int_t adult_present;    /**< 성인 존재 (0/1) */
    ecl_raw_int_t isofix_left;      /**< 좌 ISOFIX 연결 (0/1) */
    ecl_raw_int_t isofix_right;     /**< 우 ISOFIX 연결 (0/1) */
    ecl_raw_int_t ignition_on;      /**< 점화 상태 (0/1) */
    ecl_raw_int_t sensor_fault;     /**< 센서 고장 (0/1) */
} ecl_raw_snapshot_t;

/**
 * @brief   원시 운전자 명령 (OEM-IF-004).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_raw_int_t side;    /**< 대상 코드 (ecl_side_t) */
    ecl_raw_int_t action;  /**< 동작 코드 (ecl_action_t) */
    ecl_raw_int_t source;  /**< 입력 경로 코드 (ecl_source_t) */
} ecl_raw_command_t;

/**
 * @brief   검증된 수치 값.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int valid;     /**< 1 이면 유효 */
    double value;  /**< 유효할 때의 값 */
} ecl_val_num_t;

/**
 * @brief   검증된 정수 코드 값 (불리언은 0 또는 1).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int valid;  /**< 1 이면 유효 */
    int value;  /**< 유효할 때의 값 */
} ecl_val_int_t;

/**
 * @brief   검증 입력: 필드마다 유효성이 표시된 입력 묶음.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_val_num_t speed_kph;        /**< 차속 */
    ecl_val_int_t gear;             /**< 기어 */
    ecl_val_num_t timestamp_s;      /**< 입력 시각 */
    ecl_val_int_t crash_status;     /**< 충돌 상태 */
    ecl_val_int_t rear_left_risk;   /**< 좌 접근위험 */
    ecl_val_int_t rear_right_risk;  /**< 우 접근위험 */
    ecl_val_int_t fire;             /**< 화재 */
    ecl_val_int_t overtemp;         /**< 과온 */
    ecl_val_int_t adult_present;    /**< 성인 존재 */
    ecl_val_int_t isofix_left;      /**< 좌 ISOFIX */
    ecl_val_int_t isofix_right;     /**< 우 ISOFIX */
    ecl_val_int_t ignition_on;      /**< 점화 */
    ecl_val_int_t sensor_fault;     /**< 센서 고장 */
    int stale;                      /**< 1 이면 입력 묶음이 신선하지 않음 */
    int all_valid;                  /**< 1 이면 모든 필수 입력이 유효하고 신선함 */
} ecl_validated_t;

/**
 * @brief   도어 출력 (OEM-IF-005).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_OUT_RELEASE = 0,  /**< 해제 */
    ECL_OUT_LOCK          /**< 잠금 */
} ecl_output_t;

/**
 * @brief   제어 상태 (SRS 1.4절).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_STATE_ACTIVE = 0,  /**< 정상 */
    ECL_STATE_DEGRADED,    /**< 입력 오류 또는 stale */
    ECL_STATE_FAULT,       /**< 센서 고장 */
    ECL_STATE_OFF          /**< 점화 꺼짐 */
} ecl_state_t;

/**
 * @brief   출력을 정한 규칙 (priority_reason, SRS 1.3절 순위).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_PRIO_SENSOR_FAULT = 1,  /**< 규칙 1 */
    ECL_PRIO_CRASH,             /**< 규칙 2 */
    ECL_PRIO_APPROACH,          /**< 규칙 3 */
    ECL_PRIO_EMERGENCY_RELEASE, /**< 규칙 4 */
    ECL_PRIO_IGNITION_OFF,      /**< 규칙 5 */
    ECL_PRIO_ISOFIX,            /**< 규칙 6 */
    ECL_PRIO_AUTO_LOCK,         /**< 규칙 7 */
    ECL_PRIO_DRIVER_CMD,        /**< 규칙 8 */
    ECL_PRIO_HOLD               /**< 규칙 9 */
} ecl_priority_t;

/**
 * @brief   이유코드 (SRS 부록 A).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef enum
{
    ECL_REASON_NONE = 0,           /**< 변화 없음 */
    ECL_REASON_SENSOR_FAULT,       /**< 센서 고장으로 출력 유지 */
    ECL_REASON_CRASH_CONFIRMED,    /**< 충돌 확정으로 해제 */
    ECL_REASON_CRASH_PENDING,      /**< 충돌 판정 중 */
    ECL_REASON_APPROACH_RISK_HOLD, /**< 접근위험으로 잠금 유지 */
    ECL_REASON_OVERRIDE,           /**< 접근위험 억제 override */
    ECL_REASON_FIRE,               /**< 화재로 해제 */
    ECL_REASON_OVERTEMP,           /**< 과온으로 해제 */
    ECL_REASON_ADULT_PRESENT,      /**< 성인 탑승으로 해제 */
    ECL_REASON_IGNITION_OFF,       /**< 점화 꺼짐으로 해제 */
    ECL_REASON_ISOFIX_LEFT,        /**< 좌 ISOFIX 로 좌 잠금 */
    ECL_REASON_ISOFIX_RIGHT,       /**< 우 ISOFIX 로 우 잠금 */
    ECL_REASON_AUTO_LOCK_MOVING,   /**< 이동 시작으로 잠금 */
    ECL_REASON_DRIVER_CMD,         /**< 운전자 명령 적용 */
    ECL_REASON_CMD_REJECTED_INVALID, /**< 형식 오류 명령 거절 */
    ECL_REASON_CMD_REJECTED_EXTRA,   /**< 같은 주기의 추가 명령 거절 */
    ECL_REASON_CMD_REJECTED_MOVING,  /**< 이동 중 명령 미적용 */
    ECL_REASON_INPUT_INVALID,      /**< 입력 INVALID */
    ECL_REASON_INPUT_STALE         /**< 입력 stale */
} ecl_reason_t;

/**
 * @brief   경고코드: sensor_fault 가 유효한 TRUE.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_WARN_SENSOR_FAULT 0x1u

/**
 * @brief   경고코드: 필수 입력이 INVALID.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_WARN_INPUT_INVALID 0x2u

/**
 * @brief   경고코드: 입력 묶음이 stale.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#define ECL_WARN_INPUT_STALE 0x4u

/**
 * @brief   운전자 명령 채택 결과.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int accepted;           /**< 1 이면 유효한 명령 1건을 채택함 */
    ecl_side_t side;        /**< 채택한 명령의 대상 */
    ecl_action_t action;    /**< 채택한 명령의 동작 */
    ecl_source_t source;    /**< 채택한 명령의 입력 경로 */
    double request_time_s;  /**< 채택한 명령의 요청 시각 [s] */
    ecl_reason_t note;      /**< NONE 또는 CMD_REJECTED_* */
    size_t extra_rejected;  /**< 같은 주기에 거절한 추가 명령 수 */
} ecl_command_result_t;

/**
 * @brief   상태 판정 결과.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_state_t state;  /**< 제어 상태 */
    unsigned warnings;  /**< ECL_WARN_* 의 비트 합 */
} ecl_status_result_t;

/**
 * @brief   규칙 평가 결정 (공개 출력).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_output_t left;              /**< 좌 도어 출력 */
    ecl_output_t right;             /**< 우 도어 출력 */
    ecl_priority_t priority_reason; /**< 출력을 정한 규칙 */
    ecl_reason_t reason_code;       /**< 이유코드 */
    ecl_reason_t command_note;      /**< 명령 거절·미적용 이유 */
} ecl_decision_t;

/**
 * @brief   평가주기 한 번의 공개 결과.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_decision_t decision;  /**< 출력과 이유 */
    ecl_state_t state;        /**< 제어 상태 */
    unsigned warnings;        /**< 경고코드 비트 합 */
} ecl_cycle_result_t;

/**
 * @brief   도어 하나의 접근위험 억제 상태 (override 판정용 기억, REQ-F-023).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    int has_first;          /**< 1 이면 첫 억제된 해제 요청의 시각을 기억함 */
    double first_time_s;    /**< 첫 억제된 해제 요청의 시각 [s] */
    int override_active;    /**< 1 이면 override 상태 (접근위험이 FALSE 가 될 때까지) */
} ecl_door_memory_t;

/**
 * @brief   규칙 엔진이 평가주기 사이에 기억하는 상태 (좌/우 도어별).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
typedef struct
{
    ecl_door_memory_t left;   /**< 좌 도어 */
    ecl_door_memory_t right;  /**< 우 도어 */
} ecl_rule_memory_t;

#ifdef __cplusplus
}
#endif

#endif /* ECL_ECL_TYPES_H */
