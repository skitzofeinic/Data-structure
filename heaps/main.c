#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prioq.h"

#define BUF_SIZE 1024
#define MAX_ITERATIONS 10

static char buf[BUF_SIZE];

struct config {
    /* Set to 1 if -y is specified, 0 otherwise. */
    int year;
};

static int parse_options(struct config *cfg, int argc, char *argv[]);

typedef struct {
    char *name;
    int age;
} patient_t;

static int compare_patient_name(const void *a, const void *b) {
    return strcmp(((const patient_t *) a)->name, ((const patient_t *) b)->name);
}

static int compare_patient_age(const void *a, const void *b) {
    return ((const patient_t *) a)->age - ((const patient_t *) b)->age;
}

static patient_t *patient_init(char *name, int age) {
    patient_t *p = malloc(sizeof(patient_t));
    if (!p) return NULL;

    p->name = malloc(strlen(name) + 1);
    if (!p->name) {
        free(p);
        return NULL;
    }

    strcpy(p->name, name);
    p->age = age;

    return p;
}

static void patient_cleanup(void *p) {
    if (!p) return;

    patient_t *patient = (patient_t *)p;
    free(patient->name);
    free(patient);
}

static int tokenize_input(char *input, char **name, int *age) {
    char *token;

    token = strtok(input, " ");
    if (!token) return 1;

    *name = malloc(strlen(token) + 1);
    if (!*name) return 1;

    strcpy(*name, token);
    token = strtok(NULL, " ");
    if (!token) {
        free(*name);
        return 1;
    }
    
    *age = atoi(token);

    return 0;
}

static void append_patient_in_queue(prioq *queue) {
    char *name_cpy;
    int age;

    while (1) {
        char *s = fgets(buf, BUF_SIZE, stdin);

        if (!s) {
            if (feof(stdin)) break;

            fprintf(stderr, "Unexpected end of file. Exiting\n");
            prioq_cleanup(queue, patient_cleanup);
            exit(EXIT_FAILURE);
        }

        if (tokenize_input(buf, &name_cpy, &age)) break;

        patient_t *patient = patient_init(name_cpy, age);
        if (!patient) {
            prioq_cleanup(queue, patient_cleanup);
            exit(EXIT_FAILURE);
        }

        free(name_cpy);
        prioq_insert(queue, patient);
    }
}

static void patient_print_and_free(prioq *queue) {
    patient_t *p = prioq_pop(queue);
    printf("%s\n", p->name);
    free(p->name);
    free(p);
}

int main(int argc, char *argv[]) {
    prioq *queue;
    struct config cfg;

    if (parse_options(&cfg, argc, argv) != 0) return EXIT_FAILURE;

    if (cfg.year) {
        queue = prioq_init(&compare_patient_age);
    } else {
        queue = prioq_init(&compare_patient_name);
    }   

    for (int iterations = 0;;) {
        append_patient_in_queue(queue);

        if (prioq_size(queue) > 0) {
            patient_print_and_free(queue);
        }

        printf(".\n");

        if (++iterations == MAX_ITERATIONS) {
            while (prioq_size(queue) > 0) {
                patient_print_and_free(queue);
            }
            break;
        }

    }

    prioq_cleanup(queue, patient_cleanup);
    return EXIT_SUCCESS;
}

int parse_options(struct config *cfg, int argc, char *argv[]) {
    memset(cfg, 0, sizeof(struct config));
    int c;
    while ((c = getopt(argc, argv, "y")) != -1) {
        switch (c) {
        case 'y':
            cfg->year = 1;
            break;
        default:
            fprintf(stderr, "invalid option: -%c\n", optopt);
            return 1;
        }
    }
    return 0;
}