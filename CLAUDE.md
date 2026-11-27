# ClaudeClang

Claude Code가 이 저장소에서 작업할 때 참고하는 프로젝트 안내 문서입니다. 이 문서의 값은 `.claude/skills/`의 스킬에 적힌 임시 기본값보다 우선합니다.

## 현재 상태

- 소스 코드, 빌드 설정, 테스트는 아직 없습니다.
- 개발 환경(Docker 이미지, 빌드 설정)과 CI(Jenkins)는 **정의만 되어 있고 아직 구축되지 않았습니다** (아래 4, 5절).

## 구조

- `.claude/skills/` — 프로젝트 전용 스킬. 스킬마다 `<스킬이름>/SKILL.md`를 둡니다.
- `.claude/agents/` — 프로젝트 전용 서브에이전트. 에이전트마다 `<에이전트이름>.md`를 둡니다.
- `prompt.md` — 사용자 프롬프트와 Claude의 주요 답변을 기록하는 작업 로그.

---

## 1. 개발 절차

### 1.1 기본 원칙

- 개발은 **A-SPICE**를 기준으로 한다.
- 개발 생명주기는 **애자일**이다. 아래 단계를 **하나의 이터레이션**으로 한다.

  `분석 → 아키텍처 설계 → 상세 설계 → 구현 → 단위 테스트 → 통합 테스트 → 시스템 테스트`

- 개발은 기본적으로 **메인 에이전트**가 수행하며, 각 단계에 맞는 스킬을 사용한다.

| 단계 | 스킬 | A-SPICE |
|---|---|---|
| 분석(요구사항) | `requirements-analysis` | SWE.1 |
| 아키텍처 설계 | `architecture-design` | SWE.2 |
| 상세 설계 | `detailed-design` | SWE.3 |
| 구현 | `test-driven-development` + `implementation` | SWE.3 |
| 단위 테스트 | `unit-test` | SWE.4 |
| 통합 테스트 | `integration-test` | SWE.5 |
| 시스템 테스트 | `sw-system-test-case` | SWE.6 |
| 리뷰 | `review` (실행은 `reviewer` 서브에이전트) | SUP.2 |

### 1.2 단계 게이트 (필수)

- 각 단계(분석, 아키텍처 설계, 상세 설계, 구현, 단위/통합/시스템 테스트)가 완료되면 **반드시 `reviewer` 서브에이전트를 호출**해 해당 산출물을 리뷰한다.
- `reviewer`가 "불통과"를 반환하면 **부적합 사항을 모두 조치**하고 재리뷰를 받는다. **통과 판정을 받기 전에는 다음 단계로 진행하지 않는다.**
- 작성자(메인 에이전트)가 스스로 지적을 "해결됨"으로 닫지 않는다. 닫는 것은 재리뷰다.

### 1.3 이터레이션 종료와 PR

1. 이터레이션의 모든 단계가 리뷰를 통과하면, **사용자에게 결과를 보고하고 확인을 받는다.**
2. **사용자의 확인을 받은 이후에만 PR 절차를 진행한다.** 확인 전에 PR을 만들지 않는다.
3. PR 절차는 3절(Git 브랜치 및 PR 전략)을 따른다.

### 1.4 구현은 TDD로 한다

- 구현 단계에서는 `test-driven-development` 스킬로 TDD(실패하는 테스트 → 구현 → 리팩터링)를 수행한다. 함수 규칙(반환값, 단일 return 등)은 `implementation` 스킬을 함께 따른다.
- **TDD로 만든 테스트는 단위 테스트와 분리한다.**
  - 위치: TDD 테스트 `test/tdd/`, 단위 테스트 `test/unit/` (서로 다른 디렉터리, 다른 빌드 타깃).
  - 결과: 별도의 시험 결과 파일/리포트를 낸다 (`docs/test/tdd-report.md`, 단위 테스트는 `docs/test/unit-test-report.md`).
  - 커버리지: **별도로 측정**한다 (출력 디렉터리도 분리, 예: `build/cov-tdd`, `build/cov-unit`).
