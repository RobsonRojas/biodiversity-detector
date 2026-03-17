/**
 * @file compat.hpp
 * @brief Compatibility shim for modern C++ features (C++ 23/26) required by the project.
 */

#pragma once

#if __has_include(<expected>)
#include <expected>
#else
#include <system_error>
#include <variant>

namespace std {
    template <typename T, typename E>
    using expected = std::variant<T, E>; // Simple shim for linting/old compilers
}
#endif

#if __has_include(<format>)
#include <format>
#else
#include <string>
namespace std {
    template<typename... Args>
    std::string format(std::string_view fmt, Args&&... args) {
        return std::string(fmt); // Dummy format for linting
    }
}
#endif
