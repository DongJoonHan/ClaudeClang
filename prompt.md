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
