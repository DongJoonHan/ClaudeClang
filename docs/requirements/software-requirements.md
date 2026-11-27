# 소프트웨어 요구사항 명세 (SRS) — 전자식 차일드락 제어 SW

- 문서 ID: SRS-ECL-001, 버전 0.2 (이터레이션 1, OEM 게이트 G1 대상, 베이스라인 BL-SWR-1.0 후보)
- 입력 문서: `OEM/OEM-SWR-001_OEM SW 요구사항 사양서.docx` (Rev 1.0, 이하 "OEM 사양서"). OEM 사양서의 실제 승인 상태는 "미수행"(교육 시나리오 베이스라인)이다. 이 SRS의 베이스라인도 같은 한계를 가진다.
- 작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료.
- 기능 안전 요구사항(ASIL B)은 `docs/safety/safety-requirements.md`(`REQ-S-`)에서 관리하고 이 문서에는 참조만 둔다. 같은 동작은 한 문서에만 정본으로 둔다 (REQ-S-008, REQ-F-007 참조).
- 사용 사례는 `docs/requirements/use-cases.md`, 변경 요청은 `docs/requirements/change-requests.md`에 둔다.
- 추적성은 `docs/traceability/trace-matrix.md`에 둔다.
- ID 결번: REQ-F 번호는 구조상 연속하지 않는다. 결번은 삭제가 아니라 사용하지 않은 번호이며 해당 번호는 재사용하지 않는다.

## 0. 적용 범위와 용어

적용 범위(OEM 사양서 1.2): 대한민국 판매용 2026년식 가상 OEM-A 승용차의 후석 좌/우 전자식 차일드락 제어. SW-only PC/SIL 범위이며 HARA, ECU/HW, HIL, 실차, 인증은 제외한다 (REQ-L-001~004).

우선순위 범례: 높음 = 안전·OEM 수용기준에 직접 관련, 중간 = 기능 완성에 필요, 낮음 = 보조 기능.

| 용어 | 정의 |
|---|---|
| 충돌(collision) | 차량 충돌 사건. 입력 `crash_status` 가 나타낸다. 규칙 사이의 모순은 "상충(conflict)"이라 쓴다. |
| 유효 수치 | NaN과 무한대가 아닌 실수 |
| 필수 입력 | OEM-IF-001(vehicle_speed_kph, gear, source_timestamp_s), IF-002, IF-003, IF-007, IF-008, IF-009의 모든 항목 |
| INVALID | 입력값이 형식·범위 요구를 만족하지 못하거나 누락된 것 (신호 입력에 사용하는 용어). 운전자 명령에는 "거절"을 쓴다. |
| 유효 | INVALID가 아니고 신선한 입력 |
| 신선하지 않음(stale) | 입력 묶음의 source_timestamp_s 와 평가 시각의 차가 200 ms 를 초과함 (가정 A-05) |
| 평가주기 | SW가 입력 묶음 하나와 운전자 명령 0건 또는 1건을 읽어 출력을 한 번 결정하는 반복 단위 (REQ-F-013) |
| 확정 출력 | 직전 평가주기에 공개된 좌/우 출력 |
| 출력 | 좌/우 각각 LOCK 또는 RELEASE (OEM-IF-005) |
| 정지 | 유효한 vehicle_speed_kph 가 3.0 km/h 미만 (가정 A-06) |
| 이동 | 유효한 vehicle_speed_kph 가 3.0 km/h 이상 |
| 적용 가능한 규칙 | 그 규칙이 사용하는 입력이 모두 유효한 규칙 (REQ-F-010) |
| 결정 | 출력, 상태, 이유코드 중 하나가 직전 평가주기와 달라진 평가주기 하나 (가정 A-13) |
| 상태 | ACTIVE, DEGRADED, FAULT, OFF 중 하나 (REQ-F-014) |

## 1. 기능 요구사항 (REQ-F)

