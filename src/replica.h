#ifndef REPLICA_H
#define REPLICA_H

#include "rng.h"
#include "sample.h"

typedef struct
{
    const sample_t *sample;
    const int *S;
    const double *h2;
    double u;

} replica_t;

void init_replica(const int * restrict n,
                  const double * restrict J4,
                  const double * restrict h2m,
                  double um,
                  int * restrict S,
                  double * restrict h2,
                  double * restrict u,
                  rng_t *rng);

void update_replica(const int * restrict n,
                    const double * restrict J4,
                    double beta,
                    int * restrict S,
                    double * restrict h2,
                    double * restrict u,
                    rng_t *rng);

#endif
