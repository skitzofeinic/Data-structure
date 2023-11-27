#include <stddef.h>
#include <stdint.h>
#include <string.h>


/* Example hash function with terrible performance */
unsigned long hash_too_simple(const unsigned char *str);

/**
 * https://en.wikipedia.org/wiki/Jenkins_hash_function
*/
unsigned long jenkins_one_at_a_time_hash(const unsigned char *str);

/**
 * https://en.wikipedia.org/wiki/MurmurHash
*/
unsigned long murmur3_32(const unsigned char *str);

/**
 * http://www.cse.yorku.ca/~oz/hash.html
*/
unsigned long djb2(const unsigned char *str);
