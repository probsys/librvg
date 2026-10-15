/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory

  Flip a rational Bernoulli coin.
*/

#ifndef BERNOULLI_H
#define BERNOULLI_H

#include <stdint.h>
#include <gmp.h>

#include "flip.h"

unsigned char bernoulli(uintmax_t k, uintmax_t n, struct flip_state * prng);
unsigned char bernoulli_gmp(mpz_t k, mpz_t n, struct flip_state * prng);

#endif
