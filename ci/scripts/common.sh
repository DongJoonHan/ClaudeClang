#!/usr/bin/env bash
# 공통 변수/함수. 다른 CI 스크립트에서 `source` 한다.
#
# 환경변수:
#   CC_PROJECT_DIR  검사·빌드할 프로젝트 루트 (기본: 저장소 루트)
#   BUILD_DIR       빌드/리포트 출력 위치 (기본: $CC_PROJECT_DIR/build)

CC_REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CC_PROJECT_DIR="${CC_PROJECT_DIR:-$CC_REPO_ROOT}"
BUILD_DIR="${BUILD_DIR:-$CC_PROJECT_DIR/build}"
REPORT_DIR="$BUILD_DIR/reports"
CC_SCRIPTS="$CC_REPO_ROOT/ci/scripts"

mkdir -p "$REPORT_DIR"

# 품질 검사 대상 디렉터리 중 실제로 존재하는 것을 한 줄에 하나씩 출력한다.
src_dirs() {
    local d
    for d in src include test; do
        if [ -d "$CC_PROJECT_DIR/$d" ]; then
            echo "$d"
        fi
    done
}

# 제품 코드(src/ 아래 .cpp)가 있으면 0, 없으면 1.
has_product_code() {
    local count=0
    if [ -d "$CC_PROJECT_DIR/src" ]; then
        count="$(find "$CC_PROJECT_DIR/src" -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' | wc -l)"
    fi
    [ "$count" -gt 0 ]
}

cc_tools() {
    python3 -I "$CC_SCRIPTS/cc_tools.py" "$@"
}
