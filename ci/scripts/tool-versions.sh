#!/usr/bin/env bash
# 빌드 이미지의 도구 버전을 출력한다. 빌드 로그에 버전을 남기기 위해 CI 첫 단계에서 실행한다.
# 종료 코드: 0 = 모든 도구 확인, 1 = 없는 도구 있음

missing=0

report() {
    local name="$1"
    shift
    local out
    if out="$("$@" 2>&1 | head -n 1)"; then
        printf '%-14s %s\n' "$name" "$out"
    else
        printf '%-14s 없음\n' "$name"
        missing=1
    fi
}

report clang        clang --version
report llvm-cov     llvm-cov --version
report llvm-profdata llvm-profdata --version
report clang-format clang-format --version
report cmake        cmake --version
report ninja        ninja --version
report cppcheck     cppcheck --version
report lizard       lizard --version
report cloc         cloc --version
report doxygen      doxygen --version
report dot          dot -V
report python3      python3 --version
report git          git --version
if command -v pict >/dev/null 2>&1; then
    printf '%-14s %s\n' pict "설치됨 ($(command -v pict))"
else
    printf '%-14s 없음\n' pict
    missing=1
fi
if [ -f /usr/local/lib/cmake/GTest/GTestConfig.cmake ]; then
    printf '%-14s %s\n' googletest "설치됨 (CMake config 확인)"
else
    printf '%-14s 없음\n' googletest
    missing=1
fi

exit "$missing"
