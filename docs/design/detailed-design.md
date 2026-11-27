# 상세 설계 — 전자식 차일드락 제어 SW (SWE.3)

- 문서 ID: SWD-ECL-001, 버전 0.1 (이터레이션 1, OEM 게이트 G2 대상)
- 입력: `docs/architecture/software-architecture.md` v0.1
- 작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료. 리뷰: 이터레이션 1~2 면제.
- 범위: 이터레이션 1~3 구현 대상 모듈(ARCH-C-001, 002, 003, 004(이터 3: 규칙 1~8·HOLD·override), 006, 009). 나머지 컴포넌트는 해당 이터레이션에서 추가한다.
- "특정 수준"(인터페이스 정의 대상 모듈) 기준은 CLAUDE.md 7절에 미정이므로 임시로 스킬 기본값(컴포넌트 경계 인터페이스를 구현하는 모듈)을 쓴다. 이 문서의 모든 모듈은 ARCH-I 를 구현하므로 모두 인터페이스 정의 대상이다. `static` 보조 함수는 비공개이며 인터페이스가 없다.

## 1. 공통 규칙

- C17, 모든 함수는 `ecl_status_t` 를 반환한다 (오류 값: `ECL_OK`, `ECL_ERR_NULL_ARG`). 단일 return 을 함수 끝에서 한 번만 쓴다.
- 오류 반환이 가능한 모든 공개 함수는 `ECL_NODISCARD`(`__attribute__((warn_unused_result))`)를 붙인다.
- 한 결정(조건식)의 조건은 6개 이하, 함수는 CCN 10 이하·80라인 이하.
- 널 인자 검사는 각 공개 함수의 시작에서 결과 변수에 반영하고 본 처리는 `else` 로 감싼다 (조기 return 금지).
- 출력 인자는 오류 시 변경하지 않는다.

### 데이터 형식 (DD-M-009 ecl_types.h, ARCH-I-009)

