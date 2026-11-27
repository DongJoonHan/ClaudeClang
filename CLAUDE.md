# ClaudeClang

Claude Code가 이 저장소에서 작업할 때 참고하는 프로젝트 안내 문서입니다. 이 문서의 값은 `.claude/skills/`의 스킬에 적힌 임시 기본값보다 우선합니다.

## 현재 상태

- 제품 소스 코드(`src/`)와 제품 테스트는 아직 없습니다.
- 개발 환경(Docker 이미지 `claudeclangbuild`, CMake 빌드), 품질 검사 스크립트(`ci/scripts/`), Lizard Hook, CI(GitHub Actions 게이트 + Jenkins 주기 검증)는 **구축되어 있습니다** (4, 5절).
- 파이프라인이 정상인지 확인하는 참조 프로젝트가 `ci/smoke/`에 있고, 새 코드의 작성 템플릿(HITL 주석, 단일 return, 상태 반환, GoogleTest/GMock)으로도 쓴다.

### 자주 쓰는 명령

모든 명령은 빌드 이미지 안에서 실행한다 (개발·CI 동일 환경).

| 목적 | 명령 |
|---|---|
| 이미지 빌드 | `bash docker/build.sh` |
| 컨테이너 셸 | `bash docker/run.sh` |
| 전체 품질 검사 | `bash docker/run.sh ci/scripts/run-all.sh` |
| 파이프라인 자체 점검(참조 프로젝트) | `bash docker/run.sh ci/scripts/smoke.sh` |
| 단계별 | `ci/scripts/{build,check-cppcheck,check-lizard,check-comments,check-doxygen,check-req-coverage}.sh`, `ci/scripts/run-tests.sh <tdd\|unit\|integration>` |
| Jenkins 시작 | `docker compose -f ci/jenkins/docker-compose.yml --env-file ci/jenkins/.env up -d --build` |

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
- 점검 규칙(구현 완료, `ci/scripts/check-cppcheck.sh`): `error`(항상) + `warning, style, performance, portability`, `--inconclusive --check-level=exhaustive`, 빌드의 `compile_commands.json` 기준(서드파티 `-isystem` 헤더 제외).
- `--enable=all`에서 **점검 목록에서 뺀 것은 세 가지**이며, 개별 지적을 억제하는 것이 아니다: `information`(checkersReport 같은 메타 메시지), `missingInclude`(헤더 누락은 `-Werror` 빌드가 이미 실패시킴), `unusedFunction`(googletest 설정이 `TEST()` 본문을 호출되지 않는 함수로 전개하는 오탐이 있고, 제품 함수가 죽은 코드가 아닌지는 통합 시험의 함수 커버리지 100%가 보장함). 이 목록을 바꾸려면 사용자 확인이 필요하다.

### 2.3 소스 코드 메트릭

- **순환복잡도(CCN) 10 이하**, **함수 길이 80라인 이하** — **Lizard**로 검사한다 (`-C 10 -L 80` 상당, 위반 시 실패).
- 구현이 끝날 때 **Claude Code Hook으로 자동 검사**한다. `.claude/settings.json`의 `Stop` 훅이 `.claude/hooks/lizard-check.sh`를 실행한다: `src/`, `include/`, `test/`에 변경된 C++ 파일이 있으면 빌드 이미지의 Lizard로 검사하고, 위반이면 종료 코드 2로 막아 위반 내용을 전달한다 (같은 문제로 3회 연속 막으면 통과시키고 사용자에게 보고하게 한다). CI에서도 같은 검사를 한다.

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
- `develop` 브랜치는 `main`(`4533ada`)에서 분기해 만들었다. 이후 모든 작업은 `develop`에서 분기한 `feature/*` 브랜치에서 하고 PR로 병합한다. `main`/`develop`에 직접 커밋하지 않는다.

### 3.2 PR과 CI

