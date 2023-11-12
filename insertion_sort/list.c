#include "list.h"

/*
 * TODO: A lot of code missing here. You will need to add implementations for
 * all the functions described in list.h here.
 *
 * Start by adding the definitions for the list and node structs. You may
 * implement any of the Linked List versions discussed in the lecture, which
 * have some different trade-offs for the functions you will need to write.
 *
 * Note: The function prototypes in list.h assume the most basic Singly Linked
 * List. If you implement some other version, you may not need all of the function
 * arguments for all of the described functions. This will produce a warning,
 * which you can suppress by adding a simple if-statement to check the value
 * of the unused parameter.
 *
 * Also, do not forget to add any required includes at the top of your file.
 */

struct node {
    int data;
    struct node *next;
    struct node *prev;
};

struct list {
    size_t size;
    struct node *head;
    struct node *tail;
};

struct list *list_init(void) {
    struct list *list = (struct list *)malloc(sizeof(struct list));

    if (list == NULL) return NULL;

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

struct node *list_new_node(int num) {
    struct node *node = (struct node *)malloc(sizeof(struct node));
    if (node == NULL)
        return NULL;

    node->data = num;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

struct node *list_head(const struct list *l) {
    if (l == NULL) return NULL;

    return l->head;
}

struct node *list_next(const struct node *n) {
    if (n == NULL) return NULL;

    return n->next;
}

int list_add_front(struct list *l, struct node *n) {
    if (l == NULL || n == NULL) return 1;

    if (l->head == NULL) {
        l->head = n;
        l->tail = n;
        n->next = NULL;
        n->prev = NULL;
    } else {
        n->next = l->head;
        n->prev = NULL;
        l->head->prev = n;
        l->head = n;
    }

    l->size++;
    return 0;
}

struct node *list_tail(const struct list *l) {
    if (l == NULL || l->head == NULL) return NULL;

    return l->tail;
}

struct node *list_prev(const struct list *l, const struct node *n) {
    if (l == NULL || n == NULL || l->head == n) return NULL;

    struct node *cur_node = l->head;
    while (cur_node != NULL && cur_node->next != n) {
        cur_node = cur_node->next;
    }

    return cur_node;
}

int list_add_back(struct list *l, struct node *n) {
    /* ... SOME CODE MISSING HERE ... */
    if (l == NULL || n == NULL) return 1;

    if (l->head == NULL) {
        l->head = n;
        l->tail = n;
    } else {
        n->prev = l->tail;
        l->tail->next = n;
        l->tail = n;
    }
    l->size++;

    return 0;
}

int list_node_get_value(const struct node *n) {
    return n->data;
}

int list_node_set_value(struct node *n, int value) {
    if (n == NULL) return 1;

    n->data = value;

    return 0;
}

int list_unlink_node(struct list *l, struct node *n) {
    if (l == NULL || n == NULL  || !list_node_present(l, n)) return 1;

    if (n->prev != NULL) {
        n->prev->next = n->next;
    } else {
        l->head = n->next;
    }

    if (n->next != NULL) {
        n->next->prev = n->prev;
    } else {
        l->tail = n->prev;
    }
    
    n->prev = NULL;
    n->next = NULL;
    l->size--;

    return 0;
}

void list_free_node(struct node *n) {
    free(n);
}

int list_cleanup(struct list *l) {
    if (l == NULL) return 1;

    struct node *cur_node = l->head;
    struct node *next;

    while (cur_node != NULL) {
        next = cur_node->next;
        list_free_node(cur_node);
        cur_node = next;
    }

    free(l);

    return 0;
}

int list_node_present(const struct list *l, const struct node *n) {
    if (l == NULL || n == NULL) return -1;

    struct node *cur_node = l->head;
    while (cur_node != NULL) {
        if (cur_node == n) return 1;

        cur_node = cur_node->next;
    }
    
    return 0;
}

int list_insert_after(struct list *l, struct node *n, struct node *m) {
    if (l == NULL || n == NULL || m == NULL || list_node_present(l, n)) return 1;

    struct node *node_m = l->head;
    while (node_m != NULL && node_m != m) {
        node_m = node_m->next;
    }

    if (node_m == NULL) return 1;

    n->next = node_m->next;
    n->prev = node_m;

    if (node_m->next != NULL) {
        node_m->next->prev = n;
    } else {
        l->tail = n;
    }

    node_m->next = n;
    l->size++;

    return 0;
}

int list_insert_before(struct list *l, struct node *n, struct node *m) {
    if (l == NULL || n == NULL || m == NULL) return 1;

    struct node *node_m = l->head;
    while (node_m != NULL && node_m != m) {
        node_m = node_m->next;
    }

    if (node_m == NULL) return 1;

    n->next = node_m;
    n->prev = node_m->prev;

    if (node_m->prev != NULL) {
        node_m->prev->next = n;
    } else {
        l->head = n;
    }

    node_m->prev = n;
    l->size++;

    return 0;
}

size_t list_length(const struct list *l) {
    if (l == NULL) return 0;

    return l->size;
}

struct node *list_get_ith(const struct list *l, size_t i) {
    if (l == NULL || i >= l->size) return NULL;

    struct node *cur_node = l->head;
    for (size_t j = 0; j < i; j++) {
        if (cur_node == NULL) return NULL;
        cur_node = cur_node->next;
    }
    
    return cur_node;
}

struct list *list_cut_after(struct list *l, struct node *n) {
    if (l == NULL || n == NULL || n->next == NULL) return NULL;

    struct list *second_half = (struct list *)malloc(sizeof(struct list));
    if (second_half == NULL) return NULL;

    second_half->size = 0;
    second_half->head = n->next;
    second_half->tail = l->tail;

    l->size -= second_half->size;
    l->tail = n;
    n->next = NULL;

    return second_half;
}
