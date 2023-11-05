#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "stack.h"

#define NOT_FOUND -1
#define ERROR -2

int node_found(struct maze *m, int r, int c, int *visited, int arr_size) {

    bool is_visited = false;

    int left = maze_index(m, r + 1, c);
    int right = maze_index(m, r - 1, c);
    int up = maze_index(m, r, c + 1);
    int down = maze_index(m, r, c + 1);

    int to_check[] = {left, right, up, down};
    int to_check_size = 4;

    for (int i = 0; i < arr_size; i++) {
        for (int j = 0; j < to_check_size; j++) {
            if (visited[i] == to_check[j]) {
                is_visited = true;
            }
        }
    }

    if (!is_visited) {
        if (maze_valid_move(m, r + 1, c)) {
            return left;
        }
        if (maze_valid_move(m, r - 1, c)) {
            return right;
        }
        if (maze_valid_move(m, r, c + 1)) {
            return up;
        }
        if (maze_valid_move(m, r, c - 1)) {
            return down;
        }
    }
    
    return -1;
}

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int dfs_solve(struct maze *m) {
    struct stack *stack = stack_init(5000); 

    int arr_size = maze_size(m) * 2;
    int visited[arr_size]; 

    int r = 0, c = 0, steps = 0; 

    maze_start(m, &r, &c);
    stack_push(stack, maze_index(m, r, c)); 

    while (!stack_empty(stack)) {
        if (steps > arr_size) {
            stack_cleanup(stack); 
            return NOT_FOUND; 
        }
        
        c = maze_col(m, stack_peek(stack)); 
        r = maze_row(m, stack_peek(stack));
        if (maze_at_destination(m, r, c)) {
            stack_cleanup(stack); 
            return steps; 
        }
        maze_set(m, r, c, VISITED);

        visited[steps] = stack_peek(stack);
        int next_move = node_found(m, c, r, visited, arr_size);
        if (next_move == -1) {
            stack_pop(stack); 
        } else {
            stack_push(stack, next_move); 
        }
        steps++; 
    }
    stack_cleanup(stack); 
    return NOT_FOUND; 
}

int main(void) {
    /* read maze */
    struct maze *m = maze_read();
    if (!m) {
        printf("Error reading maze\n");
        return 1;
    }

    /* solve maze */
    int path_length = dfs_solve(m);
    if (path_length == ERROR) {
        printf("dfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("dfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}
