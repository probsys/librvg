/**
  @file         generate.h
  @brief        Generate a random variate given a cumulative distribution function
  @author       F. Saad
  @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef GENERATE_H
#define GENERATE_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "arithmetic.h"
#include "flip.h"

/** @defgroup group1 The First Group
 *  This is the first group
 *  @{
 */
/** @brief class C1 in group 1 */
typedef float  (*cdf32_t)(double x);
/** @brief class C2 in group 1 */
typedef double (*cdf64_t)(double x);
/** @} */ // end of group1


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

/** @brief Generate random variables optimally from `cdf`. */
double generate_opt(cdf32_t cdf, struct flip_state * prng);

/** @brief Generate random variables optimally from `ddf`. */
double generate_opt_ext(ddf32_t ddf, struct flip_state * prng);

/** @brief Compute the exact `q`-quantile of the `cdf`, where `q` must be in [0,1]. */
double quantile(cdf32_t cdf, float q);

/** @brief Compute the exact `q`-quantile of the `sf`, where `q` must be in [0,1]. */
double quantile_sf(cdf32_t sf, float q);

/** @brief Compute the exact `q`-quantile of the `ddf`, where `q` must be in [0,1]. */
double quantile_ext(ddf32_t ddf, bool d, float q);

/** @brief Compute the exact lower `xlo` and upper `xhi` bound of `cdf`. */
void bounds_quantile(cdf32_t cdf, double * xlo, double * xhi);

/** @brief Compute the exact lower `xlo` and upper `xhi` bound of `sf`. */
void bounds_quantile_sf(cdf32_t sf, double * xlo, double * xhi);

/** @brief Compute the exact lower `xlo` and upper `xhi` bound of `ddf`. */
void bounds_quantile_ext(ddf32_t ddf, double * xlo, double * xhi);

/** @brief Generate random variables from `cdf` using Conditional Bit Sampling. */
double generate_cbs(cdf32_t cdf, struct flip_state * prng);

/** @brief Generate random variables from `ddf` using Conditional Bit Sampling. */
double generate_cbs_ext(ddf32_t ddf, struct flip_state * prng);

// Macros for creating a compatible CDF, SF, and DDF.

/** @brief Distribution over doubles. */
#define MAKE_CDF_GENERAL(name, func, nanx, ...) \
  float name(double x__) {                      \
    if (x__ != x__) { return nanx; }            \
    return func(x__, ##__VA_ARGS__);            \
  }

/* Make a cumulative distribution over doubles from the GSL. */
#define MAKE_CDF_P(name, func, ...) MAKE_CDF_GENERAL(name, func, 1., ##__VA_ARGS__)
/* Make a cumulative distribution over doubles from the GSL. */
#define MAKE_CDF_Q(name, func, ...) MAKE_CDF_GENERAL(name, func, 0., ##__VA_ARGS__)

/** @brief Distribution over unsigned integers. */
#define MAKE_CDF_UINT_GENERAL(name, cdf_func, nanx, ...) \
  float name(double x__) {                               \
    if (x__ != x__)                  { return nanx; }    \
    if (signbit(x__))                { return 1-nanx; }  \
    if (0xffffffffffffffffULL < x__) { return nanx; }    \
    return cdf_func(x__, ##__VA_ARGS__);                 \
  }

/* Make a cumulative distribution over unsigned integers from the GSL. */
#define MAKE_CDF_UINT_P(name, func, ...) MAKE_CDF_UINT_GENERAL(name, func, 1., ##__VA_ARGS__)
/* Make a survival distribution over unsigned integers from the GSL. */
#define MAKE_CDF_UINT_Q(name, func, ...) MAKE_CDF_UINT_GENERAL(name, func, 0., ##__VA_ARGS__)

/** @brief Make a dual distribution function. */
#define MAKE_DDF(ddf, cdf, sf)                               \
    const double ddf##__cutoff = quantile(cdf, nextafterf(.5, 1)); \
    const bool ddf##__sign = signbit(ddf##__cutoff);         \
    bool ddf##__abort = false;                               \
    if(0.5 < cdf(nextafter(ddf##__cutoff, -INFINITY))) {     \
        fprintf(stderr, "Invalid CDF detected.\n");          \
        ddf##__abort = true;                                 \
    }                                                        \
    if(0.5 <= sf(ddf##__cutoff)) {                           \
        fprintf(stderr, "Invalid SF detected.\n");           \
        ddf##__abort = true;                                 \
    }                                                        \
    if (ddf##__abort) {                                      \
        exit(1);                                             \
    }                                                        \
    void ddf(double x, bool * d, float * q) {                \
        if ((x < ddf##__cutoff)                              \
                || ((x == ddf##__cutoff)                     \
                    && signbit(x)                            \
                    && !(ddf##__sign))) {                    \
            *d = 0;                                          \
            *q = cdf(x);                                     \
        } else {                                             \
            *d = 1;                                          \
            *q = sf(x);                                      \
        }                                                    \
        assert(check_ddf_val(*d, *q));                       \
     }

#endif
