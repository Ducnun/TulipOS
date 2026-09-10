#pragma once

#include <cstdint>
#include <limits>

struct IdentifierId {
    using Underlying = uint16_t;

    Underlying value = std::numeric_limits<Underlying>::max();

    constexpr bool isValid() const {
        return value != std::numeric_limits<Underlying>::max();
    }

    friend constexpr bool operator==(IdentifierId lhs, IdentifierId rhs) {
        return lhs.value == rhs.value;
    }

    friend constexpr bool operator!=(IdentifierId lhs, IdentifierId rhs) {
        return !(lhs == rhs);
    }
};

inline constexpr IdentifierId INVALID_IDENTIFIER_ID {
    std::numeric_limits<uint16_t>::max()
};