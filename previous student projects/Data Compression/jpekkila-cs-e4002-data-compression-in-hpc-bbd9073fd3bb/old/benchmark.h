#pragma once
#include <stdlib.h>

typedef enum { RANDOM, FLUID, ALLZERO, NUM_INITCONDS } InitcondType;

/** Returns the time in nanoseconds */
long benchmark_compression(const size_t bytes, const double tolerance,
                           const InitcondType initcond);

/** Returns the time in nanoseconds */
long benchmark_communication(const size_t bytes);

/** Returns the compressed size in bytes */
size_t compressed_size(const size_t bytes, const double tolerance,
                       const InitcondType initcond);