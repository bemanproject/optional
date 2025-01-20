# cmake-format: off
# src/beman/optional26/tests/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

include(GoogleTest)

# Tests
# add_executable(beman_optional26_test)

target_sources(
    beman_optional26_test
    PRIVATE
        optional.t.cpp
        optional_constexpr.t.cpp
        optional_monadic.t.cpp
        optional_range_support.t.cpp
        optional_ref.t.cpp
        optional_ref_monadic.t.cpp
)

target_sources(
    beman_optional26_test
    PRIVATE
        FILE_SET beman_optional26_test_headers
        TYPE HEADERS
        FILES test_types.hpp test_utilities.hpp
)

target_link_libraries(
    beman_optional26_test
    PRIVATE Beman::Optional26::beman_optional26 GTest::gtest GTest::gtest_main
)

# Issue #32: Re-enable ASAN run CI/clang-19.
#
# Note: clang-19 + gtest_discover_tests + Asan setup causes errors on some
# platforms. Temporary switch to gtest_add_tests and skip some Asan checks.
# Change also applied for CI flows.
gtest_add_tests(TARGET beman_optional26_test "" AUTO)

add_library(constructor_fails test_constructor_fail.cpp)

target_link_libraries(
    constructor_fails
    PRIVATE Beman::Optional26::beman_optional26
)

set_target_properties(
    constructor_fails
    PROPERTIES EXCLUDE_FROM_ALL true EXCLUDE_FROM_DEFAULT_BUILD true
)

add_test(
    NAME constructor_fails
    COMMAND
        ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target
        constructor_fails --config $<CONFIGURATION>
)

set_tests_properties(constructor_fails PROPERTIES WILL_FAIL true)
# Alternatively -- check for a particular regex to pass
# set_tests_properties(constructor_fails PROPERTIES PASS_REGULAR_EXPRESSION
# "Assert true")

# test if the targets are findable from the build directory
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_test(
        NAME find-package-test
        COMMAND
            ${CMAKE_CTEST_COMMAND}
            # --verbose
            --output-on-failure -C $<CONFIG> --build-and-test
            "${PROJECT_SOURCE_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/find-package-test" --build-generator
            ${CMAKE_GENERATOR} --build-makeprogram ${CMAKE_MAKE_PROGRAM}
            --build-options "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
            "-DCMAKE_BUILD_TYPE=$<CONFIG>"
            "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
    )
endif()
