#include "replica.h"
#include <math.h>


void replica_init(replica_t *r, const sample_t *s)
{
    r->sample = s;
    init_replica(s->h2m, s->um, r->S, r->h2, &r->u);
}

void replica_update(replica_t *r, double beta, rng_t *rng)
{
    update_replica(r->sample->n, r->sample->J4, beta, r->S, r->h2, &r->u, rng);
}

void init_replica(const double h2m[N],
                  double um,
                  int S[N],
                  double h2[N],
                  double *u)
{
    int i;

    *u = um;

    for (i = 0; i < N; i++)
    {
        S[i] = 1;
        h2[i] = h2m[i];
    }
}

void update_replica(const int n[NZ_MAX],
                    const double J4[NZ_MAX],
                    double beta,
                    int S[N],
                    double h2[N],
                    double *u,
                    rng_t *rng)
{
    int i, k;
    double r;

    for (i = 0; i < N; i++)
    {
        if (S[i] == 1)
        {
            if (h2[i] < 0. || (r = RNG_UNIFORM(rng), r < exp(-beta*h2[i])))
            {
                S[i] = -1;
                *u += h2[i];

                for (k = 0; k < Z_MAX; k++)
                {
                    int ik = i*Z_MAX + k;
                    if (n[ik] == -1) break;
                    h2[n[ik]] -= J4[ik];
                }
            }
        }
        else    /* S[i] == -1 */
        {
            if (h2[i] > 0. || (r = RNG_UNIFORM(rng), r < exp(beta*h2[i])))
            {
                S[i] = 1;
                *u -= h2[i];

                for (k = 0; k < Z_MAX; k++)
                {
                    int ik = i*Z_MAX + k;
                    if (n[ik] == -1) break;
                    h2[n[ik]] += J4[ik];
                }
            }
        }
    }
}

