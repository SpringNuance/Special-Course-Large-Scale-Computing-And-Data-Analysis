#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"
#include "buffer.h"
#include "compress.h"
#include "error.h"
#include "fixedpoint.h"
#include "grid.h"
#include "morton.h"
#include "solver.h"
#include "timer.h"
#include "utils.h"
#include "visualize.h"

#include <mpi.h>

static void
comm(void)
{
    int nprocs, pid;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    printf("nprocs, pid: %d, %d\n", nprocs, pid);

    const MPI_Datatype datatype = MPI_INT;
    int data[]                  = {-1, -1, -1};

    Timer t;
    MPI_Barrier(MPI_COMM_WORLD); // SYNC ------------------------------
    if (!pid)
        timer_reset(&t);         // TIMER
    MPI_Barrier(MPI_COMM_WORLD); // SYNC ------------------------------

    // Post recv
    const int front = mod(pid + 1, nprocs);
    const int back  = mod(pid - 1, nprocs);

    MPI_Request reqs[4];
    MPI_Irecv(&data[0], 1, datatype, back, pid, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(&data[2], 1, datatype, front, pid, MPI_COMM_WORLD, &reqs[1]);

    // Compress
    data[1] = pid; // PLACEHOLDER

    // Send
    MPI_Isend(&data[1], 1, datatype, back, back, MPI_COMM_WORLD, &reqs[2]);
    MPI_Isend(&data[1], 1, datatype, front, front, MPI_COMM_WORLD, &reqs[3]);

    // Sync
    MPI_Waitall(ARRAY_SIZE(reqs), reqs, MPI_STATUSES_IGNORE);

    MPI_Barrier(MPI_COMM_WORLD); // SYNC ------------------------------
    if (!pid)
        timer_diff_print(t);     // TIMER
    MPI_Barrier(MPI_COMM_WORLD); // SYNC ------------------------------

    for (int i = 0; i < nprocs; ++i) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (i == pid) {
            printf("Pid %d data:\n", pid);
            for (size_t j = 0; j < ARRAY_SIZE(data); ++j)
                printf("\t%d\n", data[j]);
        }
        fflush(stdout);
    }
}

static void
print_size(const size_t bytes)
{
    if (bytes < 1024)
        printf("%lu %s", bytes, "bytes");
    else if (bytes < 1024 * 1024)
        printf("%lu %s", bytes / 1024, "KiB");
    else if (bytes < 1024 * 1024 * 1024)
        printf("%lu %s", bytes / (1024 * 1024), "MiB");
    else
        printf("%lu %s", bytes / (1024 * 1024 * 1024), "GiB");
}

static void
print_time(const long ns)
{
    if (ns < 1000)
        printf("%.2f %s", ns / 1e1, "ns");
    else if (ns < 1000000)
        printf("%.2f %s", ns / 1e3, "us");
    else if (ns < 1000000000)
        printf("%.2f %s", ns / 1e6, "ms");
    else
        printf("%.2f %s", ns / 1e9, "s");
}

#if 1
static void
compress_grid(Grid* grid, const double tolerance)
{
    for (size_t i = 0; i < NUM_FIELDS; ++i) {
        Compressor c = compressor_create(grid->field[i], tolerance);
        compress(&c);
        decompress(&c);
        compressor_destroy(&c);
    }
}

static double
get_ulp(const double range)
{
    const int base = 2;
    const int p    = 53; // Bits in the significant (53 for double)

    const double e   = floor(log(fabs(range)) / log(2));
    const double ulp = pow(base, e - (p - 1));

    return ulp;
}

static void
analyze_block_delta(const Grid grid, const size_t blocksize)
{
    const size_t count      = grid.field[LNRHO].count;
    const size_t blockcount = count / blocksize;
    ERRCHK(blockcount * blocksize == count);

    const double* data = grid.field[LNRHO].data;

    for (size_t block = 0; block < blockcount; ++block) {
        double full = -INFINITY;
        for (size_t i = 0; i < blocksize; ++i) {
            const size_t idx = i + block * blocksize;
            if (data[idx] > full)
                full = data[idx];
        }

        for (size_t i = 0; i < blocksize; ++i) {
            const size_t idx = i + block * blocksize;
            if (data[idx] == full)
                continue;

            const double delta = full - data[idx];
            if (delta > 1e-16)
                printf("Delta %g\n", delta);
        }
    }
}

