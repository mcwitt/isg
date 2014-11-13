#include "mod_hist.h"
#include "state.h"
#include <stdio.h>
#include <string.h>

static int spin_overlap(const int S1[], const int S2[], int w)
{
    int i, sum = 0;

    for (i = 0; i < w; i++)
        sum += S1[i]*S2[i];

    return sum;
}

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", COL_WIDTH_W,     "W");
    fprintf(fp, "%*s", COL_WIDTH_BIN,   "bin");
    fprintf(fp, "%*s", COL_WIDTH_F,     "f");
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

    /*
     * w = block size
     * m = number of histogram bins
     * w/m = bin size
     */

#define NEXT_BLOCK(w, m)                     \
{                                            \
    int j;                                   \
                                             \
    for (j = 0; j < N; j += w)               \
    {                                        \
        int q = spin_overlap(S1+j, S2+j, w); \
        f[(q + w)/2/(w/m)]++;                \
    }                                        \
                                             \
    f += m + 1;                              \
    w *= 2;                                  \
}

    /* one bin per q value for smaller blocks */
    for (i = 0; i <= LOG_M; i++)
        NEXT_BLOCK(w, w);

    /* "coarse-grain" histograms for larger blocks */
    for (i = LOG_M + 1; i <= LOG_N; i++)
        NEXT_BLOCK(w, M);

#undef NEXT_BLOCK
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

#define PRINT_BLOCK(w, m)                           \
{                                                   \
    int j;                                          \
                                                    \
    for (j = 0; j < m + 1; j++)                     \
    {                                               \
        index_print(idx, T, fp);                    \
        fprintf(fp, "%*d",  COL_WIDTH_W,     w);    \
        fprintf(fp, "%*d",  COL_WIDTH_BIN,   j);    \
        fprintf(fp, "%*lu", COL_WIDTH_F,     f[j]); \
        fprintf(fp, "\n");                          \
    }                                               \
                                                    \
    f += m + 1;                                     \
    w *= 2;                                         \
}

    for (i = 0; i <= LOG_M; i++)
        PRINT_BLOCK(w, w);

    for (i = LOG_M + 1; i <= LOG_N; i++)
        PRINT_BLOCK(w, M);

#undef PRINT_BLOCK
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

