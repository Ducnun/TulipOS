#pragma once
#include "variable_id.h"
#include "identifier_id.h"
#include <cstddef>

class VariableRegistry {
public:
    explicit VariableRegistry(VariableId* map, size_t mapCapacity) : _map(map), _mapCapacity(mapCapacity) {
        clear();
    }

    // Returns INVALID_VARIABLE_ID if id is invalid or out of range.
    VariableId lookup(IdentifierId id) const {
        if (!id.isValid() || id.value >= _mapCapacity) return INVALID_VARIABLE_ID;
        return _map[id.value];
    }

    // Records a binding or INVALID if already taken / exceeded.
    bool bind(IdentifierId id, VariableId var) {
        if (!id.isValid() || id.value >= _mapCapacity) return false;
        _map[id.value] = var;
        return true;
    }

    void clear() {
        for (size_t i = 0; i < _mapCapacity; i++){
            _map[i] = INVALID_VARIABLE_ID;
        }
    }

private:
    VariableId* _map;
    size_t      _mapCapacity;
};