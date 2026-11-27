# 추적 매트릭스

이터레이션 1 전 단계 기준(이터레이션 1). 시험 열은 해당 단계에서 채운다. '이터' 열은 구현 이터레이션 계획이다. '미정'은 아직 산출물이 없다는 뜻이고 '해당 없음'은 그 요구사항이 해당 산출물을 가지지 않는 유형(제한·일부 제약)이라는 뜻이다. 제한·제약의 ST 항은 검사(inspection) 시험 케이스로 둔다.

| 요구사항 | OEM 출처 | OEM SWR(사양서 9절) | 이터 | ARCH | DD | 소스 | UT | IT | ST |
|---|---|---|---|---|---|---|---|---|---|
| REQ-S-001 | OEM-SR-001 | SWR-007, 008 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-011, UT-rule-012, UT-rule-017 | IT-ecl-008 | ST-F-053 .. ST-NF-001 (4건) |
| REQ-S-005 | OEM-SR-001 | SWR-007, 008 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-011 | IT-ecl-008 | ST-NF-001 |
| REQ-S-002 | OEM-SR-002 | SWR-005, 009 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-013, UT-rule-014, UT-rule-015 | IT-ecl-009 | ST-F-056 .. ST-F-059 (4건) |
| REQ-S-006 | OEM-SR-002 | SWR-005, 006 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-013, UT-rule-014, UT-rule-027 | IT-ecl-009 | ST-F-056, ST-F-057 |
| REQ-S-007 | OEM-SR-002 | SWR-005 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-013 | IT-ecl-009 | ST-F-056 |
| REQ-S-003 | OEM-SR-003 | SWR-013 | 1 | ARCH-C-001 / ARCH-I-001; ARCH-C-003 / ARCH-I-003 | DD-M-001; DD-M-003 | src/ecl/input_validation.c, src/ecl/status_eval.c | UT-input-011 .. UT-status-009 (7건) | IT-ecl-003 | ST-F-012 .. ST-NF-002 (4건) |
| REQ-S-008 | OEM-SR-003 | SWR-013 | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | src/ecl/input_validation.c | UT-input-001 .. UT-input-012 (4건) | 미정 | ST-F-004 .. ST-F-044 (29건) |
| REQ-S-004 | OEM-SR-004 | SWR-021 | 1 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-ctl-009 .. UT-status-005 (4건) | IT-ecl-004 | ST-F-043, ST-F-044, ST-F-058 |
| REQ-S-009 | OEM-SR-004 | SWR-021 | 1 | ARCH-C-003 / ARCH-I-003 | DD-M-003 | src/ecl/status_eval.c | UT-ctl-009 .. UT-status-009 (4건) | IT-ecl-004 | ST-F-043, ST-F-048 |
| REQ-F-001 | OEM-IF-001 | - | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | src/ecl/input_validation.c | UT-input-003, UT-input-004, UT-input-005 | 미정 | ST-F-001 .. ST-F-007 (7건) |
| REQ-F-002 | OEM-IF-001 | - | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | src/ecl/input_validation.c | UT-input-003, UT-input-006, UT-input-007 | 미정 | ST-F-001 .. ST-F-011 (5건) |
| REQ-F-003 | OEM-IF-001 | - | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | src/ecl/input_validation.c | UT-ctl-007, UT-input-003, UT-input-010 | 미정 | ST-F-012, ST-F-013 |
| REQ-F-004 | OEM-IF-002 | - | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | src/ecl/input_validation.c | UT-input-003, UT-input-008 | 미정 | ST-F-001 .. ST-F-016 (4건) |
| REQ-F-005 | OEM-IF-003/007/008/009 | - | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | src/ecl/input_validation.c | UT-input-003, UT-input-009 | 미정 | ST-F-001 .. ST-F-034 (19건) |
| REQ-F-006 | OEM-IF-004 | - | 1 | ARCH-C-002 / ARCH-I-002 | DD-M-002 | src/ecl/command_intake.c | UT-cmd-004, UT-cmd-005 | 미정 | ST-F-035, ST-F-036 |
| REQ-F-018 | OEM-IF-004 | - | 1 | ARCH-C-002 / ARCH-I-002 | DD-M-002 | src/ecl/command_intake.c | UT-cmd-004, UT-cmd-006 | 미정 | ST-F-037, ST-F-038 |
| REQ-F-019 | OEM-IF-004 | - | 1 | ARCH-C-002 / ARCH-I-002 | DD-M-002 | src/ecl/command_intake.c | UT-cmd-004, UT-cmd-007 | 미정 | ST-F-039, ST-F-040 |
| REQ-F-007 | OEM-IF-004 | - | 1 | ARCH-C-002 / ARCH-I-002 | DD-M-002 | src/ecl/command_intake.c | UT-cmd-003, UT-cmd-009, UT-rule-010 | IT-ecl-006 | ST-F-035 .. ST-F-041 (7건) |
| REQ-F-008 | OEM-IF-004 | - | 1 | ARCH-C-002 / ARCH-I-002 | DD-M-002 | src/ecl/command_intake.c | UT-cmd-004 | 미정 | ST-F-042 |
| REQ-F-009 | OEM-IF-004 | - | 1 | ARCH-C-002 / ARCH-I-002 | DD-M-002 | src/ecl/command_intake.c | UT-cmd-008 .. UT-rule-010 (4건) | IT-ecl-006 | ST-F-041 |
| REQ-F-010 | OEM-SR-003 | SWR-013 | 1 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-ctl-007 .. UT-rule-015 (4건) | IT-ecl-002 | ST-F-043 .. ST-F-049 (4건) |
| REQ-F-011 | OEM-SR-003 | SWR-013 | 1 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-ctl-007 .. UT-rule-007 (4건) | IT-ecl-002 | ST-F-001, ST-F-043 |
| REQ-F-012 | OEM-SR-001/002 | SWR-007~009 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-005 .. UT-rule-034 (6건) | IT-ecl-010 | ST-F-058 .. ST-F-071 (12건) |
| REQ-F-013 | OEM-SR-003 | SWR-013 | 1 | ARCH-C-006 / ARCH-I-006 | DD-M-006 | src/ecl/controller.c | UT-ctl-006 | IT-ecl-001 | ST-F-051 |
| REQ-F-014 | OEM-IF-006, SR-003/004, FR-007 | SWR-014, 013, 021, 020 | 1 | ARCH-C-003 / ARCH-I-003 | DD-M-003 | src/ecl/status_eval.c | UT-ctl-007 .. UT-status-007 (4건) | IT-ecl-002, IT-ecl-005 | ST-F-043 .. ST-NF-002 (7건) |
| REQ-F-015 | 가정 A-12 | - | 1 | ARCH-C-006 / ARCH-I-006 | DD-M-006 | src/ecl/controller.c | UT-ctl-002 | 미정 | ST-F-050 |
| REQ-F-016 | OEM-IF-005 | - | 1 | ARCH-C-006 / ARCH-I-006 | DD-M-006 | src/ecl/controller.c | UT-ctl-006 | IT-ecl-001 | ST-F-001, ST-F-050, ST-F-051 |
| REQ-F-017 | OEM-IF-005, FR-006 | SWR-018 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-013 .. UT-rule-034 (4건) | IT-ecl-009 | ST-F-052 .. ST-F-083 (5건) |
| REQ-F-020 | OEM-SR-001/002, FR-001~007 | SWR-001~009 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-005, UT-rule-016, UT-rule-017 | IT-ecl-010 | ST-F-058 .. ST-F-071 (12건) |
| REQ-F-021 | OEM-FR-001 | SWR-001, 002, 004 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-024, UT-rule-025 | IT-ecl-012 | ST-F-072 |
| REQ-F-034 | OEM-FR-001 | SWR-001 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-cmd-004, UT-rule-024 | IT-ecl-012 | ST-F-072 |
| REQ-F-035 | OEM-FR-001/002 | SWR-004 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-025, UT-rule-026 | IT-ecl-014 | ST-F-073 |
| REQ-F-022 | OEM-FR-002 | SWR-003 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-023, UT-rule-033 | IT-ecl-014 | ST-F-060 .. ST-F-076 (4건) |
| REQ-F-023 | OEM-FR-003 | SWR-006 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-ctl-012 .. UT-rule-032 (7건) | IT-ecl-013 | ST-F-042, ST-F-077, ST-F-078 |
| REQ-F-036 | OEM-FR-003 | SWR-006 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-ctl-012 .. UT-rule-033 (5건) | IT-ecl-013 | ST-F-077, ST-F-078 |
| REQ-F-024 | OEM-FR-004 | SWR-014, 015 | 4 | ARCH-C-007 / ARCH-I-007 | 미정 | 미정 | 미정 | 미정 | ST-F-086 |
| REQ-F-025 | OEM-FR-005 | SWR-017 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-019, UT-rule-020 | IT-ecl-014 | ST-F-079, ST-F-080, ST-F-081 |
| REQ-F-026 | OEM-FR-006 | SWR-018 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-022 | 미정 | ST-F-082, ST-F-083 |
| REQ-F-027 | OEM-FR-007 | SWR-020 | 3 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-021, UT-status-004 | IT-ecl-014 | ST-F-084 |
| REQ-F-029 | OEM-SR-001 | SWR-007 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-018 | 미정 | ST-F-055, ST-F-060 |
| REQ-F-028 | OEM-IF-006 | SWR-015 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | 미정 | 미정 | ST-F-085 |
| REQ-F-037 | OEM-IF-006 | SWR-015 | 2 | ARCH-C-004 / ARCH-I-004 | DD-M-004 | src/ecl/rule_engine.c | UT-rule-019, UT-rule-032 | 미정 | ST-F-085 |
| REQ-F-030 | OEM-NFR-002 | SWR-010 | 4 | ARCH-C-005 / ARCH-I-005 | 미정 | 미정 | 미정 | 미정 | ST-F-087 |
| REQ-F-038 | OEM-NFR-002 | SWR-010 | 4 | ARCH-C-005 / ARCH-I-005 | 미정 | 미정 | 미정 | 미정 | ST-F-087 |
| REQ-F-031 | OEM-NFR-002 | SWR-011, 012 | 4 | ARCH-C-005 / ARCH-I-005 | 미정 | 미정 | 미정 | 미정 | ST-F-088 |
| REQ-F-032 | OEM-NFR-002 | SWR-012 | 4 | ARCH-C-005 / ARCH-I-005 | 미정 | 미정 | 미정 | 미정 | ST-F-089 |
| REQ-F-033 | OEM-IF-006 | SWR-015 | 5 | ARCH-C-007 / ARCH-I-007 | 미정 | 미정 | 미정 | 미정 | ST-F-090 |
| REQ-Q-001 | OEM-NFR-001 | SWR-016 | 4 | ARCH-C-006 / ARCH-I-006; ARCH-C-008 / ARCH-I-008 | DD-M-006 | src/ecl/controller.c | 미정 | IT-ecl-007, IT-ecl-011 | ST-NF-004 |
| REQ-Q-002 | OEM-NFR-002 | SWR-012 | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | 미정 | UT-input-014 | 미정 | ST-NF-005 |
| REQ-Q-004 | OEM 2절 | SWR-012 | 1 | ARCH-C-001 / ARCH-I-001 | DD-M-001 | 미정 | UT-input-014 | 미정 | ST-NF-005 |
| REQ-Q-003 | 가정 A-11 | - | 1 | ARCH-C-006 / ARCH-I-006 | DD-M-006 | 미정 | UT-ctl-011 | 미정 | ST-NF-003 |
| REQ-L-001 | OEM 3절 | - | - | 경계(범위 밖) | 해당 없음 | 해당 없음 | 해당 없음 | 해당 없음 | ST-NF-006 |
| REQ-L-002 | OEM-IF-005 | - | - | 경계(범위 밖) | 해당 없음 | 해당 없음 | 해당 없음 | 해당 없음 | ST-NF-007 |
| REQ-L-003 | OEM 3절 | - | - | 경계(범위 밖) | 해당 없음 | 해당 없음 | 해당 없음 | 해당 없음 | ST-NF-008 |
| REQ-L-004 | OEM 1.3, 7절 | - | - | 경계(범위 밖) | 해당 없음 | 해당 없음 | 해당 없음 | 해당 없음 | ST-NF-009 |
| REQ-C-001 | 사용자 결정 | - | 1 | ARCH-C-009 / ARCH-I-009 | DD-M-009 | 미정 | 미정 | 미정 | ST-NF-010 |
| REQ-C-002 | CR-001 | - | 5 | ARCH-C-008 / ARCH-I-008 | 미정 | 미정 | 미정 | 미정 | ST-NF-011 |
| REQ-C-003 | OEM 3절 | - | 5 | ARCH-C-008 / ARCH-I-008 | 미정 | 미정 | 미정 | 미정 | ST-NF-012 |
| REQ-C-004 | OEM 2절 | - | - | 경계(범위 밖) | 해당 없음 | 해당 없음 | 해당 없음 | 해당 없음 | ST-NF-013 |
| REQ-C-005 | CLAUDE.md | - | 1 | ARCH-C-009 / ARCH-I-009 | DD-M-009 | 미정 | 미정 | 미정 | ST-NF-014 |
| REQ-C-006 | OEM 1.3, 2절 | - | - | 경계(범위 밖) | 해당 없음 | 해당 없음 | 해당 없음 | 해당 없음 | ST-NF-015 |

