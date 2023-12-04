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
    int year;
};

static int parse_options(struct config *cfg, int argc, char *argv[]);

typedef struct {
    char *name;
    int age;
    int duration;
} patient_t;

static int compare_patient_name(const void *a, const void *b) {
    return strcmp(((const patient_t *) a)->name, ((const patient_t *) b)->name);
}

static int compare_patient_age(const void *a, const void *b) {
    return ((const patient_t *) a)->age - ((const patient_t *) b)->age;
}

static int compare_duration_iteration() {

}

static patient_t *patient_init(char *name, int age, int duration) {
    patient_t *p = malloc(sizeof(patient_t));
    if (!p) return NULL;

    p->name = malloc(strlen(name) + 1);
    if (!p->name) {
        free(p);
        return NULL;
    }

    strcpy(p->name, name);
    p->age = age;
    p->duration = duration;

    return p;
}

static void patient_cleanup(void *p) {
    if (!p) return;

    patient_t *patient = (patient_t *)p;
    free(patient->name);
    free(patient);
}

static int tokenize_input(char *input, char **name, int *age, int *duration) {
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

    token = strtok(NULL, " ");
    *duration = token ? atoi(token) : 0;

    return 0;
}

static void insert_patient_prioq(prioq *queue, int *prev_duration) {
    char *name_cpy;
    int age;
    int duration = 0;

    while (1) {
        char *s = fgets(buf, BUF_SIZE, stdin);

        if (!s) {
            if (feof(stdin)) break;

            fprintf(stderr, "Unexpected end of file. Exiting\n");
            prioq_cleanup(queue, patient_cleanup);
            exit(EXIT_FAILURE);
        }

        if (tokenize_input(buf, &name_cpy, &age, &duration)) {
            *prev_duration -= 1;
            break;
        };

        if ((duration += *prev_duration) < 0) duration = 0;

        patient_t *patient = patient_init(name_cpy, age, duration);
        if (!patient) {
            prioq_cleanup(queue, patient_cleanup);
            exit(EXIT_FAILURE);
        }

        free(name_cpy);
        prioq_insert(queue, patient);

    }
}

static void patient_print_and_free(prioq *queue, int *prev_duration) {
    patient_t *p = prioq_pop(queue);
    printf("name: %s\tage: %d\t\tduration: %d\t\n", p->name, p->age, p->duration);
    free(p->name);
    *prev_duration = p->duration;
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

    int prev_duration = 0;

    for (int iterations = 0;;) {
        insert_patient_prioq(queue, &prev_duration);

        if (prioq_size(queue) > 0) {
            patient_print_and_free(queue, &prev_duration);
        }

        printf(".\n");

        if (++iterations == MAX_ITERATIONS) {
            while (prioq_size(queue) > 0) {
                patient_print_and_free(queue, &prev_duration);
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
