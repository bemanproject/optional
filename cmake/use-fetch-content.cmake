# cmake/use-fetch-content.cmake                                     -*-cmake-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.24)

if(NOT BEMAN_OPTIONAL_LOCKFILE)
    set(BEMAN_OPTIONAL_LOCKFILE
        "lockfile.json"
        CACHE FILEPATH
        "Path to the dependency lockfile for the Beman Optional."
    )
endif()

set(BemanOptional_projectDir "${CMAKE_CURRENT_LIST_DIR}/..")
message(TRACE "BemanOptional_projectDir=\"${BemanOptional_projectDir}\"")

message(TRACE "BEMAN_OPTIONAL_LOCKFILE=\"${BEMAN_OPTIONAL_LOCKFILE}\"")
file(
    REAL_PATH "${BEMAN_OPTIONAL_LOCKFILE}"
    BemanOptional_lockfile
    BASE_DIRECTORY "${BemanOptional_projectDir}"
    EXPAND_TILDE
)
message(DEBUG "Using lockfile: \"${BemanOptional_lockfile}\"")

# Force CMake to reconfigure the project if the lockfile changes
set_property(
    DIRECTORY "${BemanOptional_projectDir}"
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS "${BemanOptional_lockfile}"
)

# For more on the protocol for this function, see:
# https://cmake.org/cmake/help/latest/command/cmake_language.html#provider-commands
function(BemanOptional_provideDependency method package_name)
    # Read the lockfile
    file(READ "${BemanOptional_lockfile}" BemanOptional_rootObj)

    # Get the "dependencies" field and store it in BemanOptional_dependenciesObj
    string(
        JSON BemanOptional_dependenciesObj
        ERROR_VARIABLE BemanOptional_error
        GET "${BemanOptional_rootObj}"
        "dependencies"
    )
    if(BemanOptional_error)
        message(FATAL_ERROR "${BemanOptional_lockfile}: ${BemanOptional_error}")
    endif()

    # Get the length of the libraries array and store it in BemanOptional_dependenciesObj
    string(
        JSON BemanOptional_numDependencies
        ERROR_VARIABLE BemanOptional_error
        LENGTH "${BemanOptional_dependenciesObj}"
    )
    if(BemanOptional_error)
        message(FATAL_ERROR "${BemanOptional_lockfile}: ${BemanOptional_error}")
    endif()

    # Loop over each dependency object
    math(EXPR BemanOptional_maxIndex "${BemanOptional_numDependencies} - 1")
    foreach(BemanOptional_index RANGE "${BemanOptional_maxIndex}")
        set(BemanOptional_errorPrefix
            "${BemanOptional_lockfile}, dependency ${BemanOptional_index}"
        )

        # Get the dependency object at BemanOptional_index
        # and store it in BemanOptional_depObj
        string(
            JSON BemanOptional_depObj
            ERROR_VARIABLE BemanOptional_error
            GET "${BemanOptional_dependenciesObj}"
            "${BemanOptional_index}"
        )
        if(BemanOptional_error)
            message(
                FATAL_ERROR
                "${BemanOptional_errorPrefix}: ${BemanOptional_error}"
            )
        endif()

        # Get the "name" field and store it in BemanOptional_name
        string(
            JSON BemanOptional_name
            ERROR_VARIABLE BemanOptional_error
            GET "${BemanOptional_depObj}"
            "name"
        )
        if(BemanOptional_error)
            message(
                FATAL_ERROR
                "${BemanOptional_errorPrefix}: ${BemanOptional_error}"
            )
        endif()

        # Get the "package_name" field and store it in BemanOptional_pkgName
        string(
            JSON BemanOptional_pkgName
            ERROR_VARIABLE BemanOptional_error
            GET "${BemanOptional_depObj}"
            "package_name"
        )
        if(BemanOptional_error)
            message(
                FATAL_ERROR
                "${BemanOptional_errorPrefix}: ${BemanOptional_error}"
            )
        endif()

        # Get the "git_repository" field and store it in BemanOptional_repo
        string(
            JSON BemanOptional_repo
            ERROR_VARIABLE BemanOptional_error
            GET "${BemanOptional_depObj}"
            "git_repository"
        )
        if(BemanOptional_error)
            message(
                FATAL_ERROR
                "${BemanOptional_errorPrefix}: ${BemanOptional_error}"
            )
        endif()

        # Get the "git_tag" field and store it in BemanOptional_tag
        string(
            JSON BemanOptional_tag
            ERROR_VARIABLE BemanOptional_error
            GET "${BemanOptional_depObj}"
            "git_tag"
        )
        if(BemanOptional_error)
            message(
                FATAL_ERROR
                "${BemanOptional_errorPrefix}: ${BemanOptional_error}"
            )
        endif()

        if(method STREQUAL "FIND_PACKAGE")
            if(package_name STREQUAL BemanOptional_pkgName)
                string(
                    APPEND BemanOptional_debug
                    "Redirecting find_package calls for ${BemanOptional_pkgName} "
                    "to FetchContent logic fetching ${BemanOptional_repo} at "
                    "${BemanOptional_tag} according to ${BemanOptional_lockfile}."
                )
                message(STATUS "${BemanOptional_debug}")
                FetchContent_Declare(
                    "${BemanOptional_name}"
                    GIT_REPOSITORY "${BemanOptional_repo}"
                    GIT_TAG "${BemanOptional_tag}"
                    EXCLUDE_FROM_ALL
                )
                set(INSTALL_GTEST OFF) # Disable GoogleTest installation
                FetchContent_MakeAvailable("${BemanOptional_name}")

                # Important! <PackageName>_FOUND tells CMake that `find_package` is
                # not needed for this package anymore
                message(STATUS "setting ${BemanOptional_pkgName}_FOUND to true")
                set("${BemanOptional_pkgName}_FOUND" TRUE PARENT_SCOPE)
            endif()
        endif()
    endforeach()

    # set(GTest_FOUND TRUE PARENT_SCOPE)
endfunction()

cmake_language(
    SET_DEPENDENCY_PROVIDER BemanOptional_provideDependency
    SUPPORTED_METHODS FIND_PACKAGE
)
