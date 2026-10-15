/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory

  GSL compatible random number generators.
*/

#ifndef PRNG_H
#define PRNG_H

#include <sys/random.h>
#include <gsl/gsl_rng.h>

GSL_VAR const gsl_rng_type *gsl_rng_deterministic;
GSL_VAR const gsl_rng_type *gsl_rng_urandom;

#endif
