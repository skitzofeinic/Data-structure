#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "list.h"
#define BUF_SIZE 1024

char buf[BUF_SIZE];

struct config {
    /* You can ignore these options until you implement the
       extra command-line arguments. */

    /* Set to 1 if -d is specified, 0 otherwise. */
    int descending_order;

    // Set to 1 if -c is specified, 0 otherwise.
    int combine;

    // Set to 1 if -o is specified, 0 otherwise.
    int remove_odd;

    /* Set to 1 if -z is specified, 0 otherwise. */
    int zip_alternating;
};

/**
 * Sort the list using insertion sort
 * order:   0 = ascending,
 *          1 = descending. 
*/
void insertion_sort(struct list *l, bool order) {
    if (l == NULL) return;
    
    struct node *cur_node = list_head(l);
    while (cur_node != NULL) {
        struct node *next = list_next(cur_node);

        while (next != NULL) {
            if (order == 1 && list_node_get_value(next) > list_node_get_value(cur_node)) {
                int temp = list_node_get_value(cur_node);
                list_node_set_value(cur_node, list_node_get_value(next));
                list_node_set_value(next, temp);
            }
            if (order == 0 && list_node_get_value(next) < list_node_get_value(cur_node)) {
                int temp = list_node_get_value(cur_node);
                list_node_set_value(cur_node, list_node_get_value(next));
                list_node_set_value(next, temp);
            }

            next = list_next(next);
        }

        cur_node = list_next(cur_node);
    }
}

/**
 * add two adjacent nodes up to each other in the list. Start at the tail. 
 * If the next is null you dont add up but just leave it like it is.
*/
void combine(struct list *l) {
    if (l == NULL) return;

    struct node *cur_node = list_tail(l);

    while (list_prev(l, cur_node) != NULL) {
        struct node *prev = list_prev(l, cur_node);

        int combined_value = list_node_get_value(cur_node) + list_node_get_value(prev);

        struct node *new_node = list_new_node(combined_value);
        if (new_node == NULL) return;

        struct node *prev_of_prev = list_prev(l, prev);

        list_unlink_node(l, cur_node);
        list_unlink_node(l, prev);

        if (prev_of_prev == NULL) {
            list_add_front(l, new_node);
        } else {
            list_insert_after(l, new_node, prev_of_prev);
        }

        list_free_node(cur_node);
        list_free_node(prev);
        cur_node = prev_of_prev;
    }
}

/**
 * removes all odd numbers from the sorted list.
*/
void remove_odd(struct list *l) {
    if (l == NULL) return;

    struct node *cur_node = list_head(l);
    struct node *prev_node = NULL;

    while (cur_node != NULL) {
        if (list_node_get_value(cur_node) % 2 != 0) {
            struct node *next_node = list_next(cur_node);            
            list_unlink_node(l, cur_node);
            list_free_node(cur_node);
            cur_node = next_node;
            if (prev_node != NULL) {
                cur_node = list_next(prev_node);
            } else {
                cur_node = list_head(l);
            }
        } else {
            prev_node = cur_node;
            cur_node = list_next(cur_node);
        }
    }
}

/**
 * Cuts list in half. starts alternating between list and inserting it in the first list.
*/
void zip_alt(struct list *l) {
}

/**
 * Prints the list
*/
void print_list(struct list *l) {
    struct node *cur_node = list_head(l);
    while (cur_node != NULL) {
        printf("%d\n", list_node_get_value(cur_node));
        cur_node = list_next(cur_node);
    }
}

int parse_options(struct config *cfg, int argc, char *argv[]) {
    memset(cfg, 0, sizeof(struct config));
    int c;
    while ((c = getopt(argc, argv, "dcoz")) != -1) {
        switch (c) {
        case 'd':
            cfg->descending_order = 1;
            break;
        case 'c':
            cfg->combine = 1;
            break;
        case 'o':
            cfg->remove_odd = 1;
            break;
        case 'z':
            cfg->zip_alternating = 1;
            break;
        default:
            fprintf(stderr, "invalid option: -%c\n", optopt);
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct config cfg;
    if (parse_options(&cfg, argc, argv) != 0) {
        return 1;
    }

    // if (fgets(buf, BUF_SIZE, stdin) == NULL) {
    // printf("No input or a#include <ctype.h>n error occurred while reading input.\n");
    // return 1;
    // }

    struct list *list = list_init();
    if (list == NULL) return 1;

    while (fgets(buf, BUF_SIZE, stdin)) {
        if (buf[strlen(buf) - 1] != '\n') {
            buf[strlen(buf)] = '\n';
            buf[strlen(buf) + 1] = '\0';
        }

        char *token = strtok(buf, " \t\n");
        while (token != NULL) {
            if (!isdigit((unsigned char)*token)) {
                token = strtok(NULL, " \t\n");
                continue;
            }
            int num = atoi(token);

            struct node *node = list_new_node(num);
            if (node == NULL) {
                list_cleanup(list);
                return 1;
            }

            list_add_back(list, node);
            token = strtok(NULL, " \t\n");
        }
    }

    insertion_sort(list, cfg.descending_order);
    if (cfg.combine) combine(list);
    if (cfg.remove_odd) remove_odd(list);
    if (cfg.zip_alternating) zip_alt(list);

    print_list(list);
    list_cleanup(list);

    return 0;
}
