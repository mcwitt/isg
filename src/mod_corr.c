#include "mod_corr.h"
#include "output.h"
#include "state.h"
#include <string.h>

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", 6, "r");
    fprintf(fp, "%*s", COL_WIDTH, "C4");
    fprintf(fp, "\n");
}

void mod_corr_init(mod_corr_t *self, FILE *fp)
{
    print_header(fp);
}

void mod_corr_reset(mod_corr_t *self)
{
    self->num_calls = 0;
    self->num_updates = 0;
    memset(self->C4, 0, sizeof self->C4);
}

static void accumulate(double C4[], const int S1[], const int S2[])
{
    int r;

    for (r = 0; r < N/2; r++)
    {
        int i, sum = 0;

        for (i = 0; i < N-r; i++)
            sum += S1[i]*S2[i]*S1[i+r]*S2[i+r];

        for (; i < N; i++)
            sum += S1[i]*S2[i]*S1[i+r-N]*S2[i+r-N];

        C4[r] += (double) sum/N;
    }
}

void mod_corr_update(mod_corr_t *self, const state_t *s)
{
    if (self->num_calls % MOD_CORR_UPDATE_EVERY)
    {
        int i;

        for (i = 0; i < NUM_REPLICAS; i++)
        {
            replica_t r1, r2;

            exchange_get_replica(&s->x1, i, &r1);
            exchange_get_replica(&s->x2, i, &r2);

            accumulate(self->C4[i], r1.S, r2.S);
        }

        self->num_updates++;
    }

    self->num_calls++;
}

static void print(const double C4[],
                  const index_t *idx,
                  int num_updates,
                  double T,
                  FILE *fp)
{
    int r;

    for (r = 0; r < N/2; r++)
    {
        index_print(idx, T, fp);

        fprintf(fp, "%*d", 6, r);
        fprintf(fp, "%*e", COL_WIDTH, C4[r]/num_updates);
        fprintf(fp, "\n");
    }
}

void mod_corr_output(const mod_corr_t *self,
                     const state_t *s,
                     const index_t *idx,
                     int num_meas,
                     FILE *fp)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
        print(self->C4[i], idx, self->num_updates, s->params->T[i], fp);

    fflush(fp);
}
