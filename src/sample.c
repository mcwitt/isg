#include "sample.h"
#include <assert.h>
#include <string.h>

#if DILUTE

static void add_neighbor_dilute(int i, int j, double v,
                                int *n, double *J4, int *z, double *h2m)
{
    assert(z[i] < Z_MAX);
    n[i*Z_MAX + z[i]] = j;
    J4[i*Z_MAX + z[i]] = 4.*v;
    h2m[i] += 2.*v;
    ++z[i];
}

#define ADD_NEIGHBOR(i, j, v, n, J4, z, h2m) \
    add_neighbor_dilute(i, j, v, n, J4, z, h2m)

#else

static void add_neighbor_complete(int i, int j, double v,
                                  double *J4, int *z, double *h2m)
{
    J4[i*Z_MAX + j] = 4.*v;
    h2m[i] += 2.*v;
}

#define ADD_NEIGHBOR(i, j, v, n, J4, z, h2m) \
    add_neighbor_complete(i, j, v, J4, h2m)

#endif

void sample_init(sample_t *s)
{
    s->num_bonds = 0;
    reset_bonds(s->n, s->J4, s->z, s->h2m, &s->um);
}

void sample_add_bond(sample_t *s, int i, int j, double v)
{
    add_bond(i, j, v, s->n, s->J4, s->z, s->h2m, &s->um);
    s->bond_i[s->num_bonds] = i;
    s->bond_j[s->num_bonds] = j;
    s->bond_v[s->num_bonds] = v;
    s->num_bonds++;
}

void sample_read(sample_t *s, FILE *fp)
{
    int i, j;
    double v;

    sample_init(s);

    while (fscanf(fp, "%d %d %lf", &i, &j, &v) != EOF)
    {
        assert(s->num_bonds < NUM_BONDS);
        assert(IS_SITE_INDEX(i) && IS_SITE_INDEX(j));
        sample_add_bond(s, i, j, v);
    }
}

void add_bond(int i, int j, double v,
              int n[N*Z_MAX],
              double J4[N*Z_MAX],
              int z[N],
              double h2m[N],
              double *um)
{
    ADD_NEIGHBOR(i, j, v, n, J4, z, h2m);
    ADD_NEIGHBOR(j, i, v, n, J4, z, h2m);
    *um -= v;
}

void reset_bonds(int n[N*Z_MAX],
                 double J4[N*Z_MAX],
                 int z[N],
                 double h2m[N],
                 double *um)
{
    memset(n,  -1,  N*Z_MAX*sizeof(int));
    memset(J4,  0., N*Z_MAX*sizeof(double));
    memset(z,   0,  N*sizeof(int));
    memset(h2m, 0., N*sizeof(double));
    *um = 0.;
}

