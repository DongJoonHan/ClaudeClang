# ClaudeClang

Claude Code가 이 저장소에서 작업할 때 참고하는 프로젝트 안내 문서입니다.

## 현재 상태

- 초기 단계 저장소입니다. 아직 소스 코드, 빌드 설정, 테스트가 없습니다.
- 빌드/테스트/실행 명령이 정해지면 이 문서에 추가하세요.

## 구조

- `.claude/skills/` — 프로젝트 전용 스킬. 스킬마다 `<스킬이름>/SKILL.md`를 둡니다.
- `.claude/agents/` — 프로젝트 전용 서브에이전트. 에이전트마다 `<에이전트이름>.md`를 둡니다.
- `prompt.md` — 사용자 프롬프트와 Claude의 주요 답변을 기록하는 작업 로그.

## 개발 프로세스 스킬 (A-SPICE 기준)

`.claude/skills/`의 스킬은 A-SPICE 프로세스를 따른다. 산출물은 아래 순서로 만들고, 각 산출물은 `review` 스킬로 리뷰한 뒤 다음 단계에서 사용한다.

| 단계 | 스킬 | 프로세스 |
|---|---|---|
| 요구사항 분석 | `requirements-analysis` | SWE.1 |
| 아키텍처 설계 | `architecture-design` | SWE.2 |
| 상세 설계 | `detailed-design` | SWE.3 |
| 구현 | `implementation` | SWE.3 |
| 단위 테스트 | `unit-test` | SWE.4 |
| 통합 테스트 | `integration-test` | SWE.5 |
| 리뷰 | `review` | SUP.2 |

## 서브에이전트

| 에이전트 | 파일 | 역할 |
|---|---|---|
| `reviewer` | `.claude/agents/reviewer.md` | `review` 스킬로 산출물을 엄격히 리뷰하는 독립 리뷰어. 산출물은 수정하지 않고 지적만 한다. |

### 단계 게이트 (필수)

- 요구사항 분석, 아키텍처 설계, 상세 설계, 구현, 단위 테스트, 통합 테스트 각 단계가 끝나면 **반드시 `reviewer` 서브에이전트를 호출**해 리뷰한다.
- `reviewer`가 "불통과"를 반환하면 지적 사항을 모두 조치하고 재리뷰를 받는다. **통과 판정을 받기 전에는 다음 단계로 넘어가지 않는다.**
- 작성자가 스스로 지적을 "해결됨"으로 닫지 않는다. 닫는 것은 재리뷰다.

## 산출물 위치와 ID 규칙

| 산출물 | 위치 | ID 접두 |
|---|---|---|
| 요구사항 | `docs/requirements/software-requirements.md` | `REQ-F-`(기능) `REQ-Q-`(품질) `REQ-L-`(제한) `REQ-C-`(제약사항) |
| 기능 안전 요구사항 | `docs/safety/safety-requirements.md` | `REQ-S-` |
| 아키텍처 | `docs/architecture/software-architecture.md` | `ARCH-C-`(컴포넌트) `ARCH-I-`(인터페이스) |
| 상세 설계 | `docs/design/detailed-design.md` | `DD-M-`(모듈) `DD-I-`(인터페이스) |
| 소스 코드 | `src/`, `include/` | — |
| 단위 테스트 | `test/unit/`, `docs/test/unit-test-report.md` | `UT-` |
| 통합 테스트 | `test/integration/`, `docs/test/integration-test-*.md` | `IT-` |
| 추적 매트릭스 | `docs/traceability/trace-matrix.md` | — |
| 리뷰 기록 | `docs/review/` | — |

## 프로젝트가 정해야 하는 값 (정해지면 여기에 기록)

스킬에는 아래 값의 임시 기본값만 있다. 확정되면 이 표를 채우고, 스킬은 이 표를 우선한다.

| 항목 | 현재 값 |
|---|---|
| 구현 언어 | 미정 (스킬은 C/C++ 가정) |
| 인터페이스 정의 대상 모듈 기준("특정 수준") | 미정 (기본: 2개 이상 모듈이 사용 / 컴포넌트 경계 / 외부 자원 래핑) |
| 목표 커버리지 | 미정 (임시: 구문 + 분기 100%, MC/DC는 지정 시) |
| ISO 25010 판 | 미정 (기본: 2023판) |

## 작업 규칙

- 사용자가 새 프롬프트를 주고 의미 있는 작업이 끝나면 `prompt.md`에 프롬프트 원문과 답변 요지를 이어서 기록합니다.
- 사용자와의 대화 및 문서는 한국어로 작성합니다.
