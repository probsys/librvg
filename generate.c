/*
  Name:     generate.c
  Purpose:  Generate a random variate.
  Author:   F. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <assert.h>
#include <math.h>
#include <gmp.h>

#include "bits.h"
#include "flip.h"
#include "arithmetic.h"
#include "bernoulli.h"
#include "generate.h"

void cdf64_interval(
        cdf32_t cdf            // Target CDF.
        , uint64_t b           // Current bit string (lex order).
        , unsigned int l       // Number of active bits in b, 0 <= l <= 64.
        , float * cdf_l        // Will be set to CDF(b0^m)
        , float * cdf_r        // Will be set to CDF(b1^m)
        ) { /* d */

    assert(l <= DBL_SIZE);
    // Base case, no active bits.
    if (l == 0) {
        *cdf_l = 0.;
        *cdf_r = 1.;
        return;
    }
    unsigned int m = DBL_SIZE - l;                      // m = n - len(b)
    // Compute CDF at right endpoint.
    uint64_t b_lex = (b << m) + (1ull << m) - 1;        // b+'1'*m
    uint64_t b_flt = bij64_lex2float(b_lex);
    double d = int2double(b_flt);
    *cdf_r = cdf(d); // TODO if isnan(d) then return 1?
    // Compute CDF at left endpoint.
    if (b > 0) {
        // b has a predecessor.
        b_lex = (b << m) - 1;                           // pred_lex(b+0^m)
        b_flt = bij64_lex2float(b_lex);
        d = int2double(b_flt);
        *cdf_l = cdf(d);
    } else {
        *cdf_l = 0.;
    }
}

void cdf64_interval_ext(
        ddf32_t ddf             // Target dual distribution function (DDF).
        , uint64_t b            // Current bit string (lex order).
        , unsigned int l        // Number of active bits in b, 0 <= l <= 64.
        , bool * d_l            // Will be set to CDF(b0^m)
        , float * cdf_l         // Will be set to CDF(b0^m)
        , bool * d_r            // Will be set to CDF(b1^m)
        , float * cdf_r         // Will be set to CDF(b1^m)
        ) {

    assert(l <= DBL_SIZE);
    // Base case, no active bits.
    if (l == 0) {
        *d_l = 0; *cdf_l = 0.;
        *d_r = 1; *cdf_r = 0.;
        return;
    }
    unsigned int m = DBL_SIZE - l;                      // m = n - len(b)
    // Compute CDF at right endpoint.
    uint64_t b_lex = (b << m) + (1ull << m) - 1;        // b+'1'*m
    uint64_t b_flt = bij64_lex2float(b_lex);
    double d = int2double(b_flt);
    ddf(d, d_r, cdf_r); // TODO if isnan(d) then return 1?
    // Compute CDF at left endpoint.
    if (b > 0) {
        // b has a predecessor.
        b_lex = (b << m) - 1;                           // pred_lex(b+0^m)
        b_flt = bij64_lex2float(b_lex);
        d = int2double(b_flt);
        ddf(d, d_l, cdf_l);
    } else {
        *d_l = 0; *cdf_l = 0.;
    }
}

