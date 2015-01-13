#include "sample.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void add_neighbor(int i, int j, double v,
                         int *z, int const *offset,
                         int *n, double *J4, double *h2m)
{
    int ix = offset[i] + z[i];

    n[ix] = j;
    J4[ix] = 4.*v;
    h2m[i] += 2.*v;
    z[i]++;
}

void init_sample(int const *bond_site, double const *bond_value,
                 int *n, double *J4, int *z, int *offset,
                 double *h2m, double *um)
{
    int i, b;

    /* 1st pass: calculate vertex degrees and offsets */
    memset(z, 0, N * sizeof(int));

    for (i = 0; i < NZ; i++)
        z[bond_site[i]]++;

    offset[0] = 0;

    for (i = 1; i < N; i++)
        offset[i] = offset[i-1] + z[i-1];

    /* 2nd pass: fill in values */
    memset(z, 0, N * sizeof(int));
    memset(h2m, 0., N * sizeof(double));
    *um = 0.;

    for (i = 0; i < NUM_BONDS; i++)
    {
        for (b = 0; b < 2; b++)
            add_neighbor(bond_site[2*i+b],
                         bond_site[2*i+(1^b)],
                         bond_value[i],
                         z, offset, n, J4, h2m);

        *um -= bond_value[i];
    }
}

void sample_init(sample_data *s,
                 int const *bond_site,
                 double const *bond_value)
{
    init_sample(bond_site, bond_value,
                s->n, s->J4, s->z, s->offset,
                s->h2m, &s->um);
}

void sample_init_read(sample_data *s, FILE *fp)
{
    int *bond_site, i;
    double *bond_value;

    bond_site  = malloc(NZ * sizeof(int));
    bond_value = malloc(NUM_BONDS * sizeof(double));

    for (i = 0; i < NUM_BONDS; i++)
    {
        int c, s0, s1;

        c = fscanf(fp, "%d %d %lf",
                   &s0, &s1, &(bond_value[i]));

        assert(c != EOF);
        assert(IS_SITE_INDEX(s0) && IS_SITE_INDEX(s1));
        bond_site[2*i]   = s0;
        bond_site[2*i+1] = s1;
    }

    sample_init(s, bond_site, bond_value);

    free(bond_site);
    free(bond_value);
}

