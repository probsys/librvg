/*
  Name:     rate.c
  Purpose:  Analyzing the input and output rate of random variate generators.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/


#include <assert.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#include "rvg/discrete.h"
#include "rvg/generate.h"
#include "rvg/urandom.h"
#include "rvg/deterministic.h"

#include "gsl_wrap.h"

void error (const char * s) {
  fprintf(stderr, "Error: arguments should be %s\n",s) ;
  exit (EXIT_FAILURE) ;
}

int main(int argc, char * argv[]) {

    if (argc < 4) {
        printf (
            "Usage: %s <seed> <n> <dist> <param...> \n"
            "Generates n samples from the distribution DIST with parameters param1,\n"
            "param2, etc. Valid distributions are,\n\n", argv[0]);

        printf(
        "  beta a b\n"
        "  binomial p n\n"
        "  cauchy a\n"
        "  chisq nu\n"
        "  discrete p1 p2 p3 ...\n"
        "  exponential mu\n"
        "  exppow a b\n"
        "  fdist nu1 nu2\n"
        "  flat a b\n"
        "  gamma a b\n"
        "  gaussian sigma\n"
        "  geometric p\n"
        "  gumbel1 a b\n"
        "  gumbel2 a b\n"
        "  hypergeometric n1 n2\n"
        "  laplace a\n"
        "  logistic a\n"
        "  lognormal zeta sigma\n"
        "  negative_binomial p n\n"
        "  pareto a b\n"
        "  pascal p n\n"
        "  poisson mu\n"
        "  rayleigh sigma\n"
        "  tdist nu\n"
        "  weibull a b\n"
        ) ;
        exit(0);
        }

    unsigned long int seed = 0 ;
    size_t num_samples = 0;
    const char * name;
    const char * binary;
    binary = argv[0]     ; argc-- ; argv++ ;
    seed = atol(argv[0]) ; argc-- ; argv++ ;
    num_samples = atol(argv[0])    ; argc-- ; argv++ ;
    name = argv[0]       ; argv++ ; argc-- ;

    gsl_rng_env_setup();
    if (gsl_rng_default_seed != 0) {
        fprintf(
            stderr,
            "overriding GSL_RNG_SEED with command line value, seed = %ld\n",
            seed);
    }

    gsl_rng_default_seed = seed ;
    gsl_rng * rng = gsl_rng_alloc(gsl_rng_urandom_wrap);
    struct flip_state prng = make_flip_state(rng);
    clock_t time, elapsed;

    #define NAME(x) !strcmp(name,(x))
    #define ARGS(x,y) if (argc != x) error(y) ;
    #define DBL_ARG(x) if (argc) { x = atof((argv)[0]); argv++, argc--;} else {error( #x);};
    #define INT_ARG(x) if (argc) { x = atoi((argv)[0]); argv++, argc--;} else {error( #x);};

    #define RUN_EXACT(sampler, cdf) do { \
        gsl_rng_set(prng.rng, seed); \
        prng.num_flips = 0; \
        double elapsed = 0.; \
        clock_t time; \
        for (int i = 0; i < num_samples; i++){ \
          time = clock(); \
          double x_itr = sampler(cdf, &prng); \
          elapsed += clock() - time; \
        } \
        printf("\"%s\" %zu %f %lu\n", #sampler, num_samples, (double)elapsed/CLOCKS_PER_SEC, prng.num_flips); \
        } while (0)

    #define RUN_APPROX(sampler) do { \
        gsl_rng_set(prng.rng, seed); \
        /*printf("%i %lu\n", gsl_rng_buffer_size(rng), gsl_rng_calls(gsl_rng_urandom_wrap, rng));*/ \
        prng.num_flips = 0; \
        double elapsed = 0.; \
        clock_t time; \
        for (int i = 0; i < num_samples; i++){ \
          time = clock(); \
          double x_itr = sampler; \
          elapsed += clock() - time; \
        } \
        printf("\"%s\" %zu %f %lu\n", \
            #sampler, \
            num_samples, \
            (double)elapsed/CLOCKS_PER_SEC, \
            gsl_rng_buffer_size(rng) * gsl_rng_calls(gsl_rng_urandom_wrap, rng)); \
        } while (0)

    #define RUN_EXPERIMENT(cdf, ddf, gsl_sampler)   \
        RUN_APPROX(gsl_sampler);                    \
        RUN_EXACT(generate_opt, cdf);               \
        RUN_EXACT(generate_cbs, cdf);               \
        RUN_EXACT(generate_opt_ext, ddf);           \
        RUN_EXACT(generate_cbs_ext, ddf);

    if (NAME("beta")) {
        ARGS(2, "a,b = shape parameters");
        double a; DBL_ARG(a);
        double b; DBL_ARG(b);
        MAKE_CDF_P(cdf_P, gsl_cdf_beta_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_beta_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_beta(rng, a, b));
    }
    else if (NAME("binomial")) {
        ARGS(2, "p = probability, n = number of trials");
        double p; DBL_ARG(p);
        int n; INT_ARG(n);
        MAKE_CDF_UINT_P(cdf_P, gsl_cdf_binomial_P, p, n);
        MAKE_CDF_UINT_Q(cdf_Q, gsl_cdf_binomial_Q, p, n);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_binomial(rng, p, n));
    }
    else if (NAME("cauchy")) {
        ARGS(1, "a = scale parameter");
        double a; DBL_ARG(a);
        MAKE_CDF_P(cdf_P, gsl_cdf_cauchy_P, a);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_cauchy_Q, a);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_cauchy(rng, a));
    }
    else if (NAME("chisq")) {
        ARGS(1, "nu = degrees of freedom");
        double a; DBL_ARG(a)
        MAKE_CDF_P(cdf_P, gsl_cdf_chisq_P, a);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_chisq_Q, a);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_cauchy(rng, a));
    }
    else if (NAME("exponential")) {
        ARGS(1, "mu = mean value");
        double mu; DBL_ARG(mu) ;
        MAKE_CDF_P(cdf_P, gsl_cdf_exponential_P, mu);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_exponential_Q, mu);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_exponential(rng, mu));
    }
    else if (NAME("exppow")) {
        ARGS(2, "a = scale parameter, b = power (1=exponential, 2=gaussian)");
        double a; DBL_ARG(a);
        double b; DBL_ARG(b);
        MAKE_CDF_P(cdf_P, gsl_cdf_exppow_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_exppow_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_exppow(rng, a, b));
    }
    else if (NAME("fdist")) {
        ARGS(2, "nu1, nu2 = degrees of freedom parameters");
        double nu1; DBL_ARG(nu1);
        double nu2; DBL_ARG(nu2);
        MAKE_CDF_P(cdf_P, gsl_cdf_fdist_P, nu1, nu2);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_fdist_Q, nu1, nu2);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_fdist(rng, nu1, nu2));
    }
    else if (NAME("flat")) {
        ARGS(2, "a = lower limit, b = upper limit");
        double a; DBL_ARG(a) ;
        double b; DBL_ARG(b) ;
        MAKE_CDF_P(cdf_P, gsl_cdf_flat_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_flat_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_flat(rng, a, b));
    }
    else if (NAME("gamma")) {
        ARGS(2, "a = order, b = scale");
        double a; DBL_ARG(a) ;
        double b; DBL_ARG(b) ;
        MAKE_CDF_P(cdf_P, gsl_cdf_gamma_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_gamma_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_gamma(rng, a, b));
    }
    else if (NAME("gaussian")) {
        ARGS(1, "sigma = standard deviation");
        double sigma; DBL_ARG(sigma) ;
        MAKE_CDF_P(cdf_P, gsl_cdf_gaussian_P, sigma);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_gaussian_Q, sigma);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_gaussian(rng, sigma));
    }
    else if (NAME("geometric")) {
        ARGS(1, "p = bernoulli trial probability of success");
        double p; DBL_ARG(p) ;
        MAKE_CDF_UINT_P(cdf_P, gsl_cdf_geometric_P, p);
        MAKE_CDF_UINT_Q(cdf_Q, gsl_cdf_geometric_Q, p);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_geometric(rng, p));
    }
    else if (NAME("gumbel1")) {
        ARGS(2, "a = order, b = scale parameter");
        double a; DBL_ARG(a);
        double b; DBL_ARG(b);
        MAKE_CDF_P(cdf_P, gsl_cdf_gumbel1_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_gumbel1_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_gumbel1(rng, a, b));
    }
    else if (NAME("gumbel2")) {
        ARGS(2, "a = order, b = scale parameter");
        double a; DBL_ARG(a);
        double b; DBL_ARG(b);
        MAKE_CDF_P(cdf_P, gsl_cdf_gumbel2_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_gumbel2_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_gumbel2(rng, a, b));
    }
    else if (NAME("hypergeometric")) {
        ARGS(3, "n1 = tagged population, n2 = untagged population, t = number of trials");
        int n1; INT_ARG(n1);
        int n2; INT_ARG(n2);
        int t; INT_ARG(t);
        MAKE_CDF_UINT_P(cdf_P, gsl_cdf_hypergeometric_P, n1, n2, t);
        MAKE_CDF_UINT_Q(cdf_Q, gsl_cdf_hypergeometric_Q, n1, n2, t);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_hypergeometric(rng, n1, n2 ,t));
    }
    else if (NAME("laplace")) {
        ARGS(1, "a = scale parameter");
        double a; DBL_ARG(a);
        MAKE_CDF_P(cdf_P, gsl_cdf_laplace_P, a);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_laplace_Q, a);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_laplace(rng, a));
    }
    else if (NAME("logistic")) {
        ARGS(1, "a = scale parameter");
        double a; DBL_ARG(a);
        MAKE_CDF_P(cdf_P, gsl_cdf_logistic_P, a);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_logistic_Q, a);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_logistic(rng, a));
    }
    else if (NAME("lognormal")) {
        ARGS(2, "zeta = location parameter, sigma = scale parameter");
        double zeta; DBL_ARG(zeta);
        double sigma; DBL_ARG(sigma);
        MAKE_CDF_P(cdf_P, gsl_cdf_lognormal_P, zeta, sigma);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_lognormal_Q, zeta, sigma);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_lognormal(rng, zeta, sigma));
    }
    else if (NAME("negative_binomial")) {
        ARGS(2, "p = probability, a = order");
        double p; DBL_ARG(p);
        double a; DBL_ARG(a);
        MAKE_CDF_UINT_P(cdf_P, gsl_cdf_negative_binomial_P, p, a);
        MAKE_CDF_UINT_Q(cdf_Q, gsl_cdf_negative_binomial_Q, p, a);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_negative_binomial(rng, p, a));
    }
    else if (NAME("pareto")) {
        ARGS(2, "a = power, b = scale parameter");
        double a; DBL_ARG(a);
        double b; DBL_ARG(b);
        MAKE_CDF_P(cdf_P, gsl_cdf_pareto_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_pareto_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_pareto(rng, a, b));
    }
    else if (NAME("pascal")) {
        ARGS(2, "p = probability, n = order (integer)");
        double p; DBL_ARG(p);
        double N; INT_ARG(N);
        MAKE_CDF_UINT_P(cdf_P, gsl_cdf_pascal_P, p, N);
        MAKE_CDF_UINT_Q(cdf_Q, gsl_cdf_pascal_Q, p, N);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_pascal(rng, p, N));
    }
    else if (NAME("poisson")) {
        ARGS(1, "mu = scale parameter");
        double mu; DBL_ARG(mu);
        MAKE_CDF_UINT_P(cdf_P, gsl_cdf_poisson_P, mu);
        MAKE_CDF_UINT_Q(cdf_Q, gsl_cdf_poisson_Q, mu);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_poisson(rng, mu));
    }
    else if (NAME("rayleigh")) {
        ARGS(1, "sigma = scale parameter");
        double sigma; DBL_ARG(sigma);
        MAKE_CDF_P(cdf_P, gsl_cdf_rayleigh_P, sigma);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_rayleigh_Q, sigma);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_rayleigh(rng, sigma));
    }
    else if (NAME("tdist")) {
        ARGS(1, "nu = degrees of freedom");
        double nu; DBL_ARG(nu);
        MAKE_CDF_P(cdf_P, gsl_cdf_tdist_P, nu);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_tdist_Q, nu);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_tdist(rng, nu));
    }
    else if (NAME("weibull")) {
        ARGS(2, "a = scale parameter, b = exponent");
        double a; DBL_ARG(a);
        double b; DBL_ARG(b);
        MAKE_CDF_P(cdf_P, gsl_cdf_weibull_P, a, b);
        MAKE_CDF_Q(cdf_Q, gsl_cdf_weibull_Q, a, b);
        MAKE_DDF(ddf, cdf_P, cdf_Q);
        RUN_EXPERIMENT(cdf_P, ddf, gsl_ran_weibull(rng, a, b));
    }
    else {
        fprintf(stderr, "Error: unrecognized distribution: %s\n", name);
        fprintf(stderr, "Usage: %s <seed> <n> <dist> <param...> \n", binary);
        exit(0);
    }

    gsl_rng_free(rng);
}