### 1.1 입력 검증 (OEM-IF-001~009; 안전 요구 REQ-S-003, REQ-S-008 참조)

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-F-001 | 기능 | SW는 vehicle_speed_kph 가 0.0 이상 300.0 이하의 유효 수치가 아니거나 누락되면 그 값을 INVALID로 해야 한다. | OEM-IF-001 | 높음 | 시험 | 초안 |
| REQ-F-002 | 기능 | SW는 gear 가 P, N, D, R 중 하나가 아니거나 누락되면 그 값을 INVALID로 해야 한다. | OEM-IF-001 | 높음 | 시험 | 초안 |
| REQ-F-003 | 기능 | SW는 source_timestamp_s 가 유효 수치가 아니거나 누락되면 그 값을 INVALID로 해야 한다. 이 값이 INVALID이면 입력 묶음의 신선도를 판단할 수 없으므로 같은 묶음의 모든 값을 INVALID로 한다. | OEM-IF-001 | 높음 | 시험 | 초안 |
| REQ-F-004 | 기능 | SW는 crash_status 가 NONE, PENDING, CONFIRMED 중 하나가 아니면(누락·미정 값 포함) 그 값을 INVALID로 해야 한다. | OEM-IF-002 | 높음 | 시험 | 초안 |
| REQ-F-005 | 기능 | SW는 rear_left_approach_risk, rear_right_approach_risk, fire_detected, overtemperature_detected, adult_present, isofix_left, isofix_right, ignition_on, sensor_fault 각각이 불리언이 아니거나 누락되면 그 값을 INVALID로 해야 한다. | OEM-IF-003, 007, 008, 009 | 높음 | 시험 | 초안 |
| REQ-F-006 | 기능 | SW는 운전자 명령의 side 가 left, right, all 이 아니거나 누락되면 그 명령을 거절해야 한다. | OEM-IF-004 | 높음 | 시험 | 초안 |
| REQ-F-018 | 기능 | SW는 운전자 명령의 action 이 lock, unlock 이 아니거나 누락되면 그 명령을 거절해야 한다. | OEM-IF-004 | 높음 | 시험 | 초안 |
| REQ-F-019 | 기능 | SW는 운전자 명령의 source 가 physical_button, avn, voice, mobile_app 이 아니거나 누락되면 그 명령을 거절해야 한다. | OEM-IF-004 | 높음 | 시험 | 초안 |
| REQ-F-007 | 기능 | SW는 거절된 운전자 명령을 규칙 평가에 사용하지 않아야 하고, 거절 이유코드(CMD_REJECTED_INVALID)를 기록해야 한다. | OEM-IF-004 | 높음 | 시험 | 초안 |
| REQ-F-008 | 기능 | SW는 운전자 명령의 요청 시각으로 같은 평가주기 입력 묶음의 source_timestamp_s 를 사용해야 한다 (사용처: REQ-F-023 의 10초 판정). OEM-IF-004의 `VehicleSnapshot.timestamp_s` 와 OEM-IF-001의 `source_timestamp_s` 를 같은 값으로 본다 (가정 A-14). | OEM-IF-004 | 중간 | 시험 | 초안 |
| REQ-F-009 | 기능 | SW는 한 평가주기에 운전자 명령이 2건 이상 들어오면 첫 1건만 평가에 사용하고 나머지는 거절 이유코드 CMD_REJECTED_EXTRA 로 기록해야 한다 (가정 A-09). | OEM-IF-004 | 중간 | 시험 | 초안 |

### 1.2 규칙 적용과 출력 결정 (OEM-IF-005, OEM-IF-006)

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-F-010 | 기능 | SW는 각 규칙을 그 규칙이 사용하는 입력이 모두 유효할 때만 적용 가능한 것으로 취급해야 한다 (사용 입력은 1.3절 표). | OEM-SR-003 | 높음 | 시험 | 초안 |
| REQ-F-011 | 기능 | SW는 적용 가능한 규칙이 어느 도어에도 효과를 내지 않으면 그 도어의 출력을 확정 출력으로 유지해야 한다 (가정 A-04). | OEM-SR-003 | 높음 | 시험 | 초안 |
| REQ-F-012 | 기능 | SW는 한 도어에 효과를 내는 적용 가능한 규칙이 여러 개이면 1.3절 표에서 순위가 가장 높은 규칙의 효과를 그 도어의 출력으로 해야 한다 (가정 A-02). | OEM-SR-001, OEM-SR-002 | 높음 | 시험 | 초안 |
| REQ-F-013 | 기능 | SW는 10 ms 평가주기(가정 A-01)마다 입력 묶음을 평가해 출력과 상태를 갱신해야 한다. | OEM-SR-003 | 높음 | 시험 | 초안 |
| REQ-F-014 | 기능 | SW는 매 평가주기에 상태를 1.4절 표의 우선순서(FAULT, DEGRADED, OFF, ACTIVE)로 정확히 하나 정해 공개해야 한다. 조건이 해소되면 다음 평가주기에 자동으로 복귀한다 (가정 A-08). | OEM-IF-006, OEM-SR-003, OEM-SR-004, OEM-FR-007 | 높음 | 시험 | 초안 |
| REQ-F-015 | 기능 | SW는 전원 투입 직후 첫 평가주기 전에 좌/우 출력을 RELEASE로 해야 한다 (가정 A-12). | 가정 A-12 | 중간 | 시험 | 초안 |
| REQ-F-016 | 기능 | SW는 매 평가주기에 lock_left 와 lock_right 를 LOCK 또는 RELEASE 값으로 공개해야 한다. | OEM-IF-005 | 높음 | 시험 | 초안 |
| REQ-F-017 | 기능 | SW는 좌 출력과 우 출력을 각각 독립적으로 결정해야 한다. | OEM-IF-005, OEM-FR-006 | 높음 | 시험 | 초안 |

