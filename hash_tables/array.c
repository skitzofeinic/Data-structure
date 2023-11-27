#include <stdlib.h>

#include "array.h"

struct array {
    unsigned long size;
    int *element;
    unsigned long capacity;
};

struct array *array_init(unsigned long initial_capacity) {
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
    return (!a || index >= a->size) ? -1 : a->element[index];
}

int array_append(struct array *a, int elem) {
    if (!a) return 1;

    if (a->size >= a->capacity) {
        unsigned long new_capacity = (a->capacity == 0) ? 1 : (a->capacity * 2);

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