double generate_cbs(cdf32_t cdf, struct flip_state * prng) {

    // Evolving state.
    uint64_t b = 0;
    double cdf_l = 0;
    double cdf_r = 1;

    // GMP objects.
    mpq_t cdf_w;    mpq_init(cdf_w);    mpq_set_ui(cdf_w, 1, 1);
    mpq_t cdf_w0;   mpq_init(cdf_w0);
    mpq_t cdf_w1;   mpq_init(cdf_w1);
    mpq_t w_sum;    mpq_init(w_sum);
    mpq_t r;        mpq_init(r);
    mpz_t k;        mpz_init(k);
    mpz_t n;        mpz_init(n);

    for (int l = 0; l < DBL_SIZE; l++) {

        // Compute CDF at midpoint.
        unsigned int m = DBL_SIZE - (l + 1);
        uint64_t b_lex = (b << m + 1) + (1ull << m) - 1;
        uint64_t b_flt = bij64_lex2float(b_lex);
        double d = int2double(b_flt);
        float cdf_m = cdf(d);

        // Compute b+'0' and b+'1'.
        uint64_t b_lex_0 = b << 1;
        uint64_t b_lex_1 = b_lex_0 | 1;

        #ifndef NDEBUG
        float cdf_check_l, cdf_check_r;
        float cdf_check_l_0, cdf_check_r_0;
        float cdf_check_l_1, cdf_check_r_1;
        cdf64_interval(cdf, b, l, &cdf_check_l, &cdf_check_r);
        cdf64_interval(cdf, b_lex_0, l+1, &cdf_check_l_0, &cdf_check_r_0);
        cdf64_interval(cdf, b_lex_1, l+1, &cdf_check_l_1, &cdf_check_r_1);
        assert(cdf_check_l == cdf_l);
        assert(cdf_check_r == cdf_r);
        assert(cdf_check_l_0 == cdf_l);
        assert(cdf_check_r_0 == cdf_m);
        assert(cdf_check_l_1 == cdf_m);
        assert(cdf_check_r_1 == cdf_r);
        assert(cdf_l <= cdf_m);
        assert(cdf_m <= cdf_r);
        #endif

        // Trivial case.
        if (cdf_m == cdf_r) {
            b = b_lex_0;
            cdf_r = cdf_m;
            continue;
        }
        if (cdf_m == cdf_l) {
            b = b_lex_1;
            cdf_l = cdf_m;
            continue;
        }

        // Compute p(b1)/p(b).
        subtract_gmp(SUB_0, cdf_w1, cdf_r, cdf_m);
        mpq_div(r, cdf_w1, cdf_w);
        mpq_get_num(k, r);
        mpq_get_den(n, r);

        #ifndef NDEBUG
        subtract_gmp(SUB_0, cdf_w0, cdf_m, cdf_l);
        mpq_add(w_sum, cdf_w0, cdf_w1);
        assert(mpq_cmp(cdf_w,  w_sum) == 0);
        #endif

        unsigned char z = bernoulli_gmp(k, n, prng);
        if (z == 0) {
            b = b_lex_0;
            cdf_r = cdf_m;
            subtract_gmp(SUB_0, cdf_w0, cdf_m, cdf_l);
            mpq_set(cdf_w, cdf_w0);
        } else {
            b = b_lex_1;
            cdf_l = cdf_m;
            mpq_set(cdf_w, cdf_w1);
        }
    }

    mpq_clear(cdf_w);
    mpq_clear(cdf_w0);
    mpq_clear(cdf_w1);
    mpq_clear(w_sum);
    mpq_clear(r);
    mpz_clear(k);
    mpz_clear(n);

    b = bij64_lex2float(b);
    return int2double(b);
}

