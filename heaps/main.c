/**
 * Name: Nguyen Anh Le
 * studentID: 15000370
 * BSc informatica
 * 
 * The program organizes patients in a priority queue based on names or age.
 * It treats them in sessions until completion. 
 * After ten iterations, it prints remaining patients.
*/

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prioq.h"

#define BUF_SIZE 1024
#define MAX_ITERATIONS 10
#define FILE_FAILURE 1
#define PATIENT_FAILURE 2

static char buf[BUF_SIZE];
static int elapsed_time = 0;

struct config {
    int year;
};

static int parse_options(struct config *cfg, int argc, char *argv[]);

typedef struct {
    char *name;
    int age;
    int duration;
} patient_t;

/**
 * Creates a new patient and returns a pointer to it. 
 * returns NULL on failure.
*/
static patient_t *patient_init(const char *name, int age, int duration) {
    if (!*name) return NULL;

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

/* Free the memory allocated for patient */
static void patient_cleanup(void *p) {
    if (!p) return;

    free(((patient_t *)p)->name);
    free((patient_t *)p);
}

/**
 * Cleans up priority queues and returns exit status
 * 0: Sucess.
 * 1: Unexpected end of file.
 * 2: Error intitalizing patient.
*/
static int clean_and_exit(prioq *queue, prioq *in_session, int status) {
    switch (status) {
        case FILE_FAILURE:
            fprintf(stderr, "Unexpected end of file. exiting\n");
            break;
        case PATIENT_FAILURE:
            fprintf(stderr, "Failed while initializing patient. exiting\n");
            break;
        default:
            break;
    }

    prioq_cleanup(queue, patient_cleanup);
    prioq_cleanup(in_session, patient_cleanup);

    return status;
}

/**
 * Compares names patient a and patient b.
 * Negative return: a is lexicographically less
 * Zero return: equal
 * Positive return: b is lexicographically less
*/
static int compare_patient_name(const void *a, const void *b) {
    return strcmp(((const patient_t *) a)->name, ((const patient_t *) b)->name);
}

/**
 * Compares age of patient a and patient b.
 * If equal compare by name.
*/
static int compare_patient_age(const void *a, const void *b) {
    int result = ((const patient_t *) a)->age - ((const patient_t *) b)->age;
    return result ? result : compare_patient_name(a, b);
}

/* Tokenize input string to extract name, age, and duration. */
static int tokenize_input(char **name, int *age, int *duration) {
    char *token = strtok(buf, " ");
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

/* Inserts patients into a priority queue from input. */
static void insert_patient_prioq(prioq *queue, prioq *in_session) {
    char *name;
    int age = 0, duration = 0;

    while (1) {
        char *s = fgets(buf, BUF_SIZE, stdin);
        if (!s) {
            if (feof(stdin)) break;
            clean_and_exit(queue, in_session, FILE_FAILURE);
        }

        if (tokenize_input(&name, &age, &duration)) break;
        
        patient_t *p = patient_init(name, age, duration);
        if (!p) {
            clean_and_exit(queue, in_session, PATIENT_FAILURE);
        }

        free(name);
        prioq_insert(queue, p);
    }
}

/**
 * Processes the patient in the in_session queue. 
 * If in_session queue is empty, pick from main queue.
 * Print the name of patient when done.
*/
static void process_patient(prioq *queue, prioq *in_session) {
    if (!queue || !in_session) return;

    patient_t *p =  (prioq_size(in_session) > 0) ? prioq_pop(in_session) :
                    (prioq_size(queue) > 0)      ? prioq_pop(queue) : NULL;
    if (!p) return;

    if (p->duration && p->duration != elapsed_time) {
        prioq_insert(in_session, p);
        return;
    }

    printf("%s\n", p->name);
    patient_cleanup(p);
    elapsed_time = 0;
}

/**
 * Sends remaining patients home from the queue.
 * Prints the names of the patients as they leave.
*/
static void send_remaining_home(prioq *queue) {
    if (!queue) return;

    while (prioq_size(queue) > 0) {
        patient_t *p = prioq_pop(queue);
        printf("%s\n", p->name);
        patient_cleanup(p);
    }
}

int main(int argc, char *argv[]) {
    struct config cfg;
    if (parse_options(&cfg, argc, argv) != 0) return EXIT_FAILURE;

    prioq *queue, *in_session = prioq_init(NULL);
    if (cfg.year) {
        queue = prioq_init(&compare_patient_age);
    } else {
        queue = prioq_init(&compare_patient_name);
    }

    for (int iterations = 0;++elapsed_time;) {
        insert_patient_prioq(queue, in_session);
        process_patient(queue, in_session);
        printf(".\n");

        if (++iterations == MAX_ITERATIONS) {
            send_remaining_home(in_session);
            send_remaining_home(queue);
            break;
        }
    }

    clean_and_exit(queue, in_session, EXIT_SUCCESS);
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
