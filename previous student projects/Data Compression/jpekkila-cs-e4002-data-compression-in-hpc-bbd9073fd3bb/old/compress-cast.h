#pragma once
#include <stdio.h>
#include <stdlib.h>

static const size_t compression_ratio = 2;

static void
compress(const size_t in_bytes, const double* input, //
         size_t* out_bytes, void* output)
{
    *out_bytes = in_bytes / compression_ratio;

    const size_t count = in_bytes / sizeof(input[0]);
    for (size_t i = 0; i < count; ++i) {
        float* out = (float*)output;
        out[i]     = (float)input[i];
    }

    printf("Compressing\n");
    printf("In bytes: %lu\n", in_bytes);
    printf("Out bytes: %lu\n", *out_bytes);
}

static void
decompress(const size_t in_bytes, const void* input, //
           size_t* out_bytes, double* output)
{
    *out_bytes = in_bytes * compression_ratio;

    const size_t count = in_bytes * compression_ratio / sizeof(output[0]);
    for (size_t i = 0; i < count; ++i) {
        float* in = (float*)input;
        output[i] = (float)in[i];
    }

    printf("Decompressing\n");
    printf("In bytes: %lu\n", in_bytes);
    printf("Out bytes: %lu\n", *out_bytes);
}