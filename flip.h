/**
* @file         flip.h
* @brief        Generate a sequence of pseudo-random bits
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#ifndef FLIP_H
#define FLIP_H

#include <stdint.h>
#include <gsl/gsl_rng.h>

/** A struct that maintains the state of a sequence of flips. */
struct flip_state {
  gsl_rng * rng;
  unsigned int buffer_size;
  unsigned long buffer;
  unsigned int flip_pos;
  unsigned long num_flips;
};

// Number of bits in unsigned long long.
extern const size_t ULLONG_BIT;

// Number of bits needed to represent `x`.
int get_buffer_size(unsigned long x);

/** Buffer size of `rng`, i.e., number of bits in `gsl_rng_max(rng)`. */
int gsl_rng_buffer_size(gsl_rng * rng);

/** Initialize a `flip_state` using an `rng`. */
struct flip_state make_flip_state(gsl_rng * rng);

/** Generate a single bit from a `flip_state`. */
unsigned char flip(struct flip_state * s);

/** Generate a random `k`-bit number from a `flip_state`. */
unsigned long long flip_k(struct flip_state * s, int k);

/* Generate a random `k`-bit number from a `flip_state`. */
unsigned long long randint(struct flip_state * s, int k);

#endif
