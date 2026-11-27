# 프롬프트 기록

사용자가 작성한 프롬프트와 Claude가 답한 주요 내용을 시간순으로 기록합니다.

## 2026-11-27

### 1. 저장소 클론

**프롬프트**

> 다음 저장소 클론해줘. https://github.com/DongJoonHan/ClaudeClang.git

**답변 요지**

- `D:\Repo\ClaudeClang`이 비어 있어 하위 폴더를 만들지 않고 해당 위치에 바로 클론함 (`git clone <url> .`).
- 브랜치: `main` (`origin/main` 추적)
- 커밋: `59244bb Initial commit` 1개, 내용은 `README.md`(`# ClaudeClang`)뿐.

### 2. Claude 프로젝트 구성

**프롬프트**

> 이 프로젝트를 Cluade 프로젝트로 만들어줘. .claude 폴더도 미리 만들어줘. 스킬과 서브에이전트 폴더까지. 지금까지 내가 작성한 프롬프트와 네가 답한 주요 내용은 prompt.md에 남겨줘.

**답변 요지**

- `CLAUDE.md` 생성 — 프로젝트 현재 상태, 폴더 구조, 작업 규칙(프롬프트 기록, 한국어 사용)을 기술.
- `.claude/skills/` 생성 — 프로젝트 전용 스킬 위치 (`<스킬이름>/SKILL.md`).
- `.claude/agents/` 생성 — 프로젝트 전용 서브에이전트 위치 (`<에이전트이름>.md`).
- 빈 폴더가 git에 추적되도록 두 폴더에 `.gitkeep`을 둠.
- `prompt.md` 생성 — 이 기록 파일.
- 커밋/푸시는 하지 않음 (요청 시 진행).

### 3. 커밋 및 푸시

**프롬프트**

> 커밋하고 푸시해줘

**답변 요지**

- 기본 브랜치(`main`)에 직접 커밋하지 않고 `chore/claude-project-setup` 브랜치를 만들어 커밋함.
- 커밋 대상: `CLAUDE.md`, `prompt.md`, `.claude/skills/.gitkeep`, `.claude/agents/.gitkeep`.
- 해당 브랜치를 `origin`에 푸시함.
- 이 PC에 git 사용자 정보가 없어, 초기 커밋과 같은 작성자를 해당 커밋에만 지정함 (git 설정은 변경하지 않음).

### 4. main에 반영

**프롬프트**

> main에 바로 넣어줘

**답변 요지**

- `chore/claude-project-setup`을 `main`에 fast-forward로 병합하고 `origin/main`에 푸시함.
- 이 기록(4번 항목)은 `main`에 별도 커밋으로 추가함.

### 5. A-SPICE 기준 개발 스킬 7종 추가

**프롬프트** (원문이 길어 항목별로 요약, 모델은 이 시점에 Sonnet 5.5로 변경됨)

> A-SPICE 기준으로 다음 스킬을 추가해줘.
> - 요구사항 분석: 기능/비기능(품질·제한·제약사항) 구분, 기능 안전 요구사항은 별도 문서, 품질은 ISO 25010 준수, 모든 요구사항은 이후 산출물과 양방향 추적성.
> - 아키텍처 설계: 컴포넌트와 인터페이스 정의 필수, 인터페이스로만 통신, 고응집·저결합, 모듈화·캡슐화·SOLID.
> - 상세 설계: 모듈 정의, 특정 수준 이상 모듈은 인터페이스 필수, 인터페이스로만 통신, 클린코드, 단일 책임.
> - 구현: 함수는 반드시 리턴값을 가지며 리턴은 마지막에 한 번만, 리턴할 값이 없으면 void 대신 정상 실행 여부 값을 리턴.
> - 단위 테스트: 정해진 커버리지 100%까지 반복, Stub/Mock 과의존 금지, 의미 없는 케이스 금지, Doxygen 주석(목적·입력·절차·예상 출력), 경계값·동치 클래스·PICT, MC/DC 목표 시 LLVM 도구(스킬 내 스크립트).
> - 통합 테스트: 단위 테스트와 같은 규칙 + 인터페이스 중심 컴포넌트 연동 시험.
> - 리뷰: 모든 산출물을 리뷰 스킬로 리뷰, A-SPICE 요건에 따라 산출물별 체크리스트를 개발해 리뷰.