### 1.3 규칙표 (정본)

규칙 순위 1이 최상위다. "효과"는 그 규칙이 적용될 때 해당 도어에 내리는 결정이다. 규칙의 사용 입력이 하나라도 INVALID이면 그 규칙은 적용 가능하지 않다 (REQ-F-010).

| 순위 | 규칙 | 사용 입력 | 조건 | 효과 | 요구사항 |
|---|---|---|---|---|---|
| 1 | SENSOR_FAULT | sensor_fault | TRUE | 좌/우 확정 출력 유지 | REQ-S-004 |
| 2 | CRASH | crash_status | CONFIRMED | 좌/우 RELEASE | REQ-S-001, REQ-S-005 |
| 3 | APPROACH | rear_left/right_approach_risk | 해당 도어 TRUE (override 중인 도어 제외) | 해당 도어 LOCK, 해제 요청 억제 | REQ-S-002, REQ-S-006 |
| 4 | EMERGENCY_RELEASE | fire_detected, overtemperature_detected, adult_present | 하나라도 TRUE | 좌/우 RELEASE | REQ-F-025 |
| 5 | IGNITION_OFF | ignition_on | FALSE | 좌/우 RELEASE | REQ-F-027 |
| 6 | ISOFIX | isofix_left, isofix_right | 해당 쪽 TRUE | 해당 도어 LOCK | REQ-F-026 |
| 7 | AUTO_LOCK | vehicle_speed_kph | 3.0 이상 | 좌/우 LOCK | REQ-F-022 |
| 8 | DRIVER_CMD | vehicle_speed_kph, 명령 | 정지이고 유효한 명령 1건 | 명령의 side 가 가리키는 도어에 action 적용 | REQ-F-021 |
| 9 | HOLD | — | 위 규칙이 효과를 내지 않음 | 확정 출력 유지 | REQ-F-011 |

REQ-F-020 은 이 표를 정의한다.

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-F-020 | 기능 | SW는 규칙의 순위와 효과를 1.3절 표에 따라야 한다. | OEM-SR-001, OEM-SR-002, OEM-FR-001~007 | 높음 | 시험 | 초안 |

### 1.4 상태표 (정본)

| 우선순서 | 상태 | 조건 |
|---|---|---|
| 1 | FAULT | sensor_fault 가 유효한 TRUE |
| 2 | DEGRADED | 필수 입력 중 하나라도 INVALID 또는 stale |
| 3 | OFF | ignition_on 이 유효한 FALSE |
| 4 | ACTIVE | 위에 모두 해당하지 않음 |

