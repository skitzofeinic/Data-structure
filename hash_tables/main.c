/**
 * Author: Nguyen Anh Le
 * StudentID: 15000370
 * BSc Informatica
 * 
 * The program utilizes a dynamic hash table with configurable parameters such as
 * initial size, maximum load factor, and hash function. It includes functions for
 * table initialization, insertion, lookup, deletion, resizing, and cleanup. The hash
 * table dynamically adjusts its size to maintain an efficient load factor and utilizes
 * a hash function specified during initialization.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "array.h"
#include "hash_func.h"
#include "hash_table.h"

#define LINE_LENGTH 256
#define TABLE_START_SIZE 256
#define MAX_LOAD_FACTOR 1
#define HASH_FUNCTION fnv1a
#define START_TESTS 5
#define MAX_TESTS 5
#define HASH_TESTS 5

/* Replace every non-ascii char with a space and lowercase every char. */
static void cleanup_string(char *line) {
    for (char *c = line; *c != '\0'; c++) {
        *c = (char)tolower(*c);
        if (!isalpha(*c)) {
            *c = ' ';
        }
    }
}

/* removes suffixes from the input by terminating the string when encountering empty space. */
static void remove_suffix(char *line) {
    cleanup_string(line);
    char *space = strchr(line, ' ');
    if (space) *space = '\0';
}

/* Creates a hash table with a word index for the specified file and
 * parameters. Return a pointer to the hash table or NULL if an error occurred.
 */
static struct table *create_from_file(char *filename,
                                      unsigned long start_size,
                                      double max_load,
                                      unsigned long (*hash_func)(const unsigned char *)) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    char *line = malloc(LINE_LENGTH * sizeof(char));
    if (!line) {
        fclose(fp);
        return NULL;
    }

    struct table *ht = table_init(start_size, max_load, hash_func);
    if (!ht) {
        fclose(fp);
        free(line);
        return NULL;
    }

    int line_num = 0;

    while (fgets(line, LINE_LENGTH, fp)) {
        cleanup_string(line);
        char *token = strtok(line, " \t\n");
        line_num++;
        
        while (token) {
            if (table_insert(ht, token, line_num)) {
                fclose(fp);
                free(line);
                table_cleanup(ht);
                return NULL;
            }

            token = strtok(NULL, " \t\n");
        }
    }

    fclose(fp);
    free(line);
    return ht;
}

/* Reads words from stdin and prints line lookup results per word.
 * Return 0 if successful and 1 on failure. */
static int stdin_lookup(struct table *hash_table) {
    char *line = malloc(LINE_LENGTH * sizeof(char));
    if (!line) return 1;

    while (fgets(line, LINE_LENGTH, stdin)) {
        remove_suffix(line);
        char *token = strtok(line, " \t\n");

        while (token) {
            struct array *value = table_lookup(hash_table, token);
            printf("%s\n", token);

            if (value) {
                for (size_t i = 0; i < array_size(value); ++i) {
                    printf("* %d\n", array_get(value, i));
                }
            }

            printf("\n");
            token = strtok(NULL, " \t\n");
        }
    }

    free(line);
    return 0;
}

static void timed_construction(char *filename) {
    unsigned long start_sizes[START_TESTS] = {2, 512, 4096, 32768, 65536};
    double max_loads[MAX_TESTS] = {0.2, 1.0, 3.0, 5.2, 10.0};
    unsigned long (*hash_funcs[HASH_TESTS])(const unsigned char *) = {
        hash_too_simple,
        jenkins_one_at_a_time_hash,
        murmur3_32,
        djb2,
        fnv1a,
    };

    for (int i = 0; i < START_TESTS; i++) {
        for (int j = 0; j < MAX_TESTS; j++) {
            for (int k = 0; k < HASH_TESTS; k++) {
                clock_t start = clock();
                struct table *hash_table =
                    create_from_file(filename, start_sizes[i], max_loads[j], hash_funcs[k]);
                clock_t end = clock();

                printf("Start: %ld\tMax: %.1f\tHash: %d\t -> Time: %ld microsecs\n",
                       start_sizes[i], max_loads[j], k, end - start);
                table_cleanup(hash_table);
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s text_file [-t]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 3 && !strcmp(argv[2], "-t")) {
        timed_construction(argv[1]);
    } else {
        struct table *hash_table =
            create_from_file(argv[1], TABLE_START_SIZE, MAX_LOAD_FACTOR, HASH_FUNCTION);
        if (hash_table == NULL) {
            printf("An error occurred creating the hash table, exiting..\n");
            return EXIT_FAILURE;
        }
        if (stdin_lookup(hash_table) != 0) {
            table_cleanup(hash_table);
            return EXIT_FAILURE;
        }
        table_cleanup(hash_table);
    }

    return EXIT_SUCCESS;
}
