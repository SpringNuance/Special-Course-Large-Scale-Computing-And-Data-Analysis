#include "buffer.h"

#include "morton.h"

#if USE_CUDA
#include <cuda.h>
#include <cuda_runtime_api.h>
#endif

Buffer
buffer_create(const size_t count)
{
    Buffer buffer = (Buffer){.count = count};

#if USE_CUDA
    cudaMallocManaged((void**)&buffer.data, count * sizeof(buffer.data[0]),
                      cudaMemAttachGlobal);
#else
    buffer.data = calloc(count, sizeof(buffer.data[0]));
#endif
    ERRCHK(buffer.data);

    return buffer;
}

void
buffer_destroy(Buffer* buffer)
{
    buffer->count = 0;
#if USE_CUDA
    cudaFree(buffer->data);
#else
    free(buffer->data);
#endif
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
buffer_to_file(const Buffer buffer, const char* path)
{
    FILE* fp = fopen(path, "w");
    ERRCHK(fp);

    /*
        // TODO NOTE: does translate to row order from morton atm!
        for (size_t i = 0; i < buffer.count; ++i)
            fprintf(fp, "%f\n", buffer.data[i]);
            */
    for (size_t j = 0; j < 64; ++j)
        for (size_t i = 0; i < 64; ++i)
            fprintf(fp, "%f\n",
                    buffer.data[spatial_to_morton((uint2_64){i, j})]);

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
    // TODO

    FILE* fp = fopen(path, "w");
    ERRCHK(fp);

    for (size_t i = 0; i < model.count; ++i)
        fprintf(fp, "%f\n", ulp_error(model.data[i], candidate.data[i]));

    fclose(fp);
}