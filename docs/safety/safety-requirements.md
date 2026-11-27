# 기능 안전 요구사항 — 전자식 차일드락 제어 SW

- 문서 ID: SSR-ECL-001, 버전 0.1 (이터레이션 1)
- 출처: OEM-SWR-001 5절 (OEM이 ASIL B 로 할당한 입력). 공급자는 HARA/ASIL 결정 근거를 재구성하지 않는다 (REQ-C-004).
- 작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료.
- 본 문서는 ISO 26262 준수를 주장하지 않는다 (REQ-C-006).

| ID | 구분 | ASIL | 요구사항 | 안전 상태 | 출처 | 검증 방법 | 상태 |
|---|---|---|---|---|---|---|---|
| REQ-S-001 | 안전 | B | SW는 crash_status 가 유효한 CONFIRMED가 되면 다른 규칙(sensor_fault 제외, 가정 A-02·OQ-01)보다 우선하여 후석 좌/우 출력을 300 ms 이내에 RELEASE로 하고, CONFIRMED가 유지되는 동안 이후 평가주기에도 RELEASE를 유지해야 한다. | 좌/우 RELEASE | OEM-SR-001 | 시험(PC/SIL) | 초안 |
| REQ-S-002 | 안전 | B | SW는 후측방 접근위험이 유효한 TRUE인 도어의 출력을 LOCK으로 하고, 그 도어의 해제 요청을 억제하며, 억제 원인을 이유코드로 기록해야 한다. (override 는 REQ-F-023, 충돌 OQ-02) | 해당 도어 LOCK | OEM-SR-002 | 시험(PC/SIL) | 초안 |
| REQ-S-003 | 안전 | B | SW는 필수 입력이 200 ms 를 초과해 갱신되지 않으면 그 시점부터 100 ms 이내에 상태를 DEGRADED로 해야 하고, 형식·범위 오류 입력은 제어 평가 전에 거절해야 한다. | DEGRADED, 확정 출력 유지 | OEM-SR-003 | 단위·시험(PC/SIL) | 초안 |
| REQ-S-004 | 안전 | B | SW는 정규화 입력의 sensor_fault 가 유효한 TRUE이면 그 첫 평가주기에 새 명령을 적용하지 않고 직전 확정 출력을 유지하며, 상태를 FAULT로 하고 경고코드를 제공해야 한다. | FAULT, 확정 출력 유지 | OEM-SR-004 | 단위·시험(PC/SIL) | 초안 |
