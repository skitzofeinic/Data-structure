#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2
#define VALID_MOVES 4
#define SIZE 100000
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
 * looks for all adjacent nodes that are possible as next move and add it to the the queue.
 * 
 * m: maze.
 * r: index of row.
 * c: index of column.
 * a: array of visited.
 * n: size of array.
 * 
 * Returns:
 * adds all adjacent nodes into the queue
*/
void node_search(struct maze *m, int r, int c, int *a, int n, struct queue *q, int *p, int peek) {

    for (int i = 0; i < VALID_MOVES; i++) {
        int r_new = r + m_offsets[i][0];
        int c_new = c + m_offsets[i][1];
        int idx = maze_index(m, r_new, c_new);

        if (maze_valid_move(m, r_new, c_new) && !is_visited(n, a, idx)
            && maze_get(m, r_new, c_new) != WALL) {

            queue_push(q, maze_index(m, r_new, c_new));
            p[idx] = peek;
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
 * v: visited array
 * 
 * Returns:
 * Total length of the shortest path.
*/
int shortest_path(struct maze *m, int r, int c, int *p, int peek) {
    int len = 0;

    while (!maze_at_start(m, r, c)) {
        r = maze_row(m, peek);
        c = maze_col(m, peek);
        peek = p[peek];

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
    int visited[SIZE];
    int prev[SIZE];
    int r = 0, c = 0, idx = 0;

    for (size_t i = 0; i < SIZE; i++) {
        prev[i] = -1;
        visited[i] = -1;
    }
    
    maze_start(m, &r, &c);
    queue_push(q, maze_index(m, r, c));

    while (!queue_empty(q)) {
        if (idx > SIZE) {
            queue_cleanup(q);
            return ERROR;
        }

        int peek = queue_peek(q);
        r = maze_row(m, peek);
        c = maze_col(m, peek);

        visited[idx] = queue_pop(q);
        node_search(m, r, c, visited, SIZE, q, prev, peek);
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
