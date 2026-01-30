#ifndef MYLIB_H
#define MYLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize library resources */
int init_library();

/* Cleanup library resources */
void cleanup_library();

/* Simple math operations */
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);

/* String operations */
char* get_version();

/* Array sum operation */
int sum_array(int* arr, int length);

/* Get library status */
int get_status();

/* ========== C++ Features (wrapped for C) ========== */

/**
 * Complex number operations
 * Store and manipulate complex numbers (real + imaginary part)
 */

/* Set complex number: real + imaginary */
int set_complex_number(double real, double imag);

/* Get magnitude of the stored complex number */
double get_complex_magnitude();

/* Add a complex number to the stored one */
int add_complex(double real, double imag);

/* Multiply a complex number with the stored one */
int multiply_complex(double real, double imag);

/* Get type name of stored number */
const char* get_number_type();

/**
 * Vector template operations (using double values)
 * Generic dynamic array with push, get, and aggregate operations
 */

/* Push value to vector */
int vector_push_double(double value);

/* Get vector size */
int vector_size();

/* Get value at index */
double vector_get_double(int index);

/* Clear vector */
void vector_clear();

/* Sum all values in vector */
double vector_sum();

/* Product of all values in vector */
double vector_product();

#ifdef __cplusplus
}
#endif

#endif /* MYLIB_H */