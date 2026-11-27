#!/usr/bin/env bash
# Cppcheck 정적 분석. 지적 0건이어야 한다. 예외(억제)는 허용하지 않는다.
#   --suppress, --suppressions-list, --inline-suppr 를 사용하지 않는다.
#
# 점검 규칙(CLAUDE.md 2.2절): error(항상) + warning, style, performance, portability.
# --enable=all 에서 빠진 점검은 세 가지이며, 개별 지적을 억제하는 것이 아니라 점검 목록에서 뺀 것이다.
#   information      checkersReport 같은 메타 메시지
#   missingInclude   표준 헤더 미발견 안내. 헤더 누락은 -Werror 빌드에서 이미 실패한다.
#   unusedFunction   googletest.cfg가 TEST() 본문을 "호출되지 않는 함수"로 전개하는 오탐이 있다.
#                    제품 함수가 죽은 코드가 아닌지는 통합 시험의 함수 커버리지 100%가 보장한다.
#
# 빌드 후에는 compile_commands.json으로 실행해 -isystem(서드파티) 헤더의 지적을 제외한다.
# 빌드 전이거나 컴파일 DB가 없으면 디렉터리 직접 검사로 대신한다.
#
# 출력: $BUILD_DIR/reports/cppcheck.{txt,xml}
# 종료 코드: 0 = 지적 없음, 1 = 지적 있음
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

mapfile -t dirs < <(src_dirs)
if [ "${#dirs[@]}" -eq 0 ]; then
    echo "[cppcheck] 검사 대상(src/include/test) 없음 -> 통과"
    exit 0
fi

cd "$CC_PROJECT_DIR" || exit 1
rules="warning,style,performance,portability"

if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "[cppcheck] compile_commands.json 기준으로 검사"
    target=(--project="$BUILD_DIR/compile_commands.json")
else
    echo "[cppcheck] 컴파일 DB 없음 -> 디렉터리 직접 검사"
    includes=()
    [ -d include ] && includes+=(-I include)
    [ -d src ] && includes+=(-I src)
    target=("${includes[@]}" "${dirs[@]}")
fi

common_args=(--enable="$rules" --inconclusive --check-level=exhaustive
    --std=c17 --std=c++20 --library=googletest
    --error-exitcode=1 --quiet)

result=0
cppcheck "${common_args[@]}" --template='{file}:{line}: {severity}: [{id}] {message}' \
    "${target[@]}" 2> "$REPORT_DIR/cppcheck.txt" || result=1
cppcheck "${common_args[@]}" --xml --xml-version=2 "${target[@]}" 2> "$REPORT_DIR/cppcheck.xml" || result=1

count="$(grep -c . "$REPORT_DIR/cppcheck.txt")"
cat "$REPORT_DIR/cppcheck.txt"
echo "[cppcheck] 지적 ${count}건"
if [ "$count" -gt 0 ]; then
    result=1
fi

exit "$result"
