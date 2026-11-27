#!/usr/bin/env bash
# Lizard: 순환복잡도 10 이하, 함수 길이 80라인 이하 (CLAUDE.md 2.3절).
# 사용법: check-lizard.sh            (프로젝트의 src/include/test 검사)
# 출력: $BUILD_DIR/reports/lizard.txt (gcc 형식 경고), lizard.xml
# 종료 코드: 0 = 위반 없음, 1 = 위반 있음
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

readonly MAX_CCN=10
readonly MAX_LINES=80

mapfile -t dirs < <(src_dirs)
if [ "${#dirs[@]}" -eq 0 ]; then
    echo "[lizard] 검사 대상(src/include/test) 없음 -> 통과"
    exit 0
fi

cd "$CC_PROJECT_DIR" || exit 1
result=0
lizard -l cpp -C "$MAX_CCN" -L "$MAX_LINES" -w "${dirs[@]}" > "$REPORT_DIR/lizard.txt" || result=1
lizard -l cpp --xml "${dirs[@]}" > "$REPORT_DIR/lizard.xml" 2>/dev/null

cat "$REPORT_DIR/lizard.txt"
count="$(grep -c 'warning' "$REPORT_DIR/lizard.txt")"
echo "[lizard] 기준 CCN<=${MAX_CCN}, 함수<=${MAX_LINES}라인 -> 위반 ${count}건"
if [ "$count" -gt 0 ]; then
    result=1
fi

exit "$result"
