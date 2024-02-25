#include "benchmark.h"

#include "buffer.h"
#include "compress.h"
#include "timer.h"
#include "utils.h"
#include <mpi.h>

#define NUM_SAMPLES (500) // Samples per benchmark

// clang-format off
/*

Compression test
SRUN="srun --account=project_2000403 --gres=gpu:v100:1 --mem=24000 -t 00:14:59 -p gputest -n 1 -N 1 --cpus-per-task=20"

SRUN="srun --account=project_2000403 --gres=gpu:v100:1 --mem=24000 -t 00:14:59 -p gputest -n 1 -N 1 --cpus-per-task=1"

Bandwidth test
SRUN="srun --account=project_2000403 --gres=gpu:v100:2 --mem=24000 -t 00:14:59 -p gputest -n 2 -N 2 --cpus-per-task=1"
*/
// clang-format on

// Random
static void
set_initial_cond(const InitcondType initcond, Buffer* buf)
{
    if (initcond == RANDOM) {
        for (size_t i = 0; i < buf->count; ++i)
            buf->data[i] = randf();
    }
    else if (initcond == FLUID) {
        FILE* fp = fopen("fluidsample.csv", "r");
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
    else {
        for (size_t i = 0; i < buf->count; ++i)
            buf->data[i] = 0.0;
    }
}

long
benchmark_compression(const size_t bytes, const double tolerance,
                      const InitcondType initcond)
{
    const size_t count = bytes / sizeof(double);
    Buffer buf         = buffer_create(count);
    set_initial_cond(initcond, &buf);

    Compressor cmpr = compressor_create(buf, tolerance);

    Timer t;
    timer_reset(&t); // ---------------------------- Timing start

    for (size_t i = 0; i < NUM_SAMPLES; ++i) {
        compress(&cmpr);
        decompress(&cmpr);
    }
    const long ns = timer_diff_nsec(t) / NUM_SAMPLES; // --------- Timing end

    compressor_destroy(&cmpr);
    buffer_destroy(&buf);
    return ns;
}

size_t
compressed_size(const size_t bytes, const double tolerance,
                const InitcondType initcond)
{
    const size_t count = bytes / sizeof(double);
    Buffer buf         = buffer_create(count);
    set_initial_cond(initcond, &buf);

    Compressor cmpr = compressor_create(buf, tolerance);

    const size_t compressed_size = compress(&cmpr);

    compressor_destroy(&cmpr);
    buffer_destroy(&buf);

    return compressed_size;
}

static void
errchk_mpi(const int error)
{
    char estring[MPI_MAX_ERROR_STRING];
    int len    = 0;
    int eclass = 0;
    MPI_Error_class(error, &eclass);
    MPI_Error_string(error, estring, &len);
    printf("Error %d: %s\n", eclass, estring);
    fflush(stdout);
}

long
benchmark_communication(const size_t bytes)
{
    const MPI_Datatype datatype = MPI_DOUBLE;
    const size_t count          = bytes / sizeof(double);
    Buffer sendbuf              = buffer_create(count);
    Buffer recvbuf              = buffer_create(count);

    int nprocs, pid;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    MPI_Barrier(MPI_COMM_WORLD);
    Timer t;
    timer_reset(&t); // ---------------------------- Timing start

    for (size_t i = 0; i < NUM_SAMPLES; ++i) {
        MPI_Barrier(MPI_COMM_WORLD);

        // Post recv
        const size_t num_reqs = 2;
        MPI_Request reqs[num_reqs];

        const int front = mod(pid + 1, nprocs);
        const int back  = mod(pid - 1, nprocs);
        MPI_Irecv(recvbuf.data, count, datatype, back, pid, MPI_COMM_WORLD,
                  &reqs[0]);
        MPI_Isend(sendbuf.data, count, datatype, front, front, MPI_COMM_WORLD,
                  &reqs[1]);
        MPI_Waitall(num_reqs, reqs, MPI_STATUSES_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    const long ns = timer_diff_nsec(t) / NUM_SAMPLES; // --------- Timing end
    MPI_Barrier(MPI_COMM_WORLD);

    buffer_destroy(&sendbuf);
    buffer_destroy(&recvbuf);

    return ns;
}