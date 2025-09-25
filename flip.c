/*
  Name:     flip.c
  Purpose:  Generate a sequence of pseudo-random bits.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <assert.h>
#include <limits.h>
#include <gsl/gsl_rng.h>

#include "arithmetic.h"
#include "flip.h"

const size_t ULLONG_BIT = CHAR_BIT * sizeof(unsigned long long);

inline int get_buffer_size(unsigned long x) {
    return CHAR_BIT * sizeof(long long) - __builtin_clzll(x);
}

int gsl_rng_buffer_size(gsl_rng * rng) {
    unsigned long M_hi = gsl_rng_max(rng);
    return get_buffer_size(M_hi);
}

struct flip_state make_flip_state(gsl_rng * rng){
    unsigned long M_lo = gsl_rng_min(rng);
    unsigned long M_hi = gsl_rng_max(rng);
    int buffer_size = get_buffer_size(M_hi);
    // The RNG range [0, 2^m-1] or [1, 2^m-2] for some 1 <= m <= 64
    // is sufficient to ensure that individual bits are uniformly distributed.
    // If buffer_size = 64, the below checks remain correct even with
    // overflow, because (1ULL<<64 - 1) = 0 - 1 = 1^64.
    if (M_lo == 0) {
        // E.g., gsl_rng_default
        assert(
            ((buffer_size == 64) && M_hi == UINT64_MAX) // urandom.h
            ||
            (M_hi == (1ULL << buffer_size) - 1));
    } else if (M_lo == 1) {
        // E.g., gsl_rng_fishman18
        assert(
            ((buffer_size == 64) && M_hi == UINT64_MAX - 1)
            ||
            (M_hi == (1ULL << buffer_size) - 2));
    } else {
        // E.g., gsl_rng_fishman2x
        assert(0);
    }
    struct flip_state s = {
        .rng = rng,
        .buffer_size = buffer_size,
        .flip_pos = buffer_size,
        .num_flips = 0,
    };
    return s;
}

unsigned char flip(struct flip_state * state) {
    if (state->flip_pos == state->buffer_size) {
        state->buffer = gsl_rng_get(state->rng);
        state->flip_pos = 0;
    }
    unsigned char b = state->buffer & 1;
    state->buffer >>= 1;
    state->flip_pos += 1;
    state->num_flips += 1;
    return b;
}

unsigned long long flip_k(struct flip_state * state, int k) {
    if (state->flip_pos == state->buffer_size) {
        state->buffer = gsl_rng_get(state->rng);
        state->flip_pos = 0;
    }
    unsigned int num_bits_extract = min(k, state->buffer_size - state->flip_pos);
    // unsigned long long b = state->buffer & ((1ULL << num_bits_extract) - 1ULL);
    unsigned long long b = state->buffer & (ULLONG_MAX >> (ULLONG_BIT - num_bits_extract));
    state->buffer >>= num_bits_extract;
    state->flip_pos += num_bits_extract;
    state->num_flips += num_bits_extract;
    return (num_bits_extract == k) ? b :
        (b << (k - num_bits_extract)) + flip_k(state, k - num_bits_extract);
}

unsigned long long randint(struct flip_state * state, int k) {
    unsigned long long n = 0;
    for (int i = 0; i < k; i++) {
        int b = flip(state);
        n <<= 1;
        n += b;
    }
    return n;
}
