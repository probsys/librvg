/*
  Name:     urandom.h
  Purpose:  GSL compatible random number generator from /dev/urandom
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#ifndef URANDOM_H
#define URANDOM_H

#include <sys/random.h>
#include <gsl/gsl_types.h>

GSL_VAR const gsl_rng_type *gsl_rng_urandom;

#endif
