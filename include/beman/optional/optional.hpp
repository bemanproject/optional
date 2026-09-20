// include/beman/optional/optional.hpp                                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_OPTIONAL_OPTIONAL_HPP
#define BEMAN_OPTIONAL_OPTIONAL_HPP

#include <version>
#include <compare>
#include <concepts>
#if defined(__cpp_lib_format_ranges)
#include <format>
#endif
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>

#include <beman/optional/detail/iterator.hpp>

namespace beman::optional {

namespace detail {
template <typename T, typename U>
concept optional_eq_rel = requires(const T& t, const U& u) {
    { t == u } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept optional_ne_rel = requires(const T& t, const U& u) {
    { t != u } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept optional_lt_rel = requires(const T& t, const U& u) {
    { t < u } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept optional_gt_rel = requires(const T& t, const U& u) {
    { t > u } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept optional_le_rel = requires(const T& t, const U& u) {
    { t <= u } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept optional_ge_rel = requires(const T& t, const U& u) {
    { t >= u } -> std::convertible_to<bool>;
};

struct from_function_t {
    explicit from_function_t() = default;
};

inline constexpr from_function_t from_function{};
} // namespace detail

/**
 *
 * @brief Tag type for in-place construction
 */
struct in_place_t {
    explicit in_place_t() = default;
};

/**
 * @brief Tag for in-place construction
 */
inline constexpr in_place_t in_place{};

} // namespace beman::optional

namespace beman::optional {
template <class T>
class optional; // partially freestanding
} // namespace beman::optional

// Since P3168R2: Give std::optional Range Support.
template <class T>
inline constexpr bool std::ranges::enable_view<beman::optional::optional<T>> = true;

// Iterators for optional<T&> have lifetimes that are not tied to the optional.
template <class T>
inline constexpr bool std::ranges::enable_borrowed_range<beman::optional::optional<T&>> = true;

// Since P3168R2: Give std::optional Range Support.
#if defined(__cpp_lib_format_ranges)
template <class T>
inline constexpr auto std::format_kind<beman::optional::optional<T>> = range_format::disabled;
#endif

namespace beman::optional {
namespace detail {
template <typename T>
inline constexpr bool is_optional = false;
template <typename T>
inline constexpr bool is_optional<optional<T>> = true;
} // namespace detail
/**
 * @brief Concept for checking if derived from optional.
 */
template <class T>
concept is_derived_from_optional = requires(const T& t) { // exposition only
    []<class U>(const optional<U>&) {}(t);
};

// \ref{optional.nullopt}, no-value state indicator
/**
 * @brief Tag type for empty optional construction
 */
struct nullopt_t {
    /** @enum Tag
     * @brief Tag enum for nullopt_t construction
     */
    enum class Tag { tag /**< the tag value for nullopt_t::Tag */ };

    /**
     * @brief Construct a new nullopt_t object
     * @arg Tag
     * @details constexpr for nullopt_t to be literal.
     */
    explicit constexpr nullopt_t(Tag) noexcept {}

  private:
    friend constexpr bool operator==(nullopt_t, nullopt_t) noexcept { return true; }

    friend constexpr std::strong_ordering operator<=>(nullopt_t, nullopt_t) noexcept {
        return std::strong_ordering::equivalent;
    }
};

/// Tag to disengage optional objects.
inline constexpr nullopt_t nullopt{nullopt_t::Tag::tag};

// \ref{optional.bad.access}, class bad_optional_access
class bad_optional_access;

// \ref{optional.relops}, relational operators
template <typename T, typename U>
constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_eq_rel<T, U>;
template <typename T, typename U>
constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_ne_rel<T, U>;
template <typename T, typename U>
constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_lt_rel<T, U>;
template <typename T, typename U>
constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_gt_rel<T, U>;
template <typename T, typename U>
constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_le_rel<T, U>;
template <typename T, typename U>
constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_ge_rel<T, U>;
template <class T, std::three_way_comparable_with<T> U>
constexpr std::compare_three_way_result_t<T, U> operator<=>(const optional<T>&, const optional<U>&);

// \ref{optional.nullops}, comparison with \tcode{nullopt}
template <class T>
constexpr bool operator==(const optional<T>&, nullopt_t) noexcept;
template <class T>
constexpr std::strong_ordering operator<=>(const optional<T>&, nullopt_t) noexcept;

// \ref{optional.comp.with.t}, comparison with \tcode{T}
template <typename T, typename U>
constexpr bool operator==(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_eq_rel<T, U>;

template <typename T, typename U>
constexpr bool operator==(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_eq_rel<T, U>;

template <typename T, typename U>
constexpr bool operator!=(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_ne_rel<T, U>;

template <typename T, typename U>
constexpr bool operator!=(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_ne_rel<T, U>;

template <typename T, typename U>
constexpr bool operator<(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_lt_rel<T, U>;

template <typename T, typename U>
constexpr bool operator<(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_lt_rel<T, U>;

template <typename T, typename U>
constexpr bool operator>(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_gt_rel<T, U>;

template <typename T, typename U>
constexpr bool operator>(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_gt_rel<T, U>;

template <typename T, typename U>
constexpr bool operator<=(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_le_rel<T, U>;

template <typename T, typename U>
constexpr bool operator<=(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_le_rel<T, U>;

template <typename T, typename U>
constexpr bool operator>=(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_ge_rel<T, U>;

template <typename T, typename U>
constexpr bool operator>=(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_ge_rel<T, U>;

template <typename T, typename U>
    requires (!is_derived_from_optional<U>) && std::three_way_comparable_with<T, U>
constexpr std::compare_three_way_result_t<T, U> operator<=>(const optional<T>& x, const U& v);

// \ref{optional.specalg}, specialized algorithms
/**
 * @brief Swap
 */
template <class T>
constexpr void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
    requires std::is_move_constructible_v<T> && std::is_swappable_v<T>;

/**
 * @brief Make an optional
 */
template <int = 0, class T>
constexpr optional<std::decay_t<T>>
make_optional(T&&) noexcept(std::is_nothrow_constructible_v<optional<std::decay_t<T>>, T>)
    requires std::is_constructible_v<std::decay_t<T>, T>;

template <class T, class... Args>
constexpr optional<T> make_optional(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    requires std::is_constructible_v<T, Args...>;

template <class T, class U, class... Args>
constexpr optional<T>
make_optional(std::initializer_list<U> il,
              Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>;

// \ref{optional.hash}, hash support
/* We are not actually in ::std, don't declare beman::optional::hash.
template <class T>
struct hash;
template <class T>
struct hash<optional<T>>;
*/
/// END [optional.syn]

namespace detail {
template <class T, class U>
concept enable_forward_value = !std::is_same_v<std::decay_t<U>, optional<T>> &&
                               !std::is_same_v<std::decay_t<U>, in_place_t> && std::is_constructible_v<T, U&&>;

template <class T, class U, class Other>
concept enable_from_other = !std::is_same_v<T, U> &&                            //
                            std::is_constructible_v<T, Other> &&                //
                            !std::is_constructible_v<T, optional<U>&> &&        //
                            !std::is_constructible_v<T, optional<U>&&> &&       //
                            !std::is_constructible_v<T, const optional<U>&> &&  //
                            !std::is_constructible_v<T, const optional<U>&&> && //
                            !std::is_convertible_v<optional<U>&, T> &&          //
                            !std::is_convertible_v<optional<U>&&, T> &&         //
                            !std::is_convertible_v<const optional<U>&, T> &&    //
                            !std::is_convertible_v<const optional<U>&&, T>;

template <class T, class U>
concept enable_assign_forward = !std::is_same_v<optional<T>, std::decay_t<U>> &&
                                !std::conjunction_v<std::is_scalar<T>, std::is_same<T, std::decay_t<U>>> &&
                                std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>;

template <class T, class U, class Other>
concept enable_assign_from_other =
    std::is_constructible_v<T, Other> && std::is_assignable_v<T&, Other> &&
    !std::is_constructible_v<T, optional<U>&> && !std::is_constructible_v<T, optional<U>&&> &&
    !std::is_constructible_v<T, const optional<U>&> && !std::is_constructible_v<T, const optional<U>&&> &&
    !std::is_convertible_v<optional<U>&, T> && !std::is_convertible_v<optional<U>&&, T> &&
    !std::is_convertible_v<const optional<U>&, T> && !std::is_convertible_v<const optional<U>&&, T> &&
    !std::is_assignable_v<T&, optional<U>&> && !std::is_assignable_v<T&, optional<U>&&> &&
    !std::is_assignable_v<T&, const optional<U>&> && !std::is_assignable_v<T&, const optional<U>&&>;
} // namespace detail

// 22.5.3.1 General[optional.optional.general]

/**
 *
 * @brief Optional Objects
 */
template <class T>
class optional {
    static_assert((!std::is_same_v<T, std::remove_cv_t<in_place_t>>) &&
                  (!std::is_same_v<std::remove_cv_t<T>, nullopt_t>));
    static_assert(std::is_object_v<T> && !std::is_array_v<T>);

  public:
    /**
     * @brief Type alias for the value type contained in the optional.
     *
     */
    using value_type = T;

    /**
     * @brief Type alias for the iterator type of the optional.
     *
     * @details Since P3168R2: Give std::optional Range Support.
     */
    using iterator = detail::contiguous_iterator<T,
                                                 optional>; // see~\ref{optional.iterators}
    /**
     * @brief Type alias for the const iterator type of the optional.
     *
     * @details Since P3168R2: Give std::optional Range Support.
     */
    using const_iterator = detail::contiguous_iterator<const T,
                                                       optional>; // see~\ref{optional.iterators}

    // \ref{optional.ctor}, constructors
    /**
     * @brief Default constructs an empty optional.
     *
     */
    constexpr optional() noexcept;

    /**
     * @brief Constructs an empty optional.
     *
     */
    constexpr optional(nullopt_t) noexcept;

    /**
     * @brief   Copy constructs the value from \p rhs if it has one.
     *
     */
    constexpr optional(const optional& rhs)
        requires std::is_copy_constructible_v<T> && (!std::is_trivially_copy_constructible_v<T>);
    /**
     * @brief Copy constructs the value from \p rhs if it has one.
     * @details Defaulted if T is trivially copy constructible.
     */
    constexpr optional(const optional&)
        requires std::is_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<T>
    = default;
    /**
     *  @brief Move constructs the value from \p rhs if it has one.
     */
    constexpr optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::is_move_constructible_v<T> && (!std::is_trivially_move_constructible_v<T>);
    /**
     * @brief Move constructs the value from \p rhs if it has one.
     * @details Defaulted if T is trivially move constructible.
     */
    constexpr optional(optional&&)
        requires std::is_move_constructible_v<T> && std::is_trivially_move_constructible_v<T>
    = default;

    /**
     *  @brief   Constructs the value in-place using the given arguments.
     *
     * @param args The arguments to use for in-place construction.
     */
    template <class... Args>
    constexpr explicit optional(in_place_t, Args&&... args)
        requires std::is_constructible_v<T, Args...>;

    /**
     *  @brief   Constructs the value in-place using the given arguments.
     *
     * @param il The initializer list to use for in-place construction.
     * @param args The arguments to use for in-place construction.
     */
    template <class U, class... Args>
    constexpr explicit optional(in_place_t, std::initializer_list<U> il, Args&&... args)
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>;

    /**
     * @brief   Constructs the value from \p u, forwarding it if necessary.
     *
     * If \p u is convertible to \p T, this is an explicit constructor.
     */
    template <class U = T>
    constexpr explicit(!std::is_convertible_v<U, T>) optional(U&& u)
        requires detail::enable_forward_value<T, U>;

    /**
     * @brief   Constructs the value from \p rhs if it has one.
     *
     * @tparam U
     */
    template <class U>
    constexpr explicit(!std::is_convertible_v<U, T>) optional(const optional<U>& rhs)
        requires (detail::enable_from_other<T, U, const U&>);

    /**
     * @brief   Constructs the value from \p rhs if it has one.
     */
    template <class U>
    constexpr explicit(!std::is_convertible_v<U, T>) optional(optional<U>&& rhs)
        requires (detail::enable_from_other<T, U, U&&>);

    // \ref{optional.dtor}, destructor
    /**
     * @brief   Destructs the optional.
     *
     */
    constexpr ~optional()
        requires std::is_trivially_destructible_v<T>
    = default;

    /**
     *  @brief   Destroys the optional and its value if it has one.
     */
    constexpr ~optional()
        requires (!std::is_trivially_destructible_v<T>);

    // \ref{optional.assign}, assignment
    /**
     * @brief   Resets the optional to an empty state.
     */
    constexpr optional& operator=(nullopt_t) noexcept;

    /**
     * @brief   Copy assigns the value from \p rhs if it has one.
     */
    constexpr optional& operator=(const optional& rhs)
        requires std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> &&
                 (!std::is_trivially_copy_assignable_v<T>);

    /**
     * @brief Copy assigns the value from \p rhs if it has one.
     *
     * @return optional&
     */
    constexpr optional& operator=(const optional&)
        requires std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> &&
                     std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_assignable_v<T>
    = default;

    /**
     *  @brief   Move assigns the value from \p rhs if it has one.
     */
    constexpr optional& operator=(optional&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::is_move_constructible_v<T> && std::is_move_assignable_v<T> &&
                 (!std::is_trivially_move_assignable_v<T>);

    /**
     * @brief   Move assigns the value from \p rhs if it has one.
     */
    constexpr optional& operator=(optional&&)
        requires std::is_move_constructible_v<T> && std::is_move_assignable_v<T> &&
                     std::is_trivially_move_constructible_v<T> && std::is_trivially_move_assignable_v<T>
    = default;

    /**
     * @brief  Assigns the contained value from \p u, destroying the old value if there
     *
     * @tparam U The type of the value to assign.
     * @param u The value to assign.
     * @return optional&
     */
    template <class U = T>
    constexpr optional& operator=(U&& u)
        requires detail::enable_assign_forward<T, U>;

    /**
     * @brief   Assigns the contained value from \p rhs if it has one, destroying the old value if there
     *
     */
    template <class U>
    constexpr optional& operator=(const optional<U>& rhs)
        requires (detail::enable_assign_from_other<T, U, const U&>);

    /**
     * @brief   Assigns the contained value from \p rhs if it has one, destroying the old value if there
     */
    template <class U>
    constexpr optional& operator=(optional<U>&& rhs)
        requires (detail::enable_assign_from_other<T, U, U>);
    /**
     * @brief Constructs the value in-place, destroying the current one if there
     *
     * @tparam Args
     * @param args The argument list to use in emplacement construction.
     * @return T&
     */
    template <class... Args>
    constexpr T& emplace(Args&&... args);
    /**
     * @brief Constructs the value in-place using the given arguments, destroying the current one if there
     *
     * @tparam U
     * @tparam Args
     * @param il The initializer list to use in emplacement construction.
     * @param args The argument list to use in emplacement construction.
     * @return T&
     */
    template <class U, class... Args>
    constexpr T& emplace(std::initializer_list<U> il, Args&&... args);

    // \ref{optional.swap}, swap
    /**
     * @brief Swaps this optional with the other.
     *
     * @param rhs The optional to swap with.
     */
    constexpr void swap(optional& rhs) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                                std::is_nothrow_swappable<T>::value);

    // \ref{optional.iterators}, iterator support
    /**
     * @brief   Returns an iterator to the beginning of the optional.
     * @return iterator
     */
    constexpr iterator begin() noexcept;

    /**
     * @brief   Returns a const iterator to the beginning of the optional.
     * @return const_iterator
     */
    constexpr const_iterator begin() const noexcept;

    /**
     * @brief   Returns an iterator to the end of the optional.
     * @return iterator
     */
    constexpr iterator end() noexcept;

    /**
     * @brief   Returns a const iterator to the end of the optional.
     * @return const_iterator
     */
    constexpr const_iterator end() const noexcept;

    // \ref{optional.observe}, observers
    /**
     * @brief   Returns a pointer to the contained value.
     */
    constexpr const T* operator->() const;

    /**
     * @brief   Returns a pointer to the contained value.
     *
     * @return T*
     */
    constexpr T* operator->();
    /**
     * @brief Returns a reference to the contained value.
     *
     * @return T&
     */
    constexpr T& operator*() &;

    /**
     * @brief  Returns a reference to the contained value.
     *
     * @return const T&
     */
    constexpr const T& operator*() const&;

    /**
     * @brief   Returns a reference to the contained value.
     *
     * @return T&&
     */
    constexpr T&& operator*() &&;

    /**
     * @brief Converts the optional to a boolean indicating whether it has a value.
     *
     * @return bool
     */
    constexpr explicit operator bool() const noexcept;

    /**
     * @brief   Returns whether or not the optional has a value.
     *
     * @return bool
     */
    constexpr bool has_value() const noexcept;

    /**
     * @brief Returns a reference to the contained value.
     *
     * @return T&
     */
    constexpr T& value() &;

    /**
     * @brief Returns a reference to the contained value.
     *
     * @return const T&
     */
    constexpr const T& value() const&;

    /**
     * @brief Returns a reference to the contained value.
     *
     * @return T&&
     */
    constexpr T&& value() &&;

    /**
     * @brief Returns the contained value if there is one, otherwise returns `u`.
     *
     * @tparam U The type of the alternate value
     * @param u The value to return in the empty case
     * @return T
     */
    template <class U = std::remove_cv_t<T>>
    constexpr std::remove_cv_t<T> value_or(U&& u) const&;
    /**
     * @brief Returns the contained value if there is one, otherwise returns `u`.
     *
     * @tparam U The type of the alternate value
     * @param u The value to return in the empty case
     * @return T
     */
    template <class U = std::remove_cv_t<T>>
    constexpr std::remove_cv_t<T> value_or(U&& u) &&;

    // \ref{optional.monadic}, monadic operations

    /**
     * @brief Applies a function to the contained value if there is one.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return auto
     *
     * @details
     * The return type is the same as `std::invoke_result_t<F, T&>`,
     * but wrapped in an optional. The function \p f must return an optional type.
     */
    template <class F>
    constexpr auto and_then(F&& f) &;

    /**
     * @brief Applies a function to the contained value if there is one.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return auto
     *
     * @details
     * The return type is the same as `std::invoke_result_t<F, T&>`,
     * but wrapped in an optional. The function \p f must return an optional type.
     */
    template <class F>
    constexpr auto and_then(F&& f) &&;

    /**
     * @brief Applies a function to the contained value if there is one.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return auto
     *
     * @details
     * The return type is the same as `std::invoke_result_t<F, T&>`,
     * but wrapped in an optional. The function \p f must return an optional type.
     */
    template <class F>
    constexpr auto and_then(F&& f) const&;

    /**
     * @brief Applies a function to the contained value if there is one.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return auto
     *
     * @details
     * The return type is the same as `std::invoke_result_t<F, T&>`,
     * but wrapped in an optional. The function \p f must return an optional type.
     */
    template <class F>
    constexpr auto and_then(F&& f) const&&;

    /**
     * @brief Returns an optional containing the result of applying \p f to the
     * contained value, or an empty optional if there is no contained value.
     *
     * @tparam F
     * @param f An invocable to apply to the contained value.
     * @return optional
     */
    template <class F>
    constexpr auto transform(F&& f) &;

    /**
     * @brief Returns an optional containing the result of applying \p f to the
     * contained value, or an empty optional if there is no contained value.
     *
     * @tparam F
     * @param f An invocable to apply to the contained value.
     * @return auto
     */
    template <class F>
    constexpr auto transform(F&& f) &&;

    /**
     * @brief Returns an optional containing the result of applying \p f to the
     * contained value, or an empty optional if there is no contained value.
     *
     * @tparam F
     * @param f An invocable to apply to the contained value.
     * @return auto
     */
    template <class F>
    constexpr auto transform(F&& f) const&;

    /**
     * @brief Returns an optional containing the result of applying \p f to the
     * contained value, or an empty optional if there is no contained value.
     *
     * @tparam F
     * @param f An invocable to apply to the contained value.
     * @return auto
     */
    template <class F>
    constexpr auto transform(F&& f) const&&;

    /**
     * @brief Returns an optional containing the contained value if it has one, or the result of applying \p f to the
     * optional if it does not.
     *
     * @tparam F An invocable type returning an optional.
     * @param f The invocable to call if the optional is disengaged.
     * @return optional
     */
    template <class F>
    constexpr optional or_else(F&& f) const&
        requires (std::invocable<F> && std::copy_constructible<T>);

    /**
     * @brief Returns an optional containing the contained value if it has one, or
     * the result of calling \p f if it does not.
     *
     * @tparam F An invocable type returning an optional.
     * @param f The invocable to call if the optional is disengaged.
     * @return auto
     */
    template <class F>
    constexpr optional or_else(F&& f) &&
        requires (std::invocable<F> && std::move_constructible<T>);

    // \ref{optional.mod}, modifiers
    /**
     * @brief Resets the optional to an empty state, destroying the contained value if there is one.
     *
     */
    constexpr void reset() noexcept;

  private:
    struct empty {};
    union {
        /**
         * @brief The empty state of the optional.
         *
         */
        empty _{};

        /**
         * @brief The contained value of the optional.
         *
         */
        T value_;
    };
    bool engaged_ = false;

    template <class... Args>
    constexpr void construct(Args&&... args) {
        std::construct_at(std::addressof(value_), std::forward<Args>(args)...);
        engaged_ = true;
    }

    constexpr void hard_reset() noexcept {
        std::destroy_at(std::addressof(value_));
        engaged_ = false;
    }

    template <class U>
    friend class optional;

    template <class F, class Arg>
    constexpr optional(detail::from_function_t, F&& f, Arg&& arg)
        : value_(std::invoke(std::forward<F>(f), std::forward<Arg>(arg))), engaged_(true) {}
};

/**
 * @brief Exception thrown when trying to access the value of an empty optional
 */
class bad_optional_access : public std::exception {
  public:
    /**
     * @brief Construct a new bad optional access object
     *
     */
    bad_optional_access() = default;

    /**
     * @brief Get the error message for bad optional access
     *
     * @return const char*
     */
    const char* what() const noexcept { return "Optional has no value"; }
};

// \rSec3[optional.ctor]{Constructors}
template <class T>
inline constexpr optional<T>::optional() noexcept : _(), engaged_(false) {}

template <class T>
inline constexpr optional<T>::optional(nullopt_t) noexcept {}

template <class T>
inline constexpr optional<T>::optional(const optional& rhs)
    requires std::is_copy_constructible_v<T> && (!std::is_trivially_copy_constructible_v<T>)
{
    if (rhs.has_value()) {
        construct(rhs.value_);
    }
}

template <class T>
inline constexpr optional<T>::optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires std::is_move_constructible_v<T> && (!std::is_trivially_move_constructible_v<T>)
{
    if (rhs.has_value()) {
        construct(std::move(rhs.value_));
    }
}

/// Constructs the contained value in-place using the given arguments.
template <class T>
template <class... Args>
inline constexpr optional<T>::optional(in_place_t, Args&&... args)
    requires std::is_constructible_v<T, Args...>
    : value_(std::forward<Args>(args)...), engaged_(true) {}

template <class T>
template <class U, class... Args>
inline constexpr optional<T>::optional(in_place_t, std::initializer_list<U> il, Args&&... args)
    requires std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>
    : value_(il, std::forward<Args>(args)...), engaged_(true) {}

/// Constructs the contained value with `u`.
template <class T>
template <class U>
inline constexpr optional<T>::optional(U&& u)
    requires detail::enable_forward_value<T, U>
    : optional(in_place, std::forward<U>(u)) {}

/// Converting copy constructor.
template <class T>
template <class U>
inline constexpr optional<T>::optional(const optional<U>& rhs)
    requires (detail::enable_from_other<T, U, const U&>)
{
    if (rhs.has_value()) {
        construct(*rhs);
    }
}

/// Converting move constructor.
template <class T>
template <class U>
inline constexpr optional<T>::optional(optional<U>&& rhs)
    requires (detail::enable_from_other<T, U, U&&>)
{
    if (rhs.has_value()) {
        construct(*std::move(rhs));
    }
}

// 22.5.3.3 Destructor[optional.dtor]

template <class T>
inline constexpr optional<T>::~optional()
    requires (!std::is_trivially_destructible_v<T>)
{
    if (has_value())
        std::destroy_at(std::addressof(value_));
}

// 22.5.3.4 Assignment[optional.assign]

template <class T>
inline constexpr optional<T>& optional<T>::operator=(nullopt_t) noexcept {
    reset();
    return *this;
}

template <class T>
inline constexpr optional<T>& optional<T>::operator=(const optional<T>& rhs)
    requires std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> &&
             (!std::is_trivially_copy_assignable_v<T>)
{
    if (!rhs.has_value())
        reset();
    else if (has_value())
        value_ = rhs.value_;
    else
        construct(rhs.value_);
    return *this;
}

template <class T>
inline constexpr optional<T>&
optional<T>::operator=(optional<T>&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires std::is_move_constructible_v<T> && std::is_move_assignable_v<T> &&
             (!std::is_trivially_move_assignable_v<T>)
{
    if (!rhs.has_value())
        reset();
    else if (has_value())
        value_ = std::move(rhs.value_);
    else
        construct(std::move(rhs.value_));
    return *this;
}

/// Assigns the contained value from `u`, destroying the old value if there
/// was one.
template <class T>
template <class U>
inline constexpr optional<T>& optional<T>::operator=(U&& u)
    requires detail::enable_assign_forward<T, U>
{
    if (has_value()) {
        value_ = std::forward<U>(u);
    } else {
        construct(std::forward<U>(u));
    }

    return *this;
}

/// Converting copy assignment operator.
///
/// Copies the value from `rhs` if there is one. Otherwise resets the
/// contained value in `*this`.
template <class T>
template <class U>
inline constexpr optional<T>& optional<T>::operator=(const optional<U>& rhs)
    requires (detail::enable_assign_from_other<T, U, const U&>)
{
    if (has_value()) {
        if (rhs.has_value()) {
            value_ = *rhs;
        } else {
            hard_reset();
        }
    }

    else if (rhs.has_value()) {
        construct(*rhs);
    }

    return *this;
}

/// Converting move assignment operator.
///
/// Moves the value from `rhs` if there is one. Otherwise resets the contained
/// value in `*this`.
template <class T>
template <class U>
inline constexpr optional<T>& optional<T>::operator=(optional<U>&& rhs)
    requires (detail::enable_assign_from_other<T, U, U>)
{
    if (has_value()) {
        if (rhs.has_value()) {
            value_ = *std::move(rhs);
        } else {
            hard_reset();
        }
    }

    else if (rhs.has_value()) {
        construct(*std::move(rhs));
    }

    return *this;
}

/// Constructs the value in-place, destroying the current one if there is
/// one.
template <class T>
template <class... Args>
constexpr T& optional<T>::emplace(Args&&... args) {
    static_assert(std::is_constructible_v<T, Args&&...>);
    *this = nullopt;
    construct(std::forward<Args>(args)...);
    return value();
}

template <class T>
template <class U, class... Args>
constexpr T& optional<T>::emplace(std::initializer_list<U> il, Args&&... args) {
    static_assert(std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>);
    *this = nullopt;
    construct(il, std::forward<Args>(args)...);
    return value();
}

// 22.5.3.5 Swap[optional.swap]
/// Swaps this optional with the other.
///
/// If neither optionals have a value, nothing happens.
/// If both have a value, the values are swapped.
/// If one has a value, it is moved to the other and the movee is left
/// valueless.
template <class T>
inline constexpr void optional<T>::swap(optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                                                   std::is_nothrow_swappable<T>::value) {
    static_assert(std::is_move_constructible_v<T>);
    using std::swap;
    if (has_value()) {
        if (rhs.has_value()) {
            swap(value(), *rhs);
        } else {
            std::construct_at(std::addressof(rhs.value_), std::move(value_));
            value_.T::~T();
        }
    } else if (rhs.has_value()) {
        std::construct_at(std::addressof(value_), std::move(rhs.value_));
        rhs.value_.T::~T();
    }
    swap(engaged_, rhs.engaged_);
}

// 22.5.3.6 Iterator support[optional.iterators]
// Since P3168R2: Give std::optional Range Support.
template <class T>
inline constexpr optional<T>::iterator optional<T>::begin() noexcept {
    return iterator(has_value() ? std::addressof(value_) : nullptr);
}

template <class T>
inline constexpr optional<T>::const_iterator optional<T>::begin() const noexcept {
    return const_iterator(has_value() ? std::addressof(value_) : nullptr);
}
template <class T>
inline constexpr optional<T>::iterator optional<T>::end() noexcept {
    return begin() + has_value();
}

template <class T>
inline constexpr optional<T>::const_iterator optional<T>::end() const noexcept {
    return begin() + has_value();
}

// 22.5.3.7 Observers[optional.observe]

/// Returns a pointer to the contained value
template <class T>
inline constexpr const T* optional<T>::operator->() const {
    return std::addressof(value_);
}

template <class T>
inline constexpr T* optional<T>::operator->() {
    return std::addressof(value_);
}

/// Returns the contained value
template <class T>
inline constexpr T& optional<T>::operator*() & {
    return value_;
}

template <class T>
inline constexpr const T& optional<T>::operator*() const& {
    return value_;
}

template <class T>
inline constexpr T&& optional<T>::operator*() && {
    return std::move(value_);
}

template <class T>
inline constexpr optional<T>::operator bool() const noexcept {
    return engaged_;
}

/// Returns whether or not the optional has a value
template <class T>
inline constexpr bool optional<T>::has_value() const noexcept {
    return engaged_;
}

/// Returns the contained value if there is one, otherwise throws
/// bad_optional_access
template <class T>
inline constexpr T& optional<T>::value() & {
    return has_value() ? value_ : throw bad_optional_access();
}
template <class T>
inline constexpr const T& optional<T>::value() const& {
    return has_value() ? value_ : throw bad_optional_access();
}
template <class T>
inline constexpr T&& optional<T>::value() && {
#if defined(_MSC_VER) // MSVC BUG -- ternary creates temporary
    if (has_value()) {
        return std::move(value_);
    }
    throw bad_optional_access();
#else
    return has_value() ? std::move(value_) : throw bad_optional_access();
#endif
}

/// Returns the contained value if there is one, otherwise returns `u`
template <class T>
template <class U>
inline constexpr std::remove_cv_t<T> optional<T>::value_or(U&& u) const& {
    using X = std::remove_cv_t<T>;
    static_assert(std::is_convertible_v<const T&, X>, "Must be able to convert const T& to remove_cv_t<T>");
    static_assert(std::is_convertible_v<U, X>, "Must be able to convert u to remove_cv_t<T>");
    if (has_value())
        return value_;
    return std::forward<U>(u);
}

template <class T>
template <class U>
inline constexpr std::remove_cv_t<T> optional<T>::value_or(U&& u) && {
    using X = std::remove_cv_t<T>;
    static_assert(std::is_convertible_v<T, X>, "Must be able to convert T to remove_cv_t<T>");
    static_assert(std::is_convertible_v<U, X>, "Must be able to convert u to remove_cv_t<T>");
    if (has_value()) {
        return std::move(value_);
    }
    return std::forward<U>(u);
}

// 22.5.3.8 Monadic operations[optional.monadic]
template <class T>
template <class F>
constexpr auto optional<T>::and_then(F&& f) & {
    using U = std::invoke_result_t<F, T&>;
    static_assert(detail::is_optional<std::remove_cvref_t<U>>);
    if (has_value()) {
        return std::invoke(std::forward<F>(f), value_);
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <class T>
template <class F>
constexpr auto optional<T>::and_then(F&& f) && {
    using U = std::invoke_result_t<F, T&&>;
    static_assert(detail::is_optional<std::remove_cvref_t<U>>);
    if (has_value()) {
        return std::invoke(std::forward<F>(f), std::move(value_));
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <class T>
template <class F>
constexpr auto optional<T>::and_then(F&& f) const& {
    using U = std::invoke_result_t<F, const T&>;
    static_assert(detail::is_optional<std::remove_cvref_t<U>>);
    if (has_value()) {
        return std::invoke(std::forward<F>(f), value_);
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <class T>
template <class F>
constexpr auto optional<T>::and_then(F&& f) const&& {
    using U = std::invoke_result_t<F, const T&&>;
    static_assert(detail::is_optional<std::remove_cvref_t<U>>);
    if (has_value()) {
        return std::invoke(std::forward<F>(f), std::move(value_));
    } else {
        return std::remove_cvref_t<U>();
    }
}

/// Carries out some operation on the contained object if there is one.
template <class T>
template <class F>
constexpr auto optional<T>::transform(F&& f) & {
    using U = std::invoke_result_t<F, T&>;
    static_assert(!std::is_array_v<U>);
    static_assert(!std::is_same_v<U, in_place_t>);
    static_assert(!std::is_same_v<U, nullopt_t>);
    static_assert(std::is_object_v<U> || std::is_reference_v<U>); /// References now allowed
    return (has_value()) ? optional<U>{detail::from_function, std::forward<F>(f), value_} : optional<U>{};
}

template <class T>
template <class F>
constexpr auto optional<T>::transform(F&& f) && {
    using U = std::invoke_result_t<F, T&&>;
    static_assert(!std::is_array_v<U>);
    static_assert(!std::is_same_v<U, in_place_t>);
    static_assert(!std::is_same_v<U, nullopt_t>);
    static_assert(std::is_object_v<U> || std::is_reference_v<U>); /// References now allowed
    return (has_value()) ? optional<U>{detail::from_function, std::forward<F>(f), std::move(value_)} : optional<U>{};
}

template <class T>
template <class F>
constexpr auto optional<T>::transform(F&& f) const& {
    using U = std::invoke_result_t<F, const T&>;
    static_assert(!std::is_array_v<U>);
    static_assert(!std::is_same_v<U, in_place_t>);
    static_assert(!std::is_same_v<U, nullopt_t>);
    static_assert(std::is_object_v<U> || std::is_reference_v<U>); /// References now allowed
    return (has_value()) ? optional<U>{detail::from_function, std::forward<F>(f), value_} : optional<U>{};
}

template <class T>
template <class F>
constexpr auto optional<T>::transform(F&& f) const&& {
    using U = std::invoke_result_t<F, const T&>;
    static_assert(!std::is_array_v<U>);
    static_assert(!std::is_same_v<U, in_place_t>);
    static_assert(!std::is_same_v<U, nullopt_t>);
    static_assert(std::is_object_v<U> || std::is_reference_v<U>); /// References now allowed
    return (has_value()) ? optional<U>{detail::from_function, std::forward<F>(f), std::move(value_)} : optional<U>{};
}

/**
 * @brief Returns an optional containing the contained value if it has one, or the result of calling
 * \p `f` if it does not.
 *
 * @tparam T
 * @tparam F
 * @param f
 * @return optional<T>
 */
template <class T>
template <class F>
constexpr optional<T> optional<T>::or_else(F&& f) const&
    requires (std::invocable<F> && std::copy_constructible<T>)
{
    static_assert(std::is_same_v<std::remove_cvref_t<std::invoke_result_t<F>>, optional>);
    if (has_value())
        return value_;

    return std::forward<F>(f)();
}

/**
 * @brief Returns an optional containing the contained value if it has one, or the result of calling
 * \p `f` if it does not.
 *
 * @tparam T
 * @tparam F
 * @param f
 * @return optional<T>
 */
template <class T>
template <class F>
constexpr optional<T> optional<T>::or_else(F&& f) &&
    requires (std::invocable<F> && std::move_constructible<T>)
{
    static_assert(std::is_same_v<std::remove_cvref_t<std::invoke_result_t<F>>, optional>);
    if (has_value())
        return std::move(value_);

    return std::forward<F>(f)();
}

// 22.5.3.9 Modifiers[optional.mod]
template <class T>
constexpr void optional<T>::reset() noexcept {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        if (has_value())
            value_.~T();
    }
    engaged_ = false;
}

// 22.5.4 No-value state indicator[optional.nullopt]

// 22.5.5 Class bad_optional_access[optional.bad.access]

// 22.5.6 Relational operators[optional.relops]
template <typename T, typename U>
constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_eq_rel<T, U>
{
    return static_cast<bool>(lhs) == static_cast<bool>(rhs) && (!lhs || *lhs == *rhs);
}

template <typename T, typename U>
constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_ne_rel<T, U>
{
    return static_cast<bool>(lhs) != static_cast<bool>(rhs) || (static_cast<bool>(lhs) && *lhs != *rhs);
}

template <typename T, typename U>
constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_lt_rel<T, U>
{
    return static_cast<bool>(rhs) && (!lhs || *lhs < *rhs);
}

template <typename T, typename U>
constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_gt_rel<T, U>
{
    return static_cast<bool>(lhs) && (!rhs || *lhs > *rhs);
}

template <typename T, typename U>
constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_le_rel<T, U>
{
    return !lhs || (static_cast<bool>(rhs) && *lhs <= *rhs);
}

template <typename T, typename U>
constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
    requires detail::optional_ge_rel<T, U>
{
    return !rhs || (static_cast<bool>(lhs) && *lhs >= *rhs);
}

template <typename T, std::three_way_comparable_with<T> U>
constexpr std::compare_three_way_result_t<T, U> operator<=>(const optional<T>& x, const optional<U>& y) {
    return x && y ? *x <=> *y : bool(x) <=> bool(y);
}

// 22.5.7 Comparison with nullopt[optional.nullops]
template <class T>
constexpr bool operator==(const optional<T>& lhs, nullopt_t) noexcept {
    return !lhs;
}

template <class T>
constexpr std::strong_ordering operator<=>(const optional<T>& x, nullopt_t) noexcept {
    return bool(x) <=> false;
}

// 22.5.8 Comparison with T[optional.comp.with.t]
template <typename T, typename U>
constexpr bool operator==(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_eq_rel<T, U>
{
    return lhs && *lhs == rhs;
}

template <typename T, typename U>
constexpr bool operator==(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_eq_rel<T, U>
{
    return rhs && lhs == *rhs;
}

template <typename T, typename U>
constexpr bool operator!=(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_ne_rel<T, U>
{
    return !lhs || *lhs != rhs;
}

template <typename T, typename U>
constexpr bool operator!=(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_ne_rel<T, U>
{
    return !rhs || lhs != *rhs;
}

template <typename T, typename U>
constexpr bool operator<(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_lt_rel<T, U>
{
    return !lhs || *lhs < rhs;
}

template <typename T, typename U>
constexpr bool operator<(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_lt_rel<T, U>
{
    return rhs && lhs < *rhs;
}

template <typename T, typename U>
constexpr bool operator>(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_gt_rel<T, U>
{
    return lhs && *lhs > rhs;
}

template <typename T, typename U>
constexpr bool operator>(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_gt_rel<T, U>
{
    return !rhs || lhs > *rhs;
}

template <typename T, typename U>
constexpr bool operator<=(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_le_rel<T, U>
{
    return !lhs || *lhs <= rhs;
}

template <typename T, typename U>
constexpr bool operator<=(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_le_rel<T, U>
{
    return rhs && lhs <= *rhs;
}

template <typename T, typename U>
constexpr bool operator>=(const optional<T>& lhs, const U& rhs)
    requires (!detail::is_optional<U>) && detail::optional_ge_rel<T, U>
{
    return lhs && *lhs >= rhs;
}

template <typename T, typename U>
constexpr bool operator>=(const T& lhs, const optional<U>& rhs)
    requires (!detail::is_optional<T>) && detail::optional_ge_rel<T, U>
{
    return !rhs || lhs >= *rhs;
}

template <typename T, typename U>
    requires (!is_derived_from_optional<U>) && std::three_way_comparable_with<T, U>
constexpr std::compare_three_way_result_t<T, U> operator<=>(const optional<T>& x, const U& v) {
    return bool(x) ? *x <=> v : std::strong_ordering::less;
}

// 22.5.9 Specialized algorithms[optional.specalg]

template <class T>
constexpr void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
    requires std::is_move_constructible_v<T> && std::is_swappable_v<T>
{
    return x.swap(y);
}

template <int, class T>
constexpr optional<std::decay_t<T>>
make_optional(T&& t) noexcept(std::is_nothrow_constructible_v<optional<std::decay_t<T>>, T>)
    requires std::is_constructible_v<std::decay_t<T>, T>
{
    return optional<std::decay_t<T>>{std::forward<T>(t)};
}

template <typename T, typename... Args>
constexpr optional<T> make_optional(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    requires std::is_constructible_v<T, Args...>
{
    static_assert(!std::is_reference_v<T>, "May not make an optional containing a reference");
    return optional<T>{in_place, std::forward<Args>(args)...};
}

template <typename T, typename U, typename... Args>
constexpr optional<T>
make_optional(std::initializer_list<U> init_list,
              Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
{
    static_assert(!std::is_reference_v<U>, "Can not make optional from initializer_list of references.");
    return optional<T>{in_place, init_list, std::forward<Args>(args)...};
}

/****************/
/* optional<T&> */
/****************/

namespace detail {

#ifdef __cpp_lib_reference_from_temporary
using std::reference_constructs_from_temporary_v;
using std::reference_converts_from_temporary_v;
#else
template <class To, class From>
concept reference_converts_from_temporary_v =
    std::is_reference_v<To> &&
    (
        // A prvalue of a type similar to To, so that we're binding directly to
        // the materialized prvalue of type From
        (!std::is_reference_v<From> && std::is_convertible_v<std::remove_cvref_t<From>*, std::remove_cvref_t<To>*>) ||
        // A value of an unrelated type, convertible to To, but only by
        // materializing a To and binding a const reference; if we were trying
        // to bind a non-const reference, we'd be unable to. (This is not quite
        // exhaustive of the problem cases, but I think it's fairly close in
        // practice.)
        (std::is_lvalue_reference_v<To> && std::is_const_v<std::remove_reference_t<To>> &&
         std::is_convertible_v<From, const std::remove_cvref_t<To>&&> &&
         !std::is_convertible_v<From, std::remove_cvref_t<To>&>));

template <class To, class From>
concept reference_constructs_from_temporary_v =
    // This is close in practice, because cases where conversion and
    // construction differ in semantics are rare.
    reference_converts_from_temporary_v<To, From>;
#endif

} // namespace detail

/**
 * @brief A specialization of `optional` for references.
 */
template <class T>
class optional<T&> {
  public:
    /**
     * @brief The type of the value contained in the optional.
     *
     */
    using value_type = T;
    /**
     * @brief The type of the iterator for the optional.
     *
     */
    using iterator = detail::contiguous_iterator<T,
                                                 optional>; // see [optionalref.iterators]
  public:
    // \ref{optionalref.ctor}, constructors

    /**
     * @brief Default constructor.
     */
    constexpr optional() noexcept = default;

    /**
     * @brief Constructs an empty optional.
     */
    constexpr optional(nullopt_t) noexcept : optional() {}

    /**
     * @brief Copy constructor.
     *
     * Constructs an empty optional if the rhs is empty, otherwise constructs
     * the contained value from the rhs.
     */
    constexpr optional(const optional& rhs) noexcept = default;

    /**
     * @brief In-place constructor.
     *
     * @tparam Arg
     * @param arg The value to construct in-place from.
     */
    template <class Arg>
        requires (std::is_constructible_v<T&, Arg> && !detail::reference_constructs_from_temporary_v<T&, Arg>)
    constexpr explicit optional(in_place_t, Arg&& arg);

    /**
     * @brief Construct from a U
     * @tparam U
     * @param u The value to construct from.
     * @details
     * Constructs the contained value from `u` if it is convertible to `T&`.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, U> && !(std::is_same_v<std::remove_cvref_t<U>, in_place_t>) &&
                  !(std::is_same_v<std::remove_cvref_t<U>, optional>) &&
                  !detail::reference_constructs_from_temporary_v<T&, U>)
    constexpr explicit(!std::is_convertible_v<U, T&>)
        optional(U&& u) noexcept(std::is_nothrow_constructible_v<T&, U>) {
        convert_ref_init_val(u);
    }

    /**
     * @brief Constructs an optional from a U, but deletes the constructor if
     *
     * @tparam U
     */
    template <class U>
        requires (std::is_constructible_v<T&, U> && !(std::is_same_v<std::remove_cvref_t<U>, in_place_t>) &&
                  !(std::is_same_v<std::remove_cvref_t<U>, optional>) &&
                  detail::reference_constructs_from_temporary_v<T&, U>)
    constexpr optional(U&& u) = delete;

    // The full set of 4 overloads on optional<U> by value category, doubled to
    // 8 by deleting if reference_constructs_from_temporary_v is true. This
    // allows correct constraints by propagating the value category from the
    // optional to the value within the rhs.

    /**
     * @brief Constructs an optional from another optional of type U.
     *
     * @tparam U
     * @param rhs The optional to construct from.
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, U&> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, U&>)
    constexpr explicit(!std::is_convertible_v<U&, T&>)
        optional(optional<U>& rhs) noexcept(std::is_nothrow_constructible_v<T&, U&>);

    /**
     * @brief Constructs an optional from another optional of type U.
     * @param rhs The optional to construct from.
     * @tparam U
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, const U&> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, const U&>)
    constexpr explicit(!std::is_convertible_v<const U&, T&>)
        optional(const optional<U>& rhs) noexcept(std::is_nothrow_constructible_v<T&, const U&>);

    /**
     * @brief Constructs an optional from another optional of type U.
     * @param rhs The optional to construct from.
     * @tparam U
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, U> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, U>)
    constexpr explicit(!std::is_convertible_v<U, T&>)
        optional(optional<U>&& rhs) noexcept(noexcept(std::is_nothrow_constructible_v<T&, U>));

    /**
     * @brief Constructs an optional from another optional of type U.
     *
     * @tparam U The type contained the right hand side optional.
     * @param rhs The optional to construct from.
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, const U> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, const U>)
    constexpr explicit(!std::is_convertible_v<const U, T&>)
        optional(const optional<U>&& rhs) noexcept(noexcept(std::is_nothrow_constructible_v<T&, const U>));

    /**
     * @brief Constructs an optional from another optional of type U
     *
     * @tparam U
     * @param rhs
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, U&> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && detail::reference_constructs_from_temporary_v<T&, U&>)
    constexpr optional(optional<U>& rhs) = delete;

    /**
     * @brief Constructs an optional from another optional of type U
     *
     * @tparam U
     * @param rhs
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, const U&> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && detail::reference_constructs_from_temporary_v<T&, const U&>)
    constexpr optional(const optional<U>& rhs) = delete;

    /**
     * @brief Constructs an optional from another optional of type U
     *
     * @tparam U
     * @param rhs
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, U> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && detail::reference_constructs_from_temporary_v<T&, U>)
    constexpr optional(optional<U>&& rhs) = delete;

    /**
     * @brief Constructs an optional from another optional of type U
     *
     * @tparam U
     * @param rhs
     * @details
     * Constructs the contained value from the rhs if it has a value, otherwise
     * constructs an empty optional.
     * If `T&` can be constructed from a temporary, this constructor is
     * deleted to prevent binding a temporary to a reference.
     */
    template <class U>
        requires (std::is_constructible_v<T&, const U> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
                  !std::is_same_v<T&, U> && detail::reference_constructs_from_temporary_v<T&, const U>)
    constexpr optional(const optional<U>&& rhs) = delete;

    // \ref{optionalref.dtor}, destructor
    /**
     * @brief Destructor.
     *
     * @details
     * Does not destroy the contained value, as it is a reference.
     */
    constexpr ~optional() = default;

    // \ref{optionalref.assign}, assignment
    /**
     * @brief Nullopt assignment operator.
     *
     * @return optional&
     *
     * @details
     * Destroys the current value if there is one, and leaves the optional
     * in an empty state.
     */
    constexpr optional& operator=(nullopt_t) noexcept;

    /**
     * @brief Assignment operator.
     *
     * @param rhs The value to assign.
     * @return optional&
     * @details
     * If `rhs` has a value, assigns it to the contained value. Otherwise resets
     * the contained value in `*this`.
     */
    constexpr optional& operator=(const optional& rhs) noexcept = default;

    /**
     * @brief Emplaces a new value in the optional, destroying the current one
     * if the optional is engaged.
     *
     * @tparam U The type of the value to emplace.
     * @param u The value to emplace.
     * @return T&
     * @details
     * Constructs the contained value from `u` if it is convertible to `T&`.
     */
    template <class U>
        requires (std::is_constructible_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, U>)
    constexpr T& emplace(U&& u) noexcept(std::is_nothrow_constructible_v<T&, U>);

    // \ref{optionalref.swap}, swap
    /**
     * @brief Swaps the contents of this optional with another.
     *
     * @param rhs The value to swap with.
     */
    constexpr void swap(optional& rhs) noexcept;

    // \ref{optional.iterators}, iterator support
    /**
     * @brief Returns an iterator to the beginning of the optional.
     *
     * @return iterator
     */
    constexpr iterator begin() const noexcept;

    /**
     * @brief Returns an iterator to the end of the optional.
     *
     * @return iterator
     */
    constexpr iterator end() const noexcept;

    // \ref{optionalref.observe}, observers
    /**
     * @brief Returns a pointer to the contained value.
     *
     * @return T*
     */
    constexpr T* operator->() const noexcept;

    /**
     * @brief Returns a reference to the contained value.
     *
     * @return T&
     */
    constexpr T& operator*() const noexcept;

    /**
     * @brief Converts the optional to a boolean value.
     *
     * @return bool
     */
    constexpr explicit operator bool() const noexcept;

    /**
     * @brief Checks if the optional has a value.
     *
     * @return bool
     */
    constexpr bool has_value() const noexcept;

    /**
     * @brief Returns the contained value if there is one, otherwise throws
     *
     * @return T&
     */
    constexpr T& value() const;

    // LWG4304. std::optional<NonReturnable&> is ill-formed due to value_o
    // Resolution:
    // -?- Constraints: T is a non-array object type.
    // -?- Remarks: The return type is unspecified if T is an array type or a
    //     non-object type. [Note ?: This is to avoid the declaration being
    //     ill-formed. — end note]
    //
    // Implementer Note: Using decay_t as a detail as it is remove_cv_t for
    // non-array objects, and produces a valid type for arrays and functions,
    // which are otherwise `required` out.
    /**
     * @brief Returns the contained value if there is one, otherwise returns `u`.
     *
     * @tparam U The type of the alternate value
     * @param u The value to return in the empty case
     * @return std::remove_cv_t<T>
     */
    template <class U = std::remove_cv_t<T>>
        requires (std::is_object_v<T> && !std::is_array_v<T>)
    constexpr std::decay_t<T> value_or(U&& u) const;

    // \ref{optionalref.monadic}, monadic operations
    /**
     * @brief Applies a function to the contained value if there is one.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return auto
     *
     * @details
     * The return type is the same as `std::invoke_result_t<F, T&>`,
     * but wrapped in an optional. The function \p f must return an optional type.
     */
    template <class F>
    constexpr auto and_then(F&& f) const;

    /**
     * @brief Applies a function to the contained value if there is one.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return optional<std::invoke_result_t<F, T&>>
     *
     * @details
     * The return type is the same as `std::invoke_result_t<F, T&>`,
     * but wrapped in an optional.
     */
    template <class F>
    constexpr optional<std::invoke_result_t<F, T&>> transform(F&& f) const;

    /**
     * @brief Calls a function if the optional is empty.
     *
     * @tparam F The type of the invocable
     * @param f The invocable to apply to the contained value
     * @return optional
     * @details
     * The return type is the same as the return type of \p f, which must
     * return an optional type.
     */
    template <class F>
    constexpr optional or_else(F&& f) const
        requires (std::invocable<F>);

    // \ref{optional.mod}, modifiers
    /**
     * @brief Resets the optional to an empty state.
     *
     */
    constexpr void reset() noexcept;

  private:
    T* value_ = nullptr; // exposition only

    // \ref{optionalref.expos}, exposition only helper functions
    template <class U>
    constexpr void convert_ref_init_val(U&& u) {
        // Creates a variable, \tcode{r},
        // as if by \tcode{T\& r(std::forward<U>(u));}
        // and then initializes \exposid{val} with \tcode{addressof(r)}
        T& r(std::forward<U>(u));
        value_ = std::addressof(r);
    }

    template <class U>
    friend class optional;

    template <class F, class Arg>
    constexpr optional(detail::from_function_t, F&& f, Arg&& arg) {
        convert_ref_init_val(std::invoke(std::forward<F>(f), std::forward<Arg>(arg)));
    }
};

//  \rSec3[optionalref.ctor]{Constructors}
template <class T>
template <class Arg>
    requires (std::is_constructible_v<T&, Arg> && !detail::reference_constructs_from_temporary_v<T&, Arg>)
constexpr optional<T&>::optional(in_place_t, Arg&& arg) {
    convert_ref_init_val(std::forward<Arg>(arg));
}

// Clang is unhappy with the out-of-line definition
//
// template <class T>
// template <class U>
//     requires(std::is_constructible_v<T&, U> && !(is_same_v<remove_cvref_t<U>, in_place_t>) &&
//              !(is_same_v<remove_cvref_t<U>, optional<T&>>) && !detail::reference_constructs_from_temporary_v<T&, U>)
// constexpr optional<T&>::optional(U&& u) noexcept(is_nothrow_constructible_v<T&, U>)
//     : value_(std::addressof(static_cast<T&>(std::forward<U>(u)))) {}

template <class T>
template <class U>
    requires (std::is_constructible_v<T&, U&> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
              !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, U&>)
constexpr optional<T&>::optional(optional<U>& rhs) noexcept(std::is_nothrow_constructible_v<T&, U&>) {
    if (rhs.has_value()) {
        convert_ref_init_val(*rhs);
    }
}

template <class T>
template <class U>
    requires (std::is_constructible_v<T&, const U&> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
              !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, const U&>)
constexpr optional<T&>::optional(const optional<U>& rhs) noexcept(std::is_nothrow_constructible_v<T&, const U&>) {
    if (rhs.has_value()) {
        convert_ref_init_val(*rhs);
    }
}

template <class T>
template <class U>
    requires (std::is_constructible_v<T&, U> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
              !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, U>)
constexpr optional<T&>::optional(optional<U>&& rhs) noexcept(noexcept(std::is_nothrow_constructible_v<T&, U>)) {
    if (rhs.has_value()) {
        convert_ref_init_val(*std::move(rhs));
    }
}

template <class T>
template <class U>
    requires (std::is_constructible_v<T&, const U> && !std::is_same_v<std::remove_cv_t<T>, optional<U>> &&
              !std::is_same_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, const U>)
constexpr optional<T&>::optional(const optional<U>&& rhs) noexcept(
    noexcept(std::is_nothrow_constructible_v<T&, const U>)) {
    if (rhs.has_value()) {
        convert_ref_init_val(*std::move(rhs));
    }
}

// \rSec3[optionalref.assign]{Assignment}
template <class T>
constexpr optional<T&>& optional<T&>::operator=(nullopt_t) noexcept {
    value_ = nullptr;
    return *this;
}

template <class T>
template <class U>
    requires (std::is_constructible_v<T&, U> && !detail::reference_constructs_from_temporary_v<T&, U>)
constexpr T& optional<T&>::emplace(U&& u) noexcept(std::is_nothrow_constructible_v<T&, U>) {
    convert_ref_init_val(std::forward<U>(u));
    return *value_;
}

//   \rSec3[optionalref.swap]{Swap}

template <class T>
constexpr void optional<T&>::swap(optional<T&>& rhs) noexcept {
    std::swap(value_, rhs.value_);
}

// \rSec3[optionalref.iterators]{Iterator Support}

template <class T>
constexpr optional<T&>::iterator optional<T&>::begin() const noexcept {
    return iterator(has_value() ? value_ : nullptr);
};

template <class T>
constexpr optional<T&>::iterator optional<T&>::end() const noexcept {
    return begin() + has_value();
}

// \rSec3[optionalref.observe]{Observers}
template <class T>
constexpr T* optional<T&>::operator->() const noexcept {
    return value_;
}

template <class T>
constexpr T& optional<T&>::operator*() const noexcept {
    return *value_;
}

template <class T>
constexpr optional<T&>::operator bool() const noexcept {
    return value_ != nullptr;
}
template <class T>
constexpr bool optional<T&>::has_value() const noexcept {
    return value_ != nullptr;
}

template <class T>
constexpr T& optional<T&>::value() const {
    if (has_value()) {
        return *value_;
    }
    throw bad_optional_access();
}

template <class T>
template <class U>
    requires (std::is_object_v<T> && !std::is_array_v<T>)
constexpr std::decay_t<T> optional<T&>::value_or(U&& u) const {
    using X = std::remove_cv_t<T>;
    static_assert(std::is_convertible_v<T&, X>, "remove_cv_t<T> must be constructible from a T&");
    static_assert(std::is_convertible_v<U, X>, "Must be able to convert u to remove_cv_t<T>");
    if (has_value()) {
        return *value_;
    }
    return std::forward<U>(u);
}

//   \rSec3[optionalref.monadic]{Monadic operations}
template <class T>
template <class F>
constexpr auto optional<T&>::and_then(F&& f) const {
    using U = std::invoke_result_t<F, T&>;
    static_assert(detail::is_optional<U>, "F must return an optional");
    if (has_value()) {
        return std::invoke(std::forward<F>(f), *value_);
    } else {
        return std::remove_cvref_t<U>();
    }
}

/**
 * @brief Transforms the value contained in the optional, if it exists.
 *
 * @return optional<std::invoke_result_t<F, T&>>
 */
template <class T>
template <class F>
constexpr optional<std::invoke_result_t<F, T&>> optional<T&>::transform(F&& f) const {
    using U = std::invoke_result_t<F, T&>;
    static_assert(!std::is_same_v<std::remove_cvref_t<U>, in_place_t>, "Result must not be in_place_t");
    static_assert(!std::is_same_v<std::remove_cvref_t<U>, nullopt_t>, "Result must not be nullopt_t");
    static_assert((std::is_object_v<U> && !std::is_array_v<U>) || std::is_lvalue_reference_v<U>,
                  "Result must be an non-array object or an lvalue reference");
    if (has_value()) {
        return optional<U>{detail::from_function, std::forward<F>(f), *value_};
    } else {
        return optional<U>{};
    }
}

/**
 * @brief Returns an optional containing the contained value if it has one, or the result of calling
 * \p `f` if it does not.
 *
 * @tparam T
 * @tparam F
 * @param f
 * @return optional<T&>
 */
template <class T>
template <class F>
constexpr optional<T&> optional<T&>::or_else(F&& f) const

    requires (std::invocable<F>)
{
    using U = std::invoke_result_t<F>;
    static_assert(std::is_same_v<std::remove_cvref_t<U>, optional>, "Result must be an optional");
    if (has_value()) {
        return *this;
    } else {
        return std::forward<F>(f)();
    }
}

// \rSec3[optional.mod]{modifiers}
template <class T>
constexpr void optional<T&>::reset() noexcept {
    value_ = nullptr;
}
} // namespace beman::optional

namespace std {
template <typename T>
    requires requires(T a) {
        { std::hash<remove_const_t<T>>{}(a) } -> std::convertible_to<std::size_t>;
    }
struct hash<beman::optional::optional<T>> {
    static_assert(!is_reference_v<T>, "hash is not enabled for reference types");
    /**
     * @brief Hashes the optional value.
     *
     * @param o
     * @return size_t
     */
    size_t operator()(const beman::optional::optional<T>& o) const noexcept(noexcept(hash<remove_const_t<T>>{}(*o))) {
        if (o) {
            return std::hash<std::remove_const_t<T>>{}(*o);
        } else {
            return 0;
        }
    }
};
} // namespace std

#endif // BEMAN_OPTIONAL_OPTIONAL_HPP
