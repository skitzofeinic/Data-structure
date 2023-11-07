#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "stack.h"

#define NOT_FOUND -1
#define ERROR -2
#define VALID_MOVES 4
#define START 'S'
#define FINISH 'D'

/**
 * Searches if the index is in the visited array.
 * 
 * n: size of array visited
 * a: the array visited
 * i: index of the array
 * m: current index of the maze
 * 
 * Returns true if the array contains the current index, otherwise returns false. 
*/
bool is_visited (int n, int *a, int m) {
    for (int i = 0; i < n; i++) {
        if (a[i] == m) {
            return true; 
        }
    }

    return false;
}

/**
 * Looks for adjacent nodes for valid next move, if move is found and the node is -1(meaning empty)
 * the value will be put in node. Otherwise place a ',' as possible alternative route.
 * 
 * m: maze.
 * r: index of row.
 * c: index of column.
 * a: array of visited.
 * n: size of array of visited.
 * 
 * Returns:
 * If succesful, returns the node with index of next move, otherwise return -1.
*/
int node_search(struct maze *m, int r, int c, int *a, int n) {
    int node = -1;

    for (int i = 0; i < VALID_MOVES; i++) {
        int r_new = r + m_offsets[i][0];
        int c_new = c + m_offsets[i][1];
        int idx = maze_index(m, r_new, c_new);

        if (maze_valid_move(m, r_new, c_new) && !is_visited(n, a, idx) && maze_get(m, r_new, c_new) != WALL) {
            if (node == -1) {
                node = idx;
            } else {
                maze_set(m, r_new, c_new, TO_VISIT);
            }
        }
    }

    return node;
}


/**
 * Counts the length of the taken path. It searches the whole mazes and counts up each time it
 * encounters a path 'X'.
 * 
 * m: maze
 * n: size of maze
 * i: row index
 * j: column index
 * 
 * Returns:
 * the length of the path.
*/
int count_path(struct maze *m) {
    int path_len = 0;

    for (int i = 0; i < maze_size(m); i++) {
        for (int j = 0; j < maze_size(m); j++) {
            if (maze_get(m, i, j) == PATH) {
                path_len++;
            }
        }
    }
    return path_len;
}

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int dfs_solve(struct maze *m) {
    struct stack *s = stack_init(5000); 
    int arr_size = maze_size(m) * 10;
    int visited[arr_size]; 
    int r = 0, c = 0, idx = 0;

    maze_start(m, &r, &c);
    stack_push(s, maze_index(m, r, c)); 

    while (!stack_empty(s)) {
        if (idx > arr_size) {
            stack_cleanup(s); 
            return ERROR; 
        }
        
        c = maze_col(m, stack_peek(s)); 
        r = maze_row(m, stack_peek(s));

        if (maze_at_destination(m, r, c)) {
            stack_cleanup(s); 
            return count_path(m);
        }

        maze_set(m, r, c, PATH);

        visited[idx] = stack_peek(s);
        int next_move = node_search(m, r, c, visited, arr_size);

        if (next_move == -1) {
            maze_set(m, r, c, VISITED);
            stack_pop(s); 
        } else {
            stack_push(s, next_move);
        }
        
        idx++;
    }

    stack_cleanup(s); 
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
