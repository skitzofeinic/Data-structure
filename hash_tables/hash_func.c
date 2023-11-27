/**
 * Name: Nguyen Anh Le
 * StudentID: 15000370
 * BSc Informatica
 * 
 * Hash Function Implementations
 * 1. hash_too_simple
 * 2. jenkins_one_at_a_time_hash
 * 3. murmur3_32
 * 4. djb2
*/

#include "hash_func.h"

unsigned long hash_too_simple(const unsigned char *str) {
    return (unsigned long) *str;
}

unsigned long jenkins_one_at_a_time_hash(const unsigned char *str) {
    uint32_t hash = 0;
    size_t i = 0;

    while (str[i] != '\0') {
        hash += str[i++];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return (unsigned long)hash;
}

unsigned long murmur3_32(const unsigned char *str) {
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;

    uint32_t hash = 0;

    const size_t nblocks = strlen((const char *)str) / 4;

    const uint32_t *blocks = (const uint32_t *)str;

    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k1 = blocks[i];
        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;

        hash ^= k1;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    const uint8_t *tail = (const uint8_t *)(str + nblocks * 4);
    uint32_t k1 = 0;

    switch (strlen((const char *)tail) & 3) {
        case 3:
            k1 ^= tail[2] << 16;
            //fallthrough
        case 2:
            k1 ^= tail[1] << 8;
            //fallthrough
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }

    hash ^= (uint32_t)strlen((const char *)str);
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return (unsigned long)hash;
}

unsigned long djb2(const unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (unsigned long) c;
    }

    return hash;
}
