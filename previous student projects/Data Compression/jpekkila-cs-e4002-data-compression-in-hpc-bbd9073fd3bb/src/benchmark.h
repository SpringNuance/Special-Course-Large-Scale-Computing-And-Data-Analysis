#pragma once
#include <stdbool.h>
#include <stdlib.h>

#include "buffer.h"

/** Returns the time in nanoseconds. If do_compress is true, benchmarks the
 * compression time, otherwise decompression */
long benchmark_compression(const Buffer in, const bool do_compress);

/** Returns the time in nanoseconds to transfer some bytes at certain bandwidth.
Bandwidth must be given in bytes per second */
long transfer_time(const size_t bytes, const size_t bandwidth);

/** Returns the memory bandwidth as bytes per second */
size_t benchmark_memory_bandwidth(void);
