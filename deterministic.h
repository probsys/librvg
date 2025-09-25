/**
* @file         deterministic.h
* @brief        GSL compatible random number generator for deterministic state
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef DETERMINISTIC_H
#define DETERMINISTIC_H

#include <sys/random.h>
#include <gsl/gsl_rng.h>

GSL_VAR const gsl_rng_type *gsl_rng_deterministic;

#endif
