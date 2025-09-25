/*
  Name:     arithmetic.h
  Purpose:  Fast integer arithmetic operations.
  Author:   F. Saad
  Copyright (C) 2025 CMU Probabilistic Computing Systems Lab
*/

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <gmp.h>

#include "bits.h"
#include "arithmetic.h"


void subtract_gmp(enum subtract_mode mode, mpq_t op, double x, double y) {
    mpq_t qx; mpq_init(qx); mpq_set_d(qx, x);
    mpq_t qy; mpq_init(qy); mpq_set_d(qy, y);
    switch (mode) {
        case SUB_0:
            mpq_sub(op, qx, qy);
            break;
        case SUB_1:
            mpq_t one;  mpq_init(one);  mpq_set_d(one, 1.);
            mpq_t qxy;  mpq_init(qxy);  mpq_add(qxy, qx, qy);
            mpq_sub(op, one, qxy);
            mpq_clear(qxy);
            mpq_clear(one);
            break;
    }
    mpq_clear(qx);
    mpq_clear(qy);
}

void subtract_exact(
        enum subtract_mode mode,
        float x,
        float y,
        struct subtract_exact_s * ss
        ) {

    switch (mode) {

        case SUB_0:
            assert(y <= x);
            assert(!((x == 1) && (y==0)));
            break;

        case SUB_1:
            assert(!((x == .5) && (y == .5)));
            assert(!((x == 0.) && (y == 0.)));
            float xx = max(x, y);
            float yy = min(x, y);
            x = xx;
            y = yy;
            break;
    }

    union float_bits fields_x = {.f = x};
    union float_bits fields_y = {.f = y};

    unsigned int e_x = fields_x.b.exponent;
    unsigned int e_y = fields_y.b.exponent;

    int32_t Emax = (1 << (FLT_SIZE_E - 1)) - 1;
    int32_t ehat_x = e_x - Emax + (e_x == 0);
    int32_t ehat_y = e_y - Emax + (e_y == 0);

    unsigned int m_x = fields_x.b.mantissa;
    unsigned int m_y = fields_y.b.mantissa;
    int32_t f_x = m_x + ((int32_t)(e_x > 0) << FLT_SIZE_M);
    int32_t f_y = m_y + ((int32_t)(e_y > 0) << FLT_SIZE_M);

    int32_t f_hi = f_y >> min(ehat_x - ehat_y, FLT_SIZE - 1);
    int32_t f_lo = f_y & ((1 << min(ehat_x - ehat_y, FLT_SIZE_M+1)) - 1);

    switch (mode) {

        case SUB_0:
            ss->n_1 = -ehat_x - 1 + (x == 1);
            ss->n_2 = max((ehat_x - ehat_y) - (FLT_SIZE_M + 1), 0);
            ss->n_hi = FLT_SIZE_M + 1 - (x == 1);
            ss->n_lo = min(ehat_x - ehat_y , FLT_SIZE_M + 1);
            ss->b_1 = 0;
            ss->b_2 = f_lo > 0;
            ss->g_hi = f_x - f_hi - ss->b_2;
            ss->g_lo = (ss->b_2 << ss->n_lo) - f_lo;
            break;

        case SUB_1:
            ss->n_1 = -ehat_x - 2 + (x == .5);
            ss->n_2 = max((ehat_x - ehat_y) - (FLT_SIZE_M + 1), 0);
            ss->n_hi = FLT_SIZE_M + 2 - (x == .5);
            ss->n_lo = min(ehat_x - ehat_y , FLT_SIZE_M + 1);
            ss->b_1 = 1;
            ss->b_2 = f_lo > 0;
            ss->g_hi = (1 << ss->n_hi) - f_x - f_hi - ss->b_2;
            ss->g_lo = (ss->b_2 << ss->n_lo) - f_lo;
    }
}

void subtract_gmp_ext(mpq_t op, bool d0, double x, bool d1, double y) {
    if          ((d0 == 0) && (d1 == 0)) {subtract_gmp(SUB_0, op, x, y);}
    else if     ((d0 == 1) && (d1 == 1)) {subtract_gmp(SUB_0, op, y, x);}
    else if     ((d0 == 1) && (d1 == 0)) {subtract_gmp(SUB_1, op, x, y);}
    else                                 {exit(EXIT_FAILURE);}
}

void subtract_exact_ext(bool d0, float x, bool d1, float y, struct subtract_exact_s * ss){
    if          ((d0 == 0) && (d1 == 0)) {subtract_exact(SUB_0, x, y, ss);}
    else if     ((d0 == 1) && (d1 == 1)) {subtract_exact(SUB_0, y, x, ss);}
    else if     ((d0 == 1) && (d1 == 0)) {subtract_exact(SUB_1, x, y, ss);}
    else                                 {exit(EXIT_FAILURE);}
}

unsigned char ith_bit_of_fraction(uintmax_t k, uintmax_t n, uintmax_t i) {
    assert((0 < i) & (0 < k) & (k < n));
    unsigned char b;
    for (int j = 1; j <= i; j++){
        k <<= 1;
        if (n == k){
            b = (j == i) ? 1 : 0;
            break;
        }
        if (n < k) {
            b = 1;
            k = k - n;
        } else {
            b = 0;
        }
    }
    return b;
}

unsigned char ith_bit_of_fraction_gmp(mpz_t k, mpz_t n, uintmax_t i) {
    assert (0 < i);
    // assert (mpz_cmp_ui(k, 0) > 0);
    assert (mpz_cmp(k, n) < 0);
    unsigned char b;
    mpz_t kk; mpz_init_set(kk, k);
    for (int j = 1; j <= i; j++) {
        mpz_mul_2exp(kk, kk, 1); // kk <<= 1
        if (mpz_cmp(kk, n) == 0) {
            b = (j == i) ? 1 : 0;
            break;
        }
        if (mpz_cmp(n, kk) < 0) {
            b = 1;
            mpz_sub(kk, kk, n);
        } else {
            b = 0;
        }
    }
    mpz_clear(kk);
    return b;
}

unsigned char ith_bit_of_exact(struct subtract_exact_s * ss, uint32_t l) {
    // TODO, remove duplication.
    int32_t n_1  = ss->n_1;
    int32_t n_2  = ss->n_2;
    int32_t n_hi = ss->n_hi;
    int32_t n_lo = ss->n_lo;
    short b_1    = ss->b_1;
    short b_2    = ss->b_2;
    int32_t g_hi = ss->g_hi;
    int32_t g_lo = ss->g_lo;
    if (l <= n_1) {
        return b_1;
    }
    else if (l <= n_1 + n_hi) {
        assert(g_hi < (1 << n_hi));
        return (g_hi >> (n_hi - (l - n_1))) & 1;
    }
    else if (l <= n_1 + n_hi + n_2) {
        return b_2;
    }
    else if (l <= n_1 + n_hi + n_2 + n_lo) {
        assert(g_lo < (1 << n_lo));
        return (g_lo >> (n_lo - (l - (n_1 + n_hi + n_2)))) & 1;
    } else {
        return 0;
    }
}

bool check_ddf_val(bool d, float q) {
    return
        (d == 0) && (0 <= q) && (q <= 0.5)
        ||
        (d == 1) && (0 <= q) && (q < 0.5);
}

bool compare_lte_ext(bool d0, float q0, bool d1, float q1) {
    assert(check_ddf_val(d0, q0));
    assert(check_ddf_val(d1, q1));
    return (d0 < d1)
            || ((d0 == 0) && (d1 == 0) && (q0 <= q1))
            || ((d0 == 1) && (d1 == 1) && (q1 <= q0));
}
