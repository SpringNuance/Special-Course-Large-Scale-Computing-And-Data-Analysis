#include "buffer.h"

#include <math.h>
#include <stdio.h>

#include "errchk.h"
#include "morton.h"
#include "utils.h"

static double
abs_error(const double model, const double candidate)
{
  return fabs(model - candidate);
}

static double
ulp_error(const double model, const double candidate)
{
  const int base = 2;
  const int p    = 53; // Bits in the significant (53 for double)

  const double e   = floor(log(fabs(model)) / log(2));
  const double ulp = pow(base, e - (p - 1));

  return abs_error(model, candidate) / ulp;
}

static void
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

Buffer
buffer_create(const size_t count)
{
  Buffer buffer = (Buffer){
      .count = count,
      .data  = calloc(count, sizeof(buffer.data[0])),
  };
  ERRCHK(buffer.data);

  return buffer;
}

Buffer
buffer_clone(const Buffer in)
{
  Buffer out = buffer_create(in.count);
  for (size_t i = 0; i < in.count; ++i)
    out.data[i] = in.data[i];

  return out;
}

void
buffer_destroy(Buffer* buffer)
{
  buffer->count = 0;

  free(buffer->data);
  buffer->data = NULL;
}

void
buffer_print(const Buffer buffer)
{
  for (size_t i = 0; i < buffer.count; ++i)
    printf("%f\n", buffer.data[i]);
}

void
buffer_compare(const Buffer model, const Buffer candidate)
{
  compare_arrays(model.data, candidate.data, model.count);
}

void
buffer_set(const InitcondType initcond, Buffer* buf)
{
  switch (initcond) {
  case ZERO: {
    for (size_t i = 0; i < buf->count; ++i)
      buf->data[i] = 0.0;
  } break;
  case RANDOM: {
    for (size_t i = 0; i < buf->count; ++i)
      buf->data[i] = randf();
  } break;
  case FLUID: {
    FILE* fp = fopen("../resources/fluidsample.csv", "r");
    ERRCHK(fp);

    for (size_t i = 0; i < buf->count; ++i) {
      double tmp;
      if (fscanf(fp, "%lg\n", &tmp) != 1) {
        rewind(fp);
        ERRCHK(fscanf(fp, "%lg\n", &tmp) == 1);
      }
      buf->data[i] = tmp;
    }

    fclose(fp);
  } break;
  case GAUSSIAN: {
    fprintf(stderr, "GAUSSIAN not yet implemented in buffer_set()");
  } /* Fallthrough */
  default:
    fprintf(stderr, "FATAL ERROR: Invalid initcond passed in buffer_set()\n");
    ERRCHK(0);
  }

  /*
  if (initcond == RANDOM) {
    for (size_t i = 0; i < buf->count; ++i)
      buf->data[i] = randf();
  }
  else if (initcond == FLUID) {
    FILE* fp = fopen("../resources/fluidsample.csv", "r");
    ERRCHK(fp);

    for (size_t i = 0; i < buf->count; ++i) {
      double tmp;
      if (fscanf(fp, "%lf\n", &tmp) != 1) {
        rewind(fp);
        ERRCHK(fscanf(fp, "%lf\n", &tmp) == 1);
      }
      buf->data[i] = tmp;
    }

    fclose(fp);
  }
  else if (initcond == ZERO) {
    for (size_t i = 0; i < buf->count; ++i)
      buf->data[i] = 0.0;
  }
  else
  else {
    fprintf(stderr, "Invalid initcond passed in buffer_set()\n");
    ERRCHK(0);
  }
  */
}

void
buffer_to_file(const Buffer buffer, const char* path)
{
  FILE* fp = fopen(path, "w");
  ERRCHK(fp);

  /*
  // Save in a scan pattern
  const uint2_64 dims = morton_dims(buffer.count);
  for (size_t j = 0; j < dims[1]; ++j)
    for (size_t i = 0; i < dims[0]; ++i)
      fprintf(fp, "%f\n", buffer.data[spatial_to_morton((uint2_64){i, j})]);
  */

  // Save in Z-order
  for (size_t i = 0; i < buffer.count; ++i)
    fprintf(fp, "%.16lg\n", buffer.data[i]);

  fclose(fp);
}

void
abs_error_to_file(const Buffer model, const Buffer candidate, const char* path)
{
  ERRCHK(model.count == candidate.count);

  FILE* fp = fopen(path, "w");
  ERRCHK(fp);

  for (size_t i = 0; i < model.count; ++i)
    fprintf(fp, "%f\n", abs_error(model.data[i], candidate.data[i]));

  fclose(fp);
}

void
ulp_error_to_file(const Buffer model, const Buffer candidate, const char* path)
{
  ERRCHK(model.count == candidate.count);

  FILE* fp = fopen(path, "w");
  ERRCHK(fp);

  for (size_t i = 0; i < model.count; ++i)
    fprintf(fp, "%f\n", ulp_error(model.data[i], candidate.data[i]));

  fclose(fp);
}