```c
typedef enum { ECL_OK = 0, ECL_ERR_NULL_ARG = 1, ECL_ERR_RANGE = 2 } ecl_status_t;

typedef struct { int present; double value; } ecl_raw_num_t;
typedef struct { int present; int value; } ecl_raw_int_t;

/* 원시 코드의 의미 */
typedef enum { ECL_GEAR_P = 0, ECL_GEAR_N, ECL_GEAR_D, ECL_GEAR_R } ecl_gear_t;
typedef enum { ECL_CRASH_NONE = 0, ECL_CRASH_PENDING, ECL_CRASH_CONFIRMED } ecl_crash_t;
typedef enum { ECL_SIDE_LEFT = 0, ECL_SIDE_RIGHT, ECL_SIDE_ALL } ecl_side_t;
typedef enum { ECL_ACTION_LOCK = 0, ECL_ACTION_UNLOCK } ecl_action_t;
typedef enum { ECL_SRC_PHYSICAL_BUTTON = 0, ECL_SRC_AVN, ECL_SRC_VOICE, ECL_SRC_MOBILE_APP } ecl_source_t;

typedef struct {                       /* 원시 입력 묶음 (OEM-IF-001~003, 007~009) */
    ecl_raw_num_t speed_kph;  ecl_raw_int_t gear;  ecl_raw_num_t timestamp_s;
    ecl_raw_int_t crash_status;
    ecl_raw_int_t rear_left_risk;  ecl_raw_int_t rear_right_risk;
    ecl_raw_int_t fire;  ecl_raw_int_t overtemp;  ecl_raw_int_t adult_present;
    ecl_raw_int_t isofix_left;  ecl_raw_int_t isofix_right;
    ecl_raw_int_t ignition_on;  ecl_raw_int_t sensor_fault;
} ecl_raw_snapshot_t;

typedef struct { ecl_raw_int_t side; ecl_raw_int_t action; ecl_raw_int_t source; } ecl_raw_command_t;

typedef struct { int valid; double value; } ecl_val_num_t;
typedef struct { int valid; int value; } ecl_val_int_t;   /* 불리언은 value 0/1 */

typedef struct {                       /* 검증 입력 */
    ecl_val_num_t speed_kph;  ecl_val_int_t gear;  ecl_val_num_t timestamp_s;
    ecl_val_int_t crash_status;
    ecl_val_int_t rear_left_risk;  ecl_val_int_t rear_right_risk;
    ecl_val_int_t fire;  ecl_val_int_t overtemp;  ecl_val_int_t adult_present;
    ecl_val_int_t isofix_left;  ecl_val_int_t isofix_right;
    ecl_val_int_t ignition_on;  ecl_val_int_t sensor_fault;
    int stale;        /* 1 = 신선하지 않음 */
    int all_valid;    /* 1 = 모든 필수 입력이 유효하고 신선함 */
} ecl_validated_t;

typedef enum { ECL_OUT_RELEASE = 0, ECL_OUT_LOCK = 1 } ecl_output_t;
typedef enum { ECL_STATE_ACTIVE = 0, ECL_STATE_DEGRADED, ECL_STATE_FAULT, ECL_STATE_OFF } ecl_state_t;
typedef enum { ECL_PRIO_SENSOR_FAULT = 1, ECL_PRIO_CRASH, ECL_PRIO_APPROACH, ECL_PRIO_EMERGENCY_RELEASE,
               ECL_PRIO_IGNITION_OFF, ECL_PRIO_ISOFIX, ECL_PRIO_AUTO_LOCK, ECL_PRIO_DRIVER_CMD,
               ECL_PRIO_HOLD } ecl_priority_t;
typedef enum { ECL_REASON_NONE = 0, ECL_REASON_SENSOR_FAULT, ECL_REASON_CRASH_CONFIRMED, ECL_REASON_CRASH_PENDING,
               ECL_REASON_APPROACH_RISK_HOLD, ECL_REASON_OVERRIDE, ECL_REASON_FIRE, ECL_REASON_OVERTEMP,
               ECL_REASON_ADULT_PRESENT, ECL_REASON_IGNITION_OFF, ECL_REASON_ISOFIX_LEFT, ECL_REASON_ISOFIX_RIGHT,
               ECL_REASON_AUTO_LOCK_MOVING, ECL_REASON_DRIVER_CMD, ECL_REASON_CMD_REJECTED_INVALID,
               ECL_REASON_CMD_REJECTED_EXTRA, ECL_REASON_CMD_REJECTED_MOVING, ECL_REASON_INPUT_INVALID,
               ECL_REASON_INPUT_STALE } ecl_reason_t;       /* SRS 부록 A */
#define ECL_WARN_SENSOR_FAULT 0x1u
#define ECL_WARN_INPUT_INVALID 0x2u
#define ECL_WARN_INPUT_STALE 0x4u

typedef struct { int accepted; ecl_side_t side; ecl_action_t action; ecl_source_t source;
                 double request_time_s; ecl_reason_t note; size_t extra_rejected; } ecl_command_result_t;
typedef struct { ecl_state_t state; unsigned warnings; } ecl_status_result_t;
typedef struct { ecl_output_t left; ecl_output_t right; ecl_priority_t priority_reason;
                 ecl_reason_t reason_code; ecl_reason_t command_note; } ecl_decision_t;
typedef struct { ecl_decision_t decision; ecl_state_t state; unsigned warnings; } ecl_cycle_result_t;
```

`command_note` 는 부록 A의 CMD_REJECTED_* 코드 또는 NONE 이다.

## 2. 모듈 (DD-M)

