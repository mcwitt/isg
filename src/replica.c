#include "replica.h"
#include <math.h>

#define UPDATE_DILUTE(j, Si, ni, J4i, h2, u)    \
{                                               \
    for (j = 0; j < Z_MAX; j++)                 \
    {                                           \
        if (ni[j] == -1) break;                 \
        h2[ni[j]] += Si*J4i[j];                 \
    }                                           \
}

#define UPDATE_COMPLETE(j, Si, J4i, h2, u) \
    for (j = 0; j < N; j++) h2[j] += Si*J4i[j];

#if DILUTE
#define UPDATE(j, Si, ni, J4i, h2, u) UPDATE_DILUTE(j, Si, ni, J4i, h2, u)
#else
#define UPDATE(j, Si, ni, J4i, h2, u) UPDATE_COMPLETE(j, Si, J4i, h2, u)
#endif

void init_replica(const int * restrict n,
                  const double * restrict J4,
                  const double * restrict h2m,
                  double um,
                  int * restrict S,
                  double * restrict h2,
                  double * restrict u,
                  rng_t *rng)
{
    int i, j;

    *u = um;

    for (i = 0; i < N; i++)
        h2[i] = h2m[i];

    /* randomize spins */

    for (i = 0; i < N; i++)
    {
        if (RNG_UNIFORM(rng) < 0.5) S[i] = 1;
        else
        {
            S[i] = -1;
            *u += h2[i];
            UPDATE(j, -1, n, J4, h2, u);
        }

        n += Z_MAX;
        J4 += Z_MAX;
    }
}

void update_replica(const int * restrict n,
                    const double * restrict J4,
                    double beta,
                    int * restrict S,
                    double * restrict h2,
                    double * restrict u,
                    rng_t *rng)
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        double r;

        if (S[i] == 1)
        {
            if (h2[i] < 0. || (r = RNG_UNIFORM(rng), r < exp(-beta*h2[i])))
            {
                S[i] = -1;
                *u += h2[i];
                UPDATE(j, -1, n, J4, h2, u);
            }
        }
        else    /* S[i] == -1 */
        {
            if (h2[i] > 0. || (r = RNG_UNIFORM(rng), r < exp(beta*h2[i])))
            {
                S[i] = 1;
                *u -= h2[i];
                UPDATE(j, 1, n, J4, h2, u);
            }
        }

        n += Z_MAX;
        J4 += Z_MAX;
    }
}

