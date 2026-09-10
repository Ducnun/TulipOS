#include "runtime/core/identifier_table.h"

#include <cstring>
#include <limits>

IdentifierTable::IdentifierTable(Arena& identifierArena, Record* records, size_t recordCapacity, Bucket* buckets, size_t bucketCapacity) :
    _identifierArena(identifierArena),
    _arenaStart(identifierArena.mark()),
    _records(records),
    _recordCapacity(recordCapacity),
    _buckets(buckets),
    _bucketCapacity(bucketCapacity) {
    clear();
}

unsigned char IdentifierTable::foldAscii(unsigned char character){
    if (character >= 'a' && character <= 'z') {
        return static_cast<unsigned char>(character - 'a' + 'A');
    }

    return character;
}

uint32_t IdentifierTable::hashName(StringView name){ 
    // 32-bit FNV-1a.
    uint32_t hash = 2166136261u;

    for (size_t index = 0; index < name.length; ++index) {
        const unsigned char character = foldAscii(static_cast<unsigned char>(name.data[index]));
        hash ^= character;
        hash *= 16777619u;
    }

    return hash;
}

bool IdentifierTable::namesEqual(StringView lhs, const Record& rhs){
    if (lhs.length != rhs.length) return false;

    for (size_t index = 0; index < lhs.length; ++index) {
        const unsigned char lhsCharacter = foldAscii(static_cast<unsigned char>(lhs.data[index]));
        const unsigned char rhsCharacter = foldAscii(static_cast<unsigned char>(rhs.data[index]));

        if (lhsCharacter != rhsCharacter) return false;
    }

    return true;
}

size_t IdentifierTable::findBucket(StringView name, uint32_t hash, bool& found) const {
    found = false;
    if (_bucketCapacity == 0) return 0;
    size_t bucketIndex = hash % _bucketCapacity;

    for (size_t probe = 0; probe < _bucketCapacity; ++probe) {
        const Bucket& bucket = _buckets[bucketIndex];

        // Because identifiers are never individually
        // deleted, the first empty bucket ends lookup.
        if (!bucket.identifier.isValid()) return bucketIndex;

        const size_t recordIndex = static_cast<size_t>(bucket.identifier.value);

        if (recordIndex < _recordCount) {
            const Record& record = _records[recordIndex];

            if (record.hash == hash && namesEqual(name, record)) {
                found = true;
                return bucketIndex;
            }
        }

        bucketIndex = (bucketIndex + 1) % _bucketCapacity;
    }

    // The bucket table is completely full.
    return _bucketCapacity;
}

IdentifierId IdentifierTable::find(StringView name) const {
    if (!name.data || name.length == 0 || !_buckets || _bucketCapacity == 0) return INVALID_IDENTIFIER_ID;

    const uint32_t hash = hashName(name);
    bool found = false;
    const size_t bucketIndex = findBucket(name, hash, found);

    if (!found || bucketIndex >= _bucketCapacity) return INVALID_IDENTIFIER_ID;
    return _buckets[bucketIndex].identifier;
}

IdentifierId IdentifierTable::getOrCreate(StringView name) {
    if (!name.data || name.length == 0 || !_records || !_buckets || _recordCount >= _recordCapacity || _bucketCapacity == 0) {
        return INVALID_IDENTIFIER_ID;
    }

    /*
     * The invalid value cannot also be used as a valid
     * array index.
     */
    constexpr size_t invalidValue = static_cast<size_t>(std::numeric_limits<IdentifierId::Underlying>::max());

    if (_recordCount >= invalidValue) return INVALID_IDENTIFIER_ID;

    const uint32_t hash = hashName(name);
    bool found = false;

    const size_t bucketIndex = findBucket(name, hash, found);

    if (found) return _buckets[bucketIndex].identifier;
    if (bucketIndex >= _bucketCapacity) return INVALID_IDENTIFIER_ID;

    /*
     * Include a terminator for debugging and interoperability,
     * although StringView itself uses the stored length.
     */
    char* copiedName = _identifierArena.allocArray<char>(name.length + 1);
    if (!copiedName) return INVALID_IDENTIFIER_ID;
    std::memcpy(copiedName, name.data, name.length);
    copiedName[name.length] = '\0';

    const IdentifierId identifier {static_cast<IdentifierId::Underlying>(_recordCount)};

    Record& record = _records[_recordCount];
    record.data = copiedName;
    record.length = name.length;
    record.hash = hash;

    _buckets[bucketIndex].identifier = identifier;
    ++_recordCount;
    return identifier;
}

StringView IdentifierTable::getName(IdentifierId id) const {
    if (!id.isValid()) return {};

    const size_t index = static_cast<size_t>(id.value);
    if (index >= _recordCount) return {};

    const Record& record = _records[index];
    return StringView {record.data, record.length};
}

void IdentifierTable::clear() {
    /*
     * Rewind only the memory allocated by this table.
     * The arena start was captured during construction.
     */
    _identifierArena.rewind(_arenaStart);

    _recordCount = 0;

    if (_records) {
        for (size_t index = 0; index < _recordCapacity; ++index) _records[index] = {};
    }

    if (_buckets) {
        for (size_t index = 0; index < _bucketCapacity; ++index) _buckets[index].identifier = INVALID_IDENTIFIER_ID;
    }
}

size_t IdentifierTable::getCount() const {
    return _recordCount;
}

size_t IdentifierTable::getCapacity() const {
    return _recordCapacity;
}