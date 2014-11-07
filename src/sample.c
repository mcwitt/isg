#include "sample.h"
#include <assert.h>
#include <string.h>

void sample_init(sample_t *s)
{
    s->num_bonds = 0;
    reset_bonds(s->n, s->J4, s->z, s->h2m, &s->um);
}

void sample_add_bond(sample_t *s, int i, int j, double v)
{
    assert(s->num_bonds < MAX_BONDS);
    add_bond(i, j, v, s->n, s->J4, s->z, s->h2m, &s->um);
    s->bond_i[s->num_bonds] = i;
    s->bond_j[s->num_bonds] = j;
    s->bond_v[s->num_bonds] = v;
    s->num_bonds++;
}

int sample_read(sample_t *s, FILE *fp, int *num_bonds)
{
    int i, j;
    double v;

    sample_init(s);

    for (*num_bonds = 0;
         fscanf(fp, "%d %d %lf", &i, &j, &v) != EOF;
         ++(*num_bonds))
    {
        if (IS_SITE_INDEX(i) && IS_SITE_INDEX(j))
            sample_add_bond(s, i, j, v);
        else return 1;
    }

    return 0;
}

void add_bond(int i, int j, double v,
              int n[N*Z_MAX],
              double J4[N*Z_MAX],
              int z[N],
              double h2m[N],
              double *um)
{
#define ADD_NEIGHBOR(i, j) {\
    assert(z[i] < Z_MAX);\
    n[i*Z_MAX + z[i]] = j;\
    J4[i*Z_MAX + z[i]] = 4.*v;\
    h2m[i] += 2.*v;\
    ++z[i];\
}

    ADD_NEIGHBOR(i, j);
    ADD_NEIGHBOR(j, i);

#undef ADD_NEIGHBOR

    *um -= v;
}

void reset_bonds(int n[N*Z_MAX],
                 double J4[N*Z_MAX],
                 int z[N],
                 double h2m[N],
                 double *um)
{
    *um = 0.;

    memset(z,   0,  N*sizeof(int));
    memset(h2m, 0., N*sizeof(double));
    memset(n,  -1,  N*Z_MAX*sizeof(int));
    memset(J4,  0., N*Z_MAX*sizeof(double));
}