- 2.1절의 단위 테스트 커버리지 기준은 **`test/unit/`의 테스트만으로** 달성해야 한다. TDD 테스트의 커버리지를 합산하지 않는다.
- TDD 테스트 커버리지의 합격 기준은 정해지지 않았다. 측정·보고만 하며 CI 합격 기준에는 넣지 않는다.

---

## 2. 소스 코드 품질 기준

모든 기준은 CI의 합격 조건이다 (3.2절). **예외는 없다.**

### 2.1 테스트 커버리지·성공률

| 테스트 | 기준 | 측정 |
|---|---|---|
| 시스템 테스트 | 요구사항 커버리지 100% | 추적 매트릭스 기준 (`REQ-` ↔ 시스템 테스트 케이스) |
| 통합 테스트 | 함수 커버리지 100%, 테스트 성공률 100% | LLVM(`llvm-cov`) |
| 단위 테스트 | MC/DC 커버리지 100%, 테스트 성공률 100% | LLVM(`-fcoverage-mcdc`, `llvm-cov`) |

### 2.2 정적 분석

- **Cppcheck** 룰 대상 100% 만족. **예외 처리는 없다**: `--suppress`, 소스 내 `cppcheck-suppress`, 억제 파일을 사용하지 않는다. 지적은 코드를 고쳐서 해결한다.
- 실행 기준(가정, 변경 가능): `--enable=all --inconclusive --error-exitcode=1`, 빌드의 `compile_commands.json` 사용. 시스템 헤더 경고는 억제하지 말고 `-I`/`--library`로 해결한다.

### 2.3 소스 코드 메트릭

- **순환복잡도(CCN) 10 이하**, **함수 길이 80라인 이하** — **Lizard**로 검사한다 (`-C 10 -L 80` 상당, 위반 시 실패).
- 구현이 끝날 때 **Claude Code Hook으로 자동 검사**한다. (Hook은 아직 구성되지 않았다. `.claude/settings.json`에 구성할 것. CI에서도 같은 검사를 한다.)

### 2.4 주석 — Doxygen + Human in the Loop

- 주석은 **Doxygen** 형식으로 작성한다.
- 모든 소스/헤더 **파일 헤더**와 **함수·클래스 주석**에 아래 Human-in-the-Loop 항목을 **반드시** 포함한다. 이 항목도 Doxygen 형식을 따른다.
  1. **작성 정보**: 어느 AI 모델이, 언제, 누가 요청해서 작성했는가
  2. **사용자 리뷰 정보**: 사용자의 리뷰가 완료된 코드인가. 완료했다면 언제, 누가 했는가

사용자 정의 Doxygen 태그(`Doxyfile`의 `ALIASES`로 정의):

| 태그 | 내용 | 예 |
|---|---|---|
| `@ai_model` | 작성한 AI 모델 | `claude-sonnet-5-5` |
| `@ai_date` | 작성 일시 | `2026-11-27` |
| `@requested_by` | 작성을 요청한 사람 | `DongJoonHan` |
| `@human_reviewed` | 사용자 리뷰 완료 여부 | `yes` / `no` |
| `@reviewed_by` | 리뷰한 사람 (완료 시) | `DongJoonHan` / `-` |
| `@reviewed_date` | 리뷰 일자 (완료 시) | `2026-11-28` / `-` |

예:

```cpp
/**
 * @brief   센서 값을 읽는다.
 * @param   sensor     센서 핸들
 * @param   out_value  읽은 값을 돌려받는 출력 인자
 * @return  STATUS_OK 또는 오류 상태
 * @satisfies DD-M-012, REQ-F-045
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
```

