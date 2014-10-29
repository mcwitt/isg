#include "mod_modes.h"
#include <math.h>
#include <string.h>

double q2k(const int S1[], const int S2[],
           const double sinkr[], const double coskr[])
{
    int i;
    double re = 0., im = 0.;

    for (i = 0; i < N; i++) re += coskr[i]*S1[i]*S2[i];
    for (i = 0; i < N; i++) im += sinkr[i]*S1[i]*S2[i];

    re /= N;
    im /= N;

    return re*re + im*im;
}

static void print_header(FILE *fp)
{
    print_index_header(fp);
    fprintf(fp, "%*s", COL_WIDTH, "q2k1");
    fprintf(fp, "\n");
}

void mod_modes_init(mod_modes_t *self, FILE *fp)
{
    int i;

    self->fp = fp;
    print_header(fp);

    /* trig tables for k=1 measurement */
    for (i = 0; i < N; i++)
    {
        double kr = i*M_2_PI/N;

        self->coskr[i] = cos(kr);
        self->sinkr[i] = sin(kr);
    }
}

void mod_modes_reset(mod_modes_t *self)
{
    memset(self->q2k1, 0., sizeof self->q2k1);
}

void mod_modes_update(mod_modes_t *self, const state_t *s)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica_t r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        self->q2k1[i] += q2k(r1.S, r2.S, self->sinkr, self->coskr);
    }
}

void mod_modes_output(const mod_modes_t *self,
                      const index_t *idx,
                      const state_t *s,
                      int num_meas)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        index_print(idx, s->params->T[i], self->fp);
        fprintf(self->fp, "%*e", COL_WIDTH, self->q2k1[i]);
        fprintf(self->fp, "\n");
    }

    fflush(self->fp);
}
