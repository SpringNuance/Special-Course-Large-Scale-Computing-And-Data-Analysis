#include "solver.h"

#include "errchk.h"
#include "grid.h"
#include "morton.h"
#include "utils.h"

#define NX (64)
#define NY (NX)
#define NXY (NX * NY)
#define BOUND (0) // Note: no need for bounds if we use bit twiddling
#define MX (NX + 2 * BOUND)
#define MY (NY + 2 * BOUND)
#define MXY (MX * MY)

#define NX_MIN (BOUND)
#define NY_MIN (BOUND)
#define NX_MAX (BOUND + NX)
#define NY_MAX (BOUND + NY)

//#define VISC (0.000001) // Not implemented
#define DIFF (0.00001)

#define WRAPPED(a, n) ((a + n) & (n - 1)) // a mod n when n is a power of two
#define IDX(i, j) spatial_to_morton((uint2_64){WRAPPED(i, MX), WRAPPED(j, MY)})

#define SWAP(a, b)                                                             \
  {                                                                            \
    double* tmp = a;                                                           \
    a           = b;                                                           \
    b           = tmp;                                                         \
  }

static void
add_source(const double* src, const double dt, double* dst)
{
#pragma omp parallel for
  for (int i = 0; i < MXY; ++i)
    dst[i] += src[i] * dt;
}

static void
diffuse(const double* x0, const double dt, double* x)
{
  const double a = dt * DIFF * NXY;
  for (int k = 0; k < 20; ++k) {
    for (int i = NX_MIN; i < NX_MAX; i++) {
      for (int j = NY_MIN; j < NY_MAX; j++) {
        x[IDX(i, j)] = (x0[IDX(i, j)] +
                        a * (x[IDX(i - 1, j)] + x[IDX(i + 1, j)] +
                             x[IDX(i, j - 1)] + x[IDX(i, j + 1)])) /
                       (1.0 + 4.0 * a);
      }
    }
    // set bounds
  }
}

static void
advect(const double* ux, const double* uy, const double dt, const double* d0,
       double* d)
{
  const double dt0 = dt * NX; // TODO check for arbitrary dims

#pragma omp parallel for
  for (int i = NX_MIN; i < NX_MAX; i++) {
    for (int j = NY_MIN; j < NY_MAX; j++) {
      double x = i - dt0 * ux[IDX(i, j)];
      double y = j - dt0 * uy[IDX(i, j)];
      if (x < .5)
        x = .5;
      if (x > NX + .5)
        x = NX + .5;
      const int i0 = (int)x;
      const int i1 = i0 + 1;
      if (y < .5)
        y = .5;
      if (y > NY + .5)
        y = NY + .5;
      const int j0    = (int)y;
      const int j1    = j0 + 1;
      const double s1 = x - i0;
      const double s0 = 1.0 - s1;
      const double t1 = y - j0;
      const double t0 = 1.0 - t1;
      d[IDX(i, j)]    = s0 * (t0 * d0[IDX(i0, j0)] + t1 * d0[IDX(i0, j1)]) +
                     s1 * (t0 * d0[IDX(i1, j0)] + t1 * d0[IDX(i1, j1)]);
    }
  }
  // set bounds
}

void
project(double* ux, double* uy, double* p, double* div)
{
  const double h = 1.0 / NX; // TODO check for arbitrary dims

#pragma omp parallel for
  for (int i = NX_MIN; i < NX_MAX; i++) {
    for (int j = NY_MIN; j < NY_MAX; j++) {
      div[IDX(i, j)] = -.5 * h *
                       (ux[IDX(i + 1, j)] - ux[IDX(i - 1, j)] +
                        uy[IDX(i, j + 1)] - uy[IDX(i, j - 1)]);
      p[IDX(i, j)] = .0;
    }
  }

#if USE_JACOBI
  const double alpha = 1.;
  const double beta  = .25;
  double p_tmp[MXY];
  double* curr_p = p_tmp;
  double* prev_p = p;
  for (int k = 0; k < 250; ++k) {
#pragma omp parallel for
    for (int i = NX_MIN; i < NX_MAX; i++) {
      for (int j = NY_MIN; j < NY_MAX; j++) {
        curr_p[IDX(i, j)] = beta *
                            (alpha * div[IDX(i, j)] + prev_p[IDX(i - 1, j)] +
                             prev_p[IDX(i + 1, j)] + prev_p[IDX(i, j - 1)] +
                             prev_p[IDX(i, j + 1)]);
      }
    }
    double* tmp = curr_p;
    curr_p      = prev_p;
    prev_p      = tmp;
    // set bounds
  }
  assert(prev_p == p); // s.t. no useless iterations

#else // Use Gauss-Seidel
  // set bounds for div, p
  for (int k = 0; k < 20; k++) {
    for (int i = NX_MIN; i < NX_MAX; i++) {
      for (int j = NY_MIN; j < NY_MAX; j++) {
        p[IDX(i, j)] = .25 *
                       (div[IDX(i, j)] + p[IDX(i - 1, j)] + p[IDX(i + 1, j)] +
                        p[IDX(i, j - 1)] + p[IDX(i, j + 1)]);
      }
    }
    // set bounds for p
  }
#endif
#pragma omp parallel for
  for (int i = NX_MIN; i < NX_MAX; i++) {
    for (int j = NY_MIN; j < NY_MAX; j++) {
      ux[IDX(i, j)] -= .5 * (p[IDX(i + 1, j)] - p[IDX(i - 1, j)]) / h;
      uy[IDX(i, j)] -= .5 * (p[IDX(i, j + 1)] - p[IDX(i, j - 1)]) / h;
    }
  }
  // set bounds for ux, uy
}