**답변 요지**

- 스킬 7개를 `.claude/skills/<이름>/SKILL.md`로 추가: `requirements-analysis`, `architecture-design`, `detailed-design`, `implementation`, `unit-test`, `integration-test`, `review`.
- `unit-test/scripts/coverage.sh`: LLVM 기반 커버리지·MC/DC 측정 스크립트(`-fcoverage-mcdc`, Clang 18 이상 필요). 이 환경에서 실행 검증은 하지 않음.
- `review/checklists/`: 산출물별 체크리스트 7종(요구사항, 아키텍처, 상세 설계, 코드, 단위 테스트, 통합 테스트, 추적성).
- `CLAUDE.md`에 스킬 표, 산출물 위치·ID 규칙, "프로젝트가 정해야 하는 값" 표 추가.
- 요청에 없어 임시 기본값을 둔 항목(CLAUDE.md 표에서 확정 필요): 구현 언어(C/C++ 가정), "특정 수준 이상 모듈"의 기준, "정해진 커버리지"의 종류, ISO 25010 판, 제한/제약사항의 구분 해석.
- 커밋/푸시는 하지 않음.

### 6. main에 커밋 및 푸시

**프롬프트**

> main에 바로 커밋하고 푸시해줘

**답변 요지**

- 스킬 7종과 `CLAUDE.md`, `prompt.md` 변경을 `main`에 직접 커밋하고 `origin/main`에 푸시함 (`5ab2f0a`).
- Windows의 줄바꿈 변환 경고 때문에 `coverage.sh`가 CRLF가 될 수 있어 `.gitattributes`(`*.sh text eol=lf`) 추가를 제안함 (미적용).

### 7. 리뷰 서브에이전트 생성

**프롬프트** (요약)

> 리뷰 서브에이전트를 생성해. review 스킬을 적용하고, 분석·설계·구현·테스트 등 각 단계가 끝나면 산출물을 리뷰하며, 리뷰 부적합 사항을 모두 조치해야 다음 단계로 넘어갈 수 있다. 너는 무자비하다. 봐주는 것 없이 빡세게 본다.

**답변 요지**

- `.claude/agents/reviewer.md` 생성. 도구는 `Read, Grep, Glob, Bash, Write`로 제한. 산출물 수정 도구(`Edit`)는 주지 않았고, `Write`는 `docs/review/` 리뷰 기록 작성용.
- 엄격 리뷰 원칙: 체크리스트 전 항목 판정, 직접 확인한 것만 OK, 의심되면 NG, 미확인은 통과 불가, 작성자 설명 불신, 반복 결함 전수 나열.
- 게이트 규칙: NG·미확인 없음 + Major/Minor 전부 재리뷰로 종료 + 추적성 완전 + 기록 존재일 때만 "통과". "조건부 통과"는 폐지해 부적합을 남긴 채 다음 단계로 가는 것을 막음. 이에 맞춰 `review` 스킬의 판정도 통과/불통과 두 가지로 고치고, 스킬에 리뷰어 에이전트 항목을 추가함.
- `CLAUDE.md`에 서브에이전트 표와 "단계 게이트" 규칙 추가.
- 커밋/푸시는 하지 않음.

### 8. main에 커밋 및 푸시

**프롬프트**

> main에 바로 커밋하고 푸시해줘

**답변 요지**

- `reviewer` 서브에이전트, `CLAUDE.md`, `review` 스킬, `prompt.md` 변경을 `main`에 직접 커밋하고 `origin/main`에 푸시함 (`8637421`).
- 이 8번 기록은 편집 실패로 해당 커밋에 빠져 별도 커밋으로 추가함.

### 9. 리뷰어 모델 변경

**프롬프트**

> 리뷰어 서브에이전트를 haiku 모델을 쓰도록 변경해줘.

**답변 요지**

