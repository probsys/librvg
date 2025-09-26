/*
  Name:     flip.h
  Purpose:  Generate a sequence of pseudo-random bits.
  Author:   F. Saad
  Copyright (C) 2024 Feras A. Saad.
*/

#ifndef DISCRETE_H
#define DISCRETE_H

#include <stddef.h>

/* Wrap array of cumulative probabilities into a CDF. */
float cdf_discrete(double x, const float *P, size_t K);
#endif
