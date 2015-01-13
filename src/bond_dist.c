#include "bond_dist.h"
#include <math.h>

#include "bisect.c"

#define R(n) fabs(N/M_PI*sin(M_PI*n/N))

void bond_dist_dilute(bonds *b, double sigma, rng *rand)
{
    int n, num_bonds = 0;
    double *f, sum = 0., p = -2.*sigma;

    f = malloc(N * sizeof(double));

    /* compute probability distribution */

    f[0] = 0.;

    for (n = 1; n < N; n++)
    {
        double r, w;

        r = R(n);
        w = pow(r, p);
        f[n] = f[n-1] + w;
        sum += w;
    }

    for (n = 1; n < N; n++) f[n] /= sum;

    /* generate bonds */

    while (num_bonds < NUM_BONDS)
    {
        int i, j;
        double r;

        i = (int) (N * RNG_UNIFORM(rand));
        r = RNG_UNIFORM(rand);
        n = bisect(f, r, 1, N);
        j = i + n;

        if (j >= N)
        {
            int _i = i;
            i = j - N;
            j = _i;
        }

        if (! bonds_have(b, i, j))
        {
            double v = RNG_GAUSS(rand, 1.);
            bonds_add(b, i, j, v);
            num_bonds++;
        }
    }

    free(f);
}

