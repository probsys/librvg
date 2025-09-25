/**
* @file         urandom.h
* @brief        GSL compatible random number generator from /dev/urandom
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef URANDOM_H
#define URANDOM_H

#include <sys/random.h>
#include <gsl/gsl_rng.h>

/** @brief const */
GSL_VAR const gsl_rng_type *gsl_rng_urandom;

#endif