static void
analyze_spatial_deltas(const Grid in, const size_t window_size)
{
    const size_t count = in.field[LNRHO].count;
    const double* data = in.field[LNRHO].data;

    size_t bins[16]        = {0};
    const size_t bin_count = ARRAY_SIZE(bins);

    double max_delta = 0.0;
    double avg_delta = 0.0;
    double min_delta = INFINITY;

    for (size_t i = 0; i + window_size - 1 < count; i += window_size) {

        const double base = data[i];
        double avg        = 0.0;
        for (size_t j = 1; j < window_size; ++j) {
            const double delta = fabs(base - data[i + j]);

            avg += delta;

            if (delta > max_delta)
                max_delta = delta;
            if (delta < min_delta)
                min_delta = delta;

            size_t bin = fabs(floor(log10(delta)));
            printf("Delta, bin: %g, %lu\n", delta, bin);
            if (bin >= bin_count)
                bin = bin_count - 1;
            ERRCHK(bin >= 0);
            ERRCHK(bin < bin_count);
            ++bins[bin];
        }
        avg /= (window_size - 1);
        avg_delta += avg;
    }
    const size_t window_count = count / window_size;
    avg_delta /= window_count;

    const double ulp = get_ulp(1.0);
    printf("--SPATIAL DELTAS----\n");
    printf("Max delta: %g (%g ulps)\n", max_delta, max_delta / ulp);
    printf("Avg delta: %g (%g ulps)\n", avg_delta, avg_delta / ulp);
    printf("Min delta: %g (%g ulps)\n", min_delta, min_delta / ulp);

    printf("Diff buckets (10^-n):\n");
    for (size_t bin = 0; bin < bin_count; ++bin) {
        printf("%lu, %g\n", bin,
               (double)bins[bin] / (window_count * (window_size - 1)));
    }
    printf("--------\n");
}

static void
analyze_temporal_deltas(const Grid in, const Grid out)
{
    const size_t count   = in.field[LNRHO].count;
    const double* data_a = in.field[LNRHO].data;
    const double* data_b = out.field[LNRHO].data;

    size_t bins[16]        = {0};
    const size_t bin_count = ARRAY_SIZE(bins);

    double max_delta = 0.0;
    double avg_delta = 0.0;
    double min_delta = INFINITY;
    for (size_t i = 0; i < count; ++i) {
        const double a = data_a[i];
        const double b = data_b[i];

        const double delta = fabs(a - b);
        avg_delta += delta;

        if (delta > max_delta)
            max_delta = delta;
        if (delta < min_delta)
            min_delta = delta;

        size_t bin = fabs(floor(log10(delta)));
        if (bin >= bin_count)
            bin = bin_count - 1;
        ERRCHK(bin >= 0);
        ERRCHK(bin < bin_count);
        ++bins[bin];
    }
    avg_delta /= count;
    const double ulp = get_ulp(1.0);
    printf("---TEMPORAL DELTAS---\n");
    printf("Max delta: %g (%g ulps)\n", max_delta, max_delta / ulp);
    printf("Avg delta: %g (%g ulps)\n", avg_delta, avg_delta / ulp);
    printf("Min delta: %g (%g ulps)\n", min_delta, min_delta / ulp);

    printf("Diff buckets (10^-n):\n");
    for (size_t bin = 0; bin < bin_count; ++bin) {
        printf("%lu, %g\n", bin, (double)bins[bin] / count);
    }
    printf("--------\n");
}

static void
test_fixedpoint(void)
{
    for (size_t i = 0; i < 1000; ++i) {
        const double r = 16.0 * randf() - 8.0;
        printf("%.17g -> %ld -> %.17g\n", r, dbl2q(r), q2dbl(dbl2q(r)));
        ERRCHK(r == q2dbl(dbl2q(r)));
    }

    printf("Precision test:\n");
    for (size_t i = 1; i < 1e64; i *= 2) {
        const double r = 1.0 / i;
        printf("binary representations, r, dbl2q, ...\n");
        print_binary(r);
        print_binary(r * (0x1l << 60));
        print_binary(dbl2q(r));
        print_binary((long)(r * (0x1l << 60)) * (1.0 / (0x1l << 60)));
        print_binary(q2dbl(dbl2q(r)));
        // NOTE! Quantization errors when r smaller than the representable
        // range with Q3.60 (min ~1e-18, max 8.0)

        printf("%.17g -> %ld -> %.17g\n", r, dbl2q(r), q2dbl(dbl2q(r)));
        if (r != q2dbl(dbl2q(r))) {
            fprintf(stderr, "Fixed-point precision limit at %g\n", r);
            break;
        }
    }
}

