/*
  Name:     bounds.c
  Purpose:  Computing bounds on GSL and CDF, SF, and DDF generators.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <time.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#include "rvg/generate.h"
#include "rvg/deterministic.h"

int main(int argc, char * argv[]) {

    if (argc < 2) {
        printf("Usage: %s [0|1] \n", argv[0]);
        exit(1);
    }

    int choice = atoi((argv)[1]);
    if ((choice != 0) && (choice != 1)) {
        printf("Choice must be 0 or 1; but received %d\n", choice);
        exit(1);
    }

    clock_t time, elapsed;

    // GSL BENCHMARK
    gsl_rng * rng = gsl_rng_alloc(gsl_rng_deterministic);
    unsigned long max_seed = 0x100000000; // pow(2, 32)
    float xmin, xmax;

    #define ENUMERATE_SAMPLER(sampler, check, offset) \
        xmin = INFINITY;                                                    \
        xmax = -INFINITY;                                                   \
        time = clock();                                                     \
        for (unsigned long seed = 0 + offset; seed < max_seed; seed++) {    \
            if (check) {                                                    \
                gsl_rng_set(rng, seed);                                     \
                double x = sampler;                                         \
                xmin = min(xmin, x);                                        \
                xmax = max(xmax, x);                                        \
            }                                                               \
        }                                                                   \
        elapsed = clock() - time;                                           \
        printf("\"%s\" ", #sampler);                                        \
        printf("%f %f ", xmin, xmax);                                       \
        printf("%f\n", (double)elapsed/CLOCKS_PER_SEC);                     \

    // Run the GSL benchmark.
    if (choice == 0) {
        ENUMERATE_SAMPLER(gsl_ran_cauchy(rng, 1), 2 * seed != max_seed, 0);
        ENUMERATE_SAMPLER(gsl_ran_exponential(rng, 1), 1, 0);
        ENUMERATE_SAMPLER(gsl_ran_flat(rng, .1, 3.14), 1, 0);
        ENUMERATE_SAMPLER(gsl_ran_gumbel1(rng, 1, 1), 1, 1);
        ENUMERATE_SAMPLER(gsl_ran_gumbel2(rng, 1, 1), 1, 1);
        ENUMERATE_SAMPLER(gsl_ran_laplace(rng, 1), 2 * seed != max_seed, 0);
        ENUMERATE_SAMPLER(gsl_ran_logistic(rng, 1), 1, 1);
        ENUMERATE_SAMPLER(gsl_ran_pareto(rng, 3, 2), 1, 1);
        ENUMERATE_SAMPLER(gsl_ran_rayleigh(rng, 1), 1, 1);
        ENUMERATE_SAMPLER(gsl_ran_weibull(rng, 1, 1), 1, 1);
        // These samplers cannot be enumerated.
        printf("\"gsl_ran_gaussian(rng, 1)\" 0 0 0\n");
        printf("\"gsl_ran_gamma(rng, .5, 1)\" 0 0 0\n");
        printf("\"gsl_ran_tdist(rng, 1)\" 0 0 0\n");
        return 0;
    }

    // OPT BENCHMARK
    double cl, ch, sl, sh, el, eh;

    #define QUANTILE_SAMPLER(sampler, i, ...)             \
        float cdf_##i(double x8aea59a2) {                 \
            if (x8aea59a2 != x8aea59a2) { return 1.; }    \
            return sampler##_P(x8aea59a2, ##__VA_ARGS__); \
        }                                                 \
        float sf_##i(double x8aea59a2) {                  \
            if (x8aea59a2 != x8aea59a2) { return 0.; }    \
            return sampler##_Q(x8aea59a2, ##__VA_ARGS__); \
        }                                                 \
        MAKE_DDF(ddf_##i, cdf_##i, sf_##i);               \
        time = clock();                                   \
        bounds_quantile(cdf_##i, &cl, &ch);               \
        bounds_quantile_sf(sf_##i, &sl, &sh);             \
        bounds_quantile_ext(ddf_##i, &el, &eh);           \
        elapsed = clock() - time;                         \
        printf("\"%s(%s)\" ", #sampler, #__VA_ARGS__);    \
        printf("%1.17e %1.17e ", cl, ch);                 \
        printf("%1.17e %1.17e ", sl, sh);                 \
        printf("%1.17e %1.17e ", el, eh);                 \
        printf("%f\n", (double)elapsed/CLOCKS_PER_SEC);   \

    QUANTILE_SAMPLER(gsl_cdf_cauchy, 1, 1);
    QUANTILE_SAMPLER(gsl_cdf_exponential, 2, 1);
    QUANTILE_SAMPLER(gsl_cdf_flat, 3, .1, 3.14);
    QUANTILE_SAMPLER(gsl_cdf_gumbel1, 5, 1, 1);
    QUANTILE_SAMPLER(gsl_cdf_gumbel2, 6, 1, 1);
    QUANTILE_SAMPLER(gsl_cdf_laplace, 7, 1);
    QUANTILE_SAMPLER(gsl_cdf_logistic, 8, 1);
    QUANTILE_SAMPLER(gsl_cdf_pareto, 9, 3, 2);
    QUANTILE_SAMPLER(gsl_cdf_rayleigh, 10, 1);
    QUANTILE_SAMPLER(gsl_cdf_weibull, 11, 1, 1);
    QUANTILE_SAMPLER(gsl_cdf_gaussian, 14, 1);
    QUANTILE_SAMPLER(gsl_cdf_gamma, 16, .5, 1);
    QUANTILE_SAMPLER(gsl_cdf_tdist, 18, 1);
}
