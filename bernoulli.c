/**
* @brief        Generate a Bernoulli flip with rational weight
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <gmp.h>

#include "bits.h"
#include "flip.h"
#include "bernoulli.h"

// ================ bernoulli ================

unsigned char bernoulli(uintmax_t k, uintmax_t n, struct flip_state * prng) {
    assert(k < n);
    if (k == 0) { assert(0); return 0; }
    if (k == n) { assert(0); return 1; }

    #ifndef NDEBUG
    uintmax_t j = k;
    #endif

    unsigned char b;
    while (1) {
        k <<= 1;

        if (k == n) {
            return flip(prng);
        }
        if (n <= k) {
            k -= n;
            b = 1;
        } else {
            b = 0;
        }

        #ifndef NDEBUG
        j <<= 1;
        j %= n;
        assert(j == k);
        #endif

        if (flip(prng)) {
            return b;
        }
    }
}

unsigned char bernoulli_gmp(mpz_t k, mpz_t n, struct flip_state * prng) {
    assert(mpz_cmp(k, n) < 0);
    if (mpz_cmp_ui(k, 0) == 0)  { assert(0); return 0; }
    if (mpz_cmp(k, n) == 0)     { assert(0); return 1; }

    #ifndef NDEBUG
    mpz_t j; mpz_init(j); mpz_set(j, k);
    #endif

    unsigned char b;
    while (1) {
        mpz_mul_2exp(k, k, 1); // k <<= 1

        if (mpz_cmp(k, n) == 0) {
             b = flip(prng);
             break;
        }
        if (mpz_cmp(n, k) <= 0) {
            mpz_sub(k, k, n);
            b = 1;
        } else {
            b = 0;
        }

        #ifndef NDEBUG
        mpz_mul_2exp(j, j, 1); // j <<= 1
        mpz_mod(j, j, n);   // j %= n
        assert(mpz_cmp(j, k) == 0);
        #endif

        if (flip(prng)) {
            break;
        }
    }

    #ifndef NDEBUG
    mpz_clear(j);
    #endif

    return b;
}

// ================ sample_random_Em ================

static const union float_bits lo_Emf = {.f = 0.};
static const union float_bits hi_Emf = {.f = 1.};

void sample_random_Emf(
        uint32_t * p_exp,
        uint32_t * p_mant,
        bool exp_offset,
        struct flip_state * prng
        ) {
    assert(lo_Emf.b.exponent == ((lo_Emf.i >> FLT_SIZE_M) & 0xFF));
    assert(hi_Emf.b.exponent == ((hi_Emf.i >> FLT_SIZE_M) & 0xFF));
    // Prepare return values.
    uint32_t mant, exp;
    // Chose random bits and decrement exp until 1 appears.
    uint32_t exp_hi = hi_Emf.b.exponent - 1 - exp_offset;
    uint32_t exp_lo = lo_Emf.b.exponent;
    for (exp = exp_hi; exp > exp_lo; exp--) {
        if (flip(prng)) {
            break;
        }
    }
    // Choose random 23-bit mantissa.
    mant = 0;
    for (int i = 0; i < FLT_SIZE_M; i++) {
        uint32_t b = flip(prng);
        mant = (b << i) | mant;
        // mant = (mant << 1) | b;
    }
    // Set the pointers.
    *p_exp = exp;
    *p_mant = mant;
}

static const union double_bits lo_Em = {.f = 0.};
static const union double_bits hi_Em = {.f = 1.};

void sample_random_Em(
        uint64_t * p_exp,
        uint64_t * p_mant,
        bool exp_offset,
        struct flip_state * prng
        ) {
    // Prepare return values.
    uint64_t mant, exp;
    // Extract exponent fields form lo and hi.
    assert(lo_Em.b.exponent == ((lo_Em.i >> DBL_SIZE_M) & 0x7FF));
    assert(hi_Em.b.exponent == ((hi_Em.i >> DBL_SIZE_M) & 0x7FF));
    // Choose random bits and decrement exp until 1 appears.
    uint64_t exp_hi = hi_Em.b.exponent - 1 - exp_offset;
    uint64_t exp_lo = lo_Em.b.exponent;
    for (exp = exp_hi; exp > exp_lo; exp--) {
        if (flip(prng)) {
            break;
        }
    }
    // Choose random 52-bit mantissa.
    mant = 0;
    for (int i = 0; i < DBL_SIZE_M; i++) {
        uint64_t b = flip(prng);
        mant = (b << i) | mant;
        // mant = (mant << 1) | b;
    }
    // Set the pointers.
    *p_exp = exp;
    *p_mant = mant;
}

// ================ uniform ================

float uniformf(enum round_mode mode, struct flip_state * prng) {
    // Sample mantissa and exponent.
    uint32_t mant, exp;
    sample_random_Emf(&exp, &mant, 0, prng);
    // If RND_N (Downey) and 0 mantissa, move to next exponent range w.p. 0.5
    (mode == RND_N) && (mant == 0) && (flip(prng)) && (++exp);
    // Combine exponent and mantissa.
    union float_bits ans;
    ans.b.sign = 0;
    ans.b.exponent = exp;
    ans.b.mantissa = mant;
    return (mode == RND_U) ? nextafter(ans.f, 1.) : ans.f;
}

double uniform(enum round_mode mode, struct flip_state * prng) {
    // Sample mantissa and exponent.
    uint64_t mant, exp;
    sample_random_Em(&exp, &mant, 0, prng);
    // If RND_N (Downey) and 0 mantissa, move to next exponent range w.p. 0.5
    (mode == RND_N) && (mant == 0) && (flip(prng)) && (++exp);
    // Combine exponent and mantissa.
    union double_bits ans;
    ans.b.sign = 0;
    ans.b.exponent = exp;
    ans.b.mantissa = mant;
    return (mode == RND_U) ? nextafter(ans.f, 1.) : ans.f;
}

// ================ uniform_ext ================

void uniformf_ext(
        bool * d
        , float * q
        , struct flip_state * prng
        ) {
    // Sample mantissa and exponent.
    uint32_t mant, exp;
    sample_random_Emf(&exp, &mant, 1, prng);
    // Combine exponent and mantissa.
    union float_bits ans;
    ans.b.sign = 0;
    ans.b.exponent = exp;
    ans.b.mantissa = mant;
    // Sample direction.
    *d = flip(prng);
    // Determine q according to rule.
    switch (*d) {
        case 0:
            *q = nextafterf(ans.f, 1.);
            assert((0 <= *q) & (*q <= 0.5));
        case 1:
            *q = ans.f;
            assert((0 <= *q) & (*q < 0.5));
    }
}

void uniform_ext(
        bool * d
        , double * q
        , struct flip_state * prng
        ) {
    // Sample mantissa and exponent.
    uint64_t mant, exp;
    sample_random_Em(&exp, &mant, 1, prng);
    // Combine exponent and mantissa.
    union double_bits ans;
    ans.b.sign = 0;
    ans.b.exponent = exp;
    ans.b.mantissa = mant;
    // Sample direction.
    *d = flip(prng);
    // Determine q according to rule.
    switch (*d) {
        case 0:
            *q = nextafter(ans.f, 1.);
            assert((0 <= *q) & (*q <= 0.5));
        case 1:
            *q = ans.f;
            assert((0 <= *q) & (*q < 0.5));
    }
}
