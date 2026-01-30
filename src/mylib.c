#include "DSA/mylib.h"
#include <stdio.h>
#include <string.h>

/* Global state */
static int is_initialized = 0;
static const char* LIBRARY_VERSION = "1.0.0";

/* Initialize library resources */
int init_library() {
    if (is_initialized) {
        return 1;  /* Already initialized */
    }
    printf("Library initialized (v%s)\n", LIBRARY_VERSION);
    is_initialized = 1;
    return 0;
}

/* Cleanup library resources */
void cleanup_library() {
    if (is_initialized) {
        printf("Library cleaned up\n");
        is_initialized = 0;
    }
}

/* Simple math operations */
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

/* String operations */
char* get_version() {
    return (char*)LIBRARY_VERSION;
}

/* Array sum operation */
int sum_array(int* arr, int length) {
    if (arr == NULL || length <= 0) {
        return 0;
    }
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

/* Get library status */
int get_status() {
    return is_initialized;
}
