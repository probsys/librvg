/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory

  Sample uniform random floats in the unit interval.
*/

#ifndef UNIFORM_H
#define UNIFORM_H

#include <stdbool.h>
#include <stdint.h>

#include "flip.h"

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
