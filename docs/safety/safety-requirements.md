# 기능 안전 요구사항 — 전자식 차일드락 제어 SW

- 문서 ID: SSR-ECL-001, 버전 0.2 (이터레이션 1)
- 출처: OEM-SWR-001 5절 (OEM이 ASIL B 로 할당한 입력). 공급자는 HARA/ASIL 결정 근거를 재구성하지 않는다 (REQ-C-004).
- 작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료.
- 본 문서는 ISO 26262 준수를 주장하지 않는다 (REQ-C-006).
- 규칙 순위, 상태표, 이유코드의 정본은 `docs/requirements/software-requirements.md` 1.3, 1.4절과 부록 A이다. 이 문서는 ASIL B 요구의 원문과 수용기준만 둔다.
- 우선순위 범례: 모든 안전 요구사항의 우선순위는 높음이다.

| ID | 구분 | ASIL | 요구사항 | 안전 상태 | 출처 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|---|
| REQ-S-001 | 안전 | B | SW는 유효한 crash_status 가 CONFIRMED가 되면 후석 좌/우 출력을 300 ms 이내에 RELEASE로 해야 한다. 단, 규칙 1(sensor_fault)이 적용되는 경우는 제외한다 (잠정: CF-01, OQ-01, G1 전 OEM 회신 필수). | 좌/우 RELEASE | OEM-SR-001 | 시험(PC/SIL) | 초안 |
| REQ-S-005 | 안전 | B | SW는 유효한 crash_status 가 CONFIRMED인 동안 이후 평가주기에도 좌/우 출력을 RELEASE로 유지해야 한다. | 좌/우 RELEASE | OEM-SR-001 | 시험(PC/SIL) | 초안 |
| REQ-S-002 | 안전 | B | SW는 유효한 후측방 접근위험이 TRUE인 도어의 출력을 LOCK으로 해야 한다 (override 중인 도어 제외: REQ-F-036, CF-03). | 해당 도어 LOCK | OEM-SR-002 | 시험(PC/SIL) | 초안 |
| REQ-S-006 | 안전 | B | SW는 접근위험이 TRUE인 도어에 대한 해제 요청을 억제해야 한다 (RELEASE 요청에도 LOCK 유지). | 해당 도어 LOCK | OEM-SR-002 | 시험(PC/SIL) | 초안 |
| REQ-S-007 | 안전 | B | SW는 접근위험으로 해제 요청을 억제할 때 원인을 이유코드 APPROACH_RISK_HOLD 로 기록해야 한다. | 해당 도어 LOCK | OEM-SR-002 | 시험(PC/SIL) | 초안 |
| REQ-S-003 | 안전 | B | SW는 필수 입력이 200 ms 를 초과해 갱신되지 않으면 그 시점부터 100 ms 이내에 상태를 DEGRADED로 해야 한다 (판정 단위: A-05). | DEGRADED | OEM-SR-003 | 단위·시험(PC/SIL) | 초안 |
| REQ-S-008 | 안전 | B | SW는 형식 또는 범위 오류로 INVALID가 된 입력을 제어 평가에 사용하지 않아야 한다 (INVALID 판정은 REQ-F-001~005). | 해당 규칙 미적용 | OEM-SR-003 | 단위·시험(PC/SIL) | 초안 |
| REQ-S-004 | 안전 | B | SW는 유효한 sensor_fault 가 TRUE인 첫 평가주기에 새 명령을 적용하지 않고 직전 확정 출력을 유지해야 한다. | 확정 출력 유지 | OEM-SR-004 | 단위·시험(PC/SIL) | 초안 |
| REQ-S-009 | 안전 | B | SW는 유효한 sensor_fault 가 TRUE인 동안 상태를 FAULT로 하고 경고코드 W_SENSOR_FAULT 를 제공해야 한다. | FAULT | OEM-SR-004 | 단위·시험(PC/SIL) | 초안 |
