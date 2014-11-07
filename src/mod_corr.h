#include "exchange.h"
#include "sample.h"
#include <complex.h>
#include <fftw3.h>
#include <stdio.h>

typedef struct
{
    double C4[NUM_REPLICAS][N/2];

    double *cr;
    complex *ck;
    fftw_plan dft, idct;

} mod_corr_t;