static void
density_step(const double dt, const double* ux, const double* uy, double* x0,
             double* x)
{
  add_source(x0, dt, x);
  SWAP(x0, x);
  diffuse(x0, dt, x);
  SWAP(x0, x);
  advect(ux, uy, dt, x0, x); // Final result now in x
}

static void
velocity_step(const double dt, double* ux0, double* uy0, double* ux, double* uy)
{
  add_source(ux0, dt, ux);
  add_source(uy0, dt, uy);
  SWAP(ux0, ux);
  SWAP(uy0, uy);

  diffuse(ux0, dt, ux);
  diffuse(uy0, dt, uy);
  project(ux, uy, ux0, uy0);

  SWAP(ux0, ux);
  SWAP(uy0, uy);

  advect(ux0, uy0, dt, ux0, ux);
  advect(ux0, uy0, dt, uy0, uy);
  project(ux, uy, ux0, uy0); // Result now in ux, uy
}

static double ux[MXY], uy[MXY], rho[MXY];
static double ux0[MXY], uy0[MXY], rho0[MXY];

void
hydro_init(void)
{
  for (int i = 0; i < MXY; ++i) {
    rho[i] = .0; // randf() - .5;
    ux[i]  = .0; // randf() - .5;
    uy[i]  = .0; // randf() - .5;
  }
}

void
hydro_step(const double dt)
{
#pragma omp parallel for
  for (int i = 0; i < MXY; ++i) {
    rho0[i] = .0;
    ux0[i]  = .0;
    uy0[i]  = .0;
  }
  // rho0[IDX(NX / 2 + 10, NY / 2 + 10)] = 10.;
  // ux0[IDX(NX / 2 + 10, NY / 2 + 10)]  = 10.;
  // uy0[IDX(NX / 2 + 50, NY / 2 - 10)]  = 10.;
  // rho0[IDX(0, NY / 2)] = 10.;
  // ux0[IDX((size_t)(0.1 * NX), NY / 2)] = 10.;
  // uy0[IDX(0, NY / 2)]  = 10.;

  // rho0[IDX(NX / 2, NY / 2)]            = 10.;
  // ux0[IDX((size_t)(0.9 * NX), NY / 2)] = -30.;

  // Basic x flow
  rho0[IDX((size_t)(0.1 * NX), NY / 2)] = 40.;
  ux0[IDX((size_t)(0.1 * NX), NY / 2)]  = 40.;

  // Rotating
  /*
  rho0[IDX(NX / 2, NY / 2)] = 10.;
      double theta = 0.0;
      const double r      = 0.25 * NX;
      theta               = fmod(theta + 0.05, 2 * 3.14159);
      ux0[IDX((size_t)(NX / 2.0 + r * cos(theta)),
              (size_t)(NY / 2.0 + r * sin(theta)))] = 50.0;
  */

  /*
  // Wiggling
  rho0[IDX((size_t)(0.5 * NX), (size_t)(0.5 * NX))] = 40.;
  double theta                               = 0.0;
  const double r                                    = 0.25 * NX;
  theta = fmod(theta + 0.1, 2 * 3.14159);
  ux0[IDX((size_t)(0.5 * NX), (size_t)(0.5 * NY))] = 40.0 * cos(theta);
  uy0[IDX((size_t)(0.5 * NX), (size_t)(0.5 * NY))] = 40.0 * sin(theta);
  */

  velocity_step(dt, ux0, uy0, ux, uy);
  density_step(dt, ux, uy, rho0, rho);
}

void
update(const Grid input, Grid* output)
{
  const size_t count = input.field[0].count;
  ERRCHK(MXY == count);

// Load previous state
#pragma omp parallel for
  for (size_t i = 0; i < MXY; ++i) {
    rho[i] = input.field[RHO].data[i];
    ux[i]  = input.field[UX].data[i];
    uy[i]  = input.field[UY].data[i];
  }
  /*
  // Load previous state (FLUSH VERY SMALL VALUES TO ZERO, test)
#pragma omp parallel for
  for (size_t i = 0; i < MXY; ++i) {
      const double cutoff = 1e-16;
      for (size_t j = 0; j < NUM_FIELDS; ++j)
          if (fabs(input.field[j].data[i]) < cutoff)
              input.field[j].data[i] = 0.0;

      rho[i] = input.field[RHO].data[i];
      ux[i]  = input.field[UX].data[i];
      uy[i]  = input.field[UY].data[i];
  }
  */

  hydro_step(0.1);

  /*
  #define IX(i, j) spatial_to_morton((uint2_64){i, j})
      // Final results in ux,uy,rho
      for (size_t j = 0; j < NY; ++j)
          for (size_t i = 0; i < NX; ++i) {
              const size_t src = IDX(i, j);
              const size_t dst = IX(i, j);
              ERRCHK(src < count);
              ERRCHK(dst < count);

              output->field[RHO].data[dst] = rho[src];
              output->field[UX].data[dst]    = ux[src];
              output->field[UY].data[dst]    = uy[src];
          }
          */
#pragma omp parallel for
  for (size_t i = 0; i < MXY; ++i) {
    output->field[RHO].data[i] = rho[i];
    output->field[UX].data[i]  = ux[i];
    output->field[UY].data[i]  = uy[i];
  }
}
