#!/usr/bin/env bash
# 주석 점검: CLOC으로 주석률을 측정하고(보고만), Doxygen HITL 태그를 검사한다(합격 기준).
# 출력: $BUILD_DIR/reports/cloc.{xml,txt}, hitl.txt
# 종료 코드: 0 = HITL 검사 통과, 1 = 실패
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

mapfile -t dirs < <(src_dirs)
if [ "${#dirs[@]}" -eq 0 ]; then
    echo "[comments] 검사 대상(src/include/test) 없음 -> 통과"
    exit 0
fi

cd "$CC_PROJECT_DIR" || exit 1
result=0

cloc --by-file --xml --out="$REPORT_DIR/cloc.xml" "${dirs[@]}" > /dev/null
cloc "${dirs[@]}" | tee "$REPORT_DIR/cloc.txt"

cc_tools hitl "$CC_PROJECT_DIR" "${dirs[@]}" | tee "$REPORT_DIR/hitl.txt"
if [ "${PIPESTATUS[0]}" -ne 0 ]; then
    result=1
fi

exit "$result"
