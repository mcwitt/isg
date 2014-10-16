#include "sample.h"
#include "rng.h"


typedef struct
{
    const sample_t *sample;
    int S[N];
    double h2[N];
    double u;

} replica_t;

void replica_init(replica_t *r, const sample_t *s);

void replica_update(replica_t *r, double beta, rng_t *rng);

void init_replica(const double h2m[N],
                  double um,
                  int S[N],
                  double h2[N],
                  double *u);

void update_replica(const int n[NZ_MAX],
                    const double J4[NZ_MAX],
                    double beta,
                    int S[N],
                    double h2[N],
                    double *u,
                    rng_t *rng);
