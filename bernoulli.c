/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory
*/

#include <assert.h>
#include <stdint.h>
#include <gmp.h>

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
