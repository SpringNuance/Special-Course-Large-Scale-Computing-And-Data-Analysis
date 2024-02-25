#!/bin/bash
rm -r external/zfp/build
mkdir -p external/zfp/build
cd external/zfp/build
cmake -DBUILD_UTILITIES=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF ..
make -j $(nproc)
mv -f lib lib64 # On some systems zfp creates lib instead of lib64 for some arcane reason