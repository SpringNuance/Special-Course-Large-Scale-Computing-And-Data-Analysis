#include <stdint.h>

int64_t dbl2q(const double x);

double q2dbl(const int64_t x);

int64_t dbl2qn(const double x, const uint8_t n);

double qn2dbl(const int64_t x, const uint8_t n);

void print_binary(const double val);

uint8_t count_rightmost_zeros(const uint64_t x);