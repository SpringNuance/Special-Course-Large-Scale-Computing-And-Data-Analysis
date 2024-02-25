#include "morton.h"

#include "errchk.h"

uint2_64
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

uint64_t
spatial_to_morton(const uint2_64 idx)
{
  uint64_t i = 0;

  for (int bit = 0; bit <= 31; ++bit) {
    const uint64_t mask = 0x1l << bit;
    i |= ((idx[0] & mask) << 0) << 1 * bit;
    i |= ((idx[1] & mask) << 1) << 1 * bit;
  }

  return i;
}

static bool
is_a_power_of_two(const size_t x)
{
  return !(x & (x - 1)) && x;
}

uint2_64
morton_dims(const size_t count)
{
  ERRCHK(is_a_power_of_two(count));

  // Last element position + 1 to get the dimensions
  return morton_to_spatial(count - 1) + (uint2_64){1, 1};
}

bool
morton_test(const uint64_t count)
{
  for (uint64_t i = 0; i < count; ++i) {
    const uint2_64 idx = morton_to_spatial(i);
    ERRCHK(i == spatial_to_morton(idx));
  }
  ERRCHK(is_a_power_of_two(count));
  return 1;
}
