#!/usr/bin/env bash
# 시험 스위트 하나를 실행하고 결과·커버리지를 평가한다.
#
# 사용법: run-tests.sh <tdd|unit|integration>
#
# 합격 기준 (CLAUDE.md 2.1절)
#   unit        테스트 성공률 100% + MC/DC 커버리지 100%
#   integration 테스트 성공률 100% + 함수 커버리지 100%
#   tdd         측정·보고만 한다 (합격 기준 없음, 단 시험 실패는 실패로 본다)
#
# 출력: $BUILD_DIR/reports/<suite>/{junit.xml, summary.json, lcov.info, coverage.txt, html/}
# 종료 코드: 0 = 합격, 1 = 불합격, 2 = 사용법 오류
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

suite="${1:-}"
case "$suite" in
    tdd) required=() ; mandatory=0 ;;
    unit) required=(--require mcdc=100) ; mandatory=1 ;;
    integration) required=(--require functions=100) ; mandatory=1 ;;
    *) echo "사용법: $0 <tdd|unit|integration>" >&2 ; exit 2 ;;
esac

bin="$BUILD_DIR/test/$suite/${suite}_tests"
out="$REPORT_DIR/$suite"
rm -rf "$out"
mkdir -p "$out"
result=0

if [ ! -x "$bin" ]; then
    if [ "$mandatory" -eq 1 ] && has_product_code; then
        echo "[$suite] error: 제품 코드(src/)가 있는데 $suite 시험 스위트가 없음"
        result=1
    else
        echo "[$suite] 시험 스위트 없음 (제품 코드 없음 또는 선택 스위트) -> 건너뜀"
    fi
else
    echo "== [$suite] 시험 실행 =="
    LLVM_PROFILE_FILE="$out/%p.profraw" "$bin" --gtest_output="xml:$out/junit.xml" || result=1
    cc_tools tests "$out/junit.xml" || result=1

    echo "== [$suite] 커버리지 =="
    ignore="^(${CC_PROJECT_DIR}/(test|build)/|/usr/|/opt/)"
    if ls "$out"/*.profraw >/dev/null 2>&1 \
        && llvm-profdata merge -sparse "$out"/*.profraw -o "$out/merged.profdata"; then
        llvm-cov report "$bin" -instr-profile="$out/merged.profdata" \
            -ignore-filename-regex="$ignore" -show-mcdc-summary > "$out/coverage.txt"
        cat "$out/coverage.txt"
        llvm-cov export "$bin" -instr-profile="$out/merged.profdata" \
            -ignore-filename-regex="$ignore" -summary-only > "$out/summary.json"
        llvm-cov export "$bin" -instr-profile="$out/merged.profdata" \
            -ignore-filename-regex="$ignore" -format=lcov > "$out/lcov.info"
        llvm-cov show "$bin" -instr-profile="$out/merged.profdata" \
            -ignore-filename-regex="$ignore" -format=html -output-dir="$out/html" \
            -show-branches=count -show-mcdc > /dev/null
        llvm-cov show "$bin" -instr-profile="$out/merged.profdata" \
            -ignore-filename-regex="$ignore" -show-branches=count -show-mcdc \
            > "$out/coverage-detail.txt"
        cc_tools coverage "$out/summary.json" "${required[@]}" || result=1
    else
        echo "[$suite] error: 프로파일 데이터(.profraw)를 만들지 못함 (계측 빌드 확인)"
        result=1
    fi
fi

exit "$result"
