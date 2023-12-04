/**
 * Name: Nguyen Anh Le
 * studentID: 15000370
 * BSc informatica
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "prioq.h"

#define INIT_SIZE 0
#define LEFT_CHILD(i) (2 * (i) + 1)
#define RIGHT_CHILD(i) (2 * (i) + 2)
#define PARENT(i) ((i - 1) / 2)
#define HALF(n) ((n) / 2)
#define ARRAY_SIZE_MINUS_ONE(arr) (array_size(arr) - 1)

static struct heap *heap_init(int (*compare)(const void *, const void *)) {
    struct heap *h = malloc(sizeof(struct heap));
    if (!h) return NULL;

    h->array = array_init(INIT_SIZE);
    if (!h->array) return NULL;

    h->compare = compare;
    return h;
}

prioq *prioq_init(int (*compare)(const void *, const void *)) {
    return heap_init(compare);
}

long int prioq_size(const prioq *q) {
    return q ? array_size(q->array) : -1;
}

static int heap_cleanup(struct heap *h, void free_func(void *)) {
    if (!h) return 1;
    
    if (h->array) {
        array_cleanup(h->array, free_func);
    }
    
    free(h);
    return 0;
}

int prioq_cleanup(prioq *q, void free_func(void *)) {
    return q ? heap_cleanup(q, free_func) : -1;
}

static void array_swap_elements(struct array *arr, long i, long j) {
    void *temp = array_get(arr, i);
    array_set(arr, i, array_get(arr, j));
    array_set(arr, j, temp);
}

static int heap_insert(struct heap *h, void *p) {
    if (!h || !p) return 1;

    array_append(h->array, p);
    long idx = ARRAY_SIZE_MINUS_ONE(h->array);

    while (idx) {
        long parent_idx = (idx - 1) / 2;
        void *parent_val = array_get(h->array, parent_idx);

        if (h->compare(p, parent_val) > 0) break;
        
        array_swap_elements(h->array, idx, parent_idx);
        idx = parent_idx;
    }

    return 0;
}

int prioq_insert(prioq *q, void *p) {
    return heap_insert(q, p);
}

static void heapify(struct heap *h, long i) {
    long size = array_size(h->array);

    while (i < HALF(size)) {
        long l = LEFT_CHILD(i);
        long r = RIGHT_CHILD(i);
        long smallest = i;

        if (l < size && h->compare(array_get(h->array, l), array_get(h->array, smallest)) < 0) {
            smallest = l;
        }

        if (r < size && h->compare(array_get(h->array, r), array_get(h->array, smallest)) < 0) {
            smallest = r;
        }

        if (smallest == i) break;

        array_swap_elements(h->array, i, smallest);
        i = smallest;
    }

}

static void *heap_pop(struct heap *h) {
    if (!h) return NULL;

    long idx = 0;
    void *min_element = array_get(h->array, idx);

    array_set(h->array, idx, array_get(h->array, ARRAY_SIZE_MINUS_ONE(h->array)));
    array_pop(h->array);
    heapify(h, idx);

    return min_element;
}


void *prioq_pop(prioq *q) {
    return heap_pop(q);
}