- `.claude/agents/reviewer.md`의 `model`을 `inherit`에서 `haiku`로 변경함.
- 엄격한 리뷰가 목적이라 경량 모델에서 놓치는 항목이 생길 수 있음을 알림.

### 10. main에 커밋 및 푸시

**프롬프트**

> main에 바로 커밋하고 푸시해줘

**답변 요지**

- 리뷰어 모델 변경(`haiku`)과 `prompt.md` 기록을 `main`에 직접 커밋하고 `origin/main`에 푸시함 (`4533ada`).

### 11. CLAUDE.md에 개발 절차·품질 기준·Git·환경·CI 반영

**프롬프트** (원문이 길어 항목별로 요약)

> CLAUDE.md를 다음 내용으로 수정해줘.
> 1. 개발 절차: A-SPICE 기준, 애자일, 분석~시스템 테스트를 한 이터레이션으로, 단계마다 Reviewer 서브에이전트 리뷰와 부적합 조치 후 진행, 이터레이션 완료 시 사용자 확인 후 PR, 메인 에이전트가 단계별 스킬로 개발, 구현은 TDD(TDD 테스트는 단위 테스트와 분리, 별도 결과·커버리지).
> 2. 품질 기준: 시스템 요구사항 100% / 통합 함수 100%+성공률 100% / 단위 MC/DC 100%+성공률 100%, Cppcheck 100%(예외 없음), Lizard CCN 10·함수 80라인(구현 종료 시 Hook), Doxygen 주석 + Human in the Loop 항목(AI 모델·일시·요청자, 사용자 리뷰 여부·일시·리뷰어).
> 3. Gitflow, CI 통과해야 PR 완료(빌드 + 품질 기준).
> 4. 개발 환경: Clang 18+, 빌드 도구 추천, Cppcheck, GoogleTest/GMock, LLVM(MC/DC), Lizard, CLOC, 나머지 추천, 개발·CI 동일 Docker 이미지 `ClaudeCLangBuild`.
> 5. CI: Jenkins 안정화 버전, 현재 PC의 Docker, 포트 8080, 플러그인 설치 완료, 결과를 플러그인에서 확인.

**답변 요지**

- `CLAUDE.md`를 8개 절로 재구성: 개발 절차, 품질 기준, Git/PR, 개발 환경, CI, 산출물 위치, 확정 값, 작업 규칙.
- 빌드 도구는 CMake + Ninja를 추천. 추가 도구로 Doxygen+Graphviz, clang-format, PICT 등을 제안.
- 시스템 테스트는 `sw-system-test-case` 스킬, 구현은 `test-driven-development` 스킬을 단계 표에 연결. 시험 ID `TDD-`, `ST-`와 `test/tdd/`, `test/system/` 위치 추가.
- HITL 주석용 Doxygen 사용자 정의 태그 6종(`@ai_model`, `@ai_date`, `@requested_by`, `@human_reviewed`, `@reviewed_by`, `@reviewed_date`)과 AI가 `@human_reviewed yes`를 스스로 쓰지 않는 규칙을 정의.
- 이미지명 `ClaudeCLangBuild`는 Docker가 대문자를 허용하지 않아 실제 이름은 소문자 `claudeclangbuild`로 명시.
- 이번에는 `CLAUDE.md` 수정만 수행. Dockerfile, Jenkins 구축, Lizard Hook, `develop` 브랜치 생성은 하지 않음 (문서에 "아직 구축되지 않음"으로 명시). 커밋/푸시는 하지 않음.

### 12. 스킬을 CLAUDE.md와 정합

**프롬프트**

> 스킬 정합부터 하자

**답변 요지**

