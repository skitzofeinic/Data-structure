#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "array.h"
#include "hash_table.h"

struct table {
    /* The (simple) array used to index the table */
    struct node **array;
    /* The function used for computing the hash values in this table */
    unsigned long (*hash_func)(const unsigned char *);
    /* Maximum load factor after which the table array should be resized */
    double max_load_factor;
    /* Capacity of the array used to index the table */
    unsigned long capacity;
    /* Current number of elements stored in the table */
    unsigned long load;
};

struct node {
    /* The string of characters that is the key for this node */
    char *key;
    /* A resizing array, containing the all the integer values for this key */
    struct array *value;
    /* Next pointer */
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

struct node *node_init(const char *key, struct node *next) {
    struct node *n = malloc(sizeof(struct node));
    if (!n) return NULL;

    n->key = malloc(strlen(key) + 1);
    if (!n->key) {
        free(n);
        return NULL;
    }

    n->value = array_init(1000);
    if (!n->value) {
        free(n->key);
        free(n);
        return NULL;
    }

    strcpy(n->key, key);
    n->next = next;
    return n;
}

unsigned long next_prime(unsigned long new_capacity) {
    if (new_capacity <= 1) return 2;

    if (new_capacity % 2 == 0) ++new_capacity;

    while (1) {
        bool is_prime = true;

        for (unsigned long i = 2; i * i <= new_capacity; ++i) {
            if (new_capacity % i == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) return new_capacity;

        new_capacity += 2;
    }
}

int table_resize(struct table *t, unsigned long new_capacity) {
    if (!t) return 1;

    struct node **n = calloc(next_prime(new_capacity), sizeof(struct node *));
    if (!n) return 1;

    for (size_t i = 0; i < t->capacity; ++i) {
        struct node *current = t->array[i];
        while (current) {
            struct node *temp = current;
            current = current->next;

            unsigned long idx = t->hash_func((const unsigned char *)temp->key) % next_prime(new_capacity);
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

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;

    struct node *current = t->array[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return !array_append(current->value, value) ? 0 : 1;
        }
        current = current->next;
    }

    struct node *new_node = node_init(key, t->array[index]);
    if (!new_node) return 1;

    if (array_append(new_node->value, value)){
        free(new_node->key);
        array_cleanup(new_node->value);
        free(new_node);
        return 1;
    }

    t->array[index] = new_node;
    t->load++;

    if (table_load_factor(t) >= t->max_load_factor) {
        if (table_resize(t, t->capacity * 2)) return 1;
    }
    
    return 0;
}

struct array *table_lookup(const struct table *t, const char *key) {
    if (!t || !key) return NULL;

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;

    struct node *current = t->array[index];
    while (current) {
        if (strcmp(current->key, key) == 0) return current->value;
        current = current->next;
    }

    return NULL;
}

double table_load_factor(const struct table *t) {
    return (!t || t->capacity == 0) ? -1.0 : (double)t->load / (double)t->capacity;
}

int table_delete(struct table *t, const char *key) {
    if (!t || !key) return 1;

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;

    struct node *current = t->array[index];
    struct node *prev = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                t->array[index] = current->next;
            }

            free(current->key);
            array_cleanup(current->value);
            free(current);
            t->load--;

            return 0;
        }

        prev = current;
        current = current->next;
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
