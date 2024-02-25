#include "analysis.h"

#include <math.h>

#include "errchk.h"

double
expected_value(const Buffer buffer)
{
  double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
  for (size_t i = 0; i < buffer.count; ++i)
    sum += buffer.data[i];

  return sum / buffer.count;
}

double
variance(const Buffer buffer)
{
  const double mu = expected_value(buffer);

  double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
  for (size_t i = 0; i < buffer.count; ++i)
    sum += (buffer.data[i] - mu) * (buffer.data[i] - mu);

  return sum / buffer.count;
}

double
standard_deviation(const Buffer buffer)
{
  return sqrt(variance(buffer));
}

static inline double
min(const double a, const double b)
{
  return a < b ? a : b;
}

static inline double
max(const double a, const double b)
{
  return a > b ? a : b;
}

double
range(const Buffer buffer)
{
  ERRCHK(buffer.count > 0);

  double hi = buffer.data[0];
  double lo = buffer.data[0];

  for (size_t i = 1; i < buffer.count; ++i) {
    hi = max(hi, buffer.data[i]);
    lo = min(lo, buffer.data[i]);
  }

  return hi - lo;
}

double
distortion(const Buffer model, const Buffer candidate)
{
  ERRCHK(model.count == candidate.count);

  double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
  for (size_t i = 0; i < model.count; ++i)
    sum += (model.data[i] - candidate.data[i]) *
           (model.data[i] - candidate.data[i]);

  return sum / model.count;
}