## OEM 요구사항에서 우리 요구사항으로 (상향 역추적)

| OEM ID | 우리 요구사항 |
|---|---|
| OEM-SR-001 | REQ-S-001, REQ-S-005, REQ-F-012, REQ-F-020, REQ-F-029 |
| OEM-SR-002 | REQ-S-002, REQ-S-006, REQ-S-007, REQ-F-012, REQ-F-020 |
| OEM-SR-003 | REQ-S-003, REQ-S-008, REQ-F-010, REQ-F-011, REQ-F-013, REQ-F-014 |
| OEM-SR-004 | REQ-S-004, REQ-S-009, REQ-F-014 |
| OEM-FR-001 | REQ-F-020, REQ-F-021, REQ-F-034, REQ-F-035 |
| OEM-FR-002 | REQ-F-020, REQ-F-022, REQ-F-035 |
| OEM-FR-003 | REQ-F-020, REQ-F-023, REQ-F-036 |
| OEM-FR-004 | REQ-F-024 |
| OEM-FR-005 | REQ-F-020, REQ-F-025 |
| OEM-FR-006 | REQ-F-020, REQ-F-017, REQ-F-026 |
| OEM-FR-007 | REQ-F-020, REQ-F-027, REQ-F-014 |
| OEM-NFR-001 | REQ-Q-001 |
| OEM-NFR-002 | REQ-F-030, REQ-F-031, REQ-F-032, REQ-F-038, REQ-Q-002 |
| OEM-IF-001 | REQ-F-001, REQ-F-002, REQ-F-003 |
| OEM-IF-002 | REQ-F-004 |
| OEM-IF-003/007/008/009 | REQ-F-005 |
| OEM-IF-004 | REQ-F-006, REQ-F-007, REQ-F-008, REQ-F-009, REQ-F-018, REQ-F-019 |
| OEM-IF-005 | REQ-F-016, REQ-F-017, REQ-L-002 |
| OEM-IF-006 | REQ-F-014, REQ-F-024, REQ-F-028, REQ-F-033, REQ-F-037 |
| OEM 사양서 2절(개인정보) | REQ-Q-004 |
| OEM 사양서 3절 | REQ-L-001, REQ-L-003, REQ-C-003 |
| OEM 사양서 1.3, 7절 | REQ-L-004, REQ-C-006 |
| OEM 사양서 2절(ASIL 할당) | REQ-C-004 |
| 가정 A-12 (초기 출력) | REQ-F-015 |
| 가정 A-11 (처리 시간 예산) | REQ-Q-003 |
| 사용자 결정 2026-11-27 (C 언어) | REQ-C-001 |
| 변경 요청 CR-001 | REQ-C-002 |
| CLAUDE.md 2절 | REQ-C-005 |
