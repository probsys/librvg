/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory
*/

#include <math.h>
#include "discrete.h"

float cdf_discrete(double x, const float P[], size_t K) {
    if      (x != x)        { return 1.; }       // nan
    else if (signbit(x))    { return 0; }        // <= -0.0
    else if (K <= x)        { return 1; }
    else                    { return P[(size_t)x]; }
}
