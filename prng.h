/*
  Name:     prng.h
  Purpose:  GSL compatible random number generators.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#ifndef PRNG_H
#define PRNG_H

#include <sys/random.h>
#include <gsl/gsl_rng.h>

GSL_VAR const gsl_rng_type *gsl_rng_deterministic;
GSL_VAR const gsl_rng_type *gsl_rng_urandom;

#endif