double generate_cbs_ext(ddf32_t ddf, struct flip_state * prng) {

    // Evolving state.
    uint64_t b = 0;
    bool d_l = 0; float cdf_l = 0;
    bool d_r = 1; float cdf_r = 0;

    // GMP objects.
    mpq_t cdf_w;    mpq_init(cdf_w);    mpq_set_ui(cdf_w, 1, 1);
    mpq_t cdf_w0;   mpq_init(cdf_w0);
    mpq_t cdf_w1;   mpq_init(cdf_w1);
    mpq_t w_sum;    mpq_init(w_sum);
    mpq_t r;        mpq_init(r);
    mpz_t k;        mpz_init(k);
    mpz_t n;        mpz_init(n);

    for (int l = 0; l < DBL_SIZE; l++) {
        // Compute CDF at midpoint.
        unsigned int m = DBL_SIZE - (l + 1);             // m = n_max - (len(b)+1)
        uint64_t b_lex = (b << m + 1) + (1ull << m) - 1; // b+'0' + '1'*m
        uint64_t b_flt = bij64_lex2float(b_lex);
        double d = int2double(b_flt);
        bool d_m; float cdf_m;
        ddf(d, &d_m, &cdf_m);

        // Compute b+'0' and b+'1'.
        uint64_t b_lex_0 = b << 1;
        uint64_t b_lex_1 = b_lex_0 | 1;

        #ifndef NDEBUG
        bool cdf_check_dl, cdf_check_dr;        float cdf_check_l, cdf_check_r;
        bool cdf_check_dl_0, cdf_check_dr_0;    float cdf_check_l_0, cdf_check_r_0;
        bool cdf_check_dl_1, cdf_check_dr_1;    float cdf_check_l_1, cdf_check_r_1;
        cdf64_interval_ext(ddf, b, l, &cdf_check_dl, &cdf_check_l, &cdf_check_dr, &cdf_check_r);
        cdf64_interval_ext(ddf, b_lex_0, l+1, &cdf_check_dl_0, &cdf_check_l_0, &cdf_check_dr_0, &cdf_check_r_0);
        cdf64_interval_ext(ddf, b_lex_1, l+1, &cdf_check_dl_1, &cdf_check_l_1, &cdf_check_dr_1, &cdf_check_r_1);
        assert(cdf_check_dl == d_l);           assert(cdf_check_l == cdf_l);
        assert(cdf_check_dr == d_r);           assert(cdf_check_r == cdf_r);
        assert(cdf_check_dl_0 == d_l);         assert(cdf_check_l_0 == cdf_l);
        assert(cdf_check_dr_0 == d_m);         assert(cdf_check_r_0 == cdf_m);
        assert(cdf_check_dl_1 == d_m);         assert(cdf_check_l_1 == cdf_m);
        assert(cdf_check_dr_1 == d_r);         assert(cdf_check_r_1 == cdf_r);
        assert(compare_lte_ext(d_l, cdf_l, d_m, cdf_m));
        assert(compare_lte_ext(d_m, cdf_m, d_r, cdf_r));
        // assert(cdf_l <= cdf_m);
        // assert(cdf_m <= cdf_r);
        #endif

        // Trivial case.
        if ((d_m == d_r) && (cdf_m == cdf_r)) {
            b = b_lex_0;
            d_r = d_m; cdf_r = cdf_m;
            continue;
        }
        if ((d_m == d_l) && (cdf_m == cdf_l)) {
            b = b_lex_1;
            d_l = d_m; cdf_l = cdf_m;
            continue;
        }

        // Compute p(b1)/p(b)
        mpq_t cdf_w;   mpq_init(cdf_w);
        mpq_t cdf_w1;  mpq_init(cdf_w1);
        subtract_gmp_ext(cdf_w, d_r, cdf_r, d_l, cdf_l);
        subtract_gmp_ext(cdf_w1, d_r, cdf_r, d_m, cdf_m);
        mpq_t r; mpq_init(r);
        mpz_t k; mpz_init(k);
        mpz_t n; mpz_init(n);
        mpq_div(r, cdf_w1, cdf_w);
        mpq_get_num(k, r);
        mpq_get_den(n, r);

        #ifndef NDEBUG
        subtract_gmp_ext(cdf_w0, d_m, cdf_m, d_l, cdf_l);
        mpq_add(w_sum, cdf_w0, cdf_w1);
        assert(mpq_cmp(cdf_w, w_sum) == 0);
        #endif

        unsigned char z = bernoulli_gmp(k, n, prng);
        if (z == 0) {
            b = b_lex_0;
            d_r = d_m; cdf_r = cdf_m;
            subtract_gmp_ext(cdf_w0, d_m, cdf_m, d_l, cdf_l);
            mpq_set(cdf_w, cdf_w0);
        } else {
            b = b_lex_1;
            d_l = d_m; cdf_l = cdf_m;
            mpq_set(cdf_w, cdf_w1);
        }
    }

    mpq_clear(cdf_w);
    mpq_clear(cdf_w0);
    mpq_clear(cdf_w1);
    mpq_clear(w_sum);
    mpq_clear(r);
    mpz_clear(k);
    mpz_clear(n);

    b = bij64_lex2float(b);
    return int2double(b);
}

// ================ Simulate Opt ================

