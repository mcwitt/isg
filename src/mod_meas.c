#include "mod_meas.h"
#include "state.h"
#include <math.h>

static void print_header(FILE *fp)
{
    print_index_header(fp);

#define X(type, name, fmt, width) fprintf(fp, "%*s", width, #name);
    MEAS_FIELDS
#undef X

    fprintf(fp, "\n");
}

void mod_meas_init(mod_meas_t *self, FILE *fp)
{
    print_header(fp);
}

void mod_meas_reset(mod_meas_t *self)
{
    int i;

#define X(type, name, fmt, width) \
    for (i = 0; i < NUM_REPLICAS; i++) self->name[i] = (type) 0;
    MEAS_FIELDS
#undef X
}

void mod_meas_update(mod_meas_t *self, const state_t *s)
{
    int i, j, nq[NUM_REPLICAS];
    const int (*S1)[NUM_REPLICAS], (*S2)[NUM_REPLICAS];

    S1 = s->x1.S;
    S2 = s->x2.S;

    for (j = 0; j < NUM_REPLICAS; j++) nq[j] = 0;

    for (i = 0; i < N; i++)
    {
        int j;

        for (j = 0; j < NUM_REPLICAS; j++)
        {
            int j1 = s->x1.b2r[j],
                j2 = s->x2.b2r[j];

            nq[j] += S1[i][j1]*S2[i][j2];
        }
    }

    for (j = 0; j < NUM_REPLICAS; j++)
    {
        double q = (1./N)*nq[j];
        self->q2[j] += q*q;
        self->q4[j] += q*q*q*q;
    }

    for (i = 0; i < s->sample.num_bonds; i++)
    {
        int a, b, j;

        a = s->sample.bond_i[i];
        b = s->sample.bond_j[i];

        for (j = 0; j < NUM_REPLICAS; j++)
        {
            int j1 = s->x1.r2b[j],
                j2 = s->x2.r2b[j];

            self->ql[j] += (1./s->sample.num_bonds)*S1[a][j1]*S1[b][j1]*\
                                          S2[a][j2]*S2[b][j2];
        }
    }

    for (j = 0; j < NUM_REPLICAS; j++)
    {
        int j1 = s->x1.b2r[j],
            j2 = s->x2.b2r[j];

        self->u[j] += 0.5*(s->x1.u[j1] + s->x2.u[j2]);
    }

#if REPLICA_EXCHANGE
    for (j = 0; j < NUM_REPLICAS; j++)
        self->rx[j] += 0.5*(s->x1.swapped[j] + s->x2.swapped[j]);
#endif
}

void mod_meas_output(const mod_meas_t *self,
                     const state_t *s,
                     const index_t *idx,
                     int num_meas,
                     FILE *fp)
{
    int j;

    for (j = 0; j < NUM_REPLICAS; j++)
    {
        index_print(idx, s->params->T[j], fp);
#define X(type, name, fmt, width) \
    fprintf(fp, fmt, width, self->name[j] / num_meas);
        MEAS_FIELDS
#undef X
        fprintf(fp, "\n");
    }

    fflush(fp);
}

