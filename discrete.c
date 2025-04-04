/*
  Name:     flip.h
  Purpose:  Generate a sequence of pseudo-random bits.
  Author:   F. A. Saad
  Copyright (C) 2025 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <math.h>
#include "discrete.h"

float cdf_discrete(double x, const float P[], size_t K) {
    if      (x != x)        { return 1.; }       // nan
    else if (signbit(x))    { return 0; }        // <= -0.0
    else if (K <= x)        { return 1; }
    else                    { return P[(size_t)x]; }
}