| ID | 모듈 | 책임 (한 문장) | 소속 | 할당 요구사항 | 인터페이스 |
|---|---|---|---|---|---|
| DD-M-001 | input_validation | 원시 입력을 검사해 필드별 유효성·신선도가 있는 검증 입력을 만든다. | ARCH-C-001 | REQ-F-001~005, REQ-S-003, REQ-S-008, REQ-Q-002, REQ-Q-004 | DD-I-001 |
| DD-M-002 | command_intake | 원시 운전자 명령에서 첫 유효 명령 1건을 채택하고 거절 이유를 만든다. | ARCH-C-002 | REQ-F-006~009, 018, 019 | DD-I-002 |
| DD-M-003 | status_eval | 검증 입력에서 상태와 경고코드를 정한다. | ARCH-C-003 | REQ-F-014, REQ-S-003, REQ-S-009 | DD-I-003 |
| DD-M-004 | rule_engine | 규칙표 순위에 따라 도어별 출력과 이유코드를 결정한다 (이터 1: 규칙 1, HOLD / 이터 2: 규칙 1~3, HOLD / 이터 3: 규칙 1~8, HOLD, override). | ARCH-C-004 | REQ-F-010, 011, 012, 017, 020, 028, 037; REQ-S-004 | DD-I-004 |
| DD-M-006 | controller | 평가주기마다 모듈을 순서대로 호출하고 확정 출력을 보관한다. | ARCH-C-006 | REQ-F-013, 015, 016, REQ-Q-001, REQ-Q-003 | DD-I-006 |
| DD-M-009 | ecl_types | 공유 데이터 형식만 정의한다 (동작 없음). | ARCH-C-009 | REQ-C-001, REQ-C-005 | DD-I-009 |

모듈 간 의존: controller → {input_validation, command_intake, status_eval, rule_engine} 의 공개 헤더, 모든 모듈 → ecl_types. 다른 의존 없음.

## 3. 인터페이스와 함수 설계

### DD-M-001 input_validation (DD-I-001 = ARCH-I-001)

```c
ECL_NODISCARD ecl_status_t ecl_input_validate(const ecl_raw_snapshot_t* raw, double eval_time_s,
                                              ecl_validated_t* out);
```

- 반환: `ECL_OK`, raw 또는 out 이 널이면 `ECL_ERR_NULL_ARG`(out 변경 없음).
- 처리: ① 각 필드를 검사 ② timestamp 가 INVALID 이면 모든 필드 `valid=0`, `stale=0`(신선도 판단 불가), `all_valid=0` ③ timestamp 가 유효하고 `eval_time_s - timestamp > 0.2` 이면 모든 필드 `valid=0`, `stale=1`, `all_valid=0` ④ 아니면 `all_valid` 는 모든 필드의 valid 의 논리곱. timestamp 가 eval_time 보다 미래이면 나이가 음수이므로 신선하다(가정 A-05, 별도 처리 없음).
- 범위: speed 0.0~300.0 (경계 포함), gear 0~3, crash 0~2, 불리언 0~1, timestamp 는 유한 수치.

| 함수 (static) | 책임 | 반환 |
|---|---|---|
| `check_num_range(raw, lo, hi, out)` | 수치 필드를 `present` 와 유한성과 [lo,hi]로 검사 | `ecl_val_num_t` 값 반환이 아니라 `ecl_status_t`; 결과는 `out` |
| `check_int_range(raw, lo, hi, out)` | 정수 코드 필드를 `present` 와 [lo,hi]로 검사 | `ecl_status_t` |
| `is_finite_number(v)` | `(v - v) == 0.0` 로 NaN/무한대 배제 | int(0/1) — 내부 계산 도우미(상태 의미 없음, 설계 예외로 int 반환) |
| `invalidate_all(out, stale)` | 모든 필드 valid=0, stale 설정, all_valid=0 | `ecl_status_t` |
| `compute_all_valid(out)` | 필드 valid 의 논리곱을 `out->all_valid` 에 설정 | `ecl_status_t` |

`is_finite_number` 는 bool 의미의 술어이므로 상태 반환 규칙의 예외(설계 문서 기록, 구현 규칙 "언어상 불가피한 경우"에 준함)로 둔다.

### DD-M-002 command_intake (DD-I-002 = ARCH-I-002)

```c
ECL_NODISCARD ecl_status_t ecl_command_intake(const ecl_raw_command_t* cmds, size_t count,
                                              double request_time_s, ecl_command_result_t* out);
```

