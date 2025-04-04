/*
  Name:     deterministic.h
  Purpose:  GSL compatible random number generator for deterministic state.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#ifndef DETERMINISTIC_H
#define DETERMINISTIC_H

#include <sys/random.h>
#include <gsl/gsl_types.h>

GSL_VAR const gsl_rng_type *gsl_rng_deterministic;

#endif
