#include "replica.h"
#include <math.h>

#define UPDATE_UNDILUTE(i, Si, J4i, h2, u) { \
    int j; \
    *u -= Si*h2[i]; \
    for (j = 0; j < N; j++) \
        h2[j] += Si*J4i[j]; \
}

#define UPDATE_DILUTE(i, Si, ni, J4i, h2, u) { \
    int j; \
    *u -= Si*h2[i]; \
    for (j = 0; j < Z_MAX; j++) { \
        if (ni[j] == -1) break; \
        h2[ni[j]] += Si*J4i[j]; \
    } \
}

#if Z == 0  /* all bonds are present */
#define UPDATE(i, Si, ni, J4i, h2, u) UPDATE_UNDILUTE(i, Si, J4i, h2, u)
#else
#define UPDATE(i, Si, ni, J4i, h2, u) UPDATE_DILUTE(i, Si, ni, J4i, h2, u)
#endif

void init_replica(const int n[N*Z_MAX],
                  const double J4[N*Z_MAX],
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
        if (RNG_UNIFORM(rng) < 0.5) S[i] = 1;
        else
        {
            S[i] = -1;
            UPDATE(i, -1, n, J4, h2, u);
        }

        n += Z_MAX; J4 += Z_MAX;
    }
}

void update_replica(const int n[N*Z_MAX],
                    const double J4[N*Z_MAX],
                    double beta,
                    int S[N],
                    double h2[N],
                    double *u,
                    rng_t *rng)
{
    int i;

    for (i = 0; i < N; i++)
    {
        double r;

        if (S[i] == 1)
        {
            if (h2[i] < 0. || (r = RNG_UNIFORM(rng), r < exp(-beta*h2[i])))
            {
                S[i] = -1;
                UPDATE(i, -1, n, J4, h2, u);
            }
        }
        else    /* S[i] == -1 */
        {
            if (h2[i] > 0. || (r = RNG_UNIFORM(rng), r < exp(beta*h2[i])))
            {
                S[i] = 1;
                UPDATE(i, 1, n, J4, h2, u);
            }
        }

        n += Z_MAX; J4 += Z_MAX;
    }
}

