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

# Jenkins Plot 플러그인용 CSV (첫 줄 = 계열 이름, 둘째 줄 = 값). 주석률 = 주석 / (코드 + 주석)
cloc --json "${dirs[@]}" > "$REPORT_DIR/cloc.json"
if jq -e '.SUM' "$REPORT_DIR/cloc.json" > /dev/null 2>&1; then
    jq -r '.SUM | "comment_ratio_percent\n\(if (.code + .comment) > 0 then ((.comment * 10000 / (.code + .comment) | round) / 100) else 0 end)"' \
        "$REPORT_DIR/cloc.json" > "$REPORT_DIR/cloc-ratio.csv"
    jq -r '.SUM | "code,comment,blank\n\(.code),\(.comment),\(.blank)"' \
        "$REPORT_DIR/cloc.json" > "$REPORT_DIR/cloc-lines.csv"
    echo "[cloc] 주석률 $(tail -n 1 "$REPORT_DIR/cloc-ratio.csv")% (보고만, 합격 기준 없음)"
else
    echo "[cloc] 집계 대상 파일 없음"
fi

cc_tools hitl "$CC_PROJECT_DIR" "${dirs[@]}" | tee "$REPORT_DIR/hitl.txt"
if [ "${PIPESTATUS[0]}" -ne 0 ]; then
    result=1
fi

exit "$result"
