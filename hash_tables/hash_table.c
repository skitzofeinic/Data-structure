/**
 * Name: Nguyen Anh Le
 * StudentID: 15000370
 * BSc Informatica
 * 
 * Hash table implementation for generic key-value storage. This implementation
 * includes functions for table initialization, insertion, lookup, deletion,
 * resizing, and cleanup. The hash table dynamically adjusts its size to maintain
 * an efficient load factor and utilizes a hash function specified during initialization.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "array.h"
#include "hash_table.h"

#define RESIZE_FACTOR 2
#define PRIME_INCR 2
#define PRIME_START 3

struct table {
    struct node **array;
    unsigned long (*hash_func)(const unsigned char *);
    double max_load_factor;
    unsigned long capacity;
    unsigned long load;
};

struct node {
    char *key;
    struct array *value;
    struct node *next;
};

struct table *table_init(unsigned long capacity,
                         double max_load_factor,
                         unsigned long (*hash_func)(const unsigned char *)) {

    struct table *t = malloc(sizeof(struct table));
    if (!t) return NULL;

    t->array = calloc(capacity, (sizeof(struct node *)));
    if (!t->array) {
        table_cleanup(t);
        return NULL;
    }

    t->hash_func = hash_func;
    t->max_load_factor = max_load_factor;
    t->capacity = capacity;
    t->load = 0;
    return t;
}

/**
 * Initialise a node and return a pointer to it. Returns NULL on failure.
 * Requires a key and a pointer to the next node.
 * Returns: A pointer to the initialized node or NULL on failure.
 */
struct node *node_init(const char *key, struct node *next, unsigned long capacity) {
    struct node *n = malloc(sizeof(struct node));
    if (!n) return NULL;

    n->key = malloc(strlen(key) + 1);
    if (!n->key) {
        free(n);
        return NULL;
    }

    n->value = array_init(capacity);
    if (!n->value) {
        free(n->key);
        free(n);
        return NULL;
    }

    strcpy(n->key, key);
    n->next = next;
    return n;
}

/**
 * Check if the capacity is a prime number. If it is not, iteratively search for
 * the next prime number greater than or equal to the given capacity.
 * Returns: The next prime number greater than or equal to the given capacity.
 */
unsigned long next_prime(unsigned long capacity) {
    if (capacity <= 1) return PRIME_INCR;
    if (capacity % PRIME_INCR == 0) ++capacity;

    while (1) {
        bool is_prime = true;

        for (unsigned long i = PRIME_START; i * i <= capacity; i += PRIME_INCR) {
            if (capacity % i == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) return capacity;
        capacity += PRIME_INCR;
    }
}

/**
 * Resizes the table with the new capacity. It copies all the old nodes into the new table.
 * With the new capacity, it recalculates all the indexes.
 * Returns 0 if successful, otherwise returns 1.
*/
int table_resize(struct table *t, unsigned long new_capacity) {
    if (!t) return 1;

    struct node **n = calloc(next_prime(new_capacity), sizeof(struct node *));
    if (!n) return 1;

    for (unsigned long i = 0; i < t->capacity; ++i) {
        struct node *cur = t->array[i];

        while (cur) {
            struct node *temp = cur;
            cur = cur->next;

            unsigned long idx = t->hash_func((const unsigned char *)temp->key) 
                                % next_prime(new_capacity);
            temp->next = n[idx];
            n[idx] = temp;
        }
    }

    free(t->array);
    t->array = n;
    t->capacity = next_prime(new_capacity);
    return 0;
}

int table_insert(struct table *t, const char *key, int value) {
    if (!t || !key) return 1;

    unsigned long idx = t->hash_func((const unsigned char *)key) % t->capacity;
    struct node *cur = t->array[idx];

    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            return array_append(cur->value, value) ? 1 : 0;
        }
        cur = cur->next;
    }

    struct node *new_node = node_init(key, t->array[idx], sizeof(t));
    if (!new_node) return 1;

    if (array_append(new_node->value, value)){
        free(new_node->key);
        array_cleanup(new_node->value);
        free(new_node);
        return 1;
    }

    t->array[idx] = new_node;
    t->load++;

    if (table_load_factor(t) >= t->max_load_factor) {
        if (table_resize(t, t->capacity * RESIZE_FACTOR)) return 1;
    }
    
    return 0;
}

struct array *table_lookup(const struct table *t, const char *key) {
    if (!t || !key) return NULL;

    unsigned long idx = t->hash_func((const unsigned char *)key) % t->capacity;
    struct node *cur = t->array[idx];
    
    while (cur) {
        if (strcmp(cur->key, key) == 0) return cur->value;
        cur = cur->next;
    }

    return NULL;
}

double table_load_factor(const struct table *t) {
    return (!t || t->capacity == 0) ? -1.0 : (double)t->load / (double)t->capacity;
}

int table_delete(struct table *t, const char *key) {
    if (!t || !key) return 1;

    unsigned long idx = t->hash_func((const unsigned char *)key) % t->capacity;
    struct node *cur = t->array[idx];
    struct node *prev = NULL;

    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) {
                prev->next = cur->next;
            } else {
                t->array[idx] = cur->next;
            }

            free(cur->key);
            array_cleanup(cur->value);
            free(cur);
            t->load--;
            return 0;
        }

        prev = cur;
        cur = cur->next;
    }

    return 1;
}

void table_cleanup(struct table *t) {
    if (!t) return;

    for (unsigned long i = 0; i < t->capacity; ++i) {
        struct node *cur = t->array[i];

        while (cur) {
            struct node *temp = cur;
            cur = cur->next;

            free(temp->key);
            array_cleanup(temp->value);
            free(temp);
        }
    }

    free(t->array);
    free(t);
}
