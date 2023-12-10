#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

struct tree {
    struct node *root;
    int turbo;
};

struct node {
    int data;
    struct node *lhs;
    struct node *rhs;
};

typedef struct node node;

static int global_node_counter = 0;

static node *make_node(int data) {
    node *n = malloc(sizeof(node));
    if (!n) return NULL;

    n->data = data;
    n->lhs = NULL;
    n->rhs = NULL;

    return n;
}

static int print_tree_dot_r(node *root, FILE *dotf) {
    int left_id, right_id, my_id = global_node_counter++;

    if (root == NULL) {
        fprintf(dotf, "    %d [shape=point];\n", my_id);
        return my_id;
    }

    fprintf(dotf, "    %d [color=%s label=\"%d\"]\n", my_id, "black", root->data);

    left_id = print_tree_dot_r(root->lhs, dotf);
    fprintf(dotf, "    %d -> %d [label=\"l\"]\n", my_id, left_id);

    right_id = print_tree_dot_r(root->rhs, dotf);
    fprintf(dotf, "    %d -> %d [label=\"r\"]\n", my_id, right_id);

    return my_id;
}

void tree_dot(const struct tree *tree, const char *filename) {
    node *root = tree->root;
    global_node_counter = 0;
    FILE *dotf = fopen(filename, "w");
    if (!dotf) {
        printf("error opening file: %s\n", filename);
        exit(1);
    }
    fprintf(dotf, "digraph {\n");
    if (root) {
        print_tree_dot_r(root, dotf);
    }
    fprintf(dotf, "}\n");
    fclose(dotf);
}

int tree_check(const struct tree *tree) {
    return 0;
}

struct tree *tree_init(int turbo) {
    struct tree *t = malloc(sizeof(struct tree)); 
    if (!t) return NULL;

    t->root = NULL;
    t->turbo = turbo;

    return t;
}

/**
 * Recursively find the position of the data in the tree.
 * Return the root at the position in the tree.
*/
static node *node_insert(node *root, int data) {
    if (!root) {
        node *n = make_node(data);
        return n ? n : NULL;
    }

    if (data < root->data) {
        root->lhs = node_insert(root->lhs, data);
    } 
    
    if (data > root->data) {
        root->rhs = node_insert(root->rhs, data);
    }

    return root;
}

int tree_insert(struct tree *tree, int data) {
    if (!tree) return -1;

    if (tree_find(tree, data)) return 1;

    tree->root = node_insert(tree->root, data);

    return tree->root ? 0 : -1;
}

/**
 * Recursively find the position of the data in the tree.
*/
static node *node_find(node *root, int data) {
    if (!root) return NULL;

    if (data < root->data) {
        return node_find(root->lhs, data);
    } 
    
    if (data > root->data) {
        return node_find(root->rhs, data);
    }

    return root;
}

int tree_find(struct tree *tree, int data) {
    if (!tree) return 0;
    return node_find(tree->root, data) ? 1 : 0;
}

static node *tree_find_smallest(node *root) {
    if (!root) return NULL;

    while (root->lhs) {
        root = root->lhs;
    }

    return root;
}

static node *remove_node(node *root, int data) {
    if (!root) return NULL;

    if (data < root->data) {
        root->lhs = remove_node(root->lhs, data);
    }
    
    if (data > root->data) {
        root->rhs = remove_node(root->rhs, data);
    } 
    
    if (data == root->data) {
        if (!root->lhs) {
            node *temp = root->rhs;
            free(root);
            return temp;
        }

        if (!root->rhs) {
            node *temp = root->lhs;
            free(root);
            return temp;
        }

        node *temp = tree_find_smallest(root->rhs);
        root->data = temp->data;
        root->rhs = remove_node(root->rhs, temp->data);
    }

    return root;
}

int tree_remove(struct tree *tree, int data) {
    if (!tree || !tree->root) return -1;

    if (!tree_find(tree, data)) return 1;

    tree->root = remove_node(tree->root, data);

    return 0;
}

static void in_order(const struct node *root) {
    if (root) {
        in_order(root->lhs);
        printf("%d\n", root->data);
        in_order(root->rhs);
    }
}

void tree_print(const struct tree *tree) {
    if (tree && tree->root) {
        in_order(tree->root);
    }
}

static void node_cleanup(node *root) {
    if (!root) return;

    node_cleanup(root->lhs);
    node_cleanup(root->rhs);

    free(root);
}

void tree_cleanup(struct tree *tree) {
    if (!tree) return;
    node_cleanup(tree->root);
    free(tree);
}
