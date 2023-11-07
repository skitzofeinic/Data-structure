#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

struct stack {
    int top;
    size_t capacity;
    int *data;
    int pop_count;
    int push_count;
    size_t max_elem;
};

struct stack *stack_init(size_t capacity) {
    struct stack* s = malloc(sizeof(struct stack));
    if (s == NULL) {
        return NULL;
    }

    s->data = malloc(sizeof(int) * capacity);
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    s->capacity = capacity;
    s->top = -1;
    s->pop_count = 0;
    s->push_count = 0;

    return s;
}

void stack_cleanup(struct stack *s) {
    if (s == NULL) return;
    free(s->data);
    free(s);
}

void stack_stats(const struct stack *s) {
    if (s == NULL) return;
    fprintf(stderr, "stats: %d, %d, %ld\n", s->push_count, s->pop_count, s->max_elem);
}

int stack_push(struct stack *s, int c) {
    if (s == NULL || s->top == (int)s->capacity - 1) return 1;
    s->top++;
    s->data[s->top] = c;
    s->push_count++;

    if (stack_size(s) > s->max_elem) {
        s->max_elem = stack_size(s);
    }

    return 0;
}

int stack_pop(struct stack *s) {
    if (s == NULL || s->top == -1) return -1;
    int item = s->data[s->top];
    s->top--;
    s->pop_count++;

    return item;
}

int stack_peek(const struct stack *s) {
    if (s == NULL || s->top == -1) return -1;
    return s->data[s->top];
}

int stack_empty(const struct stack *s) {
    if (s == NULL) {
        return -1;
    } else if (s->top == -1) {
        return 1;
    } else {
        return 0;
    }
}

size_t stack_size(const struct stack *s) {
    if (s == NULL) return 1;
    return (size_t)s->top + 1;
}
