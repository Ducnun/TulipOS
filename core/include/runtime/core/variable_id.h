#pragma once

#include <cstdint>
#include <limits>

struct VariableId {
    using Underlying = uint16_t;

    Underlying value = std::numeric_limits<Underlying>::max();

    constexpr bool isValid() const {
        return value != std::numeric_limits<Underlying>::max();
    }

    friend constexpr bool operator==(VariableId lhs, VariableId rhs){
        return lhs.value == rhs.value;
    }

    friend constexpr bool operator!=(VariableId lhs, VariableId rhs) {
        return !(lhs == rhs);
    }
};

inline constexpr VariableId INVALID_VARIABLE_ID {};
using VariableSlot = uint32_t;