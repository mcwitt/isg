#include "mod_hist.h"
#include "output.h"
#include "state.h"
#include <stdio.h>
#include <string.h>

int spin_overlap_part(const int S1[], const int S2[], int n)
{
    int i, sum = 0;

    for (i = 0; i < n; i++)
        sum += S1[i]*S2[i];

    return sum;
}

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", 6, "n");
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
    int i, n;

    for (i = LOG_N, n = N;
         i >= LOG_MIN_BLOCK_SIZE;
         i--, f += n + 1, n /= 2)
    {
        int j;

        for (j = 0; j < N; j += n)
        {
            int nq = spin_overlap_part(S1 + j, S2 + j, n);
            f[(nq + n)/2]++;
        }
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
    int i, n;

    for (i = LOG_N, n = N;
         i >= LOG_MIN_BLOCK_SIZE;
         i--, f += n + 1, n /= 2)
    {
        int j;

        for (j = 0; j < n+1; j++)
        {
            index_print(idx, T, fp);

            fprintf(fp, "%*d", 6, n);
            fprintf(fp, "%*d", 6, j);
            fprintf(fp, "%*lu", 12, f[j]);
            fprintf(fp, "\n");
        }
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

