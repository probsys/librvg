/*
  Name:     main.c
  Purpose:  Example use of the librvg library.
  Author:   F. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>

#include "rvg/generate.h"
#include "rvg/discrete.h"

int main(int argc, char * argv[]) {

    // Prepare the random number generator.
    gsl_rng * rng = gsl_rng_alloc(gsl_rng_default);
    struct flip_state prng = make_flip_state(rng);

    double sample;

    // EXAMPLE 1: Gaussian distribution from the GNU library (continuous).
    MAKE_CDF_P(gaussian_cdf, gsl_cdf_gaussian_P, 1);
    MAKE_CDF_Q(gaussian_sf, gsl_cdf_gaussian_Q, 1);
    MAKE_DDF(gaussian_ddf, gaussian_cdf, gaussian_sf);

    sample = generate_opt(gaussian_cdf, &prng);
    printf("%f\n", sample);

    sample = generate_opt_ext(gaussian_ddf, &prng);
    printf("%f\n", sample);

    // EXAMPLE 2: Poisson distribution from the GNU library (discrete).
    MAKE_CDF_UINT_P(poisson_cdf, gsl_cdf_poisson_P, 5);
    MAKE_CDF_UINT_Q(poisson_sf, gsl_cdf_poisson_Q, 5);
    MAKE_DDF(poisson_ddf, poisson_cdf, poisson_sf);

    sample = generate_opt(poisson_cdf, &prng);
    printf("%f\n", sample);

    sample = generate_opt_ext(poisson_ddf, &prng);
    printf("%f\n", sample);

    // EXAMPLE 3: A custom discrete distribution.
    float P[4] = {0.1, 0.3, 0.5, 0.8};
    MAKE_CDF_UINT_P(custom_discrete_cdf, cdf_discrete, P, 4);
    sample = generate_opt(custom_discrete_cdf, &prng);
    printf("%f\n", sample);

    // EXAMPLE 4: A custom continuous distribution, F(x) = x^2 over [0,1].
    float custom_continuous_cdf(double x) {
        if      (x != x)        { return 1.; }       // nan
        else if (signbit(x))    { return 0; }        // <= -0.0
        else if (1 <= x)        { return 1; }
        else                    { return x*x; }
    }
    printf("%f\n", generate_opt(custom_continuous_cdf, &prng));

    // EXAMPLE 5: A custom CDF that is a point mass at NaN.
    float custom_delta_cdf(double x) {
        return (x != x) ? 1. : 0.;
    }
    printf("%f\n", generate_opt(custom_delta_cdf, &prng));

    // Free the random number generator.
    gsl_rng_free(rng);

}
