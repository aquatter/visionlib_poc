#pragma once
#include "point.h"
#include "pointcloud.h"
#include "roi.h"
#include "sensor3d_connector.h"
#include "welib3d_export.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <memory>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace we {

enum class cmd {
    PIXEL_X_MAX,
    PIXEL_Y_MAX,
    SENSOR_MODE,
    TRIGGER_SOURCE,
    LED_PATTERN,
    LED_POWER,
    EXPOSURE_TIME_LIMIT,
    EXPOSURE_TIME,
    ACQUISITION_TIME,
    ACQUISITION_START,
    ACQUISITION_STOP,
    SET_TRIGGER_SOFTWARE,
    CONTRAST_COMPARISON_FILTER,
    EXTRINSIC_MATRIX,
    INTRINSIC_MATRIX,
    DISTORTION,
};

enum class SensorMode { MODE_3D = 4, MODE_2D = 5 };
enum class TriggerSource { INTERNAL = 0, SOFTWRARE = 1, IO1 = 2, IO2 = 3, IO3 = 4, IO4 = 5 };
enum class PatternType { PATTERN_16 = 16, PATTERN_28 = 28, PATTERN_SOLID };

namespace detail {

#define SINGLE_ARG(A, B) A, B

#define PARAM_NAME_INFO(F)                                                                         \
    F(we::cmd::PIXEL_X_MAX, int, "GetPixelXMax")                                                   \
    F(we::cmd::PIXEL_Y_MAX, int, "GetPixelYMax")                                                   \
    F(we::cmd::SENSOR_MODE, SensorMode, "SetSensorMode")                                           \
    F(we::cmd::TRIGGER_SOURCE, TriggerSource, "SetTriggerSource")                                  \
    F(we::cmd::LED_PATTERN, PatternType, "SetLEDPattern")                                          \
    F(we::cmd::LED_POWER, int, "SetLEDPower")                                                      \
    F(we::cmd::EXPOSURE_TIME_LIMIT, std::chrono::microseconds, "SetExposureTimeLimit")             \
    F(we::cmd::EXPOSURE_TIME, std::chrono::microseconds, "SetExposureTime")                        \
    F(we::cmd::ACQUISITION_TIME, std::chrono::microseconds, "SetAcquisitionTime")                  \
    F(we::cmd::ACQUISITION_START, void, "SetAcquisitionStart")                                     \
    F(we::cmd::ACQUISITION_STOP, void, "SetAcquisitionStop")                                       \
    F(we::cmd::SET_TRIGGER_SOFTWARE, void, "SetTriggerSoftware")                                   \
    F(we::cmd::CONTRAST_COMPARISON_FILTER, float, "SetContrastComparisonFilterMinPhase")           \
    F(we::cmd::EXTRINSIC_MATRIX, Matrix4f, "GetExtrinsicCameraParameters")                         \
    F(we::cmd::INTRINSIC_MATRIX, Matrix3f, "GetIntrinsicCameraParameters")                         \
    F(we::cmd::DISTORTION, SINGLE_ARG(std::array<float, 5>), "GetIntrinsicCameraParameters")

template <we::cmd name> struct param_traits {};

#define DECLARE_PARAM_TRAITS_(param_name, T, str)                                                  \
    template <> struct param_traits<param_name> {                                                  \
        using param_type = T;                                                                      \
        static constexpr std::string_view param_str{str};                                          \
    };

PARAM_NAME_INFO(DECLARE_PARAM_TRAITS_)

template <typename T> T string_to_val(const std::string_view buf);

template <> inline float string_to_val<float>(const std::string_view buf) {
    return std::stof(buf.data());
}

template <> inline int string_to_val<int>(const std::string_view buf) {
    return std::stoi(buf.data());
}

template <> WELIB3D_EXPORT Matrix4f string_to_val<Matrix4f>(const std::string_view buf);

template <> WELIB3D_EXPORT Matrix3f string_to_val<Matrix3f>(const std::string_view buf);

template <>
WELIB3D_EXPORT std::array<float, 5> string_to_val<std::array<float, 5>>(const std::string_view buf);

template <typename T> inline std::string val_to_string(const T &val) { return {}; }

template <typename T>
    requires std::is_integral_v<T> and std::is_convertible_v<T, int>
inline std::string val_to_string(const T &val) {
    return std::format("{}", static_cast<int>(val));
}

template <typename T>
    requires std::is_floating_point_v<T> and std::is_convertible_v<T, double>
inline std::string val_to_string(const T &val) {
    return std::format("{:.3}", static_cast<double>(val));
}

template <typename T>
    requires std::is_enum_v<T>
inline std::string val_to_string(const T &val) {
    return std::format("{}", static_cast<int>(val));
}

template <> inline std::string val_to_string<PatternType>(const PatternType &val) {
    switch(val) {
    case PatternType::PATTERN_16:
        return "16";
    case PatternType::PATTERN_28:
        return "28";
    case PatternType::PATTERN_SOLID:
        return "solid";
    default:
        return "28";
    }
}

template <>
inline std::string val_to_string<std::chrono::microseconds>(const std::chrono::microseconds &val) {
    return std::format("{}", val.count());
}

} // namespace detail

