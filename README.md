# beman.optional: C++26 Extensions for std::optional

<!--
SPDX-License-Identifier: 2.0 license with LLVM exceptions
-->

<!-- markdownlint-disable -->
<img src="https://github.com/bemanproject/beman/blob/main/images/logos/beman_logo-beman_library_production_ready_api_may_undergo_changes.png" style="width:5%; height:auto;"> ![CI Tests](https://github.com/bemanproject/optional/actions/workflows/ci.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/bemanproject/optional/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/optional?branch=main)
<!-- markdownlint-enable -->

This repository implements `std::optional` extensions targeting C++26. The `beman.optional` library aims to evaluate
the stability, the usability, and the performance of these proposed changes before they are officially adopted by WG21
into the C++ Working Draft. Additionally, it allows developers to use these new features before they are implemented in
major standard library compilers.

**Implements**: [Give *std::optional* Range Support (P3168R2)](https://wg21.link/P3168R2) and [`std::optional<T&>` (P2988R5)](https://wg21.link/P2988R5)

**Status**: [Production ready. API may undergo changes.](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_LIBRARY_MATURITY_MODEL.md#production-ready-api-may-undergo-changes)

## License

Source is licensed with the Apache 2.0 license with LLVM exceptions

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

Documentation and associated papers are licensed with the Creative Commons Attribution 4.0 International license.

// SPDX-License-Identifier: CC-BY-4.0

The intent is that the source and documentation are available for use by people implementing their own optional types
as well as people using the optional presented here as-is.

The README itself is licensed with CC0 1.0 Universal. Copy the contents and incorporate in your own work as you see
fit.

// SPDX-License-Identifier: CC0-1.0

## Examples

Full runable examples can be found in `examples/` - please check [./examples/README.md](./examples/README.md).

### range_loop

The next code snippet shows optional range support added in [Give *std::optional* Range Support(P3168R2)](https://wg21.link/P3168R2):

```cpp
#include <beman/optional/optional.hpp>
...

// Example from P3168R2: basic range loop over C++26 optional.

beman::optional::optional<int> empty_opt{};
for ([[maybe_unused]] const auto& i : empty_opt) {
    // Should not see this in console.
    std::cout << "\"for each loop\" over C++26 optional: empty_opt\n";
}

beman::optional::optional<int> opt{26};
for (const auto& i : opt) {
    // Should see this in console.
    std::cout << "\"for each loop\" over C++26 optional: opt = " << i << "\n";
}
```

Full code can be found in [./examples/range_loop.cpp](./examples/range_loop.cpp). Build and run instructions in
[./examples/README.md](./examples/README.md). Or try it on Compiler Explorer: [range_loop.cpp@Compiler Explorer](https://godbolt.org/z/Gc6Y9j6zf).

### optional_ref

The next code snippet shows optional reference support added in [`std::optional<T&>`
(P2988R5)](https://wg21.link/P2988R5):

```cpp
#include <beman/optional/optional.hpp>
...

{
    // Empty optional example.
    std::optional<int>             std_empty_opt;
    beman::optional::optional<int> beman_empty_opt;
    assert(!std_empty_opt.has_value() &&
            !beman_empty_opt.has_value()); // or assert(!std_empty_opt && !beman_empty_opt);
    std::cout << "std_vs_beman: .has_value() matches?: "
              << (std_empty_opt.has_value() == beman_empty_opt.has_value() ? "yes" : "no") << "\n";
}

{
    // Optional with value example.
    std::optional<int>             std_opt   = 26;
    beman::optional::optional<int> beman_opt = 26;
    assert(std_opt.has_value() && beman_opt.has_value()); // or assert(std_opt && beman_opt);
    assert(std_opt.value() == beman_opt.value());         // or assert(*std_opt == *beman_opt);
    std::cout << "std_vs_beman: .value() matches?: " << (std_opt.value() == beman_opt.value() ? "yes" : "no")
              << "\n";
}
```

Full code can be found in [./examples/optional_ref.cpp](./examples/optional_ref.cpp). Build and run instructions in [./examples/README.md](./examples/README.md).

## How to Build

### Compiler Support

This is a modern C++ project which can be compiled with the latest C++ standards (**C++20 or later**).

Default build: `C++23`. Please check `etc/${compiler}-flags.cmake`.

### Dependencies

This project is mainly tested on `Ubuntu 22.04` and `Ubuntu 24.04`, but it should be as portable as CMake is. This
project has no C or C++ dependencies.

Build-time dependencies:

* `cmake`
* `ninja`, `make`, or another CMake-supported build system
  * CMake defaults to "Unix Makefiles" on POSIX systems

Example of installation on `Ubuntu 24.04`:

```shell
# Install tools:
apt-get install -y cmake make ninja-build

# Example of toolchains:
apt-get install                           \
  g++-14 gcc-14 gcc-13 g++-13             \
  clang-18 clang++-18 clang-17 clang++-17
```

### Instructions

Full set of supported toolchains can be found in [.github/workflows/ci.yml](.github/workflows/ci.yml).

#### Preset CMake Flows

This project strives to be as normal and simple a CMake project as possible. This build workflow in particular will
work, producing a static `beman_optional` library, ready to package:

```shell
# List available preset configurations:
$ cmake --workflow --list-presets
Available workflow presets:

  "system"
  "gcc-14"
  "gcc-13"
  "clang-18"
  "clang-17"

# Run examples:
$ cmake --workflow --preset gcc-14
cmake --workflow --preset gcc-14
Executing workflow step 1 of 3: configure preset "gcc-14"
...
-- Build files have been written to: /path/to/repo/.build/gcc-14

Executing workflow step 2 of 3: build preset "gcc-14"

ninja: no work to do.

Executing workflow step 3 of 3: test preset "gcc-14"

Test project /path/to/repo/.build/gcc-14
        Start   1: OptionalTest.TestGTest
  1/... Test   #1: OptionalTest.TestGTest ...........................   Passed    0.00 sec
...
        Start   x: RangeSupportTest.RangeConcepts
.../... Test   #x: RangeSupportTest.RangeConcepts ...................   Passed    0.00 sec
        Start x+1: RangeSupportTest.IteratorConcepts
.../... Test #x+1: RangeSupportTest.IteratorConcepts ................   Passed    0.00 sec
...

100% tests passed, 0 tests failed out of ...

Total Test time (real) =   0.09 sec
```

This should build and run the tests with GCC 14 with the address and undefined behavior sanitizers enabled.

#### Custom CMake Flows

##### Build and Run Tests

CI current build and test flows:

```shell
# Configure build: default build production code + tests (OPTIONAL_ENABLE_TESTING=ON by default).
$ cmake -G "Ninja Multi-Config" \
      -DCMAKE_CONFIGURATION_TYPES="RelWithDebInfo;Asan" \
      -DCMAKE_TOOLCHAIN_FILE=etc/clang-19-toolchain.cmake \
      -B .build -S .
-- The CXX compiler identification is Clang 19.0.0
...
-- Build files have been written to: /path/to/optional/.build

# Build.
$ cmake --build .build --config Asan --target all -- -k 0
...
[30/30] Linking CXX executable ... # Note: 30 targets here (including tests).

# Run tests.
$ ctest --build-config Asan --output-on-failure --test-dir .build
Internal ctest changing into directory: /path/to/optional/.build
Test project /path/to/optional/.build
...
100% tests passed, 0 tests failed out of 82

Total Test time (real) =   0.67 sec
```

##### Build Production, but Skip Tests

By default, we build and run tests. You can provide `-DOPTIONAL_ENABLE_TESTING=OFF` and completely disable building tests:

```shell
# Configure build: build production code, skip tests (OPTIONAL_ENABLE_TESTING=OFF).
$ cmake -G "Ninja Multi-Config" \
      -DCMAKE_CONFIGURATION_TYPES="RelWithDebInfo;Asan" \
      -DCMAKE_TOOLCHAIN_FILE=etc/clang-19-toolchain.cmake \
      -DOPTIONAL_ENABLE_TESTING=OFF \
      -B .build -S .
-- The CXX compiler identification is Clang 19.0.0
...
-- Build files have been written to: /path/to/optional/.build

# Build.
$ cmake --build .build --config Asan --target all -- -k 0
...
[15/15] Linking CXX executable ... # Note: 15 targets here (tests were not built).

# Check that tests are not built/installed.
$ ctest --build-config Asan --output-on-failure --test-dir .build
Internal ctest changing into directory: /path/to/beman.optional/.build
Test project /path/to/beman.optional/.build
No tests were found!!!
```

#### Pre-Commit for Linting

Various linting tools are configured and installed via the [pre-commit](https://pre-commit.com/) framework. This
requires a working python environment, but also allows the tools, such as clang-format and cmake-lint, to be versioned
on a per project basis rather than being installed globally. Version changes in lint checks often means differences in
success or failure between the versions in CI and the versions used by a developer. By using the same configurations,
this problem is avoided.

In order to set up a python environment, using a python virtual environment can simplify maintaining different
configurations between projects. There is no particular dependency on a particular python3 version.

##### Creating and configuring a venv

```shell
python3 -m venv .venv
. .venv/bin/activate && python3 -m pip install --upgrade pip setuptools wheel
. .venv/bin/activate && python3 -m pip install pip-tools
. .venv/bin/activate && python3 -m piptools sync requirements.txt
. .venv/bin/activate && python3 -m piptools sync requirements-dev.txt
. .venv/bin/activate && exec bash
```

This will create the venv, install the python and python development tools, and run bash with the PATH and other
environment variables set to use the venv preferentially.

##### Running the linting tools

```shell
pre-commit run -a
```

This will download and configure the lint tools specified in .pre-commit-config.yaml.

There is also a Makefile that will automate this process and keep everything up to date.

```shell
make lint
```

## Papers

Latest revision(s) of the papers can be built / found at:

* [give-std-optional-range-support](https://github.com/neatudarius/give-std-optional-range-support/) for
`Give *std::optional* Range Support (P3168)`
  * issue: [#1831](https://github.com/cplusplus/papers/issues/1831)
  * LEWG:
    * Reviewed in Tokyo 2024.
    * Forwarded by LEWG April electronic poll to LWG.
  * LWG:
    * Reviewed and approved in Saint Louis 2024.
* [./papers/P2988/README.md](./papers/P2988/README.md) for `std::optional<T&> (P2988)`.
  * issue: [#1661](https://github.com/cplusplus/papers/issues/1661)
  * LEWG:
    * Reviewed in Tokyo 2024.
