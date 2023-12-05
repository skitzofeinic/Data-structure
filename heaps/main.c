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
int elapsed = 0;

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

static void patient_cleanup(void *p) {
    if (!p) return;

    patient_t *patient = (patient_t *)p;
    free(patient->name);
    free(patient);
}

int main(int argc, char *argv[]) {
    char *token, *name_cpy;
    int age, duration;
    prioq *queue, *being_treated;
    struct config cfg;

    if (parse_options(&cfg, argc, argv) != 0) return EXIT_FAILURE;

    if (cfg.year) {
        queue = prioq_init(&compare_patient_age);
        being_treated = prioq_init(&compare_patient_age);
    } else {
        queue = prioq_init(&compare_patient_name);
        being_treated = prioq_init(&compare_patient_name);
    }   

    for (int iterations = 0;;) {
        ++elapsed;

        while (1) {
            char *s = fgets(buf, BUF_SIZE, stdin);

            if (!s) {
                if (feof(stdin)) break;

                fprintf(stderr, "Unexpected end of file. Exiting\n");
                prioq_cleanup(queue, patient_cleanup);
                prioq_cleanup(being_treated, patient_cleanup);
                return EXIT_FAILURE;
            }

            token = strtok(buf, " ");
            if (!token) break;

            name_cpy = malloc(strlen(token) + 1);
            if (!name_cpy) break;
            strcpy(name_cpy, token);

            token = strtok(NULL, " ");
            if (!token) {
                free(name_cpy);
                break;
            }
            age = atoi(token);

            token = strtok(NULL, " ");
            duration = token ? atoi(token) : -1;

            patient_t *patient = malloc(sizeof(patient_t));
            if (!patient) {
                prioq_cleanup(queue, patient_cleanup);
                prioq_cleanup(being_treated, patient_cleanup);
                return EXIT_FAILURE;
            }

            patient->name = malloc(strlen(name_cpy) + 1);
            if (!patient->name) {
                free(patient);
                return EXIT_FAILURE;
            }

            strcpy(patient->name, name_cpy);
            patient->age = age;
            patient->duration = duration;

            free(name_cpy);
            prioq_insert(queue, patient);
        }

        patient_t *p = NULL;
        if (prioq_size(being_treated)) {
            p = prioq_pop(being_treated);
        } else if (prioq_size(queue)) {
            p = prioq_pop(queue);
        }

        if (p) {
            if ( p->duration == -1 || p->duration == elapsed) {
                printf("%s\n",p->name);
                patient_cleanup(p);
                elapsed = 0;
            } else {
                prioq_insert(being_treated, p);
            }
        }
        
        printf(".\n");

        if (++iterations == MAX_ITERATIONS) {
                while (prioq_size(being_treated) > 0) {
                patient_t *p = prioq_pop(being_treated);
                // printf("PRINTING: %-10s\t age: %-3d\t dur: %-3d\t ela: %-3d\tit: %-3d\n", p->name, p->age, p->duration, elapsed, iterations);
                printf("%s\n",p->name);
                patient_cleanup(p);
                ++elapsed;
            }
            while (prioq_size(queue) > 0) {
                patient_t *p = prioq_pop(queue);
                // printf("PRINTING: %-10s\t age: %-3d\t dur: %-3d\t ela: %-3d\tit: %-3d\n", p->name, p->age, p->duration, elapsed, iterations);
                printf("%s\n",p->name);
                patient_cleanup(p);
                ++elapsed;
            }
            break;
        }
    }

    prioq_cleanup(queue, patient_cleanup);
    prioq_cleanup(being_treated, patient_cleanup);
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