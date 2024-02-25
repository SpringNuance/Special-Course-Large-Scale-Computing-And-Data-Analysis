#include "grid.h"

#include "utils.h"

Grid
grid_create(const size_t count)
{
    Grid grid;
    for (size_t i = 0; i < NUM_FIELDS; ++i)
        grid.field[i] = buffer_create(count);

    return grid;
}

void
grid_destroy(Grid* grid)
{
    for (size_t i = 0; i < NUM_FIELDS; ++i)
        buffer_destroy(&grid->field[i]);
}

void
grid_randomize(Grid* grid)
{
    const size_t count = grid->field[0].count;
    for (size_t j = 0; j < NUM_FIELDS; ++j)
        for (size_t i = 0; i < count; ++i)
            grid->field[j].data[i] = randf();
}

void
grid_to_file(const Grid grid, const char* prefix, const size_t i)
{
    const size_t bufsize = 256;
    char lnrhopath[bufsize];
    snprintf(lnrhopath, bufsize, "data/%s_lnrho_%lu.csv", prefix, i);

    char uxpath[bufsize];
    snprintf(uxpath, bufsize, "data/%s_ux_%lu.csv", prefix, i);

    char uypath[bufsize];
    snprintf(uypath, bufsize, "data/%s_uy_%lu.csv", prefix, i);

    buffer_to_file(grid.field[LNRHO], lnrhopath);
    buffer_to_file(grid.field[UX], uxpath);
    buffer_to_file(grid.field[UY], uypath);
}