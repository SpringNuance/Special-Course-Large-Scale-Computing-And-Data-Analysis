#include "compress.h"

#if USE_CUDA
Compressor
compressor_create(const Buffer io, const double tolerance)
{
    Compressor c;

    c.field = zfp_field_1d(io.data, zfp_type_double, io.count);
    c.zfp   = zfp_stream_open(NULL);

    zfp_stream_set_accuracy(c.zfp, tolerance);

    // double zfp_stream_set_rate(zfp_stream* stream, double rate, zfp_type
    // type, uint dims, int wra)¶
    // zfp_stream_set_rate(c.zfp, 1.0, zfp_type_double, 1, 0); // Fixed rate
    // zfp_stream_set_precision() // Fixed precision
    // zfp_stream_set_accuracy(c.zfp, tolerance); // Fixed accuracy

    c.buflen = zfp_stream_maximum_size(c.zfp, c.field);
    c.buffer = malloc(c.buflen);
    ERRCHK(c.buffer);

    c.stream = stream_open(c.buffer, c.buflen);
    zfp_stream_set_bit_stream(c.zfp, c.stream);

    return c;
}

void
compressor_destroy(Compressor* c)
{
    zfp_field_free(c->field);
    zfp_stream_close(c->zfp);
    stream_close(c->stream);
    free(c->buffer);
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
#else
Compressor
compressor_create(const Buffer io, const double tolerance)
{
    Compressor c;

    c.field = zfp_field_1d(io.data, zfp_type_double, io.count);
    c.zfp   = zfp_stream_open(NULL);

    zfp_stream_set_accuracy(c.zfp, tolerance);

    // double zfp_stream_set_rate(zfp_stream* stream, double rate, zfp_type
    // type, uint dims, int wra)¶
    // zfp_stream_set_rate(c.zfp, 1.0, zfp_type_double, 1, 0); // Fixed rate
    // zfp_stream_set_precision() // Fixed precision
    // zfp_stream_set_accuracy(c.zfp, tolerance); // Fixed accuracy

    c.buflen = zfp_stream_maximum_size(c.zfp, c.field);
    c.buffer = malloc(c.buflen);
    ERRCHK(c.buffer);

    c.stream = stream_open(c.buffer, c.buflen);
    zfp_stream_set_bit_stream(c.zfp, c.stream);

    return c;
}

void
compressor_destroy(Compressor* c)
{
    zfp_field_free(c->field);
    zfp_stream_close(c->zfp);
    stream_close(c->stream);
    free(c->buffer);
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
#endif