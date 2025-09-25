/**
* @file         arithmetic.h
* @brief        Fast integer arithmetic operations
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdint.h>
#include <stdbool.h>
#include <gmp.h>

// Macros for min and max.
#define max(a, b)            \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a, b)            \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

struct subtract_exact_s {
    int32_t n_1;
    int32_t n_2;
    int32_t n_hi;
    int32_t n_lo;
    short b_1;
    short b_2;
    int32_t g_hi;
    int32_t g_lo;
};

// Compute the ith bit of the fraction k/n.
unsigned char ith_bit_of_fraction(uintmax_t k, uintmax_t n, uintmax_t i);
unsigned char ith_bit_of_fraction_gmp(mpz_t k, mpz_t n, uintmax_t i);
unsigned char ith_bit_of_exact(struct subtract_exact_s * ss, uint32_t l);

// Compute exact subtraction of x - y (SUB_0) or 1 - (x+y) (SUB_1).
enum subtract_mode {SUB_0, SUB_1};
void subtract_gmp(enum subtract_mode mode, mpq_t op, double x , double y);
void subtract_exact(enum subtract_mode mode, float x, float y, struct subtract_exact_s * ss);

void subtract_gmp_ext(mpq_t op, bool d0, double x, bool d1, double y);
void subtract_exact_ext(bool d0, float x, bool d1, float y, struct subtract_exact_s * ss);

bool check_ddf_val(bool d, float q);
bool compare_lte_ext(bool d0, float q0, bool d1, float q1);

#endif
