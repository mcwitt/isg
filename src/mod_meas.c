#include "mod_meas.h"
#include "state.h"
#include <math.h>

double spin_overlap(const int S1[], const int S2[])
{
    int i, sum = 0;

    for (i = 0; i < N; i++)
        sum += S1[i]*S2[i];

    return (double) sum/N;
}

double link_overlap(const int bond_i[],
                    const int bond_j[],
                    int num_bonds,
                    const int S1[],
                    const int S2[])
{
    int k, sum = 0;

    for (k = 0; k < num_bonds; k++)
    {
        int i, j;
        i = bond_i[k];
        j = bond_j[k];
        sum += S1[i]*S1[j]*S2[i]*S2[j];
    }

    return (double) sum/num_bonds;
}

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

#define X(type, name, fmt, width) self->data[i].name = (type) 0;
    for (i = 0; i < NUM_REPLICAS; i++) { MEAS_FIELDS }
#undef X
}

void mod_meas_update(mod_meas_t *self, const state_t *s)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        double q;
        replica_t r1, r2;
        meas_data_t *p = self->data + i;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        q = spin_overlap(r1.S, r2.S);
        p->q2 += q*q;
        p->q4 += q*q*q*q;

        p->u += 0.5*(r1.u + r2.u)/N;

#if REPLICA_EXCHANGE
        p->rx += 0.5*(s->x1.swapped[i] + s->x2.swapped[i]);
#endif

        p->ql += link_overlap(s->sample.bond_i,
                              s->sample.bond_j,
                              s->sample.num_bonds,
                              r1.S, r2.S);
    }
}

static void print(const meas_data_t *data,
                  const index_t *idx,
                  double T,
                  int num_meas,
                  FILE *fp)
{
    index_print(idx, T, fp);

#define X(type, name, fmt, width) \
    fprintf(fp, fmt, width, data->name / num_meas);
    MEAS_FIELDS
#undef X

    fprintf(fp, "\n");
} 

void mod_meas_output(const mod_meas_t *self,
                     const state_t *s,
                     const index_t *idx,
                     int num_meas,
                     FILE *fp)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
        print(self->data + i, idx, s->params->T[i], num_meas, fp);

    fflush(fp);
}

