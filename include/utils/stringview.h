#pragma once

#include <cstring>
#include <memory>

namespace util{
    // Custom string view implementation for Arduino framework
    struct StringView {
        const char* data;
        size_t length;

        StringView(const std::string& s) : data(s.data()), length(s.length()) {}
        StringView(const char* s) : data(s), length(strlen(s)) {}
        StringView(const char* s, size_t len) : data(s), length(len) {}

        bool operator==(const StringView& other) const {
            if (length != other.length) return false;
            return memcmp(data, other.data, length) == 0;
        }
    };

    // Simple hasher for StringView so we can use it in unordered_map
    struct StringViewHasher {
        size_t operator()(const StringView& sv) const {
            size_t hash = 2166136261U;
            for (size_t i = 0; i < sv.length; ++i) {
                hash ^= static_cast<size_t>(sv.data[i]);
                hash *= 16777619U;
            }
            return hash;
        }
    };
}