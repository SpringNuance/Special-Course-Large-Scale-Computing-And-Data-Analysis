#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

#ifndef USE_CUDA
#define USE_CUDA (0)
#endif

typedef struct {
    size_t count;
    double* data;
} Buffer;

Buffer buffer_create(const size_t count);

void buffer_destroy(Buffer* buffer);

void buffer_print(const Buffer buffer);

void buffer_compare(const Buffer model, const Buffer candidate);

void buffer_to_file(const Buffer buffer, const char* path);

void abs_error_to_file(const Buffer model, const Buffer candidate,
                       const char* path);

void ulp_error_to_file(const Buffer model, const Buffer candidate,
                       const char* path);