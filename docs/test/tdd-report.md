# TDD 시험 보고 — 이터레이션 1

작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료. 리뷰: 이터레이션 1~2 면제(docs/review/review-waiver.md).
환경: Docker 이미지 claudeclangbuild (Clang/LLVM 23.1.6, GoogleTest v1.18.0), 명령: bash docker/run.sh ci/scripts/run-all.sh

- 시험 6개(TDD-ecl-001~006, `test/tdd/ecl_core_tdd_test.cpp`), 성공 6, 실패 0 (성공률 100%).
- red 확인: 구현 없이 빌드하면 링크 단계에서 `undefined reference`로 실패했다. 이후 구현해 green.
- 커버리지(별도 측정, 합격 기준 없음): 라인 72.35%, 함수 86.21%, 분기 47.37%, MC/DC 6.06% (2/33).
- 이 수치는 단위 테스트 합격 기준에 합산하지 않는다 (CLAUDE.md 1.4절).

## 이터레이션 2

- 시험 4개 추가(TDD-ecl-101~104, `test/tdd/ecl_rules_tdd_test.cpp`): 규칙 2(충돌 확정), 규칙 3(접근위험), 해제 요청 억제, PENDING. 누적 10개, 성공 10 (100%).
- red 확인: 구현 전에 4개가 모두 실패(`EclRulesTddTest.*`)했고 기존 6개는 통과했다. 이후 규칙 엔진을 구현해 green.
- 커버리지(별도 측정, 합격 기준 없음): 라인 86.24%, 함수 100%, 분기 64.71%, MC/DC 14.29% (5/35).

## 이터레이션 3

- 시험 7개 추가(TDD-ecl-301~307, `test/tdd/ecl_qm_rules_tdd_test.cpp`): 화재, 점화 꺼짐, ISOFIX 좌, 이동 시작, 정지 중 해제 명령, 이동 중 명령, override. 누적 17개, 성공 17 (100%).
- red 확인: 구현 전에 새 시험 7개가 모두 실패, 기존 10개는 통과. 구현 후 기본 정상 입력을 정지(0 km/h)로 바꿔 이전 이터레이션 시험 4개가 다시 통과.
- 커버리지(별도 측정, 합격 기준 없음): 라인 89.19%, 함수 100%, 분기 71.74%, MC/DC 34.85% (23/66).
