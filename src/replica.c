#include "replica.h"
#include <math.h>

void init_replica(const int n[NZ_MAX],
                  const double J4[NZ_MAX],
                  const double h2m[N],
                  double um,
                  int S[N],
                  double h2[N],
                  double *u,
                  rng_t *rng)
{
    int i;

    *u = um;

    for (i = 0; i < N; i++)
        h2[i] = h2m[i];

    /* randomize spins */

    for (i = 0; i < N; i++)
    {
        int k;

        if (RNG_UNIFORM(rng) < 0.5)
        {
            S[i] = 1;
        }
        else
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

