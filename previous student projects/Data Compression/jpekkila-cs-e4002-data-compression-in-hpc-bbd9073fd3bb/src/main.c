#include <stdio.h>
#include <stdlib.h>

#include "analysis.h"
#include "benchmark.h"
#include "buffer.h"
#include "compress.h"
#include "solver.h"

#include "errchk.h"
#include "timer.h"

/*
static void
swap(Grid** a, Grid** b)
{
  Grid* tmp = *a;
  *a        = *b;
  *b        = tmp;
}
*/

#if 0
int
main(void)
{
  /*
  Grid grid0      = grid_create(nn);
  Grid grid1      = grid_create(nn);

  Grid* input  = &grid0;
  Grid* output = &grid1;

  for (size_t i = 0; i < 100; ++i) {
    update(*input, output);
    grid_to_file(*output, "base", i);

    swap(&input, &output);
  }

  grid_destroy(&grid0);
  grid_destroy(&grid1);
  */

  /*
  Buffer buffer = buffer_create(nn);
  Compressor* c = compressor_create(buffer, 0.001);
  compressor_destroy(c);
  buffer_destroy(&buffer);
  */

  const size_t nn = 64 * 1024 * 1024;
  Buffer buffer   = buffer_create(nn);
  buffer_set(FLUID, &buffer);

  const size_t mem_bandwidth = benchmark_memory_bandwidth();
  const size_t bandwidth     = 40 * 1024 * 1024 * 1024lu;
  const size_t bytes         = sizeof(buffer.data[0]) * nn;

  // TODO this is much faster, is it optimized away or benchmark_compression
  // not optimized due to linking/ABI calls?
  float* tmp = malloc(sizeof(buffer.data[0]) * buffer.count);
  ERRCHK(tmp);

  /*
  const size_t iters = 50;
  Timer t;
  timer_reset(&t);
  for (size_t j = 0; j < iters; ++j) {
    for (size_t i = 0; i < buffer.count; ++i) {
      tmp[i]             = (float)buffer.data[i];
      buffer.data[i]     = (double)tmp[i];
      volatile double aa = (double)tmp[0] * buffer.data[0];
      (void)aa;
    }
  }
  const long ns = timer_diff_nsec(t) / iters;
  free(tmp);
  */

  // const long ns = benchmark_compression(buffer, true);
  const long ns = benchmark_compression(buffer, true) +
                  benchmark_compression(buffer, false);

  printf("Time elapsed: %f ms\n", ns / 1e6);
  printf("Mem fetch time %f ms\n",
         transfer_time(2 * bytes, mem_bandwidth) / 1e6);
  printf("Comm time: %f ms\n", transfer_time(bytes, bandwidth) / 1e6);
  printf("Compression ratio: %f\n", compression_ratio(buffer));
  printf("Memory bandwidth: %f GiB/s\n",
         mem_bandwidth / (1024 * 1024 * 1024.0));

  buffer_destroy(&buffer);
  return EXIT_SUCCESS;
}
#endif

int
main(void)
{
  const size_t nn    = 64 * 64; // 1 * 1024 * 1024;
  const size_t bytes = sizeof(double) * nn;

  /*
  // Simulate
  Grid in  = grid_create(nn);
  Grid out = grid_create(nn);
  for (size_t i = 0; i < 50; ++i) {
    update(in, &out);
    update(out, &in);
    grid_to_file(in, "base", i);
  }
  grid_destroy(&in);
  grid_destroy(&out);
  */

  Buffer buffer = buffer_create(nn);
  buffer_set(FLUID, &buffer);
  // buffer_set(RANDOM, &buffer);

  // Analysis
  const double ex  = expected_value(buffer);
  const double sd  = standard_deviation(buffer);
  const double var = variance(buffer);
  const double rr  = range(buffer);
  printf("Expected value: %f\n", ex);
  printf("Standard deviation: %f\n", sd);
  printf("Variance: %f\n", var);
  printf("Range: %f\n", rr);

  // Benchmark
  const long ns = benchmark_compression(buffer, true) +
                  benchmark_compression(buffer, false);

  const size_t bandwidth = benchmark_memory_bandwidth();
  const double ratio     = compression_ratio(buffer);

  // Estimated network bandwidth
  // MI100 vs infiniband: 1200 GiB/s vs 40 GiB/s
  const size_t nw_bandwidth = (size_t)((40.0 / 1200.0) * bandwidth);

  const double expected_compr_ms = 1e3 * (bytes + ratio * bytes) / bandwidth;
  const double measured_compr_ms = ns / 1e6;

  const double comm_time       = 1e3 * bytes / nw_bandwidth;
  const double compressed_comm = 1e3 * ratio * bytes / nw_bandwidth;
  const double compr_comm_time = (ns / 1e6) + compressed_comm;

  printf("Expected compression time: %f ms\n", expected_compr_ms);
  printf("Measured compression time: %f ms\n", measured_compr_ms);
  printf("Memory bandwidth: %f GiB/s\n", bandwidth / (1024 * 1024 * 1024.0));
  printf("Network bandwidth: %f GiB/s\n",
         nw_bandwidth / (1024 * 1024 * 1024.0));
  printf("Compression ratio: %f\n", ratio);
  printf("Data transfer size: %f MiB\n", bytes / (1024 * 1024.0));
  printf("Expected comm time: %f ms\n", comm_time);
  printf("Expected compressed comm time: %f\n", compressed_comm);
  printf("Expected compression + comm time: %f ms\n", compr_comm_time);
  printf("Speedup: %f\n", comm_time / compr_comm_time);

  // Errors
  Buffer model  = buffer_clone(buffer);
  Compressor* c = compressor_create(&buffer);
  compress(c);
  buffer_set(RANDOM, &buffer);
  decompress(c);
  compressor_destroy(c);
  buffer_compare(model, buffer);

  const double msqe = distortion(model, buffer);
  printf("Distortion (msqe): %g\n", msqe);

  buffer_destroy(&model);
  buffer_destroy(&buffer);

  ERRCHK(0.99 * expected_compr_ms <= measured_compr_ms);
  return EXIT_SUCCESS;
}
