#pragma once

#include "buffer.h"

double expected_value(const Buffer buffer);

double variance(const Buffer buffer);

double standard_deviation(const Buffer buffer);

/** Returns the diff between the largest and smallest values in the buffer */
double range(const Buffer buffer);

/** Returns the mean squared quantization error (msqe) */
double distortion(const Buffer model, const Buffer candidate);