규칙 (Human in the Loop 보장):
- **AI는 `@human_reviewed yes`를 스스로 쓰지 않는다.** 사용자가 리뷰를 마쳤다고 알려 준 경우에만 사용자가 알려 준 이름과 일자로 갱신한다.
- 리뷰 완료된 코드를 **수정하면** `@human_reviewed no`, `@reviewed_by -`, `@reviewed_date -`로 되돌린다. 작성 정보(`@ai_model`, `@ai_date`)도 수정한 모델과 일시로 갱신한다.
- 주석률은 **CLOC**으로 측정한다 (합격 기준 수치는 정해지지 않았다).
- Doxygen 경고는 오류로 취급한다 (`WARN_AS_ERROR = YES`).

---

## 3. Git 브랜치 및 PR 전략

### 3.1 브랜치: Gitflow

| 브랜치 | 용도 |
|---|---|
| `main` | 릴리스된 코드만. 직접 커밋 금지. |
| `develop` | 통합 브랜치. 직접 커밋 금지. |
| `feature/*` | 기능·이터레이션 작업. `develop`에서 분기해 `develop`으로 PR. |
| `release/*` | 릴리스 준비. `develop`에서 분기해 `main`과 `develop`으로 병합. |
| `hotfix/*` | 긴급 수정. `main`에서 분기해 `main`과 `develop`으로 병합. |

- 이터레이션 하나는 보통 `feature/<이터레이션 또는 주제>` 브랜치 하나에서 수행한다.
- 현재 저장소에는 `main`만 있다. `develop` 브랜치는 아직 만들지 않았다.

### 3.2 PR과 CI

- PR은 CI를 활용한다. **CI가 통과해야만 PR을 완료(병합)한다.**
- CI에서 통과해야 하는 항목:
  1. **빌드 성공**
  2. **2절의 소스 코드 품질 기준 전부**: 테스트 성공률, 커버리지(단위 MC/DC, 통합 함수, 시스템 요구사항), Cppcheck 0건, Lizard(CCN ≤ 10, 함수 ≤ 80라인), Doxygen 주석·HITL 항목
- PR은 1.3절에 따라 사용자 확인 후에만 만든다.

---

## 4. 개발 환경

| 항목 | 선택 | 비고 |
|---|---|---|
| 개발 언어 | **C++** (Clang **18 이상** 최신 버전) | 표준은 C++20을 권장 (미확정) |
| 컴파일러 | Clang/LLVM 18+ | MC/DC(`-fcoverage-mcdc`)에 18 이상 필요 |
| 빌드 도구 | **CMake + Ninja** (추천) | 아래 추천 근거 참고 |
| 정적 분석 | Cppcheck | |
| 테스트 프레임워크 | Google Test / Google Mock | |
| 커버리지 | LLVM (`llvm-profdata`, `llvm-cov`), MC/DC까지 | `.claude/skills/unit-test/scripts/coverage.sh` |
| 순환복잡도·함수 라인 수 | Lizard | |
| 주석률 | CLOC | |
| 조합 테스트 | Microsoft PICT | 단위/통합/시스템 테스트 케이스 도출 |
| 문서·주석 검증 | Doxygen (+ Graphviz) | `WARN_AS_ERROR`로 주석 검증, HITL 태그는 `ALIASES` |
| 코드 포맷 | clang-format | 추천. 스타일 파일은 저장소에 둔다 |

추천 도구(요청하신 목록에 추가):
- **CMake + Ninja** (빌드 도구 추천): Clang·`-fcoverage-mcdc` 옵션 지정이 쉽고, GoogleTest/GMock 연동이 표준적이며(`FetchContent`/`find_package`, `gtest_discover_tests`), `compile_commands.json`을 만들어 Cppcheck·Lizard·정적 도구가 그대로 쓴다. CTest로 `test/tdd`, `unit`, `integration` 타깃을 분리해 실행하기 좋고, Ninja는 증분 빌드가 빠르다.
- **Doxygen + Graphviz**: 2.4절의 주석 규칙을 기계적으로 검증한다.
- **clang-format**: 포맷 논쟁을 없앤다. (선택: `clang-tidy`는 Cppcheck 외 보조 분석용으로 쓸 수 있으나, CI 합격 조건은 Cppcheck이다.)

### 4.1 Docker 빌드 이미지

