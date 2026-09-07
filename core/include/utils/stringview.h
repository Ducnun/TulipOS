#pragma once

#include <cstdint>
#include <cstring>

namespace util {
    /**
     * @brief A non-owning reference to a string. 
     * Optimized for embedded use to avoid allocations and 
     * provide O(1) length checks.
     */
    struct StringView {
        const char* data;
        uint32_t length; // Using uint32_t for cross-platform bit-width consistency

        StringView() : data(nullptr), length(0) {}
        StringView(const char* s) : data(s), length(s ? (uint32_t)strlen(s) : 0) {}
        StringView(const char* s, size_t len) : data(s), length((uint32_t)len) {}

        // Fast equality check against another StringView
        bool operator==(const StringView& other) const {
            if (length != other.length) return false;
            if (data == other.data) return true;
            return memcmp(data, other.data, length) == 0;
        }

        // Fast equality check against a null-terminated C-string
        // Used by the SymbolTable to check arena-stored names
        bool equals(const char* other) const {
            if (!other) return false;
            // Check length first (if other is longer/shorter, it can't match)
            // Note: This assumes 'other' is null-terminated
            if (strlen(other) != length) return false;
            return memcmp(data, other, length) == 0;
        }

        /**
         * @brief FNV-1a Hash implementation.
         * Excellent distribution and very fast for embedded CPUs.
         */
        uint32_t hash() const {
            uint32_t h = 2166136261U;
            for (uint32_t i = 0; i < length; ++i) {
                h ^= static_cast<uint8_t>(data[i]);
                h *= 16777619U;
            }
            return h;
        }
    };

    /**
     * @brief Simple hasher wrapper for use with STL containers 
     * (if we still use unordered_map in non-kernel parts of the project)
     */
    struct StringViewHasher {
        size_t operator()(const StringView& sv) const {
            return static_cast<size_t>(sv.hash());
        }
    };
}