- `unit-test`: 합격 기준을 MC/DC 100% + 성공률 100%로 변경, 항상 `--mcdc` 실행, 커버리지 예외 폐지(도달 불가·조건 6개 초과 결정은 코드/설계 수정), GoogleTest/GMock·C++ 예시, HITL 태그, TDD 테스트와 커버리지 분리(`build/cov-unit` / `build/cov-tdd`).
- `integration-test`: 합격 기준을 함수 커버리지 100% + 성공률 100%로 변경(기존 인터페이스·시나리오 커버리지는 완결성 기준으로 유지), C++ 예시, HITL 태그.
- `implementation`: 언어 C++(Clang 18+), TDD 연계(`test/tdd/`), `enum class Status` 기반 예시, Cppcheck 0건(억제 금지)·Lizard(CCN ≤ 10, ≤ 80라인), HITL 태그 규칙(AI는 `@human_reviewed yes`를 스스로 쓰지 않음).
- `detailed-design`: 함수 CCN/길이 제한과 결정당 조건 6개 이하 설계 규칙 추가. `requirements-analysis`: 시스템 테스트(`ST-`)·요구사항 커버리지 100% 추적으로 수정.
- `review`: 시스템 테스트 체크리스트 `system-test.md` 신설, 코드(CD-14, CD-18~22)·단위(UT-09, 10, 15, 18, 19)·통합(IT-12, 19)·추적성(TR-11) 체크리스트 갱신. `reviewer` 에이전트에 시스템 테스트·TDD 스킬 참조, Cppcheck/Lizard/HITL 검증 항목 추가.
- `coverage.sh`는 수정하지 않음(이 PC에는 clang/llvm이 없어 실행 검증 불가). 커밋/푸시는 하지 않음.

### 13. Gitflow로 개발 환경 전체 구성

**프롬프트**

> gitflow 따라가고, 이후 작업 네가 알아서 진행해. 문제가 발생하면, 원인 파악해서 해결하고, 마지막에 나에게 보고해. 환경 모두 구성 진행해. 사전에 내가 정해줘야 하는 것 알려줘.

**답변 요지**

- Gitflow: `main`에서 `develop` 생성·푸시, `feature/dev-environment`에서 작업.
- Docker 이미지 `claudeclangbuild`(Ubuntu 24.04, Clang/LLVM 23.1.6, CMake/Ninja, Cppcheck 2.13, Lizard 1.26.1, CLOC, Doxygen, PICT 3.7.4, GoogleTest/GMock 1.18.0) 구성·빌드.
- CMake 모듈(`cmake/ClaudeClang.cmake`), `Doxyfile`, `.clang-format`, 품질 검사 스크립트(`ci/scripts/`), 참조 프로젝트(`ci/smoke/`) 작성. 참조 프로젝트로 전체 파이프라인과 각 게이트(MC/DC, 함수 커버리지, 성공률, Lizard, HITL, Cppcheck, Doxygen, 요구사항 커버리지, `-Werror`)가 위반 시 실제로 실패함을 검증.
- 해결한 문제: (1) 통합 시험 함수 커버리지가 `ISensor` 가상 소멸자 때문에 75%로 나옴 → Clang이 비어 있는 소멸자 호출을 생략하는 것이 원인, protected non-virtual 소멸자(C.35)로 해결; (2) Cppcheck이 gtest 내부를 검사해 서드파티 지적이 쏟아짐 → `compile_commands.json` 기준 실행; `TEST()` 본문 `unusedFunction` 오탐은 점검 목록에서 제외(문서화); (3) Doxygen 제외 패턴 `*/ci/*`가 참조 프로젝트까지 제외 → 수정; (4) Lizard 훅이 새 디렉터리의 파일을 못 찾음(`git status` 접힘) → `--untracked-files=all`.
- Claude Code `Stop` 훅(`.claude/hooks/lizard-check.sh`)으로 구현 종료 시 Lizard 검사, 통과/차단/복구 동작 검증.
- Jenkins LTS(포트 8080, Docker Compose, JCasC, 플러그인 사전 설치)와 `ci/Jenkinsfile` 작성.
- `.gitattributes`(LF 고정), `.gitignore` 추가. `CLAUDE.md`를 구축 상태에 맞게 갱신.
- Jenkins 구축 중 해결한 문제: (5) `cloc` 플러그인이 업데이트 센터에서 제거되어 설치 실패 → CLOC 결과를 CSV로 만들어 Plot 플러그인으로 표시; (6) Git 플러그인이 `file://` 로컬 체크아웃을 차단 → `-Dhudson.plugins.git.GitSCM.ALLOW_LOCAL_CHECKOUT=true`; (7) 빌드 트리거 POST가 403 → CSRF 크럼 사용.
- Jenkins 검증: 작업 `ClaudeClang`(멀티브랜치) 생성, 플러그인 90개 설치, `feature/dev-environment` 빌드 성공(13단계). 제품 코드가 있는 임시 브랜치로 시험 13개·커버리지 3종·Cppcheck/Lizard/HITL/Doxygen 이슈·Plot·HTML 리포트·산출물 게시를 확인했고, 위반을 넣은 임시 브랜치에서 Lizard·Doxygen·단위 MC/DC 단계가 FAILED로 표시되고 빌드가 FAILURE가 되는 것을 확인(임시 브랜치와 Jenkins 작업은 삭제함).
- PR: `feature/dev-environment` → `develop` 생성, 병합하지 않음.

