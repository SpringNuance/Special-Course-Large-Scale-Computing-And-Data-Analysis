#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERROR(str)                                                             \
    {                                                                          \
        time_t terr;                                                           \
        time(&terr);                                                           \
        fprintf(stderr, "%s", ctime(&terr));                                   \
        fprintf(stderr, "\tError in file %s line %d: %s\n", __FILE__,          \
                __LINE__, str);                                                \
        fflush(stderr);                                                        \
        exit(EXIT_FAILURE);                                                    \
        abort();                                                               \
    }

#define WARNING(str)                                                           \
    {                                                                          \
        time_t terr;                                                           \
        time(&terr);                                                           \
        fprintf(stderr, "%s", ctime(&terr));                                   \
        fprintf(stderr, "\tWarning in file %s line %d: %s\n", __FILE__,        \
                __LINE__, str);                                                \
        fflush(stderr);                                                        \
    }

#define ERRCHK(retval)                                                         \
    {                                                                          \
        if (!(retval))                                                         \
            ERROR(#retval " was false");                                       \
    }
#define WARNCHK(retval)                                                        \
    {                                                                          \
        if (!(retval))                                                         \
            WARNING(#retval " was false");                                     \
    }

static inline double
abs_error(const double model, const double candidate)
{
    return fabs(model - candidate);
}

static inline double
ulp_error(const double model, const double candidate)
{
    const int base = 2;
    const int p    = 53; // Bits in the significant (53 for double)

    const double e   = floor(log(fabs(model)) / log(2));
    const double ulp = pow(base, e - (p - 1));

    return abs_error(model, candidate) / ulp;
}

static inline void
compare_arrays(const double* model, const double* candidate, const size_t count)
{
    double largest_abs = 0.0;
    double largest_ulp = 0.0;

    for (size_t i = 0; i < count; ++i) {

        const double abs_err = abs_error(model[i], candidate[i]);
        if (abs_err > largest_abs)
            largest_abs = abs_err;

        const double ulp_err = ulp_error(model[i], candidate[i]);
        if (ulp_err > largest_ulp)
            largest_ulp = ulp_err;

        /*
        printf("%-18s%f\n", "Model:", model[i]);
        printf("%-18s%f\n", "Candidate:", candidate[i]);
        printf("%-18s%g\n", "Abs error:", abs_err);
        printf("%-18s%g\n", "ULP error:", ulp_err);
        printf("%-18s10^%g\n\n", "Error magnitude:", floor(log10(abs_err)));
        */
    }

    printf("Largest abs error: %g\n", largest_abs);
    printf("Largest ulp error: %g\n", largest_ulp);
}