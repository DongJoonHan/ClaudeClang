#!/usr/bin/env bash
# 개발자와 CI가 공통으로 쓰는 전체 검사. 한 단계가 실패해도 끝까지 실행하고 마지막에 요약한다.
#
# 사용법: ci/scripts/run-all.sh
# 환경변수: CC_PROJECT_DIR, BUILD_DIR (common.sh 참고)
# 종료 코드: 0 = 모든 기준 충족, 1 = 하나 이상 미달
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

declare -a names=()
declare -a statuses=()
overall=0

run_stage() {
    local name="$1"
    shift
    echo
    echo "################ $name ################"
    local rc=0
    "$@" || rc=$?
    names+=("$name")
    if [ "$rc" -eq 0 ]; then
        statuses+=("통과")
    else
        statuses+=("실패")
        overall=1
    fi
}

run_stage "도구 버전" "$CC_SCRIPTS/tool-versions.sh"
run_stage "빌드" "$CC_SCRIPTS/build.sh"
run_stage "Cppcheck" "$CC_SCRIPTS/check-cppcheck.sh"
run_stage "Lizard" "$CC_SCRIPTS/check-lizard.sh"
run_stage "주석(CLOC/HITL)" "$CC_SCRIPTS/check-comments.sh"
run_stage "Doxygen" "$CC_SCRIPTS/check-doxygen.sh"
run_stage "TDD 시험(보고)" "$CC_SCRIPTS/run-tests.sh" tdd
run_stage "단위 시험(MC/DC)" "$CC_SCRIPTS/run-tests.sh" unit
run_stage "통합 시험(함수)" "$CC_SCRIPTS/run-tests.sh" integration
run_stage "시스템 시험(성공률)" "$CC_SCRIPTS/run-tests.sh" system
run_stage "시스템(요구사항)" "$CC_SCRIPTS/check-req-coverage.sh"

echo
echo "================ 요약 ================"
for i in "${!names[@]}"; do
    printf '%-22s %s\n' "${names[$i]}" "${statuses[$i]}"
done
if [ "$overall" -eq 0 ]; then
    echo "결과: 모든 기준 충족"
else
    echo "결과: 기준 미달 있음"
fi

exit "$overall"
