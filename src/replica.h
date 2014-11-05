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

void init_replica(const int n[NZ_MAX],
                  const double J4[NZ_MAX],
                  const double h2m[N],
                  double um,
                  int S[N],
                  double h2[N],
                  double *u,
                  rng_t *rng);

void update_replica(const int n[NZ_MAX],
                    const double J4[NZ_MAX],
                    double beta,
                    int S[N],
                    double h2[N],
                    double *u,
                    rng_t *rng);

#endif
