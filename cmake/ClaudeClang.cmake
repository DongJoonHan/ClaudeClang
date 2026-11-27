# ClaudeClang 공통 CMake 모듈
#
# 사용:
#   include(<repo>/cmake/ClaudeClang.cmake)
#   cc_setup_project()
#
# 프로젝트 구조 규약 (CLAUDE.md 6절):
#   src/        제품 코드  (src/CMakeLists.txt가 있으면 포함)
#   include/    공개 헤더
#   test/tdd, test/unit, test/integration, test/system  각 스위트 (CMakeLists.txt가 있으면 포함)
#
# 스위트마다 실행 파일 `<suite>_tests`가 build/test/<suite>/ 에 만들어진다.
# 커버리지 계측은 제품 코드와 시험 코드 모두에 적용되고, 리포트에서 시험 코드는 제외한다.

include_guard(GLOBAL)

set(CC_SUITES tdd unit integration system)

# 컴파일러 요구 사항: Clang 18 이상 (MC/DC 계측: -fcoverage-mcdc)
macro(cc_require_clang)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR "Clang이 필요합니다 (현재: ${CMAKE_CXX_COMPILER_ID}). 빌드 이미지 claudeclangbuild를 사용하세요.")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 18)
        message(FATAL_ERROR "Clang 18 이상이 필요합니다 (현재: ${CMAKE_CXX_COMPILER_VERSION}).")
    endif()
endmacro()

# 대상에 공통 경고/오류 옵션과 커버리지 계측 옵션을 적용한다.
function(cc_apply_options target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(CC_COVERAGE)
        target_compile_options(${target} PRIVATE
            -O0 -g -fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc)
        target_link_options(${target} PRIVATE -fprofile-instr-generate)
    endif()
endfunction()

# 제품 라이브러리 정의 도우미:
#   cc_add_library(<name> SOURCES a.cpp ... [PUBLIC_INCLUDE dir] [LIBS ...])
function(cc_add_library name)
    cmake_parse_arguments(ARG "" "PUBLIC_INCLUDE" "SOURCES;LIBS" ${ARGN})
    add_library(${name} STATIC ${ARG_SOURCES})
    if(ARG_PUBLIC_INCLUDE)
        target_include_directories(${name} PUBLIC ${ARG_PUBLIC_INCLUDE})
    endif()
    if(ARG_LIBS)
        target_link_libraries(${name} PUBLIC ${ARG_LIBS})
    endif()
    cc_apply_options(${name})
endfunction()

# 시험 스위트 정의 도우미:
#   cc_add_test_suite(<tdd|unit|integration|system> SOURCES a.cpp ... [LIBS ...])
# GoogleTest + GoogleMock(gtest_main)에 링크하고 CTest 라벨로 스위트를 표시한다.
function(cc_add_test_suite suite)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
    if(NOT suite IN_LIST CC_SUITES)
        message(FATAL_ERROR "알 수 없는 스위트: ${suite} (허용: ${CC_SUITES})")
    endif()
    set(target ${suite}_tests)
    add_executable(${target} ${ARG_SOURCES})
    # 제품 라이브러리는 whole-archive로 링크한다. 시험이 참조하지 않는 함수도 바이너리에
    # 포함되어야 함수 커버리지가 "도달하지 못한 함수"를 놓치지 않는다.
    foreach(lib IN LISTS ARG_LIBS)
        target_link_libraries(${target} PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,${lib}>")
    endforeach()
    target_link_libraries(${target} PRIVATE GTest::gmock GTest::gtest_main)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/${suite})
    cc_apply_options(${target})
    add_test(NAME ${suite} COMMAND ${target})
    set_tests_properties(${suite} PROPERTIES LABELS ${suite})
endfunction()

# 프로젝트 전체 설정. 최상위 CMakeLists.txt에서 project() 다음에 호출한다.
macro(cc_setup_project)
    cc_require_clang()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    option(CC_COVERAGE "LLVM 소스 기반 커버리지(MC/DC 포함) 계측" ON)

    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt)
        add_subdirectory(src)
    endif()
    foreach(_suite IN LISTS CC_SUITES)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/${_suite}/CMakeLists.txt)
            add_subdirectory(test/${_suite})
        endif()
    endforeach()
endmacro()
