#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Example hash function with terrible performance */
unsigned long hash_too_simple(const unsigned char *str);

/*
 * Jenkins One-at-a-Time Hash:
 * Simple and widely-used with decent performance.
 * Processes each byte individually, avoiding collisions.
 * Source: https://en.wikipedia.org/wiki/Jenkins_hash_function
 */
unsigned long jenkins_one_at_a_time_hash(const unsigned char *str);

/*
 * MurmurHash 3 (32-bit):
 * High-performance, non-cryptographic hash for quick, well-distributed codes.
 * Uses bitwise operations, multiplication, and XOR.
 * Source: https://en.wikipedia.org/wiki/MurmurHash
 */
unsigned long murmur3_32(const unsigned char *str);

/*
 * DJB2 Hash:
 * Extremely simple and fast hash.
 * Uses bitwise operations and prime numbers for good distribution.
 * Source: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long djb2(const unsigned char *str);

/*
 * FNV-1a Hash:
 * Simple and fast hash algorithm, XOR, and multiplication on prime numbers.
 * Source: https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 */
unsigned long fnv1a(const unsigned char *str);
