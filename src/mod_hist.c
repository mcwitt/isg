#include "mod_hist.h"
#include "output.h"
#include "state.h"
#include <stdio.h>
#include <string.h>

int spin_overlap_window(const int S1[], const int S2[], int w)
{
    int i, sum = 0;

    for (i = 0; i < w; i++)
        sum += S1[i]*S2[i];

    return sum;
}

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", 6, "W");
    fprintf(fp, "%*s", 6, "bin");
    fprintf(fp, "%*s", 12, "f");
    fprintf(fp, "\n");
}

void mod_hist_init(mod_hist_t *self, FILE *fp)
{
    print_header(fp);
}

void mod_hist_reset(mod_hist_t *self)
{
    memset(self->f, 0, sizeof self->f);
}

static void accumulate(UINT *f, const int S1[], const int S2[])
{
    int i, w = 1;

    for (i = 0; i <= LOG_N; i++)
    {
        int j;

        for (j = 0; j < N; j += w)
        {
            int wq = spin_overlap_window(S1 + j, S2 + j, w);
            f[(wq + w)/2]++;
        }

        f += w + 1;
        w *= 2;
    }
}

void mod_hist_update(mod_hist_t *self, const state_t *s)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica_t r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        accumulate(self->f[i], r1.S, r2.S);
    }
}

static void print(const UINT *f, const index_t *idx, double T, FILE *fp)
{
    int i, w = 1;

    for (i = 0; i <= LOG_N; i++)
    {
        int j;

        for (j = 0; j < w+1; j++)
        {
            index_print(idx, T, fp);

            fprintf(fp, "%*d", 6, w);
            fprintf(fp, "%*d", 6, j);
            fprintf(fp, "%*lu", 12, f[j]);
            fprintf(fp, "\n");
        }

        f += w + 1;
        w *= 2;
    }
}

void mod_hist_output(const mod_hist_t *self,
                     const state_t *s,
                     const index_t *idx,
                     int num_meas,
                     FILE *fp)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
        print(self->f[i], idx, s->params->T[i], fp);

    fflush(fp);
}

void mod_hist_cleanup(mod_hist_t *self) {}

