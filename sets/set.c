/**
 * Name: Nguyen Anh Le
 * studentID: 15000370
 * BSc Informatica
 * 
 * Defines structures and functions for creating, manipulating, and cleaning up a set.
 * The set utilizes a BST to store and manage unique integer values.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"
#include "tree.h"

struct set {
    struct tree *tree;
};

struct set *set_init(int turbo) {
    struct set *s = malloc(sizeof(struct set));
    if (!s) return NULL;

    s->tree = tree_init(turbo);
    if (!s->tree) {
        free(s);
        return NULL;
    }

    return s;
}

int set_insert(struct set *s, int num) {
    return (!s) ? -1 : tree_insert(s->tree, num);
}

int set_find(struct set *s, int num) {
    return (!s) ? 0 : tree_find(s->tree, num);
}

int set_remove(struct set *s, int num) {
    return (!s) ? 1 : tree_remove(s->tree, num);
}

void set_cleanup(struct set *s) {
    if (!s) return;
    tree_cleanup(s->tree);
    free(s);
}

void set_print(const struct set *s) {
    if (!s) return;
    tree_print(s->tree);
}

int set_verify(const struct set *s) {
    return (!s) ? 0 : tree_check(s->tree);
}
