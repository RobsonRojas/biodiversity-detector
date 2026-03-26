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
    template <typename E>
    struct unexpected {
        E error;
        explicit unexpected(E e) : error(e) {}
    };

    template <typename T, typename E>
    class expected {
    public:
        expected() : val_(T()), has_val_(true) {}
        expected(T v) : val_(v), has_val_(true) {}
        expected(const E& e) : err_(e), has_val_(false) {}
        expected(unexpected<E> un) : err_(un.error), has_val_(false) {}
        
        bool has_value() const { return has_val_; }
        T& operator*() { return val_; }
        T& value() { return val_; }
        E& error() { return err_; }
        explicit operator bool() const { return has_val_; }

    private:
        T val_;
        E err_;
        bool has_val_;
    };
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
