#pragma once

#include <cstddef>
#include <cstdint>

#include "identifier_id.h"
#include "utils/arena.h"
#include "utils/stringview.h"

class IdentifierTable {
public:
    /*
     * One record per identifier.
     *
     * The record index is the IdentifierId value.
     */
    struct Record {
        const char* data = nullptr;
        size_t length = 0;
        uint32_t hash = 0;
    };

    /*
     * One entry in the open-addressed hash table.
     *
     * An invalid ID means that the bucket is empty.
     */
    struct Bucket {
        IdentifierId identifier = INVALID_IDENTIFIER_ID;
    };

    IdentifierTable(
        Arena& identifierArena,
        Record* records,
        size_t recordCapacity,
        Bucket* buckets,
        size_t bucketCapacity
    );

    /*
     * Finds an existing identifier without allocating.
     */
    IdentifierId find(StringView name) const;

    /*
     * Returns an existing identifier or creates one.
     *
     * Returns INVALID_IDENTIFIER_ID if the table or
     * identifier arena is full.
     */
    IdentifierId getOrCreate(StringView name);

    /*
     * Returns the spelling first used when the identifier
     * was created.
     *
     * Returns an empty StringView for an invalid ID.
     */
    StringView getName(IdentifierId id) const;

    /*
     * Removes all identifiers and releases their copied
     * character data.
     */
    void clear();

    size_t getCount() const;
    size_t getCapacity() const;

private:
    static unsigned char foldAscii(unsigned char character);
    static uint32_t hashName(StringView name);
    static bool namesEqual(StringView lhs, const Record& rhs);
    size_t findBucket(StringView name, uint32_t hash, bool& found) const;

    Arena& _identifierArena;
    Arena::Mark _arenaStart;

    Record* _records = nullptr;
    size_t _recordCapacity = 0;
    size_t _recordCount = 0;

    Bucket* _buckets = nullptr;
    size_t _bucketCapacity = 0;
};