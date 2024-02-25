TARGET = main
SRC = main.c #compress.c solver.c buffer.c grid.c visualize.c benchmark.c fixedpoint.c
OBJ = $(SRC:.c=.o)

CC = mpicc
WFLAGS = -pedantic -Wall -Wextra -Wdouble-promotion\
    	-Wfloat-conversion -Wno-error=unused-function\
		-Wno-error=unused-parameter -Wno-error=unused-variable\
		-Wshadow

IFLAGS = -I/usr/include/python3.9

CFLAGS = -fopenmp -O2 -march=native -DNDEBUG $(WFLAGS) $(IFLAGS) # Release build
#CFLAGS = -fopenmp -msse -mfpmath=sse -ffast-math -O2 -flto -DNDEBUG $(WFLAGS) $(IFLAGS) # Release build
#CFLAGS = -O0 -g -fno-inline $(WFLAGS) $(IFLAGS) # Debug build

LFLAGS = -lm -Wl,-rpath=${PWD}/external/zfp/build/lib64 -L external/zfp/build/lib64 -lzfp
#LFLAGS = -lm -L ${PWD}/external/zfp/build/lib64 -lzfp

# Uncomment to enable CUDA support
#CFLAGS := -DUSE_CUDA=1
#LFLAGS := -lm -Wl,-rpath=${PWD}/external/zfp/build/lib64 -L external/zfp/build/lib64 -lzfp -lcudart


$(TARGET): $(OBJ)
	$(CC) $(IFLAGS) $(CFLAGS) -o $@ $^ $(LFLAGS)

.PHONY: clean
clean:
	rm $(OBJ) $(TARGET)