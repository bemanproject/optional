# tests/beman/optional/CMakeLists.txt                               -*-cmake-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

target_sources(
    beman.optional.test
    PRIVATE
        optional.test.cpp
        optional_constexpr.test.cpp
        optional_monadic.test.cpp
        optional_range_support.test.cpp
        optional_ref.test.cpp
        optional_ref_monadic.test.cpp
)

target_sources(
    beman.optional.test
    PRIVATE
        FILE_SET beman_optional_test_headers
            TYPE HEADERS
            FILES test_types.hpp test_utilities.hpp
)

target_link_libraries(
    beman.optional.test
    PRIVATE beman.optional GTest::gtest GTest::gtest_main
)

add_library(constructor_fails test_constructor_fail.test.cpp)

target_link_libraries(constructor_fails PRIVATE beman.optional)

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
