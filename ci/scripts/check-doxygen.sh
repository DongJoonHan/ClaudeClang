#!/usr/bin/env bash
# Doxygen 주석 검증: 경고는 오류다 (Doxyfile의 WARN_AS_ERROR).
# 출력: $BUILD_DIR/doxygen/, $BUILD_DIR/reports/doxygen-warnings.txt
# 종료 코드: 0 = 경고 없음, 1 = 경고 있음
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

mapfile -t dirs < <(src_dirs)
if [ "${#dirs[@]}" -eq 0 ]; then
    echo "[doxygen] 검사 대상(src/include/test) 없음 -> 통과"
    exit 0
fi

cd "$CC_PROJECT_DIR" || exit 1
config="$BUILD_DIR/Doxyfile.run"
{
    cat "$CC_REPO_ROOT/Doxyfile"
    echo "OUTPUT_DIRECTORY = $BUILD_DIR/doxygen"
    echo "INPUT = ${dirs[*]}"
    echo "STRIP_FROM_PATH = $CC_PROJECT_DIR"
} > "$config"

result=0
doxygen "$config" 2> "$REPORT_DIR/doxygen-warnings.txt" || result=1
cat "$REPORT_DIR/doxygen-warnings.txt"
count="$(grep -c . "$REPORT_DIR/doxygen-warnings.txt")"
echo "[doxygen] 경고 ${count}건"
if [ "$count" -gt 0 ]; then
    result=1
fi

exit "$result"
