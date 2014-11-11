#include "mod_corr_logn.h"
#include "output.h"
#include "state.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", 6, "r");
    fprintf(fp, "%*s", COL_WIDTH, "C4");
    fprintf(fp, "\n");
}

void mod_corr_logn_init(mod_corr_logn_t *self, FILE *fp)
{
    print_header(fp);
}

void mod_corr_logn_reset(mod_corr_logn_t *self)
{
    memset(self->C4, 0., sizeof self->C4);
}

static void accumulate(const int S1[],
                       const int S2[],
                       double C4[])
{
    int i, r = 0;

    /*
     * compute correlation function at logarithmically-spaced points
     * (no boost from fft/convolution in this case)
     */

    for (i = 0; i < LOG_N; i++)
    {
        int j, sum = 0;

        for (j = 0; j < N-r; j++)
            sum += S1[j]*S2[j]*S1[j+r]*S2[j+r];
        for (j = N-r; j < N; j++)
            sum += S1[j]*S2[j]*S1[j+r-N]*S2[j+r-N];

        C4[i] += (double) sum/N;
        r = 2*r + 1;
    }
}

void mod_corr_logn_update(mod_corr_logn_t *self, const state_t *s)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica_t r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        accumulate(r1.S, r2.S, self->C4[i]);
    }
}

static void print(const double C4[],
                  const index_t *idx,
                  int num_meas,
                  double T,
                  FILE *fp)
{
    int i, r = 0;

    for (i = 0; i < LOG_N; i++)
    {
        index_print(idx, T, fp);

        fprintf(fp, "%*d", 6, r);
        fprintf(fp, "%*e", COL_WIDTH, C4[i]/num_meas);
        fprintf(fp, "\n");

        r = 2*r + 1;
    }
}

void mod_corr_logn_output(const mod_corr_logn_t *self,
                          const state_t *s,
                          const index_t *idx,
                          int num_meas,
                          FILE *fp)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
        print(self->C4[i], idx, num_meas, s->params->T[i], fp);

    fflush(fp);
}

void mod_corr_logn_cleanup(mod_corr_logn_t *self) {}