static void
grid_fixed_conversion(Grid* grid)
{
    for (size_t j = 0; j < NUM_FIELDS; ++j) {
        const size_t count = grid->field[j].count;
        for (size_t i = 0; i < count; ++i) {
            const double x         = grid->field[j].data[i];
            grid->field[j].data[i] = q2dbl(dbl2q(x));
            /*
            if (x != grid->field[j].data[i]) {
                fprintf(stderr, "Fixed-point mismatch %.17g, %.17g\n", x,
                        grid->field[j].data[i]);
            }
            ERRCHK(x == grid->field[j].data[i]);
            */
        }
    }
}

static void
test_randf(void)
{
    for (size_t i = 0; i < 1000; ++i) {
        const double r = randf();
        printf("%.17g\n", r);
        ERRCHK(r <= 1.0);
        ERRCHK(r >= 0.0);
    }
}

static void
grid_drop_bits(const size_t bits, Grid* grid)
{
    for (size_t j = 0; j < NUM_FIELDS; ++j)
        for (size_t i = 0; i < grid->field[j].count; ++i) {
            const uint64_t shifted = (*(uint64_t*)(&grid->field[j].data[i]) >>
                                      bits)
                                     << bits;
            grid->field[j].data[i] = *(double*)(&shifted);
        }
}

/** Adds noise after dropping some least-significant bits iff any of the
 * fractional bits to the left are set */
static void
grid_drop_bits_and_add_noise(const size_t bits, Grid* grid)
{
    for (size_t j = 0; j < NUM_FIELDS; ++j)
        for (size_t i = 0; i < grid->field[j].count; ++i) {
            uint64_t nonzero = (*(uint64_t*)(&grid->field[j].data[i]) >> bits)
                               << bits; // Drop a part of the fractional bits
            nonzero = (nonzero << 12) >> 12; // Drop sign & exponent bits

            uint64_t shifted = (*(uint64_t*)(&grid->field[j].data[i]) >> bits);
            if (nonzero)
                for (size_t bit = 0; bit < bits; ++bit)
                    shifted = (shifted << 1) | (rand() >= RAND_MAX / 2);
            else
                shifted <<= bits;

            grid->field[j].data[i] = *(double*)(&shifted);
        }
}

static void
grid_downcast(Grid* grid)
{
    for (size_t j = 0; j < NUM_FIELDS; ++j)
        for (size_t i = 0; i < grid->field[j].count; ++i)
            grid->field[j].data[i] = (float)grid->field[j].data[i];
}

/*
// Kinda works but doesn't do deltas properly
static void
grid_deltacompress(Grid* grid)
{
    const size_t blocksize = 16;
    const size_t count     = grid->field[0].count;
    const size_t blocks    = count / blocksize;
    ERRCHK(blocks * blocksize == count);

    for (size_t j = 0; j < NUM_FIELDS; ++j)
        for (size_t block = 0; block < blocks; ++block) {
            int64_t largest = INT64_MIN;
            for (size_t elem = 0; elem < blocksize; ++elem) {
                const size_t idx = elem + block * blocksize;
                const int64_t x  = log10(grid->field[j].data[idx]);

                if (x > largest)
                    largest = x;
            }
            if (largest == INT64_MIN)
                largest = 0;
            // printf("Largest exp %ld\n", largest);

            for (size_t elem = 0; elem < blocksize; ++elem) {
                const size_t idx = elem + block * blocksize;
                double x         = grid->field[j].data[idx];

                if (largest)
                    x /= largest;

                const uint8_t qn    = 61;
                const int64_t fixed = dbl2qn(x, qn);
                // printf("rightmost zeroes %u\n",
                // count_rightmost_zeros(fixed));
                x = qn2dbl(fixed, qn);

                if (largest)
                    x *= largest;

                // if (grid->field[j].data[idx] != x)
                //    printf("%.17g vs. %.17g\n", grid->field[j].data[idx], x);
                grid->field[j].data[idx] = x;
            }
        }
}
*/

static void
grid_deltacompress(Grid* grid)
{
    const size_t blocksize = 16;
    const size_t count     = grid->field[0].count;
    const size_t blocks    = count / blocksize;
    ERRCHK(blocks * blocksize == count);

    for (size_t j = 0; j < NUM_FIELDS; ++j)
        for (size_t block = 0; block < blocks; ++block) {
            int16_t largest = INT16_MIN;
            for (size_t elem = 0; elem < blocksize; ++elem) {
                const size_t idx = elem + block * blocksize;
                const uint64_t x = *(uint64_t*)(&grid->field[j].data[idx]);

                const int16_t exponent = ((x << 1) >> 53) - 1023; // 2^e
                if (exponent > largest)
                    largest = exponent;
            }
            ERRCHK(largest != -1022 && largest != 1024);
            const size_t exponent = printf("Largest exp %d\n", largest);

            for (size_t elem = 0; elem < blocksize; ++elem) {
                const size_t idx = elem + block * blocksize;
                double x         = grid->field[j].data[idx];
                printf("TODO WIP\n");
            }
        }
}

