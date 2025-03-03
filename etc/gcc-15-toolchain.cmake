include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/gcc-flags.cmake")

set(CMAKE_C_COMPILER gcc-15)
set(CMAKE_CXX_COMPILER g++-15)

set(CMAKE_CXX_FLAGS_ASAN
    "${CMAKE_CXX_FLAGS_ASAN} -Wno-maybe-uninitialized"
    CACHE STRING
    "C++ ASAN Flags"
    FORCE
)
