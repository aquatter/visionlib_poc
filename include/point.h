#pragma once
#include "we_assert.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <type_traits>

namespace we {

template <typename T, typename... Ts>
constexpr inline bool are_same_v = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename To, typename... Froms>
constexpr inline bool are_convertible_to_v = std::conjunction_v<std::is_convertible<Froms, To>...>;

template <typename T, int Rows, int Cols>
    requires requires(T t) {
        requires std::is_arithmetic_v<T>;
        requires(Rows > 0 and Rows <= 4);
        requires(Cols > 0 and Cols <= 4);
    }
struct Matrix {
    using scalar_type = T;
    using matrix_type = Matrix<scalar_type, Rows, Cols>;
    constexpr static int nRows{Rows};
    constexpr static int nCols{Cols};
    constexpr static int Size{Rows * Cols};

    constexpr Matrix() = default;
    constexpr Matrix(const scalar_type &val) { std::ranges::fill(d_, val); }

    constexpr Matrix(const std::span<scalar_type> data) {
        assert_true([&data]() { return data.size() == Size; });
        std::ranges::copy(data, d_.begin());
    }

    template <typename... Ts>
        requires requires(Ts... ts) {
            requires sizeof...(Ts) == Size;
            requires are_same_v<Ts...>;
            requires are_convertible_to_v<scalar_type, Ts...>;
        }
    constexpr Matrix(Ts... vals)
        : d_{{static_cast<scalar_type>(vals)...}} {}

    Matrix(const Matrix &) = default;
    Matrix(Matrix &&) noexcept = default;

    Matrix &operator=(const Matrix &) = default;
    Matrix &operator=(Matrix &&) noexcept = default;

    scalar_type &operator[](size_t i) { return d_[i]; }
    const scalar_type &operator[](size_t i) const { return d_[i]; }

    template <int I> constexpr scalar_type &get() {
        static_assert(I < Size);
        return d_[I];
    }

    template <int I> constexpr const scalar_type &get() const {
        static_assert(I < Size);
        return d_[I];
    }

    constexpr scalar_type &x() {
        static_assert(Size > 0 and (Rows == 1 or Cols == 1));
        return d_[0];
    }

    constexpr const scalar_type &x() const {
        static_assert(Size > 0 and (Rows == 1 or Cols == 1));
        return d_[0];
    }

    constexpr scalar_type &y() {
        static_assert(Size > 1 and (Rows == 1 or Cols == 1));
        return d_[1];
    }

    constexpr const scalar_type &y() const {
        static_assert(Size > 1 and (Rows == 1 or Cols == 1));
        return d_[1];
    }

    constexpr scalar_type &z() {
        static_assert(Size > 2 and (Rows == 1 or Cols == 1));
        return d_[2];
    }

    constexpr const scalar_type &z() const {
        static_assert(Size > 2 and (Rows == 1 or Cols == 1));
        return d_[2];
    }

    constexpr scalar_type &w() {
        static_assert(Size > 3 and (Rows == 1 or Cols == 1));
        return d_[3];
    }

    constexpr const scalar_type &w() const {
        static_assert(Size > 3 and (Rows == 1 or Cols == 1));
        return d_[3];
    }

    constexpr scalar_type &operator()(int i, int j) {
        assert_true([&]() { return i >= 0 and j >= 0 and i < nRows and j < nCols; });
        return d_[i * nRows + j];
    }
    constexpr const scalar_type &operator()(int i, int j) const {
        assert_true([&]() { return i >= 0 and j >= 0 and i < nRows and j < nCols; });
        return d_[i * nRows + j];
    }

    bool operator==(const matrix_type &rhs) const { return std::ranges::equal(rhs.d_, d_); }

    template <typename OtherScalar>
    constexpr Matrix<OtherScalar, Rows, Cols> cast() const
        requires std::is_convertible_v<scalar_type, OtherScalar> and
                 std::is_arithmetic_v<OtherScalar>
    {
        Matrix<OtherScalar, Rows, Cols> pt;

        std::ranges::transform(d_, pt.d_.begin(),
                               [](auto &&val) { return static_cast<OtherScalar>(val); });

        return pt;
    }

    constexpr matrix_type &operator-=(const matrix_type &rhs) {
        std::ranges::transform(rhs.d_, d_.begin(), [](auto &&val) { return -val; });
        return *this;
    }

    std::array<scalar_type, Size> d_{static_cast<scalar_type>(0)};
};

using Point3f = Matrix<float, 3, 1>;
using Point3d = Matrix<double, 3, 1>;
using Point3i = Matrix<int, 3, 1>;
using Point3ub = Matrix<uint8_t, 3, 1>;
using Quaternionf = Matrix<float, 4, 1>;
using Quaterniond = Matrix<double, 4, 1>;
using Matrix3f = Matrix<float, 3, 3>;
using Matrix4f = Matrix<float, 4, 4>;
using Matrix4d = Matrix<double, 4, 4>;

} // namespace we