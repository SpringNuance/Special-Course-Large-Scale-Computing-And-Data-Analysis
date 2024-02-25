#pragma once
#include "buffer.h"
#include "external/zfp/include/zfp.h"

#if USE_CUDA
// TODO
typedef struct {
    zfp_field* field;
    zfp_stream* zfp;
    bitstream* stream;
    void* buffer;
    size_t buflen;
} Compressor;
#else
typedef struct {
    zfp_field* field;
    zfp_stream* zfp;
    bitstream* stream;
    void* buffer;
    size_t buflen;
} Compressor;
#endif

Compressor compressor_create(const Buffer io, const double tolerance);

void compressor_destroy(Compressor* c);

/** Returns the buffer size in bytes after compression */
size_t compress(Compressor* c);

void decompress(Compressor* c);