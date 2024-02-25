#pragma once
#include <stdio.h>
#include <time.h>

typedef struct timespec Timer;

static inline int
timer_reset(Timer* t)
{
  const int retval = clock_gettime(CLOCK_REALTIME, t);
  if (retval == -1)
    perror("clock_gettime failure");

  return retval;
}

static inline long
timer_diff_nsec(const Timer start)
{
  Timer end;
  timer_reset(&end);
  const long diff = (end.tv_sec - start.tv_sec) * 1000000000l +
                    (end.tv_nsec - start.tv_nsec);
  return diff;
}

static inline void
timer_diff_print(const Timer t)
{
  printf("Time elapsed: %g ms\n", timer_diff_nsec(t) / 1e6);
}