// Quality
int
main(void)
{
    test_randf();
    test_fixedpoint();

    const size_t nn = 64 * 64;
    Grid base_in    = grid_create(nn);
    Grid base_out   = grid_create(nn);

    Grid a_in  = grid_create(nn);
    Grid a_out = grid_create(nn);

    Grid b_in  = grid_create(nn);
    Grid b_out = grid_create(nn);

    Grid c_in  = grid_create(nn);
    Grid c_out = grid_create(nn);

    for (size_t i = 0; i < 150; ++i) {
        update(base_in, &base_out);
        update(base_out, &base_in);

        // analyze_spatial_deltas(base_out, 4);
        // analyze_temporal_deltas(base_in, base_out);
        // analyze_block_delta(base_out, 4);

        const size_t compression_ratio = 4;
        const size_t bits              = 8 * sizeof(double) -
                            (8 * sizeof(double)) / compression_ratio;
        printf("bits dropped %lu\n", bits);
        update(a_in, &a_out);
        // grid_drop_bits(bits, &a_out);
        // grid_drop_bits_and_add_noise(bits, &a_out);
        grid_downcast(&a_out);
        // grid_deltacompress(&a_out);
        // grid_fixed_conversion(&a_out);
        // compress_grid(&a_out, 1e-12);
        update(a_out, &a_in);
        // grid_drop_bits(bits, &a_in);
        // grid_drop_bits_and_add_noise(bits, &a_in);
        // grid_downcast(&a_in);
        // grid_fixed_conversion(&a_in);
        // compress_grid(&a_in, 1e-12);

        update(b_in, &b_out);
        compress_grid(&b_out, 1e-8);
        update(b_out, &b_in);
        compress_grid(&b_in, 1e-8);

        update(c_in, &c_out);
        compress_grid(&c_out, 1e-4);
        update(c_out, &c_in);
        compress_grid(&c_in, 1e-4);

        printf("0.75:\n");
        compare_arrays(base_in.field[LNRHO].data, a_in.field[LNRHO].data,
                       base_in.field[LNRHO].count);
        printf("0.5:\n");
        compare_arrays(base_in.field[LNRHO].data, b_in.field[LNRHO].data,
                       base_in.field[LNRHO].count);
        printf("0.25:\n");
        compare_arrays(base_in.field[LNRHO].data, c_in.field[LNRHO].data,
                       base_in.field[LNRHO].count);

        if (i % 1 == 0) {
            grid_to_file(base_in, "base", i);
            grid_to_file(a_in, "a", i);
            grid_to_file(b_in, "b", i);
            grid_to_file(c_in, "c", i);
        }
    }

    grid_destroy(&base_in);
    grid_destroy(&base_out);

    grid_destroy(&a_in);
    grid_destroy(&a_out);

    grid_destroy(&b_in);
    grid_destroy(&b_out);

    grid_destroy(&c_in);
    grid_destroy(&c_out);

    /*
    // Solver test
    {
        const size_t nn = 64 * 64;
        Grid input      = grid_create(nn);
        Grid output     = grid_create(nn);

        grid_randomize(&input);
        for (size_t i = 0; i < 100; ++i) {
            update(input, &output);
            update(output, &input);

            if (i % 1 == 0)
                grid_to_file(output);
        }

        grid_destroy(&input);
        grid_destroy(&output);
    }
    */
}
#endif

