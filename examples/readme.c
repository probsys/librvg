#include <gsl/gsl_rng.h>
#include "rvg/generate.h"

int main(int argc, char * argv[]) {

  // Prepare the random number generator.
  gsl_rng * rng = gsl_rng_alloc(gsl_rng_default);
  struct flip_state prng = make_flip_state(rng);

  // Implement A custom continuous distribution, F(x) = x^2 over [0,1].
  float cdf_x2(double x) {
      if      (x != x)        { return 1; }       // nan
      else if (signbit(x))    { return 0; }       // <= -0.0
      else if (1 <= x)        { return 1; }       // >= 1
      else                    { return x*x; }     // x^2 over [0,1]
  }

  // Draw a random variate from cdf_x2.
  double sample = generate_opt(cdf_x2, &prng);
  printf("The sample is: %f\n", sample);

  // Compute some quantiles of cdf_x2.
  double median = quantile(cdf_x2, 0.5);
  double quantile_25 = quantile(cdf_x2, 0.25);
  double quantile_75 = quantile(cdf_x2, 0.75);
  printf("The quantiles are: %f %f %f\n", quantile_25, median, quantile_75);

  // Free the random number generator.
  gsl_rng_free(rng);
}
