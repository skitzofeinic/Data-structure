/**
 * Name: Nguyen Anh Le
 * StudentID: 15000370
 * BSc Informatica
 * 
 * Dynamic Array Implementation
 * This module implements a dynamic array that can grow dynamically as elements
 * are appended. It provides functions for array initialization, appending elements,
 * accessing elements by index, and cleaning up the array resources. The array dynamically
 * adjusts its size to accommodate new elements and efficiently manages memory.
 */

#include <stdlib.h>

#include "array.h"

#define RESIZE_FACTOR 2

struct array {
    unsigned long size;
    int *element;
    unsigned long capacity;
};

struct array *array_init(unsigned long initial_capacity) {
    if (initial_capacity == 0) return NULL;

    struct array *a = malloc(sizeof(struct array));
    if (!a) return NULL;

    a->element = malloc(sizeof(int) * initial_capacity);
    if (!a->element) {
        array_cleanup(a);
        return NULL;
    }

    a->size = 0;
    a->capacity = initial_capacity;
    return a;
}

void array_cleanup(struct array *a) {
    free(a->element);
    free(a);
}

int array_get(const struct array *a, unsigned long index) {
    return (!a || index >= array_size(a)) ? -1 : a->element[index];
}

int array_append(struct array *a, int elem) {
    if (!a) return 1;

    if (array_size(a) >= a->capacity) {
        unsigned long new_capacity = (a->capacity == 0) ? 1 : (a->capacity * RESIZE_FACTOR);

        int *new_element = realloc(a->element, sizeof(int) * new_capacity);
        if (!new_element) {
            free(a->element);
            return 1;
        }

        a->capacity = new_capacity;
        a->element = new_element;
    }

    a->element[a->size++] = elem;
    return 0;
}

unsigned long array_size(const struct array *a) {
    return (a) ? a->size : 0;
}
