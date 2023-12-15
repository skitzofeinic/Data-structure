/**
 * Name: Nguyen Anh Le
 * studentID: 15000370
 * BSc Informatica
 * 
 * This file contains declarations for structures and functions related to
 * the creation, manipulation, and cleanup of a Binary Search Tree (BST).
 * The BST consists of nodes, each containing an integer data value, a left
 * child, and a right child.
*/

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
    int height;
};
typedef struct node node;

static int global_node_counter = 0;

/* Return a pointer to a new node. Return NULL on failure. */
static node *make_node(int data) {
    node *n = malloc(sizeof(node));
    if (!n) return NULL;

    n->data = data;
    n->lhs = NULL;
    n->rhs = NULL;
    n->height = 1;

    return n;
}

static int print_tree_dot_r(node *root, FILE *dotf) {
    int left_id, right_id, my_id = global_node_counter++;

    if (!root) {
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

/* Check properties of the BST and balancing properties if turbo is
 * set. Return 0 if the checks pass, return an error code otherwise.
 */
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

/* Returns the maximum of two integers. */
static int max(int a, int b) {
    return (a > b) ? a : b;
}

/* Returns the height of a node in the AVL tree. */
static int height(node *n) {
    return (!n) ? 0 : n->height;
}

/* Calculates the balance factor of a node in the AVL tree. */
static int balance_factor(node *n) {
    return (!n) ? 0 : height(n->lhs) - height(n->rhs);
}

/* Updates the height of a node in the AVL tree. */
static void height_update(node *n) {
    if (!n) return;
    
    n->height = 1 + max(height(n->lhs), height(n->rhs));
}

/**
 * Places a child node in the specified direction relative to its parent.
 * Returns pointer to the new root node.
*/
static node *node_place(node *parent, node *child, int direction) {
    if (direction == 1) {
        node *temp = child->rhs;
        child->rhs = parent;
        parent->lhs = temp;
    }

    if (direction == -1) {
        node *temp = child->lhs;
        child->lhs = parent;
        parent->rhs = temp;
    }

    height_update(parent);
    height_update(child);

    return child;
}

/**
 * Checks balance factor and performs rotations as needed.
 * It is used during the insertion of a node.
 * Returns pointer to the new root after balancing.
*/
static node *avl_balance(node *root, int data) {
    const int left_weighted = 1;
    const int right_weighted = -1;

    height_update(root);

    int balance = balance_factor(root);

    if (balance > left_weighted && root->lhs) {
        if (data > root->lhs->data) {
            root->lhs = node_place(root->lhs, root->lhs->rhs, right_weighted);
        }
        
        return node_place(root, root->lhs, left_weighted);
    }

    if (balance < right_weighted && root->rhs) {
        if (data < root->rhs->data) {
            root->rhs = node_place(root->rhs, root->rhs->lhs, left_weighted);
        }
        
        return node_place(root, root->rhs, right_weighted);
    }

    height_update(root);

    return root;
}

/**
 * Inserts a new node into the binary search tree.
 * If turbo mode is enabled, it also performs AVL balancing after insertion.
 * Return pointer to the root of the updated tree after insertion.
*/
static node *node_insert(node *root, int data, int turbo) {
    if (!root) {
        return make_node(data);
    }

    if (data < root->data) {
        root->lhs = node_insert(root->lhs, data, turbo);
    }
    
    if (data > root->data) {
        root->rhs = node_insert(root->rhs, data, turbo);
    }

    if (turbo) {
        return avl_balance(root, data);
    }

    return root;
}

int tree_insert(struct tree *tree, int data) {
    if (!tree) return -1;

    if (tree_find(tree, data)) return 1;

    tree->root = node_insert(tree->root, data, tree->turbo);

    return tree->root ? 0 : -1;
}

/* Recursively find the position of the data in the tree. */
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

/**
 * Finds and returns the smallest node in the given binary search tree.
 * Traverses the left child pointers until the leftmost node is reached.
*/
static node *tree_find_smallest(node *root) {
    if (!root) return NULL;

    while (root->lhs) {
        root = root->lhs;
    }

    return root;
}

/**
 * Removes a node with the specified data from the binary search tree rooted at root.
 * If the data is not found, the tree remains unchanged.
 */
static node *node_remove(node *root, int data) {
    if (!root) return NULL;

    if (data < root->data) {
        root->lhs = node_remove(root->lhs, data);
    }
    
    if (data > root->data) {
        root->rhs = node_remove(root->rhs, data);
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
        root->rhs = node_remove(root->rhs, temp->data);
    }

    return root;
}

int tree_remove(struct tree *tree, int data) {
    if (!tree || !tree->root) return -1;

    if (!tree_find(tree, data)) return 1;

    tree->root = node_remove(tree->root, data);

    return 0;
}

/* Performs in-order traversal of the binary search tree rooted at root. */
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

/**
 * Recursively frees memory by cleaning up nodes
 * in the binary search tree rooted at root.
 */
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
