#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERROR(str)                                                             \
  {                                                                            \
    time_t terr;                                                               \
    time(&terr);                                                               \
    fprintf(stderr, "%s", ctime(&terr));                                       \
    fprintf(stderr, "\tError in file %s line %d: %s\n", __FILE__, __LINE__,    \
            str);                                                              \
    fflush(stderr);                                                            \
    exit(EXIT_FAILURE);                                                        \
    abort();                                                                   \
  }

#define WARNING(str)                                                           \
  {                                                                            \
    time_t terr;                                                               \
    time(&terr);                                                               \
    fprintf(stderr, "%s", ctime(&terr));                                       \
    fprintf(stderr, "\tWarning in file %s line %d: %s\n", __FILE__, __LINE__,  \
            str);                                                              \
    fflush(stderr);                                                            \
  }

#define ERRCHK(retval)                                                         \
  {                                                                            \
    if (!(retval))                                                             \
      ERROR(#retval " was false");                                             \
  }
#define WARNCHK(retval)                                                        \
  {                                                                            \
    if (!(retval))                                                             \
      WARNING(#retval " was false");                                           \
  }
