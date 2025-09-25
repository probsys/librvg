/*
  Name:     deterministic.c
  Purpose:  GSL compatible random number generator for deterministic state.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <sys/random.h>
#include <gsl/gsl_rng.h>

#include "prng.h"

/* This generator deterministically returns its state and does not evolve.

    It is primarily useful for analyzing and profiling. */

static inline unsigned long int deterministic_get (void *vstate);
static double deterministic_get_double (void *vstate);
static void deterministic_set (void *state, unsigned long int s);

typedef struct {
    // No state is maintained.
    unsigned int x;
} deterministic_state_t;

static inline unsigned long int
deterministic_get (void *vstate) {
    deterministic_state_t *state = (deterministic_state_t *) vstate;
    return state -> x;
}

static double
deterministic_get_double (void *vstate) {
    return deterministic_get (vstate) / 4294967296. ;
}

static void
deterministic_set (void *vstate, unsigned long int s) {
    deterministic_state_t *state = (deterministic_state_t *) vstate;
    state->x = s;
}

static const gsl_rng_type deterministic_type = {
    "deterministic",               /* name */
    0xffffffffU,                   /* RAND_MAX */ /* [fsaad]: (2 ^ 32) - 1 */
    0,                             /* RAND_MIN */
    sizeof (deterministic_state_t),
    &deterministic_set,
    &deterministic_get,
    &deterministic_get_double
};

const gsl_rng_type *gsl_rng_deterministic = &deterministic_type;