- PR은 CI를 활용한다. CI는 **GitHub Actions**(`.github/workflows/ci.yml`)이며, **필수 체크 `build-and-verify`가 통과해야만 PR을 완료(병합)한다** (5절). 이 PC의 Jenkins는 PR 게이트가 아니라 주기적 보조 검증이다.
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
- 이미지에 포함: Clang/LLVM(`llvm-cov`, `llvm-profdata` 포함), CMake, Ninja, Cppcheck, Lizard, CLOC, Doxygen, Graphviz, PICT, clang-format, GoogleTest/GMock, Git, Python.
- 도구 버전은 `docker/Dockerfile`의 `ARG`로 고정하고 CI 첫 단계(`ci/scripts/tool-versions.sh`)에서 출력한다. 현재: Clang/LLVM 23.1.6, GoogleTest v1.18.0, PICT v3.7.4, Lizard 1.26.1, Cppcheck 2.13.0(Ubuntu 24.04 패키지), CMake 3.28.3.
- 위치: `docker/Dockerfile`, `docker/build.sh`, `docker/run.sh`. 이미지 버전은 `docker/VERSION`.

### 4.2 CMake 구조

- 최상위 `CMakeLists.txt`는 `cmake/ClaudeClang.cmake`의 `cc_setup_project()`를 호출한다. `src/`와 `test/{tdd,unit,integration,system}/`에 `CMakeLists.txt`가 있으면 자동으로 포함한다.
- 제품 라이브러리는 `cc_add_library()`, 시험 스위트는 `cc_add_test_suite(<suite> SOURCES ... LIBS ...)`로 정의한다 (`ci/smoke/` 참고). 스위트 실행 파일은 `build/test/<suite>/<suite>_tests`이다.
- 컴파일은 `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror`, 커버리지 계측(`-fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc`, `-O0`)이 기본이다. 시험 스위트는 제품 라이브러리를 whole-archive로 링크해 **시험이 호출하지 않는 제품 함수도 함수 커버리지에 잡히게** 한다.

### 4.3 구현 시 주의 (함수 커버리지)

- 인터페이스 클래스에 `virtual ~I() = default;`를 두면, 시험의 대체 객체가 비어 있는 소멸자를 가질 때 Clang이 기반 소멸자 호출을 만들지 않아 **실행되지 않는 함수**가 되어 함수 커버리지 100%를 막는다. 기반 클래스 포인터로 삭제하지 않는 인터페이스는 **protected non-virtual 소멸자**를 쓴다(C++ Core Guidelines C.35). 기반 포인터로 삭제해야 한다면 시험이 그 경로를 실행해야 한다.
- 한 결정(조건식)의 조건은 6개 이하로 한다 (LLVM MC/DC 측정 한계).

---

## 5. CI 환경

CI는 두 계층이며 둘 다 4.1절의 같은 이미지(`docker/Dockerfile`)와 같은 스크립트(`ci/scripts/`)를 쓴다.

| 계층 | 역할 | 실행 시점 |
|---|---|---|
| **GitHub Actions** | **PR 병합 게이트** (필수 체크 `build-and-verify`) | `develop`/`main`/`release/**`/`hotfix/**` 대상 PR과 해당 브랜치 푸시, 수동 실행 |
| **Jenkins (이 PC, Docker)** | **주기적 보조 검증** + 플러그인 화면으로 결과 확인 | `develop`/`main`/`release/*`/`hotfix/*`를 6시간마다, 브랜치 스캔은 1시간마다 |

### 5.1 GitHub Actions (PR 게이트)

