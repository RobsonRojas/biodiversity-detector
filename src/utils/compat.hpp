/**
 * @file compat.hpp
 * @brief Compatibility shim for modern C++ features.
 * 
 * This file provides standalone implementations of expected, span, and format
 * to ensure consistent behavior across compilers and environments (ESP-IDF, Linux, Renode).
 */

#pragma once

#include <system_error>
#include <variant>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>

namespace guardian {

    // --- EXPECTED ---
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
        const T& operator*() const { return val_; }
        T* operator->() { return &val_; }
        const T* operator->() const { return &val_; }
        T& value() { return val_; }
        const E& error() const { return err_; }
        explicit operator bool() const { return has_val_; }

    private:
        T val_;
        E err_;
        bool has_val_;
    };

    template <typename E>
    class expected<void, E> {
    public:
        expected() : has_val_(true) {}
        expected(const E& e) : err_(e), has_val_(false) {}
        expected(unexpected<E> un) : err_(un.error), has_val_(false) {}
        
        bool has_value() const { return has_val_; }
        void operator*() {}
        void value() {}
        const E& error() const { return err_; }
        explicit operator bool() const { return has_val_; }

    private:
        E err_;
        bool has_val_;
    };

    // --- SPAN ---
    template <typename T, size_t Extent = (size_t)-1>
    struct span {
        T* data_;
        size_t size_;
        
        span() : data_(nullptr), size_(0) {}
        span(T* ptr, size_t s) : data_(ptr), size_(s) {}
        
        template<typename Container, typename = decltype(std::declval<Container>().data())>
        span(Container& c) : data_(c.data()), size_(c.size()) {}

        template<typename Container, typename = decltype(std::declval<const Container>().data())>
        span(const Container& c) : data_(const_cast<T*>(c.data())), size_(c.size()) {}

        T* data() const { return data_; }
        size_t size() const { return size_; }
        size_t size_bytes() const { return size_ * sizeof(T); }
        bool empty() const { return size_ == 0; }
        
        T& operator[](size_t i) { return data_[i]; }
        const T& operator[](size_t i) const { return data_[i]; }
        
        T* begin() const { return data_; }
        T* end() const { return data_ + size_; }

        span<T, (size_t)-1> subspan(size_t offset, size_t count = (size_t)-1) const {
            size_t actual_count = (count == (size_t)-1) ? (size_ - offset) : count;
            return span<T, (size_t)-1>(data_ + offset, actual_count);
        }
    };

    // --- FORMAT ---
}

// Conditionally include <format> for std::vformat if available, otherwise fallback
#if __has_include(<format>) && __cplusplus >= 202002L
#include <format>
namespace guardian {
    template<typename... Args>
    std::string format(std::string_view fmt, Args&&... args) {
        try {
            return std::vformat(fmt, std::make_format_args(args...));
        } catch (...) {
            return std::string(fmt);
        }
    }
}
#else
namespace guardian {
    template<typename... Args>
    std::string format(std::string_view fmt, [[maybe_unused]] Args&&... args) {
        return std::string(fmt); // Barebones fallback
    }
}
#endif
