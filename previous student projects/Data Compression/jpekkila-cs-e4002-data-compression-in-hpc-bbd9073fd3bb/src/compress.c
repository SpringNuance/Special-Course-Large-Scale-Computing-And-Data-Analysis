#include "compress.h"

#include <string.h>

#include "errchk.h"

#define ZFP (0)
#define CAST (1)
#define CONTROL (2)
#define XOR (3)
#define COMPRESSOR (CAST)

#if COMPRESSOR == ZFP //--------------------------------------------------------
#include "external/zfp/include/zfp.h"

#define TOLERANCE (1e-3)

struct compressor_s {
  zfp_field* field;
  zfp_stream* zfp;
  bitstream* stream;
  void* buffer;
  size_t buflen;
};

Compressor*
compressor_create(Buffer* buffer)
{
  printf("Creating ZFP compressor\n");

  Compressor* c = malloc(sizeof(Compressor));
  ERRCHK(c);

  c->field = zfp_field_1d(buffer->data, zfp_type_double, buffer->count);
  c->zfp   = zfp_stream_open(NULL);

  zfp_stream_set_accuracy(c->zfp, TOLERANCE);

  // double zfp_stream_set_rate(zfp_stream* stream, double rate, zfp_type
  // type, uint dims, int wra)¶
  // zfp_stream_set_rate(c.zfp, 1.0, zfp_type_double, 1, 0); // Fixed rate
  // zfp_stream_set_precision() // Fixed precision
  // zfp_stream_set_accuracy(c.zfp, TOLERANCE); // Fixed accuracy

  c->buflen = zfp_stream_maximum_size(c->zfp, c->field);
  c->buffer = malloc(c->buflen);
  ERRCHK(c->buffer);

  c->stream = stream_open(c->buffer, c->buflen);
  zfp_stream_set_bit_stream(c->zfp, c->stream);

  return c;
}

void
compressor_destroy(Compressor* c)
{
  zfp_field_free(c->field);
  zfp_stream_close(c->zfp);
  stream_close(c->stream);
  free(c->buffer);
  free(c);
}

size_t
compress(Compressor* c)
{
  zfp_stream_set_execution(c->zfp, zfp_exec_omp);

  zfp_stream_rewind(c->zfp);
  const size_t bytes_compressed = zfp_compress(c->zfp, c->field);
  // printf("Bytes compressed %lu\n", bytes_compressed);
  ERRCHK(bytes_compressed);

  return bytes_compressed;
}

void
decompress(Compressor* c)
{
  // Parallel decompression on CPU not supported
  zfp_stream_set_execution(c->zfp, zfp_exec_serial);

  zfp_stream_rewind(c->zfp);
  const size_t bytes_decompressed = zfp_decompress(c->zfp, c->field);
  ERRCHK(bytes_decompressed);
}

#elif COMPRESSOR == CONTROL //--------------------------------------------------
struct compressor_s {
  Buffer* uncompressed;
  double* compressed;
};

Compressor*
compressor_create(Buffer* buffer)
{
  printf("Creating CONTROL compressor\n");

  Compressor* c = malloc(sizeof(Compressor));
  ERRCHK(c);

  c->uncompressed = buffer;
  c->compressed   = malloc(sizeof(c->compressed[0]) * buffer->count);
  ERRCHK(c->compressed);

  return c;
}

void
compressor_destroy(Compressor* c)
{
  free(c->compressed);
  c->compressed = NULL;

  free(c);
}

size_t
compress(Compressor* c)
{
  const size_t n        = c->uncompressed->count;
  const size_t wordsize = sizeof(c->compressed[0]);
  memcpy(c->compressed, c->uncompressed->data, wordsize * n);
  volatile double tmp = c->compressed[0] + c->uncompressed->data[0];
  (void)tmp;

  return wordsize * c->uncompressed->count;
}

void
decompress(Compressor* c)
{
  const size_t n        = c->uncompressed->count;
  const size_t wordsize = sizeof(c->uncompressed->data[0]);
  ERRCHK(sizeof(c->uncompressed->data[0]) == sizeof(c->compressed[0]));
  memcpy(c->uncompressed->data, c->compressed, wordsize * n);
  volatile double tmp = c->compressed[0] + c->uncompressed->data[0];
  (void)tmp;
}
#elif COMPRESSOR == CAST //-----------------------------------------------------
struct compressor_s {
  Buffer* uncompressed;
  float* compressed;
};

Compressor*
compressor_create(Buffer* buffer)
{
  printf("Creating CAST compressor\n");

  Compressor* c = malloc(sizeof(Compressor));
  ERRCHK(c);

  c->uncompressed = buffer;
  c->compressed   = malloc(sizeof(c->compressed[0]) * buffer->count);
  ERRCHK(c->compressed);

  return c;
}

void
compressor_destroy(Compressor* c)
{
  free(c->compressed);
  c->compressed = NULL;

  free(c);
}

size_t
compress(Compressor* c)
{
  const size_t n = c->uncompressed->count;

#pragma omp parallel for
  for (size_t i = 0; i < n; ++i)
    c->compressed[i] = (float)c->uncompressed->data[i];

  return sizeof(c->compressed[0]) * c->uncompressed->count;
}

void
decompress(Compressor* c)
{
  const size_t n = c->uncompressed->count;
#pragma omp parallel for
  for (size_t i = 0; i < n; ++i)
    c->uncompressed->data[i] = (double)c->compressed[i];
}
#elif COMPRESSOR == XOR //-----------------------------------------------------
#include <stdint.h>

struct compressor_s {
  Buffer* uncompressed;
  void* compressed;
  size_t compressed_bytes;
};

Compressor*
compressor_create(Buffer* buffer)
{
  printf("Creating XOR compressor\n");

  Compressor* c = malloc(sizeof(Compressor));
  ERRCHK(c);

  c->uncompressed = buffer;

  // Allocate for the worst case
  const size_t bytes = sizeof(c->uncompressed[0]) * buffer->count;
  c->compressed      = malloc(bytes);
  ERRCHK(c->compressed);

  return c;
}

void
compressor_destroy(Compressor* c)
{
  free(c->compressed);
  c->compressed = NULL;

  free(c);
}

size_t
compress(Compressor* c)
{
  // TODO
  return 0;
}

void
decompress(Compressor* c)
{
  // TODO
}
#endif

double
compression_ratio(const Buffer in)
{
  Buffer buffer = buffer_clone(in);

  Compressor* cmpr             = compressor_create(&buffer);
  const size_t compressed_size = compress(cmpr);
  compressor_destroy(cmpr);

  const double ratio = (double)compressed_size /
                       (sizeof(buffer.data[0]) * buffer.count);
  ERRCHK(buffer.count > 0);

  buffer_destroy(&buffer);
  return ratio;
}
