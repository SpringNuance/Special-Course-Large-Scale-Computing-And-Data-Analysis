#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))
#define GEN_RAND_BIN() (rand() >= RAND_MAX / 2)

static inline int
mod(const int a, const int b)
{
  const int r = a % b;
  return r < 0 ? r + b : r;
}

static inline int64_t
clamp(const int64_t x, const int64_t min, const int64_t max)
{
  return x > max ? max : x < min ? min : x;
}

/** Returns a random double in range [0.0, 1.0] */
static inline double
randf(void)
{
  uint64_t x = GEN_RAND_BIN();
  for (size_t i = 0; i < 8 * sizeof(x); ++i)
    x = (x << 1) | GEN_RAND_BIN();

  const double retval = (double)x / (double)(uint64_t)(-1);

  if (retval < 0.0 || retval > 1.0) {
    fprintf(stderr, "Generated random outside [0, 1] range\n");
    exit(EXIT_FAILURE);
  }

  return retval;
}
