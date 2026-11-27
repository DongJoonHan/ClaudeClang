#!/usr/bin/env bash
# LLVM 소스 기반 커버리지 / MC/DC 측정 스크립트
#
# 사용법:
#   coverage.sh --out <출력 디렉터리> [--mcdc] [--cc <clang>] -- <컴파일 인자...>
#
# 컴파일 인자에는 시험 대상 소스, 시험 코드, 포함 경로, 링크 옵션 등을 모두 넣는다.
# 예)
#   coverage.sh --out build/cov --mcdc -- -Iinclude src/sensor.c test/unit/sensor/test_sensor.c
#
# 환경변수:
#   LLVM_BIN  LLVM 도구 디렉터리 (예: /usr/lib/llvm-18/bin). 없으면 PATH 사용.
#
# 종료 코드: 0 = 측정 성공, 1 = 인자 오류, 2 = 도구 없음, 3 = 빌드 실패,
#            4 = 시험 실행 실패(시험이 실패해도 커버리지 리포트는 출력함), 5 = 리포트 실패
#
# 주의: MC/DC(-fcoverage-mcdc)는 Clang/LLVM 18 이상이 필요하다.

set -u

OUT=""
MCDC=0
CC="clang"
ARGS=()

usage() {
    sed -n '2,16p' "$0"
}

while [ $# -gt 0 ]; do
    case "$1" in
        --out)  OUT="${2:-}"; shift 2 ;;
        --mcdc) MCDC=1; shift ;;
        --cc)   CC="${2:-}"; shift 2 ;;
        -h|--help) usage; exit 0 ;;
        --)     shift; ARGS=("$@"); break ;;
        *)      echo "알 수 없는 옵션: $1" >&2; usage; exit 1 ;;
    esac
done

if [ -z "$OUT" ] || [ ${#ARGS[@]} -eq 0 ]; then
    usage
    exit 1
fi

tool() {
    if [ -n "${LLVM_BIN:-}" ] && [ -x "${LLVM_BIN}/$1" ]; then
        echo "${LLVM_BIN}/$1"
    else
        command -v "$1" 2>/dev/null
    fi
}

if [ -n "${LLVM_BIN:-}" ] && [ -x "${LLVM_BIN}/${CC}" ]; then
    CC="${LLVM_BIN}/${CC}"
fi

PROFDATA="$(tool llvm-profdata)"
COV="$(tool llvm-cov)"
if ! command -v "$CC" >/dev/null 2>&1 || [ -z "$PROFDATA" ] || [ -z "$COV" ]; then
    echo "clang, llvm-profdata, llvm-cov 가 필요합니다. LLVM_BIN 환경변수 또는 PATH를 확인하세요." >&2
    exit 2
fi

mkdir -p "$OUT"
BIN="$OUT/unit_test.bin"
RAW="$OUT/unit_test.profraw"
DATA="$OUT/unit_test.profdata"

FLAGS=(-O0 -g -fprofile-instr-generate -fcoverage-mapping)
if [ "$MCDC" -eq 1 ]; then
    FLAGS+=(-fcoverage-mcdc)
fi

echo "== 도구 버전 =="
"$CC" --version | head -n 1

echo "== 빌드 =="
if ! "$CC" "${FLAGS[@]}" "${ARGS[@]}" -o "$BIN"; then
    echo "빌드 실패" >&2
    exit 3
fi

echo "== 시험 실행 =="
rm -f "$RAW"
STATUS=0
LLVM_PROFILE_FILE="$RAW" "$BIN" || STATUS=4

if [ ! -f "$RAW" ]; then
    echo "프로파일 데이터가 생성되지 않았습니다." >&2
    exit 5
fi

"$PROFDATA" merge -sparse "$RAW" -o "$DATA" || exit 5

echo "== 커버리지 요약 (라인/영역/분기) =="
REPORT_FLAGS=()
SHOW_FLAGS=(-show-line-counts-or-regions -show-branches=count)
if [ "$MCDC" -eq 1 ]; then
    REPORT_FLAGS+=(-show-mcdc-summary)
    SHOW_FLAGS+=(-show-mcdc)
fi
"$COV" report "$BIN" -instr-profile="$DATA" "${REPORT_FLAGS[@]}" || exit 5

echo "== 미커버 위치 상세 =="
"$COV" show "$BIN" -instr-profile="$DATA" "${SHOW_FLAGS[@]}" > "$OUT/coverage-detail.txt" || exit 5
"$COV" show "$BIN" -instr-profile="$DATA" -format=html -output-dir="$OUT/html" \
    "${SHOW_FLAGS[@]}" >/dev/null 2>&1
echo "상세 텍스트: $OUT/coverage-detail.txt"
echo "HTML 리포트:  $OUT/html/index.html"

if [ "$MCDC" -eq 1 ]; then
    echo "== MC/DC 미충족 조건 (상세 파일에서 추출) =="
    grep -n -i -E 'not covered|Condition C[0-9]+ -- Not|MC/DC Decision' "$OUT/coverage-detail.txt" | head -n 100
fi

exit "$STATUS"
