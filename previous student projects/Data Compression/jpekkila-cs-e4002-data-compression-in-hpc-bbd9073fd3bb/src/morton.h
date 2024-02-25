#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t uint2_64 __attribute__((vector_size(32)));

uint2_64 morton_to_spatial(const uint64_t idx);

uint64_t spatial_to_morton(const uint2_64 idx);

/** count: number of elements in the space-filling curve */
uint2_64 morton_dims(const size_t count);

/** Return true if self-tests succeed */
bool morton_test(const uint64_t count);
