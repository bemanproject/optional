// examples/optional_ref.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/optional/optional.hpp>

#include <string>

struct Cat {
    int catalog_index{0};
};

namespace std17 {

// Prior to C++26, the code would look like this.
// Using raw pointers to represent optional references.
// Note: Using smart pointers would also be a choice, but it involves ownership semantics.

Cat* find_cat(std::string) { return nullptr; }

Cat* do_it(Cat& cat) { return &cat; }

Cat* api() {
    Cat* cat = find_cat("Fido");
    if (cat != nullptr) {
        return do_it(*cat);
    }
    return nullptr;
}

} // namespace std17

namespace std26 {
// After C++26 with P2988R5, the code would look like this.
// Using directly optional to represent optional references.

beman::optional::optional<Cat&> find_cat(std::string) { return {}; }

beman::optional::optional<Cat&> do_it(Cat& cat) { return {cat}; }

beman::optional::optional<Cat&> api() {
    beman::optional::optional<Cat&> cat = find_cat("Fido");
    return cat.and_then(do_it);
}

} // namespace std26

int example1() {
    // Example from P2988R5: optional reference.
    [[maybe_unused]] Cat*                            old_cat = std17::api();
    [[maybe_unused]] beman::optional::optional<Cat&> new_cat = std26::api();

    return 0;
}

struct derived;
extern derived d;
struct base {
    virtual ~base() = default;
    operator derived&() { return d; }
};

struct derived : base {};

derived d;

int example2() {
    base                                b;
    derived&                            dref(b); // ok
    beman::optional::optional<derived&> dopt(b);
    return 0;
}

int main() {
    example1();
    example2();
}
// # build example:
// $ cmake --workflow --preset gcc-14
//
// # run example:
// $ .build/gcc-14/examples/RelWithDebInfo/optional_ref
