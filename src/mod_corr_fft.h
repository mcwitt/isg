#include "exchange.h"
#include "output.h"
#include "sample.h"
#include <complex.h>
#include <fftw3.h>

#define COL_WIDTH_R (NUM_DIGITS_N + COL_PAD)

typedef struct
{
    double C4[NUM_REPLICAS][N/2];

    double *cr;
    complex double *ck;
    fftw_plan dft, idct;

} mod_corr_fft_t;
