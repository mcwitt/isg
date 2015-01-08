#ifndef REPLICA_H
#define REPLICA_H

#include "rng.h"
#include "sample.h"

typedef struct
{
    sample_data const *sample;
    int const *S;
    double const *h2;
    double u;

} replica;

void init_replica(int const * restrict n,
                  double const * restrict J4,
                  int const * restrict z,
                  double const * restrict h2m,
                  double um,
                  int * restrict S,
                  double * restrict h2,
                  double * restrict u,
                  rng *rand);

void update_replica(int const * restrict n,
                    double const * restrict J4,
                    int const * restrict z,
                    double beta,
                    int * restrict S,
                    double * restrict h2,
                    double * restrict u,
                    rng *rand);

#endif
