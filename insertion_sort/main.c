/**
 * Name: Nguyen Anh Le
 * studentID: 15000370
 * BsC Informatica
 * 
 * Implementation of insertion sort using the list.c interface. depending on the input of the user
 * you can sort in descending or ascending order, remove odd numbers, combine adjacent integer values,
 * or split the linked list and zip them together by alternating between the lists and adding each
 * node into the original list.
*/

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
 * Prints the list
*/
void list_print(struct list *l) {
    struct node *cur_node = list_head(l);
    while (cur_node != NULL) {
        printf("%d\n", list_node_get_value(cur_node));
        cur_node = list_next(cur_node);
    }
}

/**
 * Sort the list using insertion sort.
 * order:   0 = ascending,
 *          1 = descending. 
*/
void insertion_sort(struct list *l, bool order) {
    if (l == NULL) return;

    struct node *cur_node = list_head(l);
    while (cur_node != NULL) {
        struct node *next = list_next(cur_node);
        struct node *prev = list_prev(l, cur_node);

        while (prev != NULL && 
              ((order == 0 && list_node_get_value(prev) > list_node_get_value(cur_node)) ||
               (order == 1 && list_node_get_value(prev) < list_node_get_value(cur_node)))) {
            
            int temp = list_node_get_value(cur_node);
            list_node_set_value(cur_node, list_node_get_value(prev));
            list_node_set_value(prev, temp);

            cur_node = prev;
            prev = list_prev(l, cur_node);
        }

        cur_node = next;
    }
}


/**
 * add two adjacent nodes up to each other in the list. Start at the tail. 
 * If the next is null you dont add up but just leave it like it is.
*/
void list_combine(struct list *l) {
    if (l == NULL) return;

    struct node *cur_node = list_head(l);

    while (list_next(cur_node) != NULL) {
        struct node *next = list_next(cur_node);
        int combined_value = list_node_get_value(cur_node) + list_node_get_value(next);
        struct node *new_node = list_new_node(combined_value);
        struct node *next_next = list_next(next);

        list_unlink_node(l, cur_node);
        list_unlink_node(l, next);

        if (next_next == NULL) {
            list_add_back(l, new_node);
        } else {
            list_insert_before(l, new_node, next_next);
        }

        list_free_node(cur_node);
        list_free_node(next);
        cur_node = next_next;
    }
}


/**
 * removes all odd numbers from the sorted list.
*/
void list_remove_odd(struct list *l) {
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
void list_zip_alt(struct list *l) {
    if (l == NULL || list_length(l) <= 2) return;

    size_t length = (list_length(l) - 1) / 2;
    struct node *mid_idx = list_get_ith(l, length);
    struct list *second_half = list_cut_after(l, mid_idx);
    if (second_half == NULL) return;

    struct node *node_first_half = list_head(l);
    struct node *node_second_half = list_head(second_half);

    while (node_first_half != NULL) {
        struct node *temp_next_second = list_next(node_second_half);
        struct node *temp_next_first = list_next(node_first_half);

        list_unlink_node(second_half, node_second_half);
        list_insert_after(l, node_second_half, node_first_half);

        node_second_half = temp_next_second;
        node_first_half = temp_next_first;
    }
    printf("h: %d, t: %d\n", list_node_get_value(list_head(second_half)), list_node_get_value(list_head(l)));
    free(second_half);
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
/**
 * Reads stdin, converts all found characters that are numbers into integers, and
 * adds them to the list by putting the value into the node and appending that node to the list.
 */
void list_append_node (struct list *l) {
    while (fgets(buf, BUF_SIZE, stdin)) {
        if (buf[strlen(buf) - 1] != '\n') {
            buf[strlen(buf)] = '\n';
            buf[strlen(buf) + 1] = '\0';
        }

        char *token = strtok(buf, " \t\n");
        while (token != NULL) {
            if (!isdigit(*token)) {
                token = strtok(NULL, " \t\n");
                continue;
            }
            
            int num = atoi(token);

            struct node *node = list_new_node(num);
            if (node == NULL) {
                list_cleanup(l);
                return;
            }

            list_add_back(l, node);
            token = strtok(NULL, " \t\n");
        }
    }
}

int main(int argc, char *argv[]) {
    struct config cfg;
    if (parse_options(&cfg, argc, argv) != 0) {
        return 1;
    }

    struct list *list = list_init();
    if (list == NULL) return 1;
    
    list_append_node(list);
    insertion_sort(list, cfg.descending_order);
    
    if (cfg.combine) {
        list_combine(list);
    }

    if (cfg.remove_odd) {
        list_remove_odd(list);
    }

    if (cfg.zip_alternating) {
        list_zip_alt(list);
    }

    list_print(list);
    list_cleanup(list);

    return 0;
}