- 반환: `ECL_OK`; out 이 널이면 `ECL_ERR_NULL_ARG`; `count > 0` 인데 cmds 가 널이면 `ECL_ERR_NULL_ARG`.
- 처리: `out` 을 `accepted=0, note=NONE, extra_rejected=0` 으로 초기화 → `count == 0` 이면 종료(명령 없음) → 첫 명령을 검사: side(0~2), action(0~1), source(0~3) 모두 `present` 이고 범위 안이어야 `accepted=1` 과 값, `request_time_s` 복사; 아니면 `note=CMD_REJECTED_INVALID` → `count > 1` 이면 `extra_rejected = count - 1` 이고, 첫 명령이 유효하면 `note=CMD_REJECTED_EXTRA`(첫 명령이 무효이면 INVALID 를 유지).
- 정수 코드 검사는 `static check_code(const ecl_raw_int_t*, int lo, int hi)` 가 `present` 와 범위로 판정한다 (술어, 설계 예외).

### DD-M-003 status_eval (DD-I-003 = ARCH-I-003)

```c
ECL_NODISCARD ecl_status_t ecl_status_evaluate(const ecl_validated_t* in, ecl_status_result_t* out);
```

- 반환: `ECL_OK`; 널이면 `ECL_ERR_NULL_ARG`.
- 처리(상태표 1.4절): `sensor_fault.valid && value==1` → FAULT, 경고 SENSOR_FAULT. 아니면 `!all_valid` → DEGRADED, 경고 `stale ? INPUT_STALE : INPUT_INVALID`. 아니면 `ignition_on.valid && value==0` → OFF. 아니면 ACTIVE. FAULT 와 DEGRADED 는 독립이므로 두 경고가 함께 올라갈 수 있다: FAULT 판정 후에도 `!all_valid` 이면 INPUT_* 경고를 추가한다 (상태는 FAULT 유지).
- 보조 `static is_true(const ecl_val_int_t*)` : `valid && value == 1` 판정, `static warnings_for_invalid(in)`.

### DD-M-004 rule_engine (DD-I-004 = ARCH-I-004, 이터 1 범위)

```c
ECL_NODISCARD ecl_status_t ecl_rule_decide(const ecl_validated_t* in, const ecl_command_result_t* cmd,
                                           const ecl_decision_t* confirmed, ecl_decision_t* out);
```

(`ecl_rule_memory_t* memory` 인자는 override 를 구현하는 이터레이션 3에서 추가하고 ARCH-I-004 를 개정한다.)

- 반환: `ECL_OK`; 널이면 `ECL_ERR_NULL_ARG`.
- 규칙 함수 형식: `static int rule_xxx(const ecl_validated_t*, const ecl_command_result_t*, const ecl_decision_t* confirmed, ecl_rule_effect_t* effect)`; `ecl_rule_effect_t` 는 `applies_left`, `applies_right`, `out_left`, `out_right`, `priority`, `reason`. 반환 1 = 규칙 적용(효과 있음), 0 = 미적용 (술어 성격의 내부 함수, 설계 예외).
- 규칙표: `static const rule_fn k_rules[]`(순위 순). 이터 1: `rule_sensor_fault` 만. `decide_door(...)` 가 각 도어에 대해 표를 순위 순으로 훑어 처음 적용되는 규칙의 효과를 채택한다. 어느 규칙도 효과가 없으면 HOLD: 확정 출력 유지, priority=HOLD, reason = `!in->all_valid ? (in->stale ? INPUT_STALE : INPUT_INVALID) : NONE`.
- `rule_sensor_fault`: `sensor_fault.valid && value==1` 이면 양 도어 `applies`, 출력은 `confirmed` 값, priority=SENSOR_FAULT, reason=SENSOR_FAULT.
- 결정의 priority_reason/reason_code 는 효과를 낸 규칙 중 순위가 가장 높은 규칙의 값. `command_note = cmd->note`.
- 도어별로 독립 결정한다 (REQ-F-017).

### DD-M-006 controller (DD-I-006 = ARCH-I-006)

