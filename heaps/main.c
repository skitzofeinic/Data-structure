/**
 * Name: Nguyen Anh Le
 * studentID: 15000370
 * BSc informatica
 * 
 * Simulation of a doctor's office. patients arrive at the front door in random order and enter the 
 * waiting area every session, the doctor picks the first patient in alphabetic order; at the end of 
 * every day, all remaining patients return home.
 * 
 * Made using implementation of the priority queue. By parsing the input
 * and extract the information (name, age, duration). Sorting by alphabetic or age the patient is 
 * being picked from the queue.
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

static char buf[BUF_SIZE];
static int elapsed = 0;

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
 * Cleans up both prioty queues and returns exit status
 * 0: EXIT_SUCCESS
 * 1: EXIT_FAILURE
 */
static int clean_and_exit(prioq *queue, prioq *treatment_queue, int status) {
    prioq_cleanup(queue, patient_cleanup);
    prioq_cleanup(treatment_queue, patient_cleanup);

    return status;
}

/**
 * Compares names patient a and patient b.
 * Negative return: a is lexicographically less
 * Zero return: equal
 * Positive return: b is lexicographically less
 * */
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

/* Tokenize input string to extract name, age, and duration. Returns success or failure. */
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

/* Inserts patients into a priority queue from user input. */
static void insert_patient_prioq(prioq *queue, prioq *treatment_queue) {
    char *name_cpy;
    int age = 0, duration = 0;

    while (1) {
        char *s = fgets(buf, BUF_SIZE, stdin);
        if (!s) {
            if (feof(stdin)) break;
            clean_and_exit(queue, treatment_queue, EXIT_FAILURE);
        }

        if (tokenize_input(&name_cpy, &age, &duration)) break;
        
        patient_t *p = patient_init(name_cpy, age, duration);
        if (!p) {
            clean_and_exit(queue, treatment_queue, EXIT_FAILURE);
        }

        free(name_cpy);
        prioq_insert(queue, p);
    }
}

/**
 * Processes the next patient in the treatment queue.
 * Moves patients from the main queue to the treatment queue based on priority.
 * Prints the name of the treated patient.
*/
static void process_patient(prioq *queue, prioq *treatment_queue) {
    patient_t *p =  (prioq_size(treatment_queue) > 0) ? prioq_pop(treatment_queue) : 
                    (prioq_size(queue) > 0)         ? prioq_pop(queue) : NULL;
    if (!p) return;
    
    if (!(p->duration == -1 || p->duration == elapsed)) {
        prioq_insert(treatment_queue, p);
        return;
    }

    printf("%s\n", p->name);
    patient_cleanup(p);
    elapsed = 0;
}

/**
 * Sends remaining patients home from the queue.
 * Prints the names of the patients as they leave.
*/
static void send_remaining_home(prioq *queue) {
    while (prioq_size(queue) > 0) {
        patient_t *p = prioq_pop(queue);
        printf("%s\n",p->name);
        patient_cleanup(p);
        ++elapsed;
    }
}

int main(int argc, char *argv[]) {
    prioq *queue, *treatment_queue;
    struct config cfg;

    if (parse_options(&cfg, argc, argv) != 0) return EXIT_FAILURE;

    if (cfg.year) {
        queue = prioq_init(&compare_patient_age);
        treatment_queue = prioq_init(&compare_patient_age);
    } else {
        queue = prioq_init(&compare_patient_name);
        treatment_queue = prioq_init(&compare_patient_name);
    }   

    for (int iterations = 0;;) {
        ++elapsed;
        insert_patient_prioq(queue, treatment_queue);
        process_patient(queue, treatment_queue);
        printf(".\n");

        if (++iterations == MAX_ITERATIONS) {
            send_remaining_home(treatment_queue);
            send_remaining_home(queue);
            break;
        }
    }

    clean_and_exit(queue, treatment_queue, EXIT_SUCCESS);
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