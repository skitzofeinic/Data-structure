#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2
#define VALID_MOVES 4
#define SIZE 5000

/**
 * looks for all adjacent nodes that are possible as next move and add it to the the queue.
 * 
 * m: maze.
 * r: index of row.
 * c: index of column.
 * q: queue stack
 * p: array of predecessor node
 * pred: index of predecessor node
 * 
 * Returns:
 * adds all adjacent nodes into the queue
*/
void node_search(struct maze *m, int r, int c, struct queue *q, int *p, int pred) {

    for (int i = 0; i < VALID_MOVES; i++) {
        int r_new = r + m_offsets[i][0];
        int c_new = c + m_offsets[i][1];
        int idx = maze_index(m, r_new, c_new);

        if (maze_valid_move(m, r_new, c_new) && maze_get(m, r_new, c_new) != VISITED
            && maze_get(m, r_new, c_new) != WALL) {

            queue_push(q, maze_index(m, r_new, c_new));
            p[idx] = pred;
            
            if (!maze_at_destination(m, r, c)) {
                maze_set(m, r_new, c_new, TO_VISIT);
            }    
        }
    }
}

/**
 * Looks for the shortest path. It starts at destination and regenates shortest path by looking at
 * the predecessor of the current node, while drawing it.
 * 
 * m: maze
 * r: row index
 * c: column index
 * p: prev array
 * pred: index of predecessor node
 * 
 * Returns:
 * Total length of the shortest path.
*/
int shortest_path(struct maze *m, int r, int c, int *p, int pred) {
    int len = 0;

    while (!maze_at_start(m, r, c)) {
        r = maze_row(m, pred);
        c = maze_col(m, pred);
        pred = p[pred];

        if (!maze_at_start(m, r, c)) {
            maze_set(m, r, c, PATH);
            len++;
        }
    }

    return len;
}


/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int bfs_solve(struct maze *m) {
    struct queue *q = queue_init(SIZE);
    size_t arr_size = (size_t) (maze_size(m) * maze_size(m));
    int prev[arr_size];
    int r = 0, c = 0, idx = 0;

    for (size_t i = 0; i < arr_size; i++) {
        prev[i] = -1;
    }
    
    maze_start(m, &r, &c);
    queue_push(q, maze_index(m, r, c));

    while (!queue_empty(q)) {
        if (idx > arr_size) {
            queue_cleanup(q);
            return ERROR;
        }

        int peek = queue_peek(q);
        r = maze_row(m, peek);
        c = maze_col(m, peek);

        queue_pop(q);
        node_search(m, r, c, q, prev, peek);
        maze_set(m, r, c, VISITED);

        if (maze_at_destination(m, r, c)) {
            queue_cleanup(q);
            return shortest_path(m, r, c, prev, peek);
        }

        idx++;
    }

    queue_cleanup(q);
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
    int path_length = bfs_solve(m);
    if (path_length == ERROR) {
        printf("bfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("bfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}
