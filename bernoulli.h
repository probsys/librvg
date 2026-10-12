/*
  Name:     bernoulli.h
  Purpose:  Flip a rational Bernoulli coin.
  Author:   F. Saad
  Copyright (C) 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef BERNOULLI_H
#define BERNOULLI_H

#include <stdbool.h>
#include <stdint.h>
#include <gmp.h>

#include "flip.h"

unsigned char bernoulli(uintmax_t k, uintmax_t n, struct flip_state * prng);
unsigned char bernoulli_gmp(mpz_t k, mpz_t n, struct flip_state * prng);

void sample_random_Emf(uint32_t * p_exp, uint32_t * p_mant, bool exp_offset, struct flip_state * prng);
void sample_random_Em(uint64_t * exp, uint64_t * mant, bool exp_offset, struct flip_state * prng);

/** IEEE 754 Floating-Point Arithmetic Rounding Modes */
enum round_mode {
    RND_U, ///< Round Up
    RND_D, ///< Round Down
    RND_N, ///< Round Nearest
    };

/// Sample a uniform float in [0,1] with the given rounding mode.
float uniformf(enum round_mode mode, struct flip_state * prng);
/// Sample a uniform double in [0,1] with the given rounding mode.
double uniform(enum round_mode mode, struct flip_state * prng);

void uniformf_ext(bool * d, float * q, struct flip_state * prng);
void uniform_ext(bool * d, double * q, struct flip_state * prng);

#endif
