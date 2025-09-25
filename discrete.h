/*
  Name:     flip.h
  Purpose:  Generate a sequence of pseudo-random bits.
  Author:   F. A. Saad
  Copyright (C) 2024 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#ifndef DISCRETE_H
#define DISCRETE_H

#include <stddef.h>

/** CDF function wrapper for an array `P` of floats (representing
 * cumulative probabilities). `K` is the length of `P` and `x` is the
 * outcome, i.e., `P[x]` when `x` is an integer `[0, K)`.*/
float cdf_discrete(double x, const float *P, size_t K);

#endif
