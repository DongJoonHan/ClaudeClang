#!/usr/bin/env bash
# 저장소를 /workspace 에 마운트한 빌드 컨테이너에서 명령을 실행한다 (개발 빌드 = CI 빌드).
#   사용법: docker/run.sh [명령...]      명령이 없으면 대화형 bash
#   예)     docker/run.sh ci/scripts/run-all.sh
#           docker/run.sh ci/scripts/smoke.sh
# Windows(Git Bash)에서도 동작한다.
set -u
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

host_dir="$(pwd -W 2>/dev/null || pwd)"
tty_args=()
if [ -t 0 ] && [ -t 1 ]; then
    tty_args=(-it)
fi

MSYS_NO_PATHCONV=1 docker run --rm "${tty_args[@]}" \
    -v "${host_dir}:/workspace" \
    claudeclangbuild:latest "${@:-bash}"