- 워크플로: `.github/workflows/ci.yml`. 순서: 이미지 빌드(GitHub 캐시) → 도구 버전 → Toolchain self-check(`ci/smoke`) → `ci/scripts/run-all.sh`(빌드, Cppcheck, Lizard, 주석/HITL, Doxygen, TDD/단위/통합 시험, 요구사항 커버리지) → 요약·리포트 업로드.
- 한 단계가 실패해도 끝까지 실행하고 마지막에 실패 처리한다. 합격 기준은 2절 전부이며 예외는 없다.
- 결과: Actions 실행 화면의 Job summary(커버리지 표, Cppcheck/Lizard/HITL/Doxygen 출력)와 `ci-reports` 아티팩트(JUnit XML, lcov, llvm-cov HTML, 요약 JSON).
- **브랜치 보호**: `main`, `develop`은 PR 필수 + 필수 체크 `build-and-verify` 통과 필수로 설정한다 (3절의 직접 커밋 금지를 서버에서도 강제).

### 5.2 Jenkins (주기적 보조 검증)

- **Jenkins 안정화(LTS)** 를 현재 PC의 **Docker 위에서** 실행하고, 포트는 **8080**을 쓴다. 필요한 **플러그인은 설치를 완료한 상태**로 이미지에 들어 있다.
- 위치: `ci/Jenkinsfile`(파이프라인), `ci/jenkins/`(이미지, `plugins.txt`, JCasC `casc/jenkins.yaml`, `docker-compose.yml`), `ci/scripts/`(공용 검사 스크립트).
- 접속: `http://localhost:8080`, 계정 `admin`, 비밀번호는 `ci/jenkins/.env`의 `JENKINS_ADMIN_PASSWORD` (git 제외). 작업 `ClaudeClang`은 멀티브랜치 파이프라인이며, 이 PC의 저장소(`/repo-src`로 마운트)에서 Gitflow 브랜치를 1시간마다 찾고, **통합 브랜치(`develop`, `main`, `release/*`, `hotfix/*`)는 6시간마다(`cron('H H/6 * * *')`) 자동 빌드**한다. 그 외 브랜치는 스캔 때 변경이 있으면 빌드한다. **로컬에 커밋된 상태만 빌드한다.**
- 빌드는 `claudeclangbuild` 이미지에서 실행한다. Jenkins 컨테이너가 Docker 소켓으로 호스트 Docker에 빌드 컨테이너를 띄우고 `--volumes-from`으로 작업 공간을 공유한다.
- 파이프라인 단계: Environment → Toolchain self-check → Build → Cppcheck → Lizard → Comments(CLOC/HITL) → Doxygen → TDD(보고) → Unit(MC/DC) → Integration(함수) → System(요구사항) → Archive. 한 단계가 실패해도 끝까지 실행하고 마지막에 빌드를 실패로 만든다.
- 결과 확인(플러그인): 시험 결과(JUnit), Cppcheck/Lizard/HITL/Doxygen(Warnings NG), 커버리지(Coverage 플러그인, LCOV), LLVM HTML 리포트(MC/DC 포함, HTML Publisher), 주석률(CLOC 결과를 Plot 플러그인으로 표시 — CLOC 전용 Jenkins 플러그인은 업데이트 센터에서 제거되어 사용할 수 없다).
- Jenkins는 GitHub에 결과를 보고하지 않는다 (로컬 PC에서만 접근 가능하고 webhook을 받지 않음). PR 병합 판단은 Actions 결과로 한다.

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
| 구현 언어 | C++20 (Clang 18 이상, 현재 이미지는 23.1.6) |
| 빌드 도구 | CMake + Ninja |
| 인터페이스 정의 대상 모듈 기준("특정 수준") | 미정 (임시: 2개 이상 모듈이 사용 / 컴포넌트 경계 / 외부 자원 래핑) |
| 목표 커버리지 | 단위: MC/DC 100%, 통합: 함수 100%, 시스템: 요구사항 100% (2.1절). 스킬의 구문+분기 기본값보다 이 값이 우선 |
| ISO 25010 판 | 미정 (임시: 2023판) |

## 8. 작업 규칙

- 사용자가 새 프롬프트를 주고 의미 있는 작업이 끝나면 `prompt.md`에 프롬프트 원문과 답변 요지를 이어서 기록합니다.
- 사용자와의 대화 및 문서는 한국어로 작성합니다.
