#pragma once

#include <vector>
#include <unordered_map>
#include "utils/arena.h"
#include "utils/stringview.h"

using Symbol = uint16_t;
const Symbol INVALID_SYMBOL = 0xFFFF;

class StringRegistry{
public:
    StringRegistry(Arena& arena) : _arena(arena) {}

    Symbol getOrCreate(util::StringView sv) {
        auto it = _lookup.find(sv);
        if (it != _lookup.end()) return it->second;

        char* storage = (char*)_arena.alloc(sv.length + 1);
        if (!storage) return INVALID_SYMBOL;

        memcpy(storage, sv.data, sv.length);
        storage[sv.length] = '\0';

        Symbol newId = (Symbol)_names.size();
        _names.push_back(storage);
        _lookup[util::StringView(storage, sv.length)] = newId;
        return newId;
    }

    const char* getString(Symbol sym) const {
        if (sym >= _names.size()) return "";
        return _names[sym];
    }

    void clear() {
        _lookup.clear();
        _names.clear();
    }

private:
    std::unordered_map<util::StringView, Symbol, util::StringViewHasher> _lookup;
    std::vector<const char*> _names;
    Arena& _arena;
};