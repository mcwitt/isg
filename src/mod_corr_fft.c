#include "mod_corr_fft.h"
#include "state.h"
#include <math.h>
#include <string.h>

static void print_header(FILE *fp)
{
    print_index_header(fp);

    fprintf(fp, "%*s", COL_WIDTH_R,     "r");
    fprintf(fp, "%*s", COL_WIDTH_FLOAT, "C4");
    fprintf(fp, "\n");
}

void mod_corr_fft_init(mod_corr_fft_t *self, FILE *fp)
{
    self->cr = (double*) fftw_malloc(sizeof(double) * N);
    self->ck = (complex double*) fftw_malloc(sizeof(complex double) * N);

    self->dft = fftw_plan_dft_r2c_1d(N, self->cr, self->ck, FFTW_MEASURE);

    self->idct = fftw_plan_r2r_1d(N/2 + 1, self->cr, self->cr,
                                  FFTW_REDFT00, FFTW_MEASURE);

    print_header(fp);
}

void mod_corr_fft_reset(mod_corr_fft_t *self)
{
    memset(self->C4, 0., sizeof self->C4);
}

static void accumulate(const int S1[],
                       const int S2[],
                       double C4[],
                       double cr[],
                       complex double ck[],
                       fftw_plan dft,
                       fftw_plan idct)
{
    int i;

    /* use convolution theorem to compute correlation function */

    /* compute the overlap field */
    for (i = 0; i < N; i++) cr[i] = (1./N)*S1[i]*S2[i];

    /* FT to k-space and square */
    fftw_execute(dft);
    for (i = 0; i <= N/2; i++) cr[i] = pow(cabs(ck[i]), 2);

    /* FT back to real space */
    fftw_execute(idct);
    for (i = 0; i < N/2; i++) C4[i] += cr[i];
}

void mod_corr_fft_update(mod_corr_fft_t *self, const state_t *s)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica_t r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        accumulate(r1.S, r2.S, self->C4[i],
                   self->cr, self->ck,
                   self->dft, self->idct);
    }
}

static void print(const double C4[],
                  const index_t *idx,
                  int num_meas,
                  double T,
                  FILE *fp)
{
    int r;

    for (r = 0; r < N/2; r++)
    {
        index_print(idx, T, fp);

        fprintf(fp, "%*d", COL_WIDTH_R,     r);
        fprintf(fp, "%*e", COL_WIDTH_FLOAT, C4[r]/num_meas);
        fprintf(fp, "\n");
    }
}

void mod_corr_fft_output(const mod_corr_fft_t *self,
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

void mod_corr_fft_cleanup(mod_corr_fft_t *self)
{
    fftw_destroy_plan(self->dft);
    fftw_destroy_plan(self->idct);

    fftw_free(self->cr);
    fftw_free(self->ck);
}