### 1.5 운전자 기능과 QM 규칙

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-F-021 | 기능 | SW는 정지 상태에서 유효한 운전자 명령의 side(left/right/all)가 가리키는 도어에만 action(lock는 LOCK, unlock는 RELEASE)을 적용해야 한다. | OEM-FR-001 | 높음 | 시험 | 초안 |
| REQ-F-034 | 기능 | SW는 physical_button, avn, voice, mobile_app 네 source 를 같은 결과가 나오도록 동등하게 처리해야 한다. | OEM-FR-001 | 중간 | 시험 | 초안 |
| REQ-F-035 | 기능 | SW는 이동 중에 들어온 유효한 운전자 명령을 적용하지 않고 이유코드 CMD_REJECTED_MOVING 으로 기록해야 한다 (가정 A-07). | OEM-FR-001, OEM-FR-002 | 높음 | 시험 | 초안 |
| REQ-F-022 | 기능 | SW는 유효한 vehicle_speed_kph 가 3.0 이상인 첫 평가주기부터 좌/우 출력을 LOCK으로 해야 한다 (이 규칙보다 순위가 높은 규칙이 효과를 내지 않는 경우). | OEM-FR-002 | 높음 | 시험 | 초안 |
| REQ-F-023 | 기능 | SW는 접근위험으로 억제된 도어에 대해, 첫 억제 후 10초 이하(요청 시각은 REQ-F-008)에 그 도어를 가리키는 해제(unlock) 명령이 다시 입력되면 그 도어를 override 상태로 해야 한다 (충돌하는 OEM 요구: OQ-02, 기준 시점: OQ-09). | OEM-FR-003 | 중간 | 시험 | 초안 |
| REQ-F-036 | 기능 | SW는 override 상태의 도어를 RELEASE로 하고 해당 도어의 접근위험이 FALSE가 될 때까지 규칙 3(APPROACH)을 적용하지 않아야 하며, 이유코드 OVERRIDE 를 기록해야 한다 (잠정, OQ-02 회신 후 확정). | OEM-FR-003 | 중간 | 시험 | 초안 |
| REQ-F-024 | 기능 | SW는 조회 요청에 현재 좌/우 출력, 상태, 입력별 유효성, 최근 결정의 이유코드를 응답으로 제공해야 한다. | OEM-FR-004, OEM-IF-006 | 중간 | 시험 | 초안 |
| REQ-F-025 | 기능 | SW는 fire_detected, overtemperature_detected, adult_present 중 하나가 유효한 TRUE이면 다음 평가주기에 좌/우 출력을 RELEASE로 해야 한다 (이 규칙보다 순위가 높은 규칙이 효과를 내지 않는 경우). | OEM-FR-005 | 높음 | 시험 | 초안 |
| REQ-F-026 | 기능 | SW는 isofix_left 가 유효한 TRUE이면 좌 출력을, isofix_right 가 유효한 TRUE이면 우 출력을 다음 평가주기에 LOCK으로 해야 한다 (이 규칙보다 순위가 높은 규칙이 효과를 내지 않는 경우). 이 규칙은 반대쪽 출력에 효과를 내지 않는다. | OEM-FR-006 | 높음 | 시험 | 초안 |
| REQ-F-027 | 기능 | SW는 ignition_on 이 유효한 FALSE인 첫 평가주기에 좌/우 출력을 RELEASE로 해야 한다 (이 규칙보다 순위가 높은 규칙이 효과를 내지 않는 경우). 상태 OFF 는 REQ-F-014 가 정한다. | OEM-FR-007 | 높음 | 시험 | 초안 |
| REQ-F-029 | 기능 | SW는 crash_status 가 PENDING이면 규칙 2(CRASH)를 적용하지 않고 이유코드 CRASH_PENDING 만 기록해야 한다. 다른 규칙의 적용은 영향받지 않는다 (가정 A-03). | OEM-SR-001 | 중간 | 시험 | 초안 |

### 1.6 이유코드·표시·이력

이유코드 열거는 부록 A가 정본이다.

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-F-028 | 기능 | SW는 결정마다 부록 A의 reason_code 중 하나를 생성해야 한다. | OEM-IF-006 | 중간 | 시험 | 초안 |
| REQ-F-037 | 기능 | SW는 결정마다 priority_reason(출력을 정한 규칙 이름, 1.3절 표)을 생성하고 이를 부록 A의 대응 reason_code 로도 매핑해야 한다. | OEM-IF-006 | 중간 | 시험 | 초안 |
| REQ-F-030 | 기능 | SW는 최근 결정 100건을 메모리에 보존하고 101건째 입력 시 가장 오래된 1건을 제거해야 한다. | OEM-NFR-002 | 중간 | 시험 | 초안 |
| REQ-F-038 | 기능 | SW는 결정 이력을 조회할 때 오래된 것부터 순서대로 제공해야 한다. | OEM-NFR-002 | 중간 | 시험 | 초안 |
| REQ-F-031 | 기능 | SW는 결정 이력에 이벤트 ID, 시각, 좌/우 출력, 상태, reason_code 외의 필드를 저장하지 않아야 한다 (허용 필드 목록은 가정 A-10). | OEM-NFR-002 | 중간 | 시험·검사 | 초안 |
| REQ-F-032 | 기능 | SW는 결정 이력을 프로세스 메모리에만 두고 재기동 후에 남기지 않아야 한다. | OEM-NFR-002 | 중간 | 시험 | 초안 |
| REQ-F-033 | 기능 | SW는 표시 응답을 직렬화하지 못하면 HTTP 500 오류를 반환해야 한다 (Web 시뮬레이터 경로). | OEM-IF-006 | 낮음 | 시험 | 초안 |

