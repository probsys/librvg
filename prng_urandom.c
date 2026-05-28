/*
  Name:     urandom.c
  Purpose:  GSL compatible random number generator from /dev/urandom
  Author:   F. Saad
  Copyright (C) 2025 CMU Probabilistic Computing Systems Lab
*/

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/random.h>
#include <gsl/gsl_rng.h>

#include "prng.h"

/* This generator is a lightweight wrapper over getrandom(2).

    https://man7.org/linux/man-pages/man2/getrandom.2.html */

static inline unsigned long int urandom_get (void *vstate);
static double urandom_get_double (void *vstate);
static void urandom_set (void *state, unsigned long int s);

typedef struct {
    // No state is maintained.
} urandom_state_t;

static inline unsigned long int
urandom_get (void *vstate) {
    unsigned char buffer[sizeof(uint64_t)];
    size_t offset = 0;
    while (offset < sizeof(buffer)) {
        ssize_t nread = getrandom(buffer + offset, sizeof(buffer) - offset, 0);
        if (nread == -1) {
            if (errno == EINTR) { continue; }
            abort();
        }
        if (nread == 0) { abort(); }
        offset += (size_t)nread;
    }
    uint64_t value = 0;
    memcpy(&value, buffer, sizeof(buffer));
    return value;
}

static double
urandom_get_double (void *vstate) {
  return urandom_get (vstate) / 18446744073709551616.0 ;
}

static void
urandom_set (void *vstate, unsigned long int s) {
  return;
}

static const gsl_rng_type urandom_type = {
    "urandom",                     /* name */
    0xffffffffffffffffUL,          /* RAND_MAX */
    0,                             /* RAND_MIN */
    sizeof (urandom_state_t),
    &urandom_set,
    &urandom_get,
    &urandom_get_double
};

const gsl_rng_type *gsl_rng_urandom = &urandom_type;
