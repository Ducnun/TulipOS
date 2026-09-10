#pragma once

#include "runtime/core/value.h"
#include "utils/arena.h"

class VariableStore {
public:
    explicit VariableStore(Value* storage, size_t capacity)
        : _storage(storage), _capacity(capacity) {
        clear();
    }

    // Get or set a variable by its ID
    Value get(VariableId id) const {
        if (!id.isValid() || id.value >= _capacity) {
            return Value::Float(0.0f); // Default value for invalid access
        }
        return _storage[id.value];
    }

    void set(VariableId id, Value value) {
        if (id.isValid() && id.value < _capacity) {
            _storage[id.value] = value;
        }
    }

    // Bulk operations
    void clear() {
        for (size_t i = 0; i < _capacity; ++i) {
            _storage[i] = Value::Float(0.0f); // Reset to default
        }
    }

    // Access the raw storage (for integration with ManagedArena)
    Value* getStorage() const { return _storage; }
    size_t getCapacity() const { return _capacity; }

    // Check if an ID is valid for this store
    bool isValid(VariableId id) const {
        return id.isValid() && id.value < _capacity;
    }

private:
    Value* _storage;
    size_t _capacity;
};