## 2. 품질 요구사항 (REQ-Q) — ISO/IEC 25010:2023 (임시, CLAUDE.md 7절 미정 항목)

| ID | 구분 | 품질 특성 > 부특성 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|---|
| REQ-Q-001 | 품질 | 신뢰성 > 무결함성 | SW는 같은 초기 상태와 같은 입력 순서에 대해, 고정 시계로 같은 벡터를 1,000회 재생했을 때 공개 결과(부록 B 정의)의 해시가 1,000회 모두 같아야 한다. | OEM-NFR-001 | 높음 | 시험 | 초안 |
| REQ-Q-002 | 품질 | 보안성 > 기밀성 | SW는 영상, 음성 원본, 개인 식별자를 입력으로 받거나 저장하지 않아야 한다. 근거: 이 정보는 개인정보이며 저장·전달을 하지 않는 것이 기밀성 확보의 최소 수단이다. | OEM-NFR-002, OEM 사양서 2절 | 높음 | 검사·시험 | 초안 |
| REQ-Q-004 | 품질 | 보안성 > 기밀성 | SW는 성인 존재를 불리언 입력(adult_present)으로만 받아야 한다. | OEM 사양서 2절 | 높음 | 검사 | 초안 |
| REQ-Q-003 | 품질 | 성능 효율성 > 시간 거동 | SW는 한 평가주기의 처리를 3 ms 이내에 끝내야 한다 (PC/SIL 호스트 기준, 가정 A-11). | 가정 A-11 | 중간 | 분석·시험 | 초안 |

### 2.1 ISO/IEC 25010:2023 적용 판정

| 특성 > 부특성 | 판정 | 근거 |
|---|---|---|
| 기능 적합성 > 완전성·정확성·적절성 | 적용 | REQ-F 전체가 다룬다 (기능 정확성은 시험으로 검증) |
| 성능 효율성 > 시간 거동 | 적용 | REQ-Q-003, REQ-S-001, REQ-S-003 |
| 성능 효율성 > 자원 활용성·용량 | 적용 | REQ-F-030 이 이력 100건으로 메모리 상한을 정의한다 |
| 호환성 > 공존성·상호운용성 | 해당 없음 | OEM이 정의한 인터페이스 외의 연동 대상이 없다 |
| 상호작용 능력 | 해당 없음 | 양산 HMI는 범위 밖이다 (REQ-L-003) |
| 신뢰성 > 무결함성 | 적용 | REQ-Q-001 |
| 신뢰성 > 가용성·결함 허용성·복구성 | 적용 | REQ-S-003, REQ-S-004, REQ-F-014 (DEGRADED/FAULT 정의와 자동 복귀) |
| 보안성 > 기밀성 | 적용 | REQ-Q-002, REQ-Q-004 |
| 보안성 > 무결성·부인방지·책임추적성·진정성·저항성 | 해당 없음 | OEM이 요구하지 않았고 PC/SIL 교육 범위의 외부 접속 경로가 없다 |
| 유지보수성 > 모듈성·시험성 등 | 적용 | REQ-C-005 (프로젝트 품질 기준) |
| 유연성 > 적응성·설치성·대체성 | 해당 없음 | PC/SIL 단일 환경이다 (REQ-C-002) |
| 안전성 | 적용 | 별도 문서 `docs/safety/safety-requirements.md` |

## 3. 제한 (REQ-L)

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-L-001 | 제한 | SW는 도어 래치와 구동기 HW, CAN/LIN 드라이버, 전기 출력을 제공하지 않는다. | OEM 사양서 3절 | 높음 | 검사 | 초안 |
| REQ-L-002 | 제한 | SW는 좌/우 출력이 구동기에 실제로 적용되었는지(feedback)를 검증하지 않는다. | OEM-IF-005 | 높음 | 검사 | 초안 |
| REQ-L-003 | 제한 | SW는 센서 성능, 진단 설계, 영상 원본 처리, 양산 HMI 승인, 사용성 인증을 포함하지 않는다. | OEM 사양서 3절 | 높음 | 검사 | 초안 |
| REQ-L-004 | 제한 | SW는 HIL, 실차, 타깃 ECU/RTOS 시험을 포함하지 않고, 인증·법규 적합성을 주장하지 않는다. | OEM 사양서 1.3, 7절 | 높음 | 검사 | 초안 |

