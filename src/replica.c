#include "replica.h"
#include <math.h>

void init_replica(int const * restrict n,
                  double const * restrict J4,
                  int const * restrict z,
                  double const * restrict h2m,
                  double um,
                  int * restrict S,
                  double * restrict h2,
                  double * restrict u,
                  rng *rand)
{
    int i, j;

    *u = um;

    for (i = 0; i < N; i++)
        h2[i] = h2m[i];

    /* randomize spins */

    for (i = 0; i < N; i++)
    {
        if (RNG_UNIFORM(rand) < 0.5) S[i] = 1;
        else
        {
            S[i] = -1;
            *u += h2[i];
            for (j = 0; j < z[i]; j++) h2[n[j]] -= J4[j];
        }

        n += z[i];
        J4 += z[i];
    }
}

void update_replica(int const * restrict n,
                    double const * restrict J4,
                    int const * restrict z,
                    double beta,
                    int * restrict S,
                    double * restrict h2,
                    double * restrict u,
                    rng *rand)
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        double r;

        if (S[i] == 1)
        {
            if (h2[i] < 0. || (r = RNG_UNIFORM(rand), r < exp(-beta*h2[i])))
            {
                S[i] = -1;
                *u += h2[i];
                for (j = 0; j < z[i]; j++) h2[n[j]] -= J4[j];
            }
        }
        else    /* S[i] == -1 */
        {
            if (h2[i] > 0. || (r = RNG_UNIFORM(rand), r < exp(beta*h2[i])))
            {
                S[i] = 1;
                *u -= h2[i];
                for (j = 0; j < z[i]; j++) h2[n[j]] += J4[j];
            }
        }

        n += z[i];
        J4 += z[i];
    }
}

