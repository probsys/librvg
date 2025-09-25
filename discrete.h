/**
* @file         discrete.h
* @brief        Define a CDF for discrete probability distributions
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef DISCRETE_H
#define DISCRETE_H

#include <stddef.h>

float cdf_discrete(double x, const float *P, size_t K);

#endif