## 4. 제약사항 (REQ-C)

| ID | 구분 | 요구사항 | 출처 | 우선순위 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|
| REQ-C-001 | 제약 | 제품 코드는 C17로 작성하고 Clang 18 이상으로 컴파일해야 한다. | 사용자 결정 2026-11-27 (CLAUDE.md 4절) | 높음 | 검사 | 초안 |
| REQ-C-002 | 제약 | SW는 PC/SIL 호스트 빌드(Docker 이미지 `claudeclangbuild`)에서 실행되어야 한다. OEM 사양서 3절의 "Python 3.12 PC/SIL"을 바꾸는 변경 요청이다 (`docs/requirements/change-requests.md` CR-001, OEM 승인 대기: OQ-10). | CR-001 | 높음 | 검사 | 초안 |
| REQ-C-003 | 제약 | 표시·조작 환경은 Web 시뮬레이터로 제공하되, 제어 판단은 C 코어만 수행해야 한다. | OEM 사양서 3절 | 중간 | 검사·시험 | 초안 |
| REQ-C-004 | 제약 | SW는 OEM이 제공한 ASIL B 할당을 입력으로 사용하고 HARA나 ASIL 결정 근거를 재구성하지 않아야 한다. | OEM 사양서 2절 | 높음 | 검사 | 초안 |
| REQ-C-005 | 제약 | 개발 산출물과 소스는 CLAUDE.md 2절의 품질 기준을 충족해야 한다. | CLAUDE.md | 높음 | 검사 | 초안 |
| REQ-C-006 | 제약 | ISO 26262-6 활동은 OEM ASIL B 할당 입력을 전제로 선정한 일부만 적용하며 준수·적합을 주장하지 않아야 한다. | OEM 사양서 1.3, 2절 | 중간 | 검사 | 초안 |

## 5. 가정 (OEM 확인 필요)

| ID | 가정 | 근거 | 영향 요구사항 |
|---|---|---|---|
| A-01 | 평가주기는 10 ms 이다. OEM-SR-003의 "100 ms 이내 DEGRADED"는 주기가 100 ms 이하여야 만족된다. | OEM에 주기 정의 없음 | REQ-F-013, REQ-S-003 |
| A-02 | 규칙 순위는 1.3절 표이다 (ASIL B 입력이 QM보다 높고, sensor_fault 가 최상위). | OEM에 우선순위 표 없음 | REQ-F-012, REQ-F-020 |
| A-03 | crash_status=PENDING 은 규칙 2를 적용하지 않을 뿐 다른 규칙에 영향이 없다. | OEM에 PENDING 동작 정의 없음 | REQ-F-029 |
| A-04 | 효과를 내는 규칙이 없는 도어는 확정 출력을 유지한다. 전역 동결은 하지 않는다. | OEM에 DEGRADED 출력 정의 없음 | REQ-F-010, REQ-F-011 |
| A-05 | 신선도는 입력 묶음 전체에 대해 source_timestamp_s 와 평가 시각의 차로 판단한다 (판정 단위는 묶음). | OEM-IF-001 에만 타임스탬프 | REQ-S-003 |
| A-06 | "정지"는 속도 3.0 km/h 미만이다. | OEM-FR-002 의 3 km/h 기준 | REQ-F-021, REQ-F-035 |
| A-07 | 이동 중 운전자 명령은 적용하지 않는다. | OEM-FR-001 수용기준이 "정지"에서만 기술 | REQ-F-035 |
| A-08 | ACTIVE 는 OEM-IF-006 에 없는 공급자 추가 상태값이다. 상태는 매 주기 입력으로만 정해지고 기억하지 않는다. | OEM에 상태 열거 정의 없음 | REQ-F-014 |
| A-09 | 평가주기당 운전자 명령은 첫 1건만 처리한다. | OEM에 동시 명령 규칙 없음 | REQ-F-009 |
| A-10 | 결정 이력의 허용 필드는 이벤트 ID, 시각, 좌/우 출력, 상태, reason_code 이다. | OEM-NFR-002 는 "허용 필드만"이라고만 기술 | REQ-F-031 |
| A-11 | 한 평가주기 처리 시간 상한 3 ms 는 평가주기 10 ms 의 30% 로 정한 공급자 설계 예산이다. 근거 자료 없음. | 공급자 판단 | REQ-Q-003 |
| A-12 | 전원 투입 직후의 초기 출력은 RELEASE 이다. | OEM-FR-007 의 "초기 해제 상태" 표현 | REQ-F-015 |
| A-13 | "결정"은 출력·상태·reason_code 중 하나가 직전 평가주기와 달라진 평가주기이다 (OEM-SWR-010 의 "이벤트 ID"에 대응). | OEM에 정의 없음 | REQ-F-028~038 |
| A-14 | OEM-IF-004 의 `VehicleSnapshot.timestamp_s` 와 OEM-IF-001 의 `source_timestamp_s` 는 같은 값이다. | OEM 문서 내 명칭 불일치 | REQ-F-008 |

