# 통합 시험 보고 — 이터레이션 1

작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료. 리뷰: 이터레이션 1 면제(docs/review/review-waiver-iteration-1.md).
환경: Docker 이미지 claudeclangbuild (Clang/LLVM 23.1.6, GoogleTest v1.18.0), 명령: bash docker/run.sh ci/scripts/run-all.sh

| 항목 | 결과 | 기준 |
|---|---|---|
| 시험 수 / 성공률 | 7개 / 100% | 100% |
| 함수 커버리지 | 29/29 = 100% | 100% |

- 대상 인터페이스: ARCH-I-001, 002, 003, 004, 006 (`test/integration/ecl_controller_integration_test.cpp`, IT-ecl-001~007).
- 하위 모듈은 대체하지 않고 실제 컴포넌트를 연결했다. 결정론 재생(IT-ecl-007)은 같은 입력 순서에서 모든 필드가 같음을 확인한다.
- 참고(합격 기준 아님): 라인 93.25%, 분기 72.81%.
