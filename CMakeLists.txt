# CMakeLists.txt                                                    -*-CMake-*-
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(CMAKE_SKIP_TEST_ALL_DEPENDENCY FALSE)

cmake_minimum_required(VERSION 3.29...3.31)

project(beman_optional26 VERSION 0.0.1 LANGUAGES CXX)

# Includes
include(CPack)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(TARGET_PACKAGE_NAME ${PROJECT_NAME})
set(TARGETS_EXPORT_NAME ${TARGET_PACKAGE_NAME}-targets)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_PACKAGE_NAME})

option(
    OPTIONAL26_ENABLE_TESTING
    "Enable building tests and test infrastructure"
    ${PROJECT_IS_TOP_LEVEL}
)

# Build the tests if enabled via the option OPTIONAL26_ENABLE_TESTING
if(OPTIONAL26_ENABLE_TESTING)
    include(FetchContent)

    # Fetch GoogleTest
    FetchContent_Declare(
        googletest
        EXCLUDE_FROM_ALL
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG
            e39786088138f2749d64e9e90e0f9902daa77c40 # release-1.15.0
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
endif()

set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

# Create the library target and named header set for beman_optional26
add_library(beman_optional26 INTERFACE)
add_library(Beman::Optional26::beman_optional26 ALIAS beman_optional26)
target_sources(
    beman_optional26
    PUBLIC FILE_SET beman_optional26_headers TYPE HEADERS BASE_DIRS include
)

if(OPTIONAL26_ENABLE_TESTING)
    # Create the library target and named header set for testing beman_optional26
    # and mark the set private
    add_executable(beman_optional26_test)
    target_sources(
        beman_optional26_test
        PRIVATE
            FILE_SET beman_optional26_test_headers
            TYPE HEADERS
            BASE_DIRS src
    )

    add_subdirectory(src/beman/optional26/tests)
endif()

add_subdirectory(include/beman/optional26)

add_subdirectory(examples)

# install
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    cmake/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-config-version.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Coverage
configure_file("cmake/gcovr.cfg.in" gcovr.cfg @ONLY)

add_custom_target(
    process_coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running gcovr to process coverage results"
    COMMAND mkdir -p coverage
    COMMAND gcovr --config gcovr.cfg .
    DEPENDS test
)
