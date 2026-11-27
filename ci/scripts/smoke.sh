#!/usr/bin/env bash
# 참조 프로젝트(ci/smoke)에 전체 파이프라인을 돌려 툴체인과 품질 게이트가 동작하는지 검증한다.
# 이 프로젝트는 모든 품질 기준을 통과하도록 작성되어 있다. 실패하면 환경(이미지/스크립트) 문제다.
#
# 종료 코드: 0 = 파이프라인 정상, 1 = 실패
CC_PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../smoke" && pwd)"
export CC_PROJECT_DIR
export BUILD_DIR="$CC_PROJECT_DIR/build"

exec "$(dirname "${BASH_SOURCE[0]}")/run-all.sh"
