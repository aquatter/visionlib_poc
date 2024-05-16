#pragma once
#include <cstdint>
#include <type_traits>

namespace we {
template <typename T>
    requires std::is_arithmetic_v<T>
class Roi {
  public:
    constexpr Roi()
        : x_{0}
        , y_{0}
        , width_{0}
        , height_{0} {}

    constexpr Roi(T x, T y, T width, T height)
        : x_{x}
        , y_{y}
        , width_{width}
        , height_{height} {}

    Roi(const Roi &) = default;
    Roi(Roi &&) noexcept = default;
    Roi &operator=(const Roi &) = default;
    Roi &operator=(Roi &&) noexcept = default;
    ~Roi() = default;

    [[nodiscard]] bool contains(const T x, const T y) const {
        return x_ <= x and x < x_ + width_ and y_ <= y and y < y_ + height_;
    }
    [[nodiscard]] constexpr T x() const noexcept { return x_; }
    [[nodiscard]] constexpr T y() const noexcept { return y_; }
    [[nodiscard]] constexpr T width() const noexcept { return width_; }
    [[nodiscard]] constexpr T height() const noexcept { return height_; }

  private:
    T x_;
    T y_;
    T width_;
    T height_;
};

using Roi2i = Roi<int32_t>;
using Roi2ui = Roi<uint32_t>;
using Roi2f = Roi<float>;
using Roi2d = Roi<double>;
} // namespace we