double generate_opt(cdf32_t cdf, struct flip_state * prng) {

    // Evolving state.
    uint64_t b = 0;
    unsigned int ell = 0;
    float cdf_l = 0;
    float cdf_r = 1;

    #ifndef NDEBUG
    mpq_t kn0; mpq_init(kn0);
    mpq_t kn1; mpq_init(kn1);
    mpz_t k0; mpz_init(k0);
    mpz_t n0; mpz_init(n0);
    mpz_t k1; mpz_init(k1);
    mpz_t n1; mpz_init(n1);
    #endif

    for (int l = 0; l < DBL_SIZE; l++) {

        // Compute CDF at midpoint.
        unsigned int m = DBL_SIZE - (l + 1);             // m = n_max - (len(b)+1)
        uint64_t b_lex = (b << m + 1) + (1ull << m) - 1; // b+'0' + '1'*m
        uint64_t b_flt = bij64_lex2float(b_lex);
        double d = int2double(b_flt);
        float cdf_m = cdf(d);
        assert(cdf_l <= cdf_m);
        assert(cdf_m <= cdf_r);

        // Compute b+'0' and b+'1'.
        uint64_t b_lex_0 = b << 1;
        uint64_t b_lex_1 = b_lex_0 | 1;

        // Trivial case.
        if (cdf_m == cdf_r) {
            // return generate_opt(cdf, b_lex_0, l, cdf_l, cdf_m, prng);
            b = b_lex_0;
            cdf_r = cdf_m;
            continue;
        }
        if (cdf_m == cdf_l) {
            // return generate_opt(cdf, b_lex_1, l, cdf_m, cdf_r, prng);
            b = b_lex_1;
            cdf_l = cdf_m;
            continue;
        }

        // Finite arithmetic case.
        struct subtract_exact_s ss0, ss1;
        subtract_exact(SUB_0, cdf_m, cdf_l, &ss0);
        subtract_exact(SUB_0, cdf_r, cdf_m, &ss1);

        #ifndef NDEBUG
        subtract_gmp(SUB_0, kn0, cdf_m, cdf_l);
        subtract_gmp(SUB_0, kn1, cdf_r, cdf_m);
        mpq_get_num(k0, kn0); mpq_get_den(n0, kn0);
        mpq_get_num(k1, kn1); mpq_get_den(n1, kn1);
        #endif

        if (ell > 0) {
            int a0 = ith_bit_of_exact(&ss0, ell);
            int a1 = ith_bit_of_exact(&ss1, ell);
            #ifndef NDEBUG
            int z0 = ith_bit_of_fraction_gmp(k0, n0, ell);
            int z1 = ith_bit_of_fraction_gmp(k1, n1, ell);
            assert((a0 == z0) && (a1 == z1));
            #endif
            if ((a0 == 1) && (a1 == 0)) {
                b = b_lex_0;
                cdf_r = cdf_m;
                continue;
            }
            if ((a0 == 0) && (a1 == 1)) {
                b = b_lex_1;
                cdf_l = cdf_m;
                continue;
            }
        }

        while (1) {
            ell += 1;
            int a0 = ith_bit_of_exact(&ss0, ell);
            int a1 = ith_bit_of_exact(&ss1, ell);
            #ifndef NDEBUG
            int z0 = ith_bit_of_fraction_gmp(k0, n0, ell);
            int z1 = ith_bit_of_fraction_gmp(k1, n1, ell);
            assert((a0 == z0) && (a1 == z1));
            #endif
            unsigned char x = flip(prng);
            if ((x == 0) && (a0 == 1)) {
                b = b_lex_0;
                cdf_r = cdf_m;
                break;
            }
            if ((x == 1) && (a1 == 1)) {
                b = b_lex_1;
                cdf_l = cdf_m;
                break;
            }
        }
    }

    b = bij64_lex2float(b);
    return int2double(b);
}

double generate_opt_ext(ddf32_t ddf, struct flip_state * prng) {

    // Evolving state.
    uint64_t b = 0;
    unsigned int ell = 0;
    bool d_l = 0; float cdf_l = 0;
    bool d_r = 1; float cdf_r = 0;

    #ifndef NDEBUG
    mpq_t kn0; mpq_init(kn0);
    mpq_t kn1; mpq_init(kn1);
    mpz_t k0; mpz_init(k0);
    mpz_t n0; mpz_init(n0);
    mpz_t k1; mpz_init(k1);
    mpz_t n1; mpz_init(n1);
    #endif

    for (int l = 0; l < DBL_SIZE; l++) {

        // Compute CDF at midpoint.
        unsigned int m = DBL_SIZE - (l + 1);             // m = n_max - (len(b)+1)
        uint64_t b_lex = (b << m + 1) + (1ull << m) - 1; // b+'0' + '1'*m
        uint64_t b_flt = bij64_lex2float(b_lex);
        double d = int2double(b_flt);
        bool d_m; float cdf_m;
        ddf(d, &d_m, &cdf_m);
        assert(compare_lte_ext(d_l, cdf_l, d_m, cdf_m));
        assert(compare_lte_ext(d_m, cdf_m, d_r, cdf_r));

        // Compute b+'0' and b+'1'.
        uint64_t b_lex_0 = b << 1;
        uint64_t b_lex_1 = b_lex_0 | 1;

        // Trivial case.
        if ((d_m == d_r) && (cdf_m == cdf_r)) {
            // return generate_opt_ext(cdf, b_lex_0, l, d_l, cdf_l, d_m, cdf_m, prng);
            b = b_lex_0;
            d_r = d_m; cdf_r = cdf_m;
            continue;
        }
        if ((d_m == d_l) && (cdf_m == cdf_l)) {
            // return generate_opt_ext(cdf, b_lex_1, l, d_m, cdf_m, d_r, cdf_r, prng);
            b = b_lex_1;
            d_l = d_m; cdf_l = cdf_m;
            continue;
        }

        // Finite arithmetic case.
        struct subtract_exact_s ss0, ss1;
        subtract_exact_ext(d_m, cdf_m, d_l, cdf_l, &ss0);
        subtract_exact_ext(d_r, cdf_r, d_m, cdf_m, &ss1);

        #ifndef NDEBUG
        subtract_gmp_ext(kn0, d_m, cdf_m, d_l, cdf_l);
        subtract_gmp_ext(kn1, d_r, cdf_r, d_m, cdf_m);
        mpq_get_num(k0, kn0); mpq_get_den(n0, kn0);
        mpq_get_num(k1, kn1); mpq_get_den(n1, kn1);
        #endif

        if (ell > 0) {
            int a0 = ith_bit_of_exact(&ss0, ell);
            int a1 = ith_bit_of_exact(&ss1, ell);
            #ifndef NDEBUG
            int z0 = ith_bit_of_fraction_gmp(k0, n0, ell);
            int z1 = ith_bit_of_fraction_gmp(k1, n1, ell);
            assert((a0 == z0) && (a1 == z1));
            #endif
            if ((a0 == 1) && (a1 == 0)) {
                b = b_lex_0;
                d_r = d_m; cdf_r = cdf_m;
                continue;
            }
            if ((a0 == 0) && (a1 == 1)) {
                b = b_lex_1;
                d_l = d_m; cdf_l = cdf_m;
                continue;
            }
        }

        while (1) {
            ell += 1;
            int a0 = ith_bit_of_exact(&ss0, ell);
            int a1 = ith_bit_of_exact(&ss1, ell);
            #ifndef NDEBUG
            int z0 = ith_bit_of_fraction_gmp(k0, n0, ell);
            int z1 = ith_bit_of_fraction_gmp(k1, n1, ell);
            assert((a0 == z0) && (a1 == z1));
            #endif
            unsigned char x = flip(prng);
            if ((x == 0) && (a0 == 1)) {
                b = b_lex_0;
                d_r = d_m; cdf_r = cdf_m;
                break;
            }
            if ((x == 1) && (a1 == 1)) {
                b = b_lex_1;
                d_l = d_m; cdf_l = cdf_m;
                break;
            }
        }
    }

    b = bij64_lex2float(b);
    return int2double(b);
}

