# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

cmake_minimum_required(VERSION 3.29...3.31)

project(beman_optional26_example VERSION 0.0.1 LANGUAGES CXX)

set(BEMAN_OPTIONAL26_LIBRARY "Beman::Optional26::beman_optional26")

if(PROJECT_IS_TOP_LEVEL)
    find_package(beman_optional26 0.0.1 EXACT REQUIRED)
endif()

# List of all buildable examples.
set(EXAMPLES
    concept_checks
    optional_ref
    pythagorean_triples
    range_loop
    sample
    std_vs_beman
)

foreach(example ${EXAMPLES})
    # Add example executable.
    add_executable(${example} "")

    # Add example source file.
    target_sources(${example} PRIVATE ${example}.cpp)

    # Link example with the library.
    target_link_libraries(${example} "${BEMAN_OPTIONAL26_LIBRARY}")

    if(NOT PROJECT_IS_TOP_LEVEL)
        # Install .
        install(
            TARGETS
                ${example}
                COMPONENT
                beman_optional26_examples
                DESTINATION
                ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endforeach()