## 6. 충돌·중복 등록부

| ID | 대상 | 내용 | 처리 |
|---|---|---|---|
| CF-01 | REQ-S-004(rank 1) ↔ REQ-S-001(rank 2), OEM-SR-001 | sensor_fault 와 유효 CONFIRMED 가 동시이면 OEM-SR-001 의 무조건 RELEASE 와 OEM-SR-004 의 출력 유지가 상충한다. 공급자는 REQ-S-004 를 우선(잠정)했다. | OQ-01, G1 전 필수 회신 |
| CF-02 | REQ-S-004(rank 1) ↔ REQ-F-025(rank 4) | sensor_fault 중에는 fire/과온/성인 탑승의 RELEASE 가 억제된다. | OQ-11 |
| CF-03 | REQ-S-002(rank 3) ↔ REQ-F-023/036 (OEM-FR-003) | 접근위험 시 해제 요청 억제(ASIL B)와 10초 내 재입력 override 가 상충한다. | OQ-02 |
| CF-04 | REQ-S-002(rank 3) ↔ REQ-F-025(rank 4) | 접근위험 LOCK 이 화재·성인 탑승 RELEASE 보다 우선한다. | OQ-03 |
| CF-05 | REQ-S-002(rank 3) ↔ REQ-F-027(rank 5) | 접근위험 LOCK 이 ignition off RELEASE 보다 우선한다. | OQ-04 |
| CF-06 | REQ-F-026(rank 6) ↔ REQ-F-025(rank 4), REQ-F-027(rank 5) | ISOFIX 강제 LOCK 이 화재 RELEASE, ignition off RELEASE 보다 낮다. | OQ-03, OQ-04 |
| DP-01 | REQ-S-003 ↔ REQ-F-010/014 | 입력 오류 시 상태 DEGRADED 는 REQ-F-014(상태표)가 정본이고 REQ-S-003 은 시간 요건만 정한다. | 정리 완료 |
| DP-02 | REQ-S-008 ↔ REQ-F-001~005 | INVALID 판정의 정본은 REQ-F-001~005, "INVALID를 제어에 쓰지 않음"의 정본은 REQ-S-008 이다. | 정리 완료 |

## 7. 미해결 질문 (OEM 회신 대상)

| ID | 질문 | 현재 처리 |
|---|---|---|
| OQ-01 | sensor_fault=TRUE 와 유효 충돌 CONFIRMED 가 동시일 때 어느 쪽이 우선하는가? (CF-01) | 잠정: sensor_fault 우선. **G1 전 필수 회신** |
| OQ-02 | OEM-SR-002 와 OEM-FR-003 은 상충하지 않는가? override 가 ASIL B 억제를 해제해도 되는가? override 후 출력은? (CF-03) | 잠정: REQ-F-036. G1 전 필수 회신 |
| OQ-03 | 화재·과온·성인 탑승 과 접근위험 LOCK 이 동시일 때 우선순위는? (CF-04, CF-06) | 잠정: 접근위험 우선 |
| OQ-04 | ignition off 와 접근위험 LOCK 또는 ISOFIX LOCK 이 동시일 때 우선순위는? (CF-05, CF-06) | 잠정: LOCK 우선 |
| OQ-05 | 평가주기, 입력 갱신 주기, 신선도 판정 단위(개별 입력인가 묶음인가)는? | 잠정: A-01, A-05 |
| OQ-06 | OEM 사양서 9절 추적성에 SWR-019 가 없다. 결번인가 누락인가? SWR-001~021 의 정의 문서는 어디 있는가? | 결번으로 가정 |
| OQ-07 | 동일 평가주기에 운전자 명령이 여러 건이면? | 잠정: A-09 |
| OQ-08 | 대상 차종, GVW, 문 형식, 트림이 확정되기 전이므로 법규 적용성은 미확정이다. 별표 14 의 충돌 후 잠금 해제 항목은 REQ-S-001 의 근거 후보일 뿐 적합성 주장이 아니다 (OEM 사양서 7절). | REQ-L-004 |
| OQ-09 | OEM-FR-003 의 기준 시점이 본문은 "접근위험 경고 후", 수용기준은 "첫 억제 후"이다. | 잠정: 첫 억제 후 |
| OQ-10 | CR-001(실행 환경 Python 3.12 에서 C 호스트 빌드로 변경)을 OEM 이 승인하는가? | 승인 대기 |
| OQ-11 | sensor_fault 중 fire/과온/성인 탑승 RELEASE 억제가 허용되는가? (CF-02) | 잠정: 규칙 1(sensor_fault)이 우선하여 RELEASE 가 억제된다 (OEM 확인 전 안전 영향 미평가) |
| OQ-12 | 결정 이력의 "결정"은 평가주기마다인가, 변화 이벤트인가? 이벤트 ID 형식은? | 잠정: A-13 |
| OQ-13 | OEM 사양서 내부 명칭 불일치(`VehicleSnapshot.timestamp_s`, `source_timestamp_s`)를 어느 쪽으로 통일하는가? | 잠정: A-14 |
| OQ-14 | OEM 사양서가 실제 승인되지 않은 교육 시나리오 베이스라인이다. SRS 베이스라인 BL-SWR-1.0 의 입력으로 인정되는가? | 교육 시나리오로 진행 |

