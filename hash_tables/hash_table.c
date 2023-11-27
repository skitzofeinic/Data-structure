#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    strcpy(n->key, key);

    n->value = array_init(1000);
    if (!n->value) {
        free(n->key);
        free(n);
        return NULL;
    }

    n->next = next;

    return n;
}

struct table *table_resize(struct table *t, unsigned long new_capacity) {
    struct table *new_table = table_init(new_capacity, t->max_load_factor, t->hash_func);
    if (!new_table) return NULL;

    for (unsigned long i = 0; i < t->capacity; ++i) {
        struct node *current = t->array[i];
        while (current) {
            for (unsigned long j = 0; j < array_size(current->value); ++j) {
                int value = array_get(current->value, j);
                table_insert(new_table, current->key, value);
            }

            current = current->next;
        }
    }

    free(t->array);
    free(t);

    return new_table;
}

unsigned long new_capacity() {
    
}

int table_insert(struct table *t, const char *key, int value) {
    if (!t || !key) return 1;

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;

    struct node *current = t->array[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return array_append(current->value, value) ? 0 : 1;
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

    if (table_load_factor(t) > t->max_load_factor) {
        t = table_resize(t, );
        if (!t) return 1;
    }
    

    printf("\n\nval = %d\nload=%ld\nidx=%ld\n", value, t->load, index);

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
