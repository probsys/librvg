librvg: C Library for Random Variate Generation with Formal Guarantees
======================================================================

librvg is an open-source C library for generating random variables.
It released under the Apache-2.0 License.

The methods implemented in librvg are described in the following
publication.

  > Random Variate Generation with Formal Guarantees.
  > Feras Saad and Wonyeol Lee.
  > Proceedings of the ACM on Programming Languages 9(PLDI), 2025.

For the latest version, please visit https://github.com/probsys/librvg

Overview
--------

The purpose of librvg is to generate random variates from a given
probability distribution. This probability distribution is specified in
terms of a numerical implementation of its cumulative distribution function
(CDF, [1]), and optionally in terms of a separate numerical implementation
of its survival function (SF, [2]).

  [1] https://en.wikipedia.org/wiki/Cumulative_distribution_function

  [2] https://en.wikipedia.org/wiki/Survival_function

Given a CDF specification as a C function, librvg automatically synthesizes
a function for generating random variates whose probability distribution is
guaranteed to exactly match that of the CDF. This generator is also
guaranteed to be entropy optimal, in the sense that it consumes the least
possible amount of random bits (on average) to produce a random bits.

Programming with librvg
-----------------------

Detailed descriptions of the librvg API can be found in [api.html](api.html).

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
  ```

The primary function provided by librvg is `generate_opt`, which takes as
input a CDF implementation and a prng, and returns as output an exact
random variate drawn from that CDF.

A CDF F takes as input a double x and returns a float. It is valid if:

  1. For all x, F(x) is a float in the unit interval [0, 1].
  2. F(x) = 1, whenever x is NaN (i.e., if x != x).
  3. F is monotonic, i.e., if x' < x then F(x') <= F(x).

A CDF can be manually implemented as in the above example, or imported from
the GNU Scientific Library (GSL, [3]), whose CDF functions are named
`gsl_cdf_[dist]_P`. To use a GSL CDF in librvg, refer to examples/main.c.

  [3] https://www.gnu.org/software/gsl/doc/html/randist.html

Given a CDF, librvg also provides an exact implementation of the
corresponding quantile function (QF, [4]), using `quantile` as shown
in the above example. The quantile takes as input a CDF F and a
float q in [0,1], and returns min{ x | q <= F(x) }. In words, it
is the smallest number x' such that q <= Pr(X <= x').

  [4] https://en.wikipedia.org/wiki/Quantile_function

Installation
------------

This software is tested on Ubuntu 24.04. Other Linux distributions are also
expected to work with minor modifications to the names of the dependencies.
If you use other OS, you can execute the following steps in Docker, as
explained in the below section "Running via Docker".

- Step 1: Install C dependencies:

      $ sudo apt install build-essential libgsl-dev libgmp-dev

- Step 2: Build the library:

      $ make

  The files are installed in
    - `build/lib/librvg.a`  (static library)
    - `build/include/*.h`   (header files)

- Step 3: Run the examples

      $ cd examples/
      $ make
      $ ./main.out
      $ ./readme.out

  To write new programs that use librvg.a, refer to examples/Makefile and
  follow the same build process.

Experiments
-----------

The experiments/ directory contains programs that produce Table 1, Table 2,
and Figure 8 of [SL25]. Running the plotting code requires `python3` to be
available in the path, with the pandas, numpy, and matplotlib libraries
installed. The experiments take around 5-10 minutes each.

- Table 1

      $ cd experiments/
      $ make table1

  The output is `experiments/results.rate/table_1.txt`

- Table 2

      $ cd experiments/
      $ make table2

  The outputs are the .png files in `experiments/results.bounds`

- Figure 8

      $ cd experiments/
      $ make figure8

  The output is `experiments/results.rate/figure_8.png`

Running via Docker
------------------

The above commands can also be executed in a virtual Docker container, by
using the Dockerfile.

To build the container, first install Docker for your platform using
the instructions at https://docs.docker.com/get-started/get-docker/.

Then run:

    $ docker build -f Dockerfile -t librvg .        # build image (~20 mins)
    $ docker run -dit --name librvg librvg:latest   # create container
    $ docker cp librvg:/librvg librvg-build         # extract to ./librvg-build

To launch an interactive terminal in the Docker container, run

    $ docker start librvg
    $ docker attach librvg
