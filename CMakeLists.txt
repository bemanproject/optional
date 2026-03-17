# CMakeLists.txt                                                    -*-CMake-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.27...4.3)

project(beman.optional VERSION 1.0.0 LANGUAGES CXX)

# Includes
include(CTest)
include(FetchContent)

set(TARGETS_EXPORT_NAME ${PROJECT_NAME}Targets)

option(
    OPTIONAL_ENABLE_TESTING
    "Enable building tests and test infrastructure"
    ${PROJECT_IS_TOP_LEVEL}
)

set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL})

# Create the library target and named header set for beman.optional
add_library(beman.optional INTERFACE)
target_sources(
    beman.optional
    PUBLIC FILE_SET beman_optional_headers TYPE HEADERS BASE_DIRS include
)

if(OPTIONAL_ENABLE_TESTING)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        # Create the library target and named header set for testing beman.optional
        # and mark the set private
        add_executable(beman.optional.test)
        target_sources(
            beman.optional.test
            PRIVATE
                FILE_SET beman_optional_test_headers
                    TYPE HEADERS
                    BASE_DIRS tests
        )
        # Tests
        add_subdirectory(tests/beman/optional)
        include(GoogleTest)
        gtest_discover_tests(beman.optional.test)
    else()
        message(
            WARNING
            "
No provider for GTest. Unable to build tests.
Consider using CMAKE_PROJECT_TOP_LEVEL_INCLUDES=./cmake/use-fetch-content.cmake as documented in the README.md
"
        )
    endif()
endif()

add_subdirectory(include/beman/optional)

add_subdirectory(examples)

include(infra/cmake/beman-install-library.cmake)
beman_install_library(beman.optional TARGETS beman.optional)

# Coverage
configure_file("cmake/gcovr.cfg.in" gcovr.cfg @ONLY)

add_custom_target(
    process_coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running gcovr to process coverage results"
    COMMAND mkdir -p coverage
    COMMAND gcovr --config gcovr.cfg .
)