### 14. CI를 GitHub Actions로 전환, Jenkins는 주기 검증

**프롬프트**

> CI는 Action으로 변경해줘. 단, 내부에서도 jenkins를 주기적으로 실행하도록 구성해줘.

**답변 요지**

- `.github/workflows/ci.yml`: 같은 `docker/Dockerfile`로 이미지를 빌드(GitHub 캐시)하고 같은 `ci/scripts/`로 도구 버전 → Toolchain self-check → 전체 품질 게이트 → 요약·리포트 업로드. PR 병합 게이트 `build-and-verify`.
- Jenkins: 통합 브랜치(develop/main/release/*/hotfix/*)를 6시간마다(`cron('H H/6 * * *')`) 자동 빌드, 브랜치 스캔은 1시간마다. feature 브랜치에는 cron 없음. release 브랜치 임시 생성으로 등록 확인 후 정리.
- 해결한 문제: (8) 컨테이너가 root로 만든 llvm-cov HTML을 러너가 읽지 못해 `upload-artifact`가 EACCES → 업로드 전 소유권 변경 단계 추가.
- Actions 실행 성공(전 단계). `main`, `develop`에 브랜치 보호 적용: PR 필수, 필수 체크 `build-and-verify`, 강제 푸시·삭제 금지, 관리자 우회 가능.
- `CLAUDE.md` 3.2, 5절을 Actions 게이트 + Jenkins 주기 검증 구조로 갱신.

### 15. PR #1 병합

**프롬프트**

> PR #1 병합해줘

**답변 요지**

- 최신 커밋에서 필수 체크 `build-and-verify` 성공을 확인한 뒤 병합 커밋(`6205f4a`)으로 `develop`에 병합. Jenkins가 `develop`을 인식해 빌드 성공, 6시간 cron 등록 확인.

### 16. OEM SW 요구사항 사양서 검토와 개발 계획

**프롬프트**

> "OEM/OEM-SWR-001_OEM SW 요구사항 사양서.docx" 을 검토하고, 우리가 SW를 개발하기 위한 개발 계획을 제시해. 내가 사전에 검토/승인해야 하는 것을 먼저 알려줘.

**답변 요지**

- 문서(전자식 차일드락 제어 SW, ASIL B 입력 4건 + QM 기능 7건 + 비기능 2건, 외부 인터페이스 9건, 인도 게이트 G1~G4)를 검토. 승인 요청 사항, 문서의 충돌·모호 사항, 이터레이션 계획을 제시.
- 핵심 블로커: (1) OEM 실행환경이 Python 3.12인데 우리 기준은 C++/MC/DC, (2) 교육용 비공개 문서와 공개 저장소, (3) 요구사항 간 우선순위 충돌.
- 안전조치: `OEM/`을 로컬 전용(`.git/info/exclude`)으로 git에서 제외 (저장소 변경 없음). 이 기록(15, 16번)은 `develop` 작업 트리에 미커밋 상태로 두었음(Gitflow상 feature 브랜치에서 커밋 예정).

### 17. 개발 언어 확정(C)과 환경 전환, 이터레이션 계획 재제시

**프롬프트**

> 요구사항 변경으로 개발언어는 CLang으로 확정되었어. CLAUDE.md를 기준으로 개발환경 맞춰줘. 저장소는 public 유지하고, 문서 배포 승인받았어. 나머지는 너의 권고안으로 진행해. 이터레이션 계획 다시 제시해

**답변 요지**

- "CLang"을 **C 언어(C17), 컴파일러는 Clang**으로 해석(가정). 시험 코드는 GoogleTest/GMock이 C++ 전용이므로 C++20 유지.
- `feature/c-language-env` 브랜치에서 전환: CMake(`LANGUAGES C CXX`, C17·C 컴파일러 검사), Cppcheck(`--std=c17 --std=c++20`), Lizard(`-l c -l cpp`), Doxygen/HITL/훅의 파일 패턴에 `.c` 추가, `.clang-format`.
- 참조 프로젝트 `ci/smoke`를 C로 재작성(클래스 → 함수 포인터+`void*` 문맥 인터페이스, GMock 어댑터). 시험 13개, 단위 MC/DC 100%, 통합 함수 100%, Cppcheck/Lizard/HITL/Doxygen 통과.
- `CLAUDE.md` 4, 4.2, 4.3, 7절과 스킬(implementation, unit-test, integration-test, detailed-design, review code 체크리스트)을 C 기준으로 갱신.
- 해결한 문제: (9) Cppcheck `constParameterCallback` — 억제하지 않고 콜백이 호출 횟수를 기록하도록 설계 정리, (10) 매크로 주석에 HITL 태그 누락.
- OEM 문서 공개 저장소 반영은 사용자 승인에 따라 PR 단계에서 `OEM/`의 로컬 제외를 풀고 포함. 아직 커밋/PR 없음.

### 18. PR #2 생성과 이터레이션 1 시작 (요구사항 분석)

**프롬프트**

> PR 만들고 이터레이션 1 시작해. 네가 요청한 것은 모두 너의 권장안으로 진행해

**답변 요지**

- PR #2(`feature/c-language-env` → `develop`) 생성(OEM 사양서 포함). `feature/iteration-1` 에서 SRS(기능 36, 품질 4, 제한 4, 제약 6)·안전 요구사항 9건·추적 매트릭스를 작성. `reviewer` 1차 리뷰는 불통과(Major 16, Minor 14, Info 8). 규칙표·상태표·이유코드·충돌 등록부·CR-001·사용 사례로 SRS v0.2 조치.

### 19. 리뷰 면제와 아키텍처~시스템 테스트

**프롬프트**

> 이번 이터레이션은 리뷰는 넘어가자. 됐다고 치고 다음 작업해.

**답변 요지**

- 리뷰를 "통과"로 기록하지 않고 `docs/review/review-waiver-iteration-1.md`에 면제로 남김 (요구사항 재리뷰는 미완료, 이터레이션 2부터 게이트 재적용).
- 아키텍처(컴포넌트 9, 인터페이스 9), 상세 설계, TDD(red 확인 후 green) 구현: input_validation, command_intake, status_eval, rule_engine(규칙 1·HOLD), controller.
- 시험: TDD 6, 단위 53(MC/DC 100%), 통합 7(함수 100%), 시스템 명세 102건(PICT PW-01 11조합, 요구사항 61/61), 실행 53건 통과. `run-all.sh` 전 단계 통과.
- 해결한 문제: (11) Cppcheck `constVariable`, (12) .c 내부 매크로·타입 Doxygen 경고(정적 상수와 문서화로 해결), (13) GoogleTest 매크로 인자의 템플릿 쉼표, (14) 규칙 엔진 반복문 조건이 독립 시험 불가(구조 변경).

### 20. lizard CSV 출력 추가

**프롬프트**

> check-lizard.sh에 lizard --csv 출력을 한 줄 추가합니다. CSV에는 함수별 전체 라인 수가 들어 있습니다. ... 이것좀 반영해줘

**답변 요지**

- `ci/scripts/check-lizard.sh`에 `lizard --csv` 출력(`$REPORT_DIR/lizard.csv`)을 한 줄 추가. 실행 확인(함수별 CSV 생성).
- `run-tests.sh`에 `system` 스위트(성공률 100%)와 `run-all.sh` 단계를 추가.