## 부록 A. 이유코드 열거

reason_code 는 아래 중 하나다. priority_reason 은 1.3절 표의 규칙 이름(SENSOR_FAULT, CRASH, APPROACH, EMERGENCY_RELEASE, IGNITION_OFF, ISOFIX, AUTO_LOCK, DRIVER_CMD, HOLD)이다.

| reason_code | 설명 | 대응 규칙 / 요구사항 |
|---|---|---|
| NONE | 변화 없음 (HOLD) | HOLD |
| SENSOR_FAULT | sensor_fault TRUE 로 출력 유지 | 규칙 1, REQ-S-004 |
| CRASH_CONFIRMED | 유효 충돌 확정으로 RELEASE | 규칙 2, REQ-S-001 |
| CRASH_PENDING | 충돌 PENDING (출력 영향 없음) | REQ-F-029 |
| APPROACH_RISK_HOLD | 접근위험으로 LOCK, 해제 요청 억제 | 규칙 3, REQ-S-002, REQ-S-006 |
| OVERRIDE | 접근위험 억제를 운전자가 override | REQ-F-036 |
| FIRE | 화재로 RELEASE | 규칙 4 |
| OVERTEMP | 과온으로 RELEASE | 규칙 4 |
| ADULT_PRESENT | 성인 탑승으로 RELEASE | 규칙 4 |
| IGNITION_OFF | ignition off 로 RELEASE | 규칙 5 |
| ISOFIX_LEFT | 좌 ISOFIX 로 좌 LOCK | 규칙 6 |
| ISOFIX_RIGHT | 우 ISOFIX 로 우 LOCK | 규칙 6 |
| AUTO_LOCK_MOVING | 이동 시작으로 LOCK | 규칙 7 |
| DRIVER_CMD | 운전자 명령 적용 | 규칙 8 |
| CMD_REJECTED_INVALID | 형식 오류 명령 거절 | REQ-F-007 |
| CMD_REJECTED_EXTRA | 같은 주기의 추가 명령 거절 | REQ-F-009 |
| CMD_REJECTED_MOVING | 이동 중 명령 미적용 | REQ-F-035 |
| INPUT_INVALID | 입력 INVALID (경고코드 W_INPUT_INVALID 와 함께) | REQ-S-008 |
| INPUT_STALE | 입력 stale (경고코드 W_INPUT_STALE 와 함께) | REQ-S-003 |

경고코드: W_SENSOR_FAULT (REQ-S-009), W_INPUT_INVALID, W_INPUT_STALE.

여러 이유가 동시에 있을 때 reason_code 는 출력을 정한 규칙의 코드를 쓴다. 거절·미적용 코드는 별도 필드 `command_note` 로 함께 기록한다 (DEGRADED/FAULT 이유는 경고코드로 공개한다).

## 부록 B. 공개 결과 (REQ-Q-001 해시 대상)

공개 결과는 평가주기마다 (lock_left, lock_right, state, reason_code, priority_reason, command_note, 경고코드 집합)을 정해진 순서로 직렬화한 것이다. 이벤트 ID와 호스트 시각은 제외한다.
