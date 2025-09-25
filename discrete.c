/**
* @brief        Define a CDF for discrete probability distributions
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
*/

#include <math.h>
#include "discrete.h"

float cdf_discrete(double x, const float P[], size_t K) {
    if      (x != x)        { return 1.; }       // nan
    else if (signbit(x))    { return 0; }        // <= -0.0
    else if (K <= x)        { return 1; }
    else                    { return P[(size_t)x]; }
}
