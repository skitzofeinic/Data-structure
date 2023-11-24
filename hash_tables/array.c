#include <stdlib.h>

#include "array.h"

struct array {
    unsigned long index;
    int *element;
    unsigned long capacity;
};

struct array *array_init(unsigned long initial_capacity) {
    struct array *a = malloc(sizeof(struct array));
    if (a == NULL) return NULL;

    a->element = malloc(sizeof(int) * initial_capacity);
    if (a->element == NULL) {
        free(a);
        return NULL;
    }

    a->index = 0;
    a->capacity = initial_capacity;

    return a;
}

void array_cleanup(struct array *a) {
    free(a->element);
    free(a);
}

int array_get(const struct array *a, unsigned long index) {
    return (a == NULL || index >= a->index) ? a->element[index] : -1;
}

/* Note: Although this operation might require the array to be resized and
 * copied, in order to make room for the added element, it is possible to do
 * this in such a way that the amortized complexity is still O(1).
 * Make sure your code is implemented in such a way to guarantee this. */
int array_append(struct array *a, int elem) {
    /* ... SOME CODE MISSING HERE ... */
}

unsigned long array_size(const struct array *a) {

}
