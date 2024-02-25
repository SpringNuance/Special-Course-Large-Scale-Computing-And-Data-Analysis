# README #

This repository is for studying how communication bottlenecks can be reduced in HPC workloads via high-quality lossy compression.


# Motivation

Network bandwidth is a significant bottleneck in communication-heavy HPC applications. The problem is accentuated with the fact that the yearly rate of growth in operational performance is notably higher than the growth in bandwidth. It seems inevitable that applications that are not already communication-bound, will be so on future HPC systems. Data compression/decompression is a natural way to reduce the amount of communicated data with the cost of redundant computations. Furthermore, lossy compression provides higher compression rates than lossless, but must be carefully tuned based on the data to avoid catastrophic loss of accuracy. For this reason, lossy compression is generally avoided in scientific computing and literature on it in such use cases is relatively sparse.


# Goals

1. Implement an MPI communication scheme for evaluating the performance and scaling of a selection of compression schemes.
2. Compare the performance and quality of a selection of lossy compression schemes. Comparison is carried out by comparing the error as units in the last place.


# Research questions

1. Computational overhead of compression vs. the time saved by communicating compressed data.
2. Compression ratio vs. compression quality.


# Measurements

### Performance

1. Compression time per byte (CPU & GPU)
2. Communication time per byte
3. ~~Strong scaling of the communication scheme~~ Unnecessary and dilutes the scope; can be estimated from 1 and 2.

### Quality

1. Error as units in the last place
2. Comparison of the divergence rate of a simple chaotic system (compressed and uncompressed)

# Test data sets

* Random, all-zero, hydrodynamics

* Hydrodynamics solver based on [J. Stam, 1999](https://doi.org/10.1145/311535.311548)

# Notes on compression schemes

[Fixed-rate compression](https://ieeexplore.ieee.org/document/6876024)

* Lindstrom 2014, demonstrates in fluid dynamics and other applications, argues for lossy compression

* Compression ratio of 4 on noisy fluid simulation data "without appreciable difference in quality". Verify this claim.

[Discrete cosine transform](https://en.wikipedia.org/wiki/Discrete_cosine_transform)

* Lossy

* JPEG, MP3

* Block artifacts at high compression ratios

[Sinusoidal-hyperbolic transform functions](https://ieeexplore.ieee.org/document/8708945)

* Lossy

* Fishy. Has the author inserted his own paper into the wikipedia article or is this really a notable paper?

Downsampling

* Lossy & simple

* Downsample resolution or precision

* Use as a control experiment?

Fractal compression

* Lossy

Reconstructing data with deep learning in fluid simulations (**out of scope**)

* [Guemes, et al. 2021](https://arxiv.org/abs/2103.07387)

* [Kim, et al. 2021](https://www.cambridge.org/core/journals/journal-of-fluid-mechanics/article/abs/unsupervised-deep-learning-for-superresolution-reconstruction-of-turbulence/CF82FEF56DD7C2711B1102209872E6D6)

* [Kim 2019](https://arxiv.org/abs/1806.02071)

* [Vinuesa](https://fcai.fi/calendar/2021/3/25/nvaitc-webinar-series-on-ai-applications-in-computational-sciences-session-2)

[LZ4](https://lz4.github.io/lz4/)

* Lossless

* Compression ratio of random data ~1.15

* Provided as a generic compressor in [NVComp](https://developer.nvidia.com/blog/optimizing-data-transfer-using-lossless-compression-with-nvcomp/) and Linux kernel

[Delta color compression](https://gpuopen.com/learn/dcc-overview/)

* Lossless

* Applied to read/writes automatically when using graphics APIs, unclear whether usable with OpenCL/ROCm/HIP


# Plan (6-week course)

> Week 1: First lecture

> Week 2: Second lecture

> Week 3: Literature review, selection of compression schemes

> Week 4: Implementing the communication and compression schemes

> Week 5: Evaluating the results

> Week 6: Report/presentation preparation


# Dependencies

* MPI installation

* CMake (for building zfp)

# Cloning

```
git clone https://bitbucket.org/jpekkila/cs-e4002-data-compression-in-hpc.git
git submodule update --init --recursive
```

# Building and running

```Bash
./build_dependencies.sh
make -j
mpirun -n <nprocs> ./main # Or via the batch job system if running on a cluster, f.ex. srun <slurm params> ./main
```
