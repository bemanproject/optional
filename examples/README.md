# Examples for beman.optional

<!--
SPDX-License-Identifier: 2.0 license with LLVM exceptions
-->

List of usage examples for `beman.optional`.

## Samples

Check basic `beman.optional` library usages:

* local [./sample.cpp](./sample.cpp) or [sample.cpp@Compiler Explorer](https://godbolt.org/z/ebEzYc5fv)
* local [./std_vs_beman.cpp](./std_vs_beman.cpp) or [std_vs_beman.cpp@Compiler Explorer](https://godbolt.org/z/P1c8x135M)
* local [./concept_checks.cpp](./concep_checks.cpp) or [concept_checks.cpp@Compiler Explorer](https://godbolt.org/z/9893TMod4)

### Local Build and Run

```shell
# build
$ cmake --workflow --preset gcc-14

# run sample.cpp
$ .build/gcc-14/examples/RelWithDebInfo/sample
empty_opt is empty!
opt = 26

# run std_vs_beman.cpp
$ .build/gcc-14/examples/RelWithDebInfo/std_vs_beman
std_vs_beman: .has_value() matches?: yes
std_vs_beman: .value() matches?: yes

# run concept_checks.cpp
$ .build/gcc-14/examples/RelWithDebInfo/concept_checks
```

## Range Support (P3168R2)

Range support added in [*Give std::optional Range Support* (P3168R2)](https://wg21.link/P3168R2) examples:

* local [./range_loop.cpp](./range_loop.cpp) or [range_loop.cpp@Compiler Explorer](https://godbolt.org/z/Gc6Y9j6zf)
* local [./pythagorean_triples.cpp](./pythagorean_triples.cpp) or [pythagorean_triples.cpp@Compiler Explorer](https://godbolt.org/z/hjvj4ez7W)

### Local Build and Run

```shell
# build
$ cmake --workflow --preset gcc-14

# run range_loop.cpp
$ .build/gcc-14/examples/RelWithDebInfo/range_loop
                                               # note: 1st log (empty_opt) is missing from console!
"for each loop" over C++26 optional: opt = 26  # 2nd log (non empty opt)

# run pythagorean_triples.cpp
$ .build/gcc-14/examples/RelWithDebInfo/pythagorean_triples
First 10 Pythagorean triples:
(3, 4, 5)
(6, 8, 10)
(5, 12, 13)
(9, 12, 15)
(8, 15, 17)
(12, 16, 20)
(7, 24, 25)
(15, 20, 25)
(10, 24, 26)
(20, 21, 29)
100th Pythagorean triple (x, y, z <= 200): (26, 168, 170)
```

## Optional Reference (P2988R5)

Reference support added in [*std::optional<T&>*(P2988R5)](https://wg21.link/P2988R5) examples:

* local [./optional_ref.cpp](./optional_ref.cpp) or [optional_ref.cpp@Compiler Explorer](https://godbolt.org/z/MxjdvTTov)

### Local Build and Run

```shell
# build
$ cmake --workflow --preset gcc-14

# run optional_ref.cpp
$ .build/gcc-14/examples/RelWithDebInfo/optional_ref
```
