#pragma once
#include "buffer.h"

typedef enum {
  RHO,
  UX,
  UY,
  NUM_FIELDS,
} Field;

typedef struct {
  Buffer field[NUM_FIELDS];
} Grid;

Grid grid_create(const size_t count);

void grid_destroy(Grid* grid);

void grid_randomize(Grid* grid);

void grid_to_file(const Grid grid, const char* prefix, const size_t i);