```c
typedef struct { ecl_decision_t confirmed; } ecl_controller_t;
ECL_NODISCARD ecl_status_t ecl_controller_init(ecl_controller_t* ctl);
ECL_NODISCARD ecl_status_t ecl_controller_step(ecl_controller_t* ctl, const ecl_raw_snapshot_t* raw,
        const ecl_raw_command_t* cmds, size_t count, double eval_time_s, ecl_cycle_result_t* out);
```

- `init`: 좌/우 RELEASE, priority HOLD, reason NONE, note NONE (REQ-F-015). 널이면 `ECL_ERR_NULL_ARG`.
- `step`: 널 검사 → validate(raw, eval_time_s) → command_intake(cmds, count, eval_time_s) → status_evaluate → rule_decide(…, &ctl->confirmed, &decision) → `ctl->confirmed = decision` → `out` 채움. 각 호출의 반환이 `ECL_OK` 가 아니면 이후 호출을 건너뛰고 그 상태를 반환한다 (`while`/상태 변수 구조, 조기 return 금지). 요청 시각은 `eval_time_s` 가 아니라 입력 묶음의 timestamp 를 써야 하지만(REQ-F-008) override 를 구현하는 이터레이션 3 이전에는 사용하지 않으므로 이터 1 은 검증된 timestamp 값(`validated.timestamp_s.value`)을 command_intake 의 `request_time_s` 로 넘기고, timestamp 가 INVALID이면 0.0 을 넘긴다.
- 출력은 오류 시 갱신하지 않는다 (확정 출력 보존).

## 4. 클린코드·SRP 점검

| 점검 | 결과 |
|---|---|
| 함수 책임 한 가지 | 검증/채택/상태/규칙/조정을 모듈로 분리, 필드 검사는 보조 함수로 분리 |
| CCN ≤ 10, 80라인 | `ecl_input_validate` 는 필드 검사를 보조 함수에 위임하고 본체는 호출 나열(CCN ≤ 5), `ecl_status_evaluate` 는 조건 4개 이하 |
| 조건 ≤ 6 | `check_num_range`: present, 유한, ≥lo, ≤hi (4). 기타 3 이하 |
| 인자 ≤ 3 권장 | 공개 인터페이스는 구조체로 묶었으나 `ecl_controller_step` 6개와 `ecl_rule_decide` 4개는 ARCH-I 시그니처의 데이터 결합 때문에 예외로 둔다 (설계 결정 AD-01) |
| 매직 넘버 | `ECL_STALE_LIMIT_S`(0.2), `ECL_SPEED_MAX_KPH`(300.0), `ECL_SPEED_MIN_KPH`(0.0) 상수 |

## 5. 추적성

요구사항 ↔ 모듈은 `docs/traceability/trace-matrix.md` 의 DD 열에 반영한다.

## 6. 변경 이력

### 이터레이션 2 (rule_engine, DD-M-004)

- 규칙 형식을 도어별 효과로 확장했다: `rule_fn` 은 `rule_effect_t`(`applies_left`, `applies_right`, 도어별 출력, priority, reason)를 값으로 돌려준다. 규칙이 없으면 효과 구조체가 0 이다. (이터레이션 1 의 "적용 여부 int + 출력 인자" 형식을 대체. 이유: 규칙 3 이 한쪽 도어에만 효과를 내므로 REQ-F-017 도어 독립을 표현해야 한다.)
- `find_rule(in, confirmed, door, effect)` 가 규칙표를 순위 순으로 훑어 해당 도어(좌, 우, 어느 한쪽)에 처음 효과를 내는 규칙을 찾는다. 도어 출력은 좌/우 각각의 승자에서, priority_reason/reason_code 는 "어느 한쪽 도어에 효과를 내는 최상위 규칙"에서 가져온다. 효과를 내는 규칙이 없으면 HOLD.
- 규칙 2 `rule_crash`: `crash_status` 가 유효한 CONFIRMED 이면 좌/우 RELEASE, priority CRASH, reason CRASH_CONFIRMED (REQ-S-001, REQ-S-005; 확정인 동안 매 주기 같은 결과이므로 유지된다).
- 규칙 3 `rule_approach`: 좌/우 접근위험이 각각 유효한 TRUE 이면 해당 도어 LOCK, priority APPROACH, reason APPROACH_RISK_HOLD (REQ-S-002, S-006, S-007). 해제 요청은 규칙 8(이터 3)이 순위가 낮아 적용되지 않으므로 억제된다.
- HOLD 이유(`hold_reason`): `all_valid == 0` 이면 INPUT_STALE/INPUT_INVALID, 아니면 `crash_status == PENDING` 이면 CRASH_PENDING, 아니면 NONE (REQ-F-029, 입력 오류 이유가 우선).
- `ecl_rule_decide` 시그니처는 그대로다. override 메모리 인자는 이터레이션 3 에서 추가한다.

