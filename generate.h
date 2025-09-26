/**
  Name:     generate.h
  Purpose:  Generate a random variate.
  Author:   F. Saad
  Copyright (C) 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef GENERATE_H
#define GENERATE_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "arithmetic.h"
#include "flip.h"

// 32-bit cumulative distribution functions, returns Pr(X <= x).
// The 64-bit version is unused.
typedef float  (*cdf32_t)(double x);
typedef double (*cdf64_t)(double x);

// 32-bit dual distribution function. It takes as input a double `x`,
// pointers to boolean `b` and float `p` that are modified with result.
// The 64-bit version is unused.
typedef void (*ddf32_t)(double x, bool * b, float * p);
typedef void (*ddf64_t)(double x, bool * b , double * p);

// Stores cdf_l = cdf(b^{-}) and cdf_r = cdf(b^{+}), where b indexes a block
// in a partition of all floating-point numbers. Refer to Section 5.2,
// Proposition 5.13 of [SL25].
void cdf64_interval(cdf32_t cdf, uint64_t b, unsigned int l, float * cdf_l, float * cdf_r);
void cdf64_interval_ext(ddf32_t ddf, uint64_t b, unsigned int l, bool * d_l , float * cdf_l , bool * d_r, float * cdf_r);

/** Generate random variables optimally from `cdf`. */
double generate_opt(cdf32_t cdf, struct flip_state * prng);

/** Generate random variables optimally from `ddf`. */
double generate_opt_ext(ddf32_t ddf, struct flip_state * prng);

/* Compute the exact `q`-quantile of the `cdf`, where `q` must be in [0,1]. */
double quantile(cdf32_t cdf, float q);

/* Compute the exact `q`-quantile of the `sf`, where `q` must be in [0,1]. */
double quantile_sf(cdf32_t sf, float q);

/* Compute the exact `q`-quantile of the `ddf`, where `q` must be in [0,1]. */
double quantile_ext(ddf32_t ddf, bool d, float q);

/* Compute the exact lower `xlo` and upper `xhi` bound of `cdf`. */
void bounds_quantile(cdf32_t cdf, double * xlo, double * xhi);

/* Compute the exact lower `xlo` and upper `xhi` bound of `sf`. */
void bounds_quantile_sf(cdf32_t sf, double * xlo, double * xhi);

/* Compute the exact lower `xlo` and upper `xhi` bound of `ddf`. */
void bounds_quantile_ext(ddf32_t ddf, double * xlo, double * xhi);

/** Generate random variables from `cdf` using Conditional Bit Sampling. */
double generate_cbs(cdf32_t cdf, struct flip_state * prng);

/** Generate random variables from `ddf` using Conditional Bit Sampling. */
double generate_cbs_ext(ddf32_t ddf, struct flip_state * prng);

// Macros for creating a compatible CDF, SF, and DDF.

/* Distribution over doules. */
#define MAKE_CDF_GENERAL(name, func, nanx, ...) \
  float name(double x__) {                      \
    if (x__ != x__) { return nanx; }            \
    return func(x__, ##__VA_ARGS__);            \
  }

/** Make a cumulative distribution over doubles from the GSL. */
#define MAKE_CDF_P(name, func, ...) MAKE_CDF_GENERAL(name, func, 1., ##__VA_ARGS__)
/** Make a survival distribution over doubles from the GSL. */
#define MAKE_CDF_Q(name, func, ...) MAKE_CDF_GENERAL(name, func, 0., ##__VA_ARGS__)

/* Distribution over unsigned integers. */
#define MAKE_CDF_UINT_GENERAL(name, cdf_func, nanx, ...) \
  float name(double x__) {                               \
    if (x__ != x__)                  { return nanx; }    \
    if (signbit(x__))                { return 1-nanx; }  \
    if (0xffffffffffffffffULL < x__) { return nanx; }    \
    return cdf_func(x__, ##__VA_ARGS__);                 \
  }

/** Make a cumulative distribution over unsigned integers from the GSL. */
#define MAKE_CDF_UINT_P(name, func, ...) MAKE_CDF_UINT_GENERAL(name, func, 1., ##__VA_ARGS__)
/** Make a survival distribution over unsigned integers from the GSL. */
#define MAKE_CDF_UINT_Q(name, func, ...) MAKE_CDF_UINT_GENERAL(name, func, 0., ##__VA_ARGS__)

/** Make a dual distribution function. */
#define MAKE_DDF(name, func_cdf, func_sf)                                \
    const double name##__cutoff = quantile(func_cdf, nextafterf(.5, 1)); \
    const bool name##__sign = signbit(name##__cutoff);        \
    bool name##__abort = false;                               \
    if(0.5 < func_cdf(nextafter(name##__cutoff, -INFINITY))) {     \
        fprintf(stderr, "Invalid CDF detected.\n");          \
        name##__abort = true;                                \
    }                                                        \
    if(0.5 <= func_sf(name##__cutoff)) {                     \
        fprintf(stderr, "Invalid SF detected.\n");           \
        name##__abort = true;                                \
    }                                                        \
    if (name##__abort) {                                     \
        exit(1);                                             \
    }                                                        \
    void name(double x, bool * d, float * q) {               \
        if ((x < name##__cutoff)                             \
                || ((x == name##__cutoff)                    \
                    && signbit(x)                            \
                    && !(name##__sign))) {                   \
            *d = 0;                                          \
            *q = func_cdf(x);                                \
        } else {                                             \
            *d = 1;                                          \
            *q = func_sf(x);                                 \
        }                                                    \
        assert(check_ddf_val(*d, *q));                       \
     }

#endif
