#!/usr/bin/env bash
# 시스템 테스트 합격 기준: 요구사항 커버리지 100% (요구사항 ↔ ST- 케이스 추적).
# 종료 코드: 0 = 100% (또는 요구사항 없음), 1 = 미달
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

cc_tools reqcov "$CC_PROJECT_DIR" | tee "$REPORT_DIR/req-coverage.txt"
exit "${PIPESTATUS[0]}"
