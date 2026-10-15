/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory

  Cumulative distribution function from an array of cumulative probabilities.
*/

#ifndef DISCRETE_H
#define DISCRETE_H

#include <stddef.h>

/* Wrap array of cumulative probabilities into a CDF. */
float cdf_discrete(double x, const float *P, size_t K);
#endif