- **개발 빌드와 CI 빌드는 모두 동일한 Docker 이미지**를 사용한다.
- 이미지명: **`ClaudeCLangBuild`**. Docker는 저장소 이름에 대문자를 허용하지 않으므로, 실제 `docker build -t`에 쓰는 이름은 소문자 `claudeclangbuild`다 (필요 시 버전 태그 `claudeclangbuild:<버전>`).
- 이미지에 포함: Clang/LLVM 18+(`llvm-cov`, `llvm-profdata` 포함), CMake, Ninja, Cppcheck, Lizard, CLOC, Doxygen, Graphviz, PICT, clang-format, GoogleTest/GMock, Git, Python.
- 도구 버전은 이미지에 고정하고 빌드 로그에 출력한다.
- 계획된 위치: `docker/Dockerfile` (**아직 생성되지 않음**).

---

## 5. CI 환경

- **Jenkins 안정화(LTS) 버전**을 사용한다. 현재 PC의 **Docker 위에서 실행**하고, 포트는 **8080**을 사용한다.
- 필요한 **플러그인은 설치를 완료한 상태**로 구성한다.
- 테스트, 정적 분석, 커버리지, 메트릭 등의 **결과를 Jenkins 플러그인에서 확인**할 수 있어야 한다.
  - 시험 결과(GoogleTest JUnit XML), 정적 분석(Cppcheck), 메트릭(Lizard), 커버리지(LLVM), 주석률(CLOC)을 각각 플러그인 화면에서 볼 수 있게 구성한다. 사용할 플러그인은 구축 시 확정한다.
- 빌드는 4.1절의 `ClaudeCLangBuild` 이미지 안에서 실행한다.
- 계획된 위치: `ci/Jenkinsfile`, `ci/jenkins/` (이미지·플러그인 목록·실행 설정). **아직 생성되지 않음.**

---

## 6. 산출물 위치와 ID 규칙

| 산출물 | 위치 | ID 접두 |
|---|---|---|
| 요구사항 | `docs/requirements/software-requirements.md` | `REQ-F-`(기능) `REQ-Q-`(품질) `REQ-L-`(제한) `REQ-C-`(제약사항) |
| 기능 안전 요구사항 | `docs/safety/safety-requirements.md` | `REQ-S-` |
| 아키텍처 | `docs/architecture/software-architecture.md` | `ARCH-C-`(컴포넌트) `ARCH-I-`(인터페이스) |
| 상세 설계 | `docs/design/detailed-design.md` | `DD-M-`(모듈) `DD-I-`(인터페이스) |
| 소스 코드 | `src/`, `include/` | — |
| TDD 테스트 | `test/tdd/`, `docs/test/tdd-report.md` | `TDD-` |
| 단위 테스트 | `test/unit/`, `docs/test/unit-test-report.md` | `UT-` |
| 통합 테스트 | `test/integration/`, `docs/test/integration-test-*.md` | `IT-` |
| 시스템 테스트 | `test/system/`, `docs/test/system/` | `ST-` |
| 추적 매트릭스 | `docs/traceability/trace-matrix.md` | — |
| 리뷰 기록 | `docs/review/` | — |

## 7. 프로젝트 확정 값

| 항목 | 값 |
|---|---|
| 구현 언어 | C++ (Clang 18 이상) |
| 인터페이스 정의 대상 모듈 기준("특정 수준") | 미정 (임시: 2개 이상 모듈이 사용 / 컴포넌트 경계 / 외부 자원 래핑) |
| 목표 커버리지 | 단위: MC/DC 100%, 통합: 함수 100%, 시스템: 요구사항 100% (2.1절). 스킬의 구문+분기 기본값보다 이 값이 우선 |
| ISO 25010 판 | 미정 (임시: 2023판) |

## 8. 작업 규칙

- 사용자가 새 프롬프트를 주고 의미 있는 작업이 끝나면 `prompt.md`에 프롬프트 원문과 답변 요지를 이어서 기록합니다.
- 사용자와의 대화 및 문서는 한국어로 작성합니다.
