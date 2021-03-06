#include "mod_corr.h"
#include "state.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", COL_WIDTH_R,     "r");
    fprintf(fp, "%*s", COL_WIDTH_FLOAT, "C4");
    fprintf(fp, "\n");
}

void mod_corr_init(mod_corr *self, FILE *fp)
{
    print_header(fp);
}

void mod_corr_reset(mod_corr *self)
{
    memset(self->C4, 0., sizeof self->C4);
}

static void accumulate(int const S1[],
                       int const S2[],
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

void mod_corr_update(mod_corr *self, state const *s)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        accumulate(r1.S, r2.S, self->C4[i]);
    }
}

static void print(double const C4[],
                  output_index const *idx,
                  int num_meas,
                  double T,
                  FILE *fp)
{
    int i, r = 0;

    for (i = 0; i < LOG_N; i++)
    {
        index_print(idx, T, fp);

        fprintf(fp, "%*d", COL_WIDTH_R,     r);
        fprintf(fp, "%*e", COL_WIDTH_FLOAT, C4[i]/num_meas);
        fprintf(fp, "\n");

        r = 2*r + 1;
    }
}

void mod_corr_output(mod_corr const *self,
                     state const *s,
                     output_index const *idx,
                     int num_meas,
                     FILE *fp)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
        print(self->C4[i], idx, num_meas, s->pm->T[i], fp);

    fflush(fp);
}

void mod_corr_cleanup(mod_corr *self) {}