#if 0
// Performance
int
main(void)
{
    MPI_Init(NULL, NULL);
    int nprocs, pid;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    const size_t bytes = 64 * 64 * sizeof(double);

    printf(
        "initcond, tolerance, compression ratio, compression time (ns), comm "
        "time (ns)\n");
    for (size_t initcond = 0; initcond < NUM_INITCONDS; ++initcond) {

        char initcond_str[128] = "";
        switch (initcond) {
        case ALLZERO:
            strcpy(initcond_str, "All-zero");
            break;
        case FLUID:
            strcpy(initcond_str, "Fluid");
            break;
        case RANDOM:
            strcpy(initcond_str, "Random");
            break;
        }

        for (int mult = 4; mult <= 12; mult += 4) {

#if USE_CUDA
            const double tolerance = mult;
#else
            const double tolerance = powf(10.0, -mult);
#endif

            const long compr_time = benchmark_compression(bytes, tolerance,
                                                          initcond);
            const long comm_time  = benchmark_communication(bytes);

            long compr_bytes   = compressed_size(bytes, tolerance, initcond);
            const double ratio = (double)compr_bytes / bytes;
            /*
                // Uhh this was a weird bug. Because of random data, nodes
                // would have different compression rates and the size of
               the
                // received data would be unpredictable
                long compr_bytes = compressed_size(bytes);
                MPI_Bcast(&compr_bytes, 1, MPI_LONG, 0, MPI_COMM_WORLD);
                const long comm_time = benchmark_communication(compr_bytes);
                */

            if (!pid) {
                printf("%s, %g, %f, %lu, %lu\n", initcond_str, tolerance, ratio,
                       compr_time, comm_time);

                /*
                printf("Chunk size: ");
                print_size(bytes);
                printf("\n");

                printf("nsecs compression: ");
                print_time(compr);
                printf("\n");
                printf("nsecs communication: ");
                print_time(comm);
                printf("\n");
                printf("nsecs compressed communication: ");
                print_time(compr_comm);
                printf("\n");

                printf("Compression ratio: %f\n", (double)compr_bytes / bytes);
                */
            }
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
#endif

#if 0
// Initial drafts
int
main(void)
{
    MPI_Init(NULL, NULL);
    int nprocs, pid;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    // for (size_t bytes = 8; bytes <= 128 * 1024 * 1024; bytes *= 2) {
    for (size_t bytes = 128 * 1024; bytes <= 128 * 1024; bytes *= 2) {

        const long compr = benchmark_compression(bytes);
        const long comm  = benchmark_communication(bytes);

        // Uhh this was a weird bug. Because of random data, nodes
        // would have different compression rates and the size of the
        // received data would be unpredictable
        long compr_bytes = compressed_size(bytes);
        MPI_Bcast(&compr_bytes, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        const long compr_comm = benchmark_compression(bytes) +
                                benchmark_communication(compr_bytes);

        if (!pid) {
            printf("Chunk size: ");
            print_size(bytes);
            printf("\n");

            printf("nsecs compression: ");
            print_time(compr);
            printf("\n");
            printf("nsecs communication: ");
            print_time(comm);
            printf("\n");
            printf("nsecs compr + comm: ");
            print_time(compr_comm);
            printf("\n");
        }
    }

    // comm();

    // Compressor test
    {

        const size_t count = 64;
        Buffer a, b;
        a = buffer_create(count);
        b = buffer_create(count);

        for (size_t i = 0; i < count; ++i)
            a.data[i] = b.data[i] = randf();

        const double tolerance = 1e-3;

        Compressor compressor = compressor_create(b, tolerance);
        compress(&compressor);
        decompress(&compressor);
        compressor_destroy(&compressor);

        buffer_compare(a, b);
        buffer_to_file(a, "data/data0.csv");
        buffer_to_file(b, "data/data1.csv");
        abs_error_to_file(a, b, "data/data2.csv");
        ulp_error_to_file(a, b, "data/data3.csv");

        buffer_destroy(&a);
        buffer_destroy(&b);
    }

    // Solver test
    {
        const size_t nn = 64 * 64;
        Grid input      = grid_create(nn);
        Grid output     = grid_create(nn);

        grid_randomize(&input);
        for (size_t i = 0; i < 1000; ++i) {
            update(input, &output);
            update(output, &input);
        }
        grid_to_file(output);
        visualize(output);

        grid_destroy(&input);
        grid_destroy(&output);
    }

    /*
    int nprocs, pid;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    if (!pid) {
        FILE* a = fopen("data/ground-truth.csv", "w");
        FILE* b = fopen("data/compressed.csv", "w");
        if (!a || !b)
            return EXIT_FAILURE;

        const size_t dim = 8;
        for (size_t j = 0; j < dim; ++j)
            for (size_t i = 0; i < dim; ++i) {
                fprintf(a, "%f\n", randf());
                fprintf(b, "%f\n", randf());
            }

        fclose(a);
        fclose(b);
    }
    */

    /*
    test_morton(128);

#define NX (64)
#define NY (64)
#define IDX(i, j) spatial_to_morton((uint2_64){mod(i, NX), mod(j, NY)})
    printf("%lu -1, -1\n", IDX(-1, -1));
    */

    MPI_Finalize();
    return EXIT_SUCCESS;
}
#endif
