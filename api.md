<!--
  This file was generated from "api.md.in" by mkdoc.py
  DO NOT EDIT
-->

API for librvg
==============

Author: [Feras A. Saad](https://cs.cmu.edu/~fsaad)

This document describes the API for librvg. Background on the
mathematical details are given in the following publication [SL25]:

  > Random Variate Generation with Formal Guarantees.
  > Feras Saad and Wonyeol Lee.
  > Proceedings of the ACM on Programming Languages 9(PLDI), 2025.

The API functions of librvg is documented using the following format.

> ------------
> <pre>
> return_type function_name(parameters ...)
> </pre>
>  -  English description.

To generate random variates, librvg operates using numerical specifications
of cumulative distribution functions (CDF) and survival functions (SF), which
are both available as the following type in `generate.h`:

    typedef float  (*cdf32_t)(double x);

In words, `cdf32_t` is a function type that takes as input a double `x`.
When used as a CDF, it returns a float representing Pr(X <= x). When used
as an SF, it returns a float representing Pr(X > x).

An advanced feature of librvg is the notion of a "dual distribution
function" (DDF), which combines a numerical CDF and SF to enable extended
accuracy sampling. This representation is described in Section 6 of [SL25].
The type of a DDF is given in `generate.h` as

    typedef void (*ddf32_t)(double x, bool * b, float * p);

The function type `ddf32_t` takes as input a double `x`, and writes the
result of evaluating a DDF at x to the pointers `b` and `p`.
The interpretation of `b` and `p` are as follows:

  - If `b` is set to 0, then `Pr(X<=x) = p`
  - If `b` is set to 1, then `Pr(X<=x) = 1 - p`

Functions that operate using a DDF have an `_ext` suffix (meaning "extended").

API in [`generate.h`](./generate.h)
-----------------------------------

### Main Interface

------------
<a id="generate_opt"></a><pre>
double <a href="generate.h#L39">generate_opt</a>(cdf32_t cdf, struct flip_state * prng);
</pre>
 -  Generate random variables optimally from `cdf`.

------------
<a id="generate_opt_ext"></a><pre>
double <a href="generate.h#L42">generate_opt_ext</a>(ddf32_t ddf, struct flip_state * prng);
</pre>
 -  Generate random variables optimally from `ddf`.

------------
<a id="quantile"></a><pre>
double <a href="generate.h#L45">quantile</a>(cdf32_t cdf, float q);
</pre>
 -  Compute the exact `q`-quantile of the `cdf`, where `q` must be in [0,1].

------------
<a id="quantile_sf"></a><pre>
double <a href="generate.h#L48">quantile_sf</a>(cdf32_t sf, float q);
</pre>
 -  Compute the exact `q`-quantile of the `sf`, where `q` must be in [0,1].

------------
<a id="quantile_ext"></a><pre>
double <a href="generate.h#L51">quantile_ext</a>(ddf32_t ddf, bool d, float q);
</pre>
 -  Compute the exact `q`-quantile of the `ddf`, where `q` must be in [0,1].

------------
<a id="bounds_quantile"></a><pre>
void <a href="generate.h#L54">bounds_quantile</a>(cdf32_t cdf, double * xlo, double * xhi);
</pre>
 -  Compute the exact lower `xlo` and upper `xhi` bound of `cdf`.

------------
<a id="bounds_quantile_sf"></a><pre>
void <a href="generate.h#L57">bounds_quantile_sf</a>(cdf32_t sf, double * xlo, double * xhi);
</pre>
 -  Compute the exact lower `xlo` and upper `xhi` bound of `sf`.

------------
<a id="bounds_quantile_ext"></a><pre>
void <a href="generate.h#L60">bounds_quantile_ext</a>(ddf32_t ddf, double * xlo, double * xhi);
</pre>
 -  Compute the exact lower `xlo` and upper `xhi` bound of `ddf`.



### Importing a CDF or SF from the GNU Scientific Library

librvg can interporate with existing CDF or SF implementations from the GNU
Scientific Library (GSL, [1]). In the GSL, the CDF are named
`gsl_cdf_[dist]_P` and SF are named `gsl_cdf_[dist]_Q`.

  [1] https://www.gnu.org/software/gsl/doc/html/randist.html

The following macros are available:

  ```c
  // Make a cumulative distribution function over doubles from the GSL.
  #define MAKE_CDF_P(name, func, ...)

  // Make a survival distribution function over doubles from the GSL.
  #define MAKE_CDF_Q(name, func, ...)

  // Make a cumulative distribution function over unsigned integers from the GSL.
  #define MAKE_CDF_UINT_P(name, func, ...)

  // Make a survival distribution function over unsigned integers from the GSL.
  #define MAKE_CDF_UINT_Q(name, func, ...)

  // Make a dual distribution function.
  #define MAKE_DDF(ddf, cdf, sf)
  ```

Refer to examples/main.c for an illustration of how to use these macros.

The `MAKE_DDF` macro is not specific to a `cdf` or `sf` created from the
GSL, it can be used for any such pairing.

### Baseline Exact Generators

librvg also includes implementations of the Conditional Bit Sampling
method. These functions are included for illustrative purposes only, and is
described in Section II.B of the following paper:

> Pseudonoise with Arbitrary Amplitude Distribution--Part I: Theory
> John S. Sobolewski; W. H. Payne
> IEEE Transactions on Computers (Volume: C-21, Issue: 4, April 1972)
> https://doi.org/10.1109/TC.1972.5008973

Their performance is generally inferior to that of `generate_opt` in terms
of entropy consumption and runtime, as it uses arbitrary precision
arithmetic via the The GNU MP Bignum Library (libgmp).

------------
<a id="generate_cbs"></a><pre>
double <a href="generate.h#L63">generate_cbs</a>(cdf32_t cdf, struct flip_state * prng);
</pre>
 -  Generate random variables from `cdf` using Conditional Bit Sampling.

------------
<a id="generate_cbs_ext"></a><pre>
double <a href="generate.h#L66">generate_cbs_ext</a>(ddf32_t ddf, struct flip_state * prng);
</pre>
 -  Generate random variables from `ddf` using Conditional Bit Sampling.




API in [`flip.h`](./flip.h)
---------------------------

The random variate generators produced by librvg consume random bits one at
a time. This design is in contrast to traditional random variate
generators, which always consume one or more full machine words of random
bits per output variate. The following functions can be used to create a
stream of random bits from a pseudorandom number generator from the GNU
Scientific Library.

------------
<a id=""></a><pre>
<a href="flip.h#L17"></a>struct flip_state;
</pre>
 -  A struct that maintains the state of a sequence of flips.

------------
<a id="gsl_rng_buffer_size"></a><pre>
int <a href="flip.h#L32">gsl_rng_buffer_size</a>(gsl_rng * rng);
</pre>
 -  Buffer size of `rng`, i.e., number of bits in `gsl_rng_max(rng)`.

------------
<a id="make_flip_state"></a><pre>
struct flip_state <a href="flip.h#L35">make_flip_state</a>(gsl_rng * rng);
</pre>
 -  Initialize a `flip_state` using an `rng`.

------------
<a id="flip"></a><pre>
unsigned char <a href="flip.h#L38">flip</a>(struct flip_state * s);
</pre>
 -  Generate a single bit from a `flip_state`.

------------
<a id="flip_k"></a><pre>
unsigned long long <a href="flip.h#L41">flip_k</a>(struct flip_state * s, int k);
</pre>
 -  Generate a random `k`-bit number from a `flip_state`.


