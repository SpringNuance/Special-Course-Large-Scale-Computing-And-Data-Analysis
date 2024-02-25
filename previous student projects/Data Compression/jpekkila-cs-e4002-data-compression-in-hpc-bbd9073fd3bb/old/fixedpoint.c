#include "fixedpoint.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define IS_POWER_OF_TWO(val) ((val & (val - 1)) == 0)
#define QN (60) // The fractional bits N of QM.N

/** Converts a double in range [-8.0, 8.0] to Q3.60 fixed-point format */
int64_t
dbl2q(const double x)
{
    if (x > 8.0 || x < -8.0) {
        fprintf(stderr, "Input double %.17g outside [-8, 8] range\n", x);
        exit(EXIT_FAILURE);
    }
    return round(x * (0x1l << QN));
}

/** Converts a Q3.60 fixed-point number into a double in range [-8, 8] */
double
q2dbl(const int64_t x)
{
    return x * (1.0 / (0x1l << QN));
}

/** Converts a double to QM.N fixed-point format */
int64_t
dbl2qn(const double x, const uint8_t n)
{
    if (n > 63) {
        fprintf(stderr, "Invalid number of fractional bits %u\n", n);
        exit(EXIT_FAILURE);
    }

    const size_t m      = 64 - n - 1; // 64 - fract bits - sign bit
    const int64_t range = 0x1 << m;
    if (x > range || x < -range) {
        fprintf(stderr, "Input double %.17g outside [%ld, %ld] range\n", x,
                -range, range);
        exit(EXIT_FAILURE);
    }
    return round(x * (0x1l << n));
}

/** Converts a QM.N fixed-point number into a double */
double
qn2dbl(const int64_t x, const uint8_t n)
{
    if (n > 63) {
        fprintf(stderr, "Invalid number of fractional bits %u\n", n);
        exit(EXIT_FAILURE);
    }

    return x * (1.0 / (0x1l << n));
}

void
print_binary(const double val)
{
    const int64_t x = *(int64_t*)(&val);
    for (size_t i = 63; i < 64; --i) {
        printf("%d", (x & (0x1l << i)) > 0);
        if (i % 8 == 0)
            printf(" ");
    }
    printf("\n");
}

uint8_t
count_rightmost_zeros(const uint64_t x)
{
    for (uint8_t i = 0; i < 64; ++i)
        if (x & (0x1l << i))
            return i;
}