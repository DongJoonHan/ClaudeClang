# 단위 시험 보고 — 이터레이션 1

작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료. 리뷰: 이터레이션 1 면제(docs/review/review-waiver-iteration-1.md).
환경: Docker 이미지 claudeclangbuild (Clang/LLVM 23.1.6, GoogleTest v1.18.0), 명령: bash docker/run.sh ci/scripts/run-all.sh

| 항목 | 결과 | 기준 |
|---|---|---|
| 시험 수 / 성공률 | 53개 / 100% (실패 0, 건너뜀 0) | 100% |
| MC/DC | 33/33 = 100% | 100% |
| 라인·함수·분기 | 311/311, 29/29, 114/114 (모두 100%) | 참고 |

- 대상: input_validation, command_intake, status_eval, rule_engine, controller (`test/unit/`).
- 기법: MC/DC 독립 영향 쌍, 경계값 분석(속도 0.0/300.0, 200 ms), 동치 클래스(코드 범위, 불리언), 널 인자.
- 설계상 도달 불가능한 오류 경로는 합산 후 단일 결정으로 만들어(controller) 모든 결정이 양쪽을 실행하도록 했다.
- 규칙 엔진은 규칙이 1개인 동안 반복문 조건이 독립적으로 시험되지 않는 구조라 `found` 판정을 삼항 결정으로 바꿨다. 이터레이션 2 에서 규칙이 늘면 이 구조를 재검토한다.