struct IPaddress {

    IPaddress(const std::string_view ip_str) {

        if(not std::regex_match(ip_str.data(),
                                std::regex{R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$)"})) {
            throw std::invalid_argument{"invalid ip address"};
        }

        std::cmatch res;
        std::regex_match(ip_str.data(), res, std::regex{R"(^(\d+)\.(\d+)\.(\d+)\.(\d+)$)"});

        for(auto &&i : std::views::iota(0, 4)) {
            d_[i] = static_cast<uint8_t>(std::stoi(res[i + 1].str()));
        }
    }

    template <typename... Ts>
        requires requires(Ts... ts) {
            requires sizeof...(Ts) == 4;
            requires are_same_v<Ts...>;
            requires are_convertible_to_v<uint8_t, Ts...>;
        }
    constexpr IPaddress(Ts... vals)
        : d_{{static_cast<uint8_t>(vals)...}} {}

    inline operator std::string() const {
        return std::format("{}.{}.{}.{}", d_[0], d_[1], d_[2], d_[3]);
    }

    std::array<uint8_t, 4> d_;
};

class Sensor3d {
  public:
    WELIB3D_EXPORT explicit Sensor3d(const IPaddress &add);

    Sensor3d(const Sensor3d &) = delete;
    Sensor3d(Sensor3d &&) = delete;
    Sensor3d &operator=(const Sensor3d &) = delete;
    Sensor3d &operator=(Sensor3d &&) = delete;

    WELIB3D_EXPORT ~Sensor3d();

    template <typename T> [[nodiscard]] T get(const std::string_view param_str) const {
        return detail::string_to_val<T>(read(param_str));
    }

    template <we::cmd name> [[nodiscard]] auto get() const {
        return get<typename detail::param_traits<name>::param_type>(
            detail::param_traits<name>::param_str);
    }

    template <typename T> void set(T param, const std::string_view param_str) const {
        write(std::format("{}={}\r", param_str.data(), detail::val_to_string(param)));
    }

    template <we::cmd name>
        requires(not std::is_void_v<typename detail::param_traits<name>::param_type>)
    void set(typename detail::param_traits<name>::param_type param) const {
        set(param, detail::param_traits<name>::param_str);
    }

    template <we::cmd name>
        requires(std::is_void_v<typename detail::param_traits<name>::param_type>)
    void set() const {
        const std::string_view param_str{detail::param_traits<name>::param_str};
        write(param_str);
    }

    [[nodiscard]] WELIB3D_EXPORT we::StructuredPointCloud<we::Point3f>
    get_pointcloud(Roi2ui roi = Roi2ui{});

  private:
    WELIB3D_EXPORT void write(const std::string_view str) const;
    [[nodiscard]] WELIB3D_EXPORT std::string_view read(const std::string_view str) const;

    struct impl;
    std::unique_ptr<impl> pimpl_;
};

} // namespace we