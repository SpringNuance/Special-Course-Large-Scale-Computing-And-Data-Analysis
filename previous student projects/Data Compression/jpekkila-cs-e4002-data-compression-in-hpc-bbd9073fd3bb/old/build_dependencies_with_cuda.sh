#!/bin/bash
mkdir -p external/zfp/build
cd external/zfp/build
cmake -DZFP_WITH_CUDA=ON -DBUILD_SHARED_LIBS=ON -DBUILD_UTILITIES=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF ..
make -j
