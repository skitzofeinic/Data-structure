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


patient_t *patient_init(const char *name, int age, int duration) {
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

    free(((patient_t *)p)->name);
    free((patient_t *)p);
}

static int clean_and_exit(prioq *queue, prioq *being_treated, int status) {
    prioq_cleanup(queue, patient_cleanup);
    prioq_cleanup(being_treated, patient_cleanup);

    return status;
}

static int compare_patient_name(const void *a, const void *b) {
    return strcmp(((const patient_t *) a)->name, ((const patient_t *) b)->name);
}

static int compare_patient_age(const void *a, const void *b) {
    return ((const patient_t *) a)->age - ((const patient_t *) b)->age;
}

static int tokenize_input(char **name, int *age, int *duration) {
    char *token = strtok(buf, " ");
    if (!token) return EXIT_FAILURE;

    *name = malloc(strlen(token) + 1);
    if (!*name) return EXIT_FAILURE;

    strcpy(*name, token);

    token = strtok(NULL, " ");
    if (!token) {
        free(*name);
        return EXIT_FAILURE;
    }

    *age = atoi(token);
    token = strtok(NULL, " ");
    *duration = token ? atoi(token) : -1;

    return EXIT_SUCCESS;
}

static void process_patient(prioq *queue, prioq *being_treated) {
    patient_t *p =  (prioq_size(being_treated) > 0) ? prioq_pop(being_treated) : 
                    (prioq_size(queue) > 0)         ? prioq_pop(queue) : NULL;
    if (!p) return;
    
    if (!(p->duration == -1 || p->duration == elapsed)) {
        prioq_insert(being_treated, p);
        return;
    }

    printf("%s\n", p->name);
    patient_cleanup(p);
    elapsed = 0;
}

static void send_remaining_home(prioq *queue) {
    while (prioq_size(queue) > 0) {
        patient_t *p = prioq_pop(queue);
        printf("%s\n",p->name);
        patient_cleanup(p);
        ++elapsed;
    }
}

static void insert_patient_prioq(prioq *queue, prioq *being_treated) {
    char *name_cpy;
    int age = 0, duration = 0;

    while (1) {
        char *s = fgets(buf, BUF_SIZE, stdin);
        if (!s) {
            if (feof(stdin)) break;
            clean_and_exit(queue, being_treated, EXIT_FAILURE);
        }

        if (tokenize_input(&name_cpy, &age, &duration)) break;
        
        patient_t *p = patient_init(name_cpy, age, duration);
        if (!p) {
            clean_and_exit(queue, being_treated, EXIT_FAILURE);
        }

        free(name_cpy);
        prioq_insert(queue, p);
    }
}

int main(int argc, char *argv[]) {
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
        insert_patient_prioq(queue, being_treated);
        process_patient(queue, being_treated);
        printf(".\n");

        if (++iterations == MAX_ITERATIONS) {
            send_remaining_home(being_treated);
            send_remaining_home(queue);
            break;
        }
    }

    clean_and_exit(queue, being_treated, EXIT_SUCCESS);
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