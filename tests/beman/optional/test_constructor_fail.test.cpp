// tests/beman/optional/test_constructor_fail.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/optional/optional.hpp>
#include <string>
#include <utility>

beman::optional::optional<std::string>&  makeRefToOptional();
beman::optional::optional<std::string>&& makeRefRefToOptional();

const beman::optional::optional<std::string>&  makeCRefToOptional();
const beman::optional::optional<std::string>&& makeCRefRefToOptional();

const beman::optional::optional<std::string&>&  makeCRefToOptionalRef();
const beman::optional::optional<std::string&>&& makeCRefRefToOptionalRef();

const beman::optional::optional<std::reference_wrapper<std::string>>&  makeCRefToOptionalRefWrap();
const beman::optional::optional<std::reference_wrapper<std::string>>&& makeCRefRefToOptionalRefWrap();

void test_constructors() {
    beman::optional::optional<std::string>        os;
    beman::optional::optional<std::string&>       or1 = os;
    beman::optional::optional<const std::string&> or2 = os;
    beman::optional::optional<std::string>        cos;
    beman::optional::optional<const std::string&> or3 = cos;

    beman::optional::optional<std::string&>       or1a{os};
    beman::optional::optional<const std::string&> or2a{os};
    beman::optional::optional<const std::string&> or3a{cos};

    // But disable:
    beman::optional::optional<const char*>        os2;
    beman::optional::optional<const std::string&> or4 = os;
}

void test_constructors_dangling() {
    beman::optional::optional<std::string&> os1 = beman::optional::optional<std::string&>();
    beman::optional::optional<std::string&> os2 = beman::optional::optional<std::reference_wrapper<std::string>>();

    /*
 1.     std::optional<T> & -> std::optional<T&> / std::optional<T const&> // OK
 2.     std::optional<T> const& -> std::optional<T const&> / std::optional<T const&> // OK
 3.     std::optional<T>&& -> std::optional<T&> / std::optional<T const&> // ILL-Formed
 4.     std::optional<T> const&& -> std::optional<T const&> // ILL-Formed
 5.     std::optional<T&> cv & -> std::optional<T&> / std::optional<T const&> // OK
 6.     std::optional<T&> cv && -> std::optional<T&> / std::optional<T const&> // OK
 7.     std::optional<std::reference_wrapper<T>> cv & -> std::optional<T&> / std::optional<T const&> // OK
 8.     std::optional<std::reference_wrapper<T>> cv && -> std::optional<T&> / std::optional<T const&> // OK
    */
    auto&& t1 = makeRefToOptional(); // std::optional<T> &
    static_assert(std::is_same_v<decltype(t1), beman::optional::optional<std::string>&>);
    beman::optional::optional<std::string&>       o01 = t1;
    beman::optional::optional<const std::string&> o02 = t1;

    auto&& t2 = makeCRefToOptional(); // std::optional<T> const&
    static_assert(std::is_same_v<decltype(t2), const beman::optional::optional<std::string>&>);
    // Ill-formed - Diagnosed
    beman::optional::optional<std::string&>       o03 = t2; // optional is deep const
    beman::optional::optional<const std::string&> o04 = t2;

    auto&& t3 = makeRefRefToOptional(); // std::optional<T> &&
    static_assert(std::is_same_v<decltype(t3), beman::optional::optional<std::string>&&>);
    // Ill-formed - Diagnosed
    beman::optional::optional<std::string&> o05 = std::move(t3);
    // Ill-formed - Diagnosed
    beman::optional::optional<const std::string&> o06 = std::move(t3);

    auto&& t4 = makeCRefRefToOptional(); // std::optional<T> const&&
    static_assert(std::is_same_v<decltype(t4), const beman::optional::optional<std::string>&&>);
    // Ill-formed - Diagnosed
    beman::optional::optional<std::string&> o07 = std::move(t4);
    // Ill-formed - Diagnosed
    beman::optional::optional<const std::string&> o07a = std::move(t4);

    auto&& t5 = makeCRefToOptionalRef(); // std::optional<T&> const &
    static_assert(std::is_same_v<decltype(t5), const beman::optional::optional<std::string&>&>);
    beman::optional::optional<std::string&>       o08 = t5;
    beman::optional::optional<const std::string&> o09 = t5;

    auto&& t6 = makeCRefRefToOptionalRef(); // std::optional<T&> const &&
    static_assert(std::is_same_v<decltype(t6), const beman::optional::optional<std::string&>&&>);
    beman::optional::optional<std::string&>       o10 = std::move(t6);
    beman::optional::optional<const std::string&> o11 = std::move(t6);

    auto&& t7 = makeCRefToOptionalRefWrap(); // std::optional<std::reference_wrapper<T>> cv &
    static_assert(std::is_same_v<decltype(t7), const beman::optional::optional<std::reference_wrapper<std::string>>&>);
    beman::optional::optional<std::string&>       o12 = t7;
    beman::optional::optional<const std::string&> o13 = t7;

    auto&& t8 = makeCRefRefToOptionalRefWrap(); // std::optional<std::reference_wrapper<T>> cv &&
    static_assert(
        std::is_same_v<decltype(t8), const beman::optional::optional<std::reference_wrapper<std::string>>&&>);
    beman::optional::optional<std::string&>       o14 = t8;
    beman::optional::optional<const std::string&> o15 = t8;

    std::ignore = o01;
    std::ignore = o02;
    std::ignore = o03;
    std::ignore = o04;
    std::ignore = o05;
    std::ignore = o06;
    std::ignore = o07;
    std::ignore = o07a;
    std::ignore = o08;
    std::ignore = o09;
    std::ignore = o10;
    std::ignore = o11;
    std::ignore = o12;
    std::ignore = o13;
    std::ignore = o14;
    std::ignore = o15;
}
