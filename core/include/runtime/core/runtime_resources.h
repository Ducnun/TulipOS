#pragma once

#include <stddef.h>
#include "utils/arena.h"
#include "utils/managed_arena.h"
#include "variable_store.h"
#include "value.h"
#include "identifier_id.h"
#include "identifier_table.h"

class RuntimeResources {
public:
    RuntimeResources(void* srcBuf, size_t srcSize,
                     void* codeBuf, size_t codeSize,
                     void* tranBuf, size_t tranSize,
                     void* symBuf,  size_t symSize,
                     void* persABuf, void* persBBuf, size_t persSize) {
        // Initialize all arenas
        srcArena.init(srcBuf, srcSize);
        codeArena.init(codeBuf, codeSize);
        tranArena.init(tranBuf, tranSize);
        symArena.init(symBuf, symSize);
        persArena.init(persABuf, persBBuf, persSize);

        // === Allocate IdentifierTable from symArena ===
        // We need: records + buckets
        constexpr size_t identifierHashTableSize = 256; // or calculate from symSize
        constexpr size_t identifierCapacity = 128;        // max identifiers

        // Allocate buckets and records from symArena
        using Bucket = typename IdentifierTable::Bucket;
        using Record = typename IdentifierTable::Record;

        size_t bucketAllocSize = identifierHashTableSize * sizeof(Bucket);
        size_t recordAllocSize = identifierCapacity * sizeof(Record);

        Bucket* buckets = static_cast<Bucket*>(symArena.alloc(bucketAllocSize));
        Record* records = static_cast<Record*>(symArena.alloc(recordAllocSize));

        // Construct IdentifierTable in-place (no dynamic allocation)
        identifierTable = IdentifierTable(
            symArena,           // Arena for future dynamic allocations (if any)
            records,            // Record array
            identifierCapacity, // Record capacity
            buckets,           // Bucket array
            identifierHashTableSize
        );

        // === Allocate VariableStore ===
        size_t remainingSize = symArena.getSize() - symArena.used();
        size_t maxVariables = (remainingSize / sizeof(Value)) & ~(15); // Align to 16
        if (maxVariables == 0) {
            // Out of memory — handle this in debug
            maxVariables = 64;
        }

        Value* variableBuffer = static_cast<Value*>(symArena.alloc(maxVariables * sizeof(Value)));
        _variableStore = new (symArena.alloc(sizeof(VariableStore)))
            VariableStore(variableBuffer, maxVariables);

        resetVariables();
    }

    // --- IdentifierTable Access ---
    IdentifierId intern(StringView name) {
        return identifierTable.getOrCreate(name);
    }

    // --- Variable Management ---
    VariableId getOrCreateVariable(StringView name) {
        IdentifierId id = intern(name);
        return getOrCreateVariable(id);
    }

    VariableId getOrCreateVariable(IdentifierId nameId) {
        if (auto it = variableRegistry.find(nameId); it != variableRegistry.end()) {
            return it->second;
        }

        VariableId newVarId(nextVarId++);
        if (nextVarId >= _variableStore->getCapacity()) {
            return VariableId::Invalid; // Out of variable slots
        }

        _variableStore->set(newVarId, Value::Float(0.0f)); // Initialize
        variableRegistry[nameId] = newVarId;
        return newVarId;
    }

    Value getVariableValue(VariableId varId) {
        if (varId.value < _variableStore->getCapacity()) {
            return _variableStore->get(varId);
        }
        return Value::Float(0.0f); // or error
    }

    void setVariableValue(VariableId varId, Value value) {
        if (varId.value < _variableStore->getCapacity()) {
            _variableStore->set(varId, value);
        }
    }

    // --- Promotion ---
    Value promoteValue(Value val) {
        if (val.type == ValueType::TransientString && val.rawPtr) {
            char* newStorage = persArena.dupString(
                val.rawPtr, val.length,
                _variableStore->getData(), _variableStore->getCapacity()
            );
            if (newStorage) return Value::PersistentStr(newStorage, val.length);
        }
        return val;
    }

    // --- Reset ---
    void reset() {
        resetArenas();
        resetVariables();
        nextVarId = 0;
        variableRegistry.clear();
        identifierTable.clear();
    }

    void resetArenas() {
        srcArena.reset();
        tranArena.reset();
        symArena.reset();      // Note: This will *not* reset IdentifierTable's internal state!
        persArena.reset();
    }

    void resetVariables() {
        size_t count = _variableStore->getCapacity();
        for (size_t i = 0; i < count; i++) {
            _variableStore->set(i, Value::Float(0.0f));
        }
    }

    // === Public Arenas (for parser/interpreter) ===
    Arena srcArena;
    Arena codeArena;
    Arena tranArena;
    Arena symArena;
    ManagedArena persArena;

    // === Accessors ===
    IdentifierTable& getIdentifierTable() { return identifierTable; }
    VariableStore* getVariableStore() { return _variableStore; }

private:
    IdentifierTable identifierTable;       // Hash table for IdentifierId
    VariableStore* _variableStore = nullptr;
    std::unordered_map<IdentifierId, VariableId> variableRegistry; // IdentifierId → VariableId
    uint32_t nextVarId = 0;
};