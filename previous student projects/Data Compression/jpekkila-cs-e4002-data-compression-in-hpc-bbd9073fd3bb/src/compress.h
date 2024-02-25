#pragma once
#include "buffer.h"

typedef struct compressor_s Compressor;

Compressor* compressor_create(Buffer* buffer);

void compressor_destroy(Compressor* c);

/** Returns the compressed size in bytes */
size_t compress(Compressor* c);

void decompress(Compressor* c);

/** Returns the compression ratio (uncompressed size / compressed size) */
double compression_ratio(const Buffer buffer);
