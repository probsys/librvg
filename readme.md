librvg: C Library for Random Variate Generation with Formal Guarantees
======================================================================

[![Build](https://github.com/probsys/librvg/actions/workflows/build.yml/badge.svg)](https://github.com/probsys/librvg/actions/workflows/build.yml)
[![Documentation](https://img.shields.io/badge/docs-latest-blue.svg)](https://probsys.github.io/librvg/)
[![Git Tag](https://img.shields.io/github/v/tag/probsys/librvg)](https://github.com/probsys/librvg/tags)
[![License](https://img.shields.io/github/license/probsys/librvg?color=lightgrey)](https://github.com/probsys/librvg/blob/main/LICENSE.txt)

librvg is an open-source C library for generating random variables.
It is released under the Apache-2.0 License.

The methods implemented in librvg are described in the following
publication.

  > Feras A. Saad and Wonyeol Lee. 2025.
  > Random Variate Generation with Formal Guarantees.
  > Proc. ACM Program. Lang. 9, PLDI, Article 152 (June 2025), 25 pages.
  > https://doi.org/10.1145/3729251

For the latest version, please visit https://github.com/probsys/librvg

Overview
--------

The purpose of librvg is to generate random variates from a given
probability distribution. This probability distribution is specified in
terms of a numerical implementation of its
[cumulative distribution function](https://en.wikipedia.org/wiki/Cumulative_distribution_function)
(CDF), and optionally in terms of a separate numerical implementation of its
[survival function](https://en.wikipedia.org/wiki/Survival_function) (SF).

Given a CDF specification as a C function, librvg automatically synthesizes
a function for generating random variates whose probability distribution is
guaranteed to exactly match that of the CDF. This generator is also
guaranteed to be entropy optimal, in the sense that it consumes the least
possible amount of random bits (on average) to produce a random bits.

Documentation
-------------

Please visit https://probsys.github.io/librvg/

Programming with librvg
-----------------------

The following is a brief synopsis of how to get started with the main tasks
supported by the library.

  ```c
  #include <gsl/gsl_rng.h>
  #include "rvg/generate.h"

  int main(int argc, char * argv[]) {

  // Prepare the random number generator.
  gsl_rng * rng = gsl_rng_alloc(gsl_rng_default);
  struct flip_state prng = make_flip_state(rng);

  // Implement a custom continuous distribution, F(x) = x^2 over [0,1].
  float cdf_x2(double x) {
      if      (x != x)        { return 1; }       // nan
      else if (signbit(x))    { return 0; }       // <= -0.0
      else if (1 <= x)        { return 1; }       // >= 1
      else                    { return x*x; }     // x^2 over [0,1]
  }

  // Draw a random variate from cdf_x2.
  double sample = generate_opt(cdf_x2, &prng);
  printf("The sample is: %f\n", sample);

  // Compute some quantiles of cdf_x2.
  double median = quantile(cdf_x2, 0.5);
  double quantile_25 = quantile(cdf_x2, 0.25);
  double quantile_75 = quantile(cdf_x2, 0.75);
  printf("The quantiles are: %f %f %f\n", quantile_25, median, quantile_75);

  // Free the random number generator.
  gsl_rng_free(rng);
  }
