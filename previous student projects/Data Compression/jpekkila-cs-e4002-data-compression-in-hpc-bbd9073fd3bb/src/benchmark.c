#include "benchmark.h"

#include <string.h>

#include "buffer.h"
#include "compress.h"
#include "errchk.h"
#include "timer.h"
#include "utils.h"

#define NUM_SAMPLES (10) // Samples per benchmark

long
benchmark_compression(const Buffer in, const bool do_compress)
{
  Buffer buffer    = buffer_clone(in);
  Compressor* cmpr = compressor_create(&buffer);

  // Warmup
  for (size_t i = 0; i < 3; ++i)
    if (do_compress)
      compress(cmpr);
    else
      decompress(cmpr);

  Timer t;
  timer_reset(&t); // ---------------------------- Timing start

  for (size_t i = 0; i < NUM_SAMPLES; ++i)
    if (do_compress)
      compress(cmpr);
    else
      decompress(cmpr);

  const long ns = timer_diff_nsec(t) / NUM_SAMPLES; // --------- Timing end

  compressor_destroy(cmpr);
  buffer_destroy(&buffer);
  return ns;
}

long
transfer_time(const size_t bytes, const size_t bandwidth)
{
  return (long)1e9 * bytes / bandwidth;
}

size_t
benchmark_memory_bandwidth(void)
{
  const size_t bytes = 256 * 1024 * 1024lu;
  const size_t count = bytes / sizeof(double);
  ERRCHK(count * sizeof(double) == bytes);

  double* in = malloc(bytes);
  ERRCHK(in);

  double* out = malloc(bytes);
  ERRCHK(out);

  for (size_t i = 0; i < count; ++i) {
    in[i]  = 0xc0ffee;
    out[i] = 0xc0ffee;
  }

  // Warmup
  for (size_t i = 0; i < 3; ++i)
    memcpy(out, in, bytes);

  Timer t;
  timer_reset(&t);
  memcpy(out, in, bytes);
  volatile double tmp = out[0]; // Prevent optimizing memcpy
  (void)tmp;
  const double seconds = timer_diff_nsec(t) / 1e9;

  free(in);
  free(out);
  return (size_t)(2 * bytes / seconds);
}