### 이터레이션 3 (rule_engine, controller)

- **시그니처**: `ecl_rule_decide(in, cmd, confirmed, memory, out)` — `ecl_rule_memory_t* memory`(좌/우 도어별 `has_first`, `first_time_s`, `override_active`) 인자를 추가했다 (ARCH-I-004 와 일치). 규칙 함수는 `rule_context_t`(in, cmd, confirmed, 이번 주기가 반영된 memory)를 받아 `rule_effect_t` 를 돌려준다.
- **제어기**: `ecl_controller_t` 가 `memory` 를 보관한다. 평가주기는 memory 의 사본으로 계산하고 모든 단계가 성공했을 때만 확정 결정과 memory 를 함께 갱신한다 (오류 주기는 어느 쪽도 바꾸지 않는다).
- **규칙 4~8**: 4 EMERGENCY_RELEASE(fire > overtemp > adult 순으로 이유를 정함), 5 IGNITION_OFF, 6 ISOFIX(도어별, 이유는 좌 우선), 7 AUTO_LOCK(유효 속도 ≥ 3.0), 8 DRIVER_CMD(채택된 명령, 모든 필수 입력 유효, 정지; 대상 도어에만 적용).
- **해석 결정**
  - 규칙 4 는 개별 신호가 유효한 TRUE 이면 적용된다. 다른 신호가 INVALID 여도 막지 않는다 (REQ-F-025). SRS 1.3절에 명시했다.
  - 규칙 8 은 REQ-F-021 문구대로 모든 필수 입력이 유효할 때만 적용한다 (기존 표의 "속도, 명령"을 정정). 이유: 접근위험 같은 안전 입력이 INVALID 인데 해제 명령을 적용하는 것을 막는다.
  - 이동 중에 채택된 명령은 `command_note = CMD_REJECTED_MOVING` (REQ-F-035). 속도가 INVALID 이면 이동 여부를 알 수 없으므로 적용도 노트도 하지 않는다.
- **override (REQ-F-023, REQ-F-036)**
  - "해제 요청" = 채택된 UNLOCK 명령 + 모든 필수 입력 유효 + 정지. 접근위험이 유효한 TRUE 인 도어가 대상에 포함되면 첫 요청 시각(명령의 요청 시각 = 입력 timestamp)을 기억하고(규칙 3 이 LOCK 유지, APPROACH_RISK_HOLD), 이후 요청이 첫 요청보다 늦고 10.0 s 이하이면 override 가 된다. 10.0 s 를 넘기면 새 첫 요청으로 보며, 같은 시각이거나 과거이면 변화 없음.
  - override 상태의 도어는 규칙 3 에서 제외되어 그 주기에 규칙 8 이 RELEASE 를 적용하고 이유는 OVERRIDE. 이후 주기에는 새 규칙이 없으면 RELEASE 가 유지(HOLD)되고, 이동을 시작하면 규칙 7 이 다시 LOCK 한다 (override 가 이동 중 해제를 유지하지 않도록 한 선택, OQ-02 와 함께 OEM 확인 필요).
  - 접근위험이 유효한 FALSE 가 되면 기억과 override 를 지운다. INVALID 이면 기억을 바꾸지 않는다.
- **기본 정상 입력 변경**: 시험의 `ValidRaw()` 속도를 10 → 0 km/h(정지)로 바꿨다. 규칙 7 이 구현되면서 10 km/h 는 자동 잠금을 일으키기 때문이다.
