#!/usr/bin/env bash
# CMake + Ninja 빌드 (Clang, 커버리지/MC/DC 계측 포함).
# 종료 코드: 0 = 성공, 그 외 = 구성/빌드 실패
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

result=0
cmake -S "$CC_PROJECT_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
    -DCC_COVERAGE=ON || result=$?
if [ "$result" -eq 0 ]; then
    cmake --build "$BUILD_DIR" || result=$?
fi

exit "$result"
