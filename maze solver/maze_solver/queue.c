#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

struct queue {
    int front;
    int rear;
    size_t capacity;
    int *data;
    int pop_count;
    int push_count;
    size_t max_elem;
};

struct queue *queue_init(size_t capacity) {
    struct queue* q = malloc(sizeof(struct queue));
    if (q == NULL) {
        return NULL;
    }

    q->data = malloc(sizeof(int) * capacity);
    if (q->data == NULL) {
        free(q);
        return NULL;
    }

    q->capacity = capacity;
    q->front = -1;
    q->rear = -1;
    q->pop_count = 0;
    q->push_count = 0;
    q->max_elem = 0;

    return q;
}

void queue_cleanup(struct queue *q) {
    if (q == NULL) return;
    free(q->data);
    free(q);
}

void queue_stats(const struct queue *q) {
    if (q == NULL) return;
    fprintf(stderr, "stats %d %d %ld\n", q->push_count, q->pop_count, q->max_elem);
}

int queue_push(struct queue *q, int e) {
    //check if queue is full
    if ((q == NULL) || (q->rear + 1) % (int)q->capacity == q->front) {
        return 1;
    } else if (q->front == -1 && q->rear == -1) {
        q->front = 0;
        q->rear = 0;
        q->data[q->rear] = e;
    } else {
        q->rear = (q->rear + 1) % (int)q->capacity;
        q->data[q->rear] = e;
    }
    q->push_count++;

    if (queue_size(q) > q->max_elem) {
        q->max_elem = queue_size(q);
    }

    return 0;
}

int queue_pop(struct queue *q) {
    if (q == NULL || queue_empty(q)) return -1; 
    
    int item;
    if (q->front == q->rear) {
        item = q->data[q->front];
        q->front = -1;
        q->rear = -1;
    } else {
        item = q->data[q->front];
        q->front = (q->front + 1) % (int) q->capacity;
    }
    q->pop_count++;

    return item;
}

int queue_peek(const struct queue *q) {
    if (q == NULL || q->front == -1) return -1;
    return q->data[q->front];
}

int queue_empty(const struct queue *q) {
    if (q == NULL) {
        return -1;
    } else if (q->front == -1 && q->rear == -1) {
        return 1;
    } else {
        return 0;
    }
}

size_t queue_size(const struct queue *q) {
    if (q == NULL) return 0;

    if (q->rear >= q->front)
        return (size_t)(q->rear - q->front) + 1;

    return (size_t)(q->front - q->rear) + 1;
}
    