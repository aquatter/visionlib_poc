#pragma once
#include <concepts>
#include <format>
#include <source_location>
#include <stdexcept>
#include <string_view>

namespace we {

inline void assert_true(std::invocable auto &&f, const std::string_view message = "",
                        const std::source_location location = std::source_location::current()) {

    if(not static_cast<bool>(f())) {
        throw std::runtime_error{std::format("{}, file: {}({}:{}) {}", message.data(),
                                             location.file_name(), location.line(),
                                             location.column(), location.function_name())};
    }
}

} // namespace we