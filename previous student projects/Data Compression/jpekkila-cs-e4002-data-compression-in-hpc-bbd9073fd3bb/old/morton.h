#pragma once
#include <stdint.h>

#include "error.h"

typedef struct {
    uint64_t i, j;
} uint2_64;

static inline uint2_64
morton_to_spatial(const uint64_t idx)
{
    uint64_t i, j;
    i = j = 0;

    for (int bit = 0; bit <= 31; ++bit) {
        const uint64_t mask = 0x1l << 2 * bit;
        i |= ((idx & (mask << 0)) >> 1 * bit) >> 0;
        j |= ((idx & (mask << 1)) >> 1 * bit) >> 1;
    }

    return (uint2_64){i, j};
}

static inline uint64_t
spatial_to_morton(const uint2_64 idx)
{
    uint64_t i = 0;

    for (int bit = 0; bit <= 31; ++bit) {
        const uint64_t mask = 0x1l << bit;
        i |= ((idx.i & mask) << 0) << 1 * bit;
        i |= ((idx.j & mask) << 1) << 1 * bit;
    }

    return i;
}

static inline void
test_morton(const uint64_t count)
{
    for (uint64_t i = 0; i < count; ++i) {
        const uint2_64 idx = morton_to_spatial(i);
        ERRCHK(i == spatial_to_morton(idx));
    }
    printf("Morton mapping OK\n");
}