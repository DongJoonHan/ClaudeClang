#!/usr/bin/env bash
# VERReport(https://github.com/DongJoonHan/VERReport)로 검증 결과 HTML 보고서를 만든다.
#   입력: $BUILD_DIR/reports (cppcheck.xml, lizard.csv, <suite>/{junit.xml,summary.json,lcov.info})
#   출력: $BUILD_DIR/verreport/index.html (+ tests/coverage/cppcheck/lizard.html)
#
# 보고서 생성이 목적이므로 게이트 판정으로 실패하지 않는다 (--fail-on-gate 를 쓰지 않는다).
# 품질 게이트의 합격/불합격은 각 검사 단계와 run-all.sh 가 판단한다.
#
# VERReport 는 main 에 README 만 있고 구현이 feature/verreport 브랜치에 있어, 검토한 커밋으로 고정한다.
# 실행 환경(빌드 이미지)에는 네트워크가 필요하다 (pip 로 VERReport 와 jinja2 설치).
#
# 환경변수(선택): GIT_COMMIT, BUILD_NUMBER, VERREPORT_PROJECT (기본 ClaudeClang)
# 종료 코드: 0 = 보고서 생성, 그 외 = 설치 또는 생성 실패
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

readonly VERREPORT_URL="https://github.com/DongJoonHan/VERReport.git"
readonly VERREPORT_REF="f06421f19f6e3dec0c085530b67c349d0ac3be73"
venv="$BUILD_DIR/.verreport"
out="$BUILD_DIR/verreport"
config="$REPORT_DIR/verreport.toml"
result=0

# 경로 접두어: 개발·GitHub Actions 는 /workspace/, Jenkins 는 작업 공간 절대 경로
cat > "$config" <<TOML
project = "${VERREPORT_PROJECT:-ClaudeClang}"
strip_prefixes = ["/workspace/", "${CC_PROJECT_DIR}/"]
TOML

if [ ! -x "$venv/bin/verreport" ]; then
    python3 -m venv "$venv" || result=1
    if [ "$result" -eq 0 ]; then
        "$venv/bin/pip" install -q "git+${VERREPORT_URL}@${VERREPORT_REF}" || result=1
    fi
fi

if [ "$result" -eq 0 ]; then
    args=(--reports "$REPORT_DIR" --out "$out" --config "$config")
    [ -n "${GIT_COMMIT:-}" ] && args+=(--commit "$GIT_COMMIT")
    [ -n "${BUILD_NUMBER:-}" ] && args+=(--build-id "$BUILD_NUMBER")
    cd "$CC_PROJECT_DIR" || exit 1
    "$venv/bin/verreport" "${args[@]}" || result=1
fi

if [ "$result" -eq 0 ] && [ -f "$out/index.html" ]; then
    echo "[verreport] 보고서: $out/index.html"
else
    echo "[verreport] error: 보고서를 만들지 못함"
    result=1
fi

exit "$result"