// ================ Quantile Function ================

double quantile(cdf32_t cdf, float q) {
    assert((0 <= q) && (q <= 1));
    uint64_t lo = 0;
    uint64_t hi = 0xffffffffffffffff;
    union double_bits mid;
    double x;
    int iter = 0;
    while (1) {
        iter++;
        uint64_t m = lo/2 + hi/2;
        mid.i = bij64_lex2float(m);
        float cdf_mid = cdf(mid.f);
        if (q <= cdf_mid) {
            x = mid.f;
            if (hi == lo) { break; }
            hi = m - 1;
        } else {
            if (lo == hi) { break; }
            lo = m + 1;
        }
    }
    assert(iter == 64);
    return x;
}

double quantile_sf(cdf32_t sf, float q) {
    assert((0 < q) && (q <= 1));
    uint64_t lo = 0;
    uint64_t hi = 0xffffffffffffffff;
    union double_bits mid;
    double x;
    int iter = 0;
    while (1) {
        iter++;
        uint64_t m = lo/2 + hi/2;
        mid.i = bij64_lex2float(m);
        float sf_mid = sf(mid.f);
        if (sf_mid < q) {
            x = mid.f;
            if (hi == lo) { break; }
            hi = m - 1;
        } else {
            if (lo == hi) { break; }
            lo = m + 1;
        }
    }
    assert(iter == 64);
    return x;
}

double quantile_ext(ddf32_t ddf, bool d, float q) {
    assert(check_ddf_val(d, q));
    uint64_t lo = 0;
    uint64_t hi = 0xffffffffffffffff;
    union double_bits mid;
    bool d_mid; float cdf_mid;
    double x;
    int iter = 0;
    while (1) {
        iter++;
        uint64_t m = lo/2 + hi/2;
        mid.i = bij64_lex2float(m);
        ddf(mid.f, &d_mid, &cdf_mid);
        if (compare_lte_ext(d, q, d_mid, cdf_mid)) {
            x = mid.f;
            if (hi == lo) { break; }
            hi = m - 1;
        } else {
            if (lo == hi) { break; }
            lo = m + 1;
        }
    }
    assert(iter == 64);
    return x;
}

void bounds_quantile(cdf32_t cdf, double * xlo, double * xhi){
    *xlo = quantile(cdf, nextafterf(0, 1.));
    *xhi = quantile(cdf, 1);
}

void bounds_quantile_sf(cdf32_t sf, double * xlo, double * xhi){
    *xlo = quantile_sf(sf, 1);
    *xhi = quantile_sf(sf, nextafterf(0, 1.));
}

void bounds_quantile_ext(ddf32_t ddf, double * xlo, double * xhi) {
    *xlo = quantile_ext(ddf, 0, nextafterf(0, 1.));
    *xhi = quantile_ext(ddf, 1, 0);
}
