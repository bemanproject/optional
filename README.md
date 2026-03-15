# beman.optional: C++26 Extensions for std::optional

<!--
SPDX-License-Identifier: 2.0 license with LLVM exceptions
-->

<!-- markdownlint-disable -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_production_ready_api_may_undergo_changes.svg)![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp26.svg)[![Compiler Explorer Example](https://img.shields.io/badge/Try%20it%20on%20Compiler%20Explorer-grey?logo=compilerexplorer&logoColor=67c52a)](https://godbolt.org/z/Gc6Y9j6zf)
<!-- markdownlint-enable -->

This repository implements `std::optional` extensions targeting C++26. The `beman.optional` library aims to evaluate the stability, the usability, and the performance of these proposed changes before they are officially adopted by WG21 into the C++ Working Draft. Additionally, it allows developers to use these new features before they are implemented in major standard library compilers.

**Implements**: [`std::optional<T&>` (P2988R12)](https://wg21.link/P2988R12) and
[Give *std::optional* Range Support (P3168R2)](https://wg21.link/P3168R2).

**Status**: [Production ready. API may undergo changes.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#production-ready-api-may-undergo-changes)

## Examples

Full runable examples can be found in `examples/` - please check [./examples/README.md](./examples/README.md).

### range_loop

The next code snippet shows optional range support added in [Give *std::optional* Range Support(P3168R2)](https://wg21.link/P3168R2):

```cpp
#include <beman/optional/optional.hpp>
...

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
(P2988)](https://wg21.link/P2988):

```cpp
#include <beman/optional/optional.hpp>
...

struct Cat { ... };

beman::optional::optional<Cat&> find_cat(std::string) { return {}; }
beman::optional::optional<Cat&> do_it(Cat& cat) { return { cat }; }

beman::optional::optional<Cat&> api() {
    beman::optional::optional<Cat&> cat = find_cat("Fido");
    return cat.and_then(do_it);
}

beman::optional::optional<Cat&> cat = api();

```

Full code can be found in [./examples/optional_ref.cpp](./examples/optional_ref.cpp). Build and run instructions in [./examples/README.md](./examples/README.md). Or try it on Compiler Explorer: [optional_ref.cpp@Compiler Explorer](https://godbolt.org/z/MxjdvTTov).

## License

`beman.optional` is licensed under the Apache License v2.0 with LLVM Exceptions.

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

Documentation and associated papers are licensed with the Creative Commons Attribution 4.0 International license.

// SPDX-License-Identifier: CC-BY-4.0

The intent is that the source and documentation are available for use by people implementing their own optional types as well as people using the optional presented here as-is.

The README itself is licensed with CC0 1.0 Universal. Copy the contents and incorporate in your own work as you see fit.

// SPDX-License-Identifier: CC0-1.0

## How to Build

### Compiler Support

This is a modern C++ project which can be compiled with the latest C++ standards (**C++20 or later**).

Default build: `C++23`. Please check `etc/${compiler}-flags.cmake`.

### Dependencies

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

<details>
<summary> Build GoogleTest dependency from github.com </summary>

If you do not have GoogleTest installed on your development system, you may
optionally configure this project to download a known-compatible release of
GoogleTest from source and build it as well.

```shell
cmake -B build -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=./cmake/use-fetch-content.cmake
```

The precise version of GoogleTest that will be used is maintained in
`./lockfile.json`.

</details>

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 15-13   | C++26-C++20   | libstdc++         |
| GCC        | 12      | C++23, C++20  | libstdc++         |
| Clang      | 22-19   | C++26-C++20   | libstdc++, libc++ |
| Clang      | 18      | C++26-C++20   | libc++            |
| AppleClang | latest  | C++26-C++20   | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |

### Instructions

#### Develop using GitHub Codespace

This project supports [GitHub Codespace](https://github.com/features/codespaces)
via [Development Containers](https://containers.dev/),
which allows rapid development and instant hacking in your browser.
We recommend using GitHub codespace to explore this project as it
requires minimal setup.

Click the following badge to create a codespace:

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/bemanproject/optional)

For more documentation on GitHub codespaces, please see
[this doc](https://docs.github.com/en/codespaces/).

> [!NOTE]
>
> The codespace container may take up to 5 minutes to build and spin-up; this is normal.

#### Preset CMake Flows

This project recommends using [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
to configure, build and test the project.
Appropriate presets for major compilers have been included by default.
You can use `cmake --list-presets` to see all available presets.

Here is an example to invoke the `gcc-debug` preset.

```shell
cmake --workflow --preset gcc-debug
```

Generally, there are two kinds of presets, `debug` and `release`.

The `debug` presets are designed to aid development, so it has debugging
instrumentation enabled and many sanitizers enabled.

> [!NOTE]
>
> The sanitizers that are enabled vary from compiler to compiler.
> See the toolchain files under ([`cmake`](cmake/)) to determine the exact configuration used for each preset.

The `release` presets are designed for production use, and
consequently have the highest optimization turned on (e.g. `O3`).

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

Various linting tools are configured and installed via the [pre-commit](https://pre-commit.com/) framework. This requires a working python environment, but also allows the tools, such as clang-format and cmake-lint, to be versioned on a per project basis rather than being installed globally. Version changes in lint checks often means differences in success or failure between the versions in CI and the versions used by a developer. By using the same configurations, this problem is avoided.

In order to set up a python environment, using a python virtual environment can simplify maintaining different configurations between projects. There is no particular dependency on a particular python3 version.

##### Creating and configuring a venv

```shell
python3 -m venv .venv
. .venv/bin/activate && python3 -m pip install --upgrade pip setuptools wheel
. .venv/bin/activate && python3 -m pip install pip-tools
. .venv/bin/activate && python3 -m piptools sync requirements.txt
. .venv/bin/activate && python3 -m piptools sync requirements-dev.txt
. .venv/bin/activate && exec bash
```

This will create the venv, install the python and python development tools, and run bash with the PATH and other environment variables set to use the venv preferentially.

##### Running the linting tools

```shell
pre-commit run -a
```

This will download and configure the lint tools specified in .pre-commit-config.yaml.

There is also a Makefile that will automate this process and keep everything up to date.

```shell
make lint
```

#### Install beman.optional


```bash
cmake --workflow --preset gcc-release
cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure at `/opt/beman`.

```txt
/opt/beman
├── include
│   └── beman
│       └── optional
│           ├── detail
│           │   ├── iterator.hpp
│           │   └── stl_interfaces
│           │       ├── config.hpp
│           │       ├── fwd.hpp
│           │       └── iterator_interface.hpp
│           └── optional.hpp
└── lib
    └── cmake
        └── beman.optional
            ├── beman.optional-config.cmake
            ├── beman.optional-config-version.cmake
            └── beman.optional-targets.cmake
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
    * Forwarded by LEWG in 2025 in Hagenberg.
  * LWG:
    * Reviewed and approved in Hagenberg 2025.
