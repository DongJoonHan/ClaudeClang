#!/usr/bin/env bash
# Claude Code Stop 훅: 구현이 끝날 때(응답 종료 시) Lizard로 순환복잡도·함수 길이를 검사한다.
#   기준: 순환복잡도 10 이하, 함수 80라인 이하 (CLAUDE.md 2.3절)
#   검사 도구는 CI와 같은 Docker 이미지(claudeclangbuild)의 Lizard를 사용한다.
#
# 동작
#   - src/ include/ test/ 에 변경(미커밋)된 C++ 파일이 없으면 아무것도 하지 않는다.
#   - 위반이 있으면 종료 코드 2 로 종료해 Claude가 멈추지 못하게 하고, 위반 내용을 stderr 로 전달한다.
#   - 도구를 실행할 수 없는 경우(Docker 중지, 이미지 없음)도 같은 방식으로 알린다.
#   - 같은 문제로 연속 3번 막았는데도 해결되지 않으면 무한 반복을 피하려고 통과시킨다.
#
# 종료 코드: 0 = 통과(또는 대상 없음), 2 = 차단(위반 또는 도구 실행 불가)

readonly IMAGE="claudeclangbuild"
readonly MAX_CONSECUTIVE_BLOCKS=3
counter_file="${TMPDIR:-/tmp}/claudeclang-lizard-hook.count"

cat > /dev/null   # 훅 입력(JSON)은 사용하지 않는다

cd "${CLAUDE_PROJECT_DIR:-$(pwd)}" || exit 0

# 변경된 C++ 파일이 있는지 확인 (추적·미추적 모두)
changed="$(git status --porcelain --untracked-files=all -- src include test 2>/dev/null \
    | grep -E '\.(cpp|cc|cxx|hpp|hh|h)$' | head -n 1)"
if [ -z "$changed" ]; then
    rm -f "$counter_file"
    exit 0
fi

block() {
    local count=0
    [ -f "$counter_file" ] && count="$(cat "$counter_file")"
    count=$((count + 1))
    if [ "$count" -gt "$MAX_CONSECUTIVE_BLOCKS" ]; then
        rm -f "$counter_file"
        echo "[lizard-hook] 같은 문제로 ${MAX_CONSECUTIVE_BLOCKS}번 차단했으나 해결되지 않아 통과시킵니다. 사용자에게 보고하세요." >&2
        exit 0
    fi
    echo "$count" > "$counter_file"
    exit 2
}

if ! docker image inspect "$IMAGE" > /dev/null 2>&1; then
    echo "[lizard-hook] 빌드 이미지 '$IMAGE'(또는 Docker)를 사용할 수 없어 Lizard를 실행하지 못했습니다." >&2
    echo "[lizard-hook] Docker를 실행하고 'bash docker/build.sh'로 이미지를 만든 뒤 다시 확인하세요." >&2
    block
fi

host_dir="$(pwd -W 2>/dev/null || pwd)"
output="$(MSYS_NO_PATHCONV=1 docker run --rm -v "${host_dir}:/workspace" "$IMAGE" \
    bash ci/scripts/check-lizard.sh 2>&1)"
status=$?

if [ "$status" -eq 0 ]; then
    rm -f "$counter_file"
    exit 0
fi

{
    echo "[lizard-hook] 소스 코드 메트릭 기준 위반 (순환복잡도 10 이하, 함수 80라인 이하):"
    echo "$output"
    echo "[lizard-hook] 함수를 작게 나눠 위반을 해결한 뒤 다시 완료하세요."
} >&2
block
