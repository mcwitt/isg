#include "exchange.h"
#include "isg.h"
#include "sample.h"
#include <stdio.h>

#define MOD_CORR_UPDATE_EVERY (N / UPDATES_PER_MEAS)

typedef struct
{
    double C4[NUM_REPLICAS][N/2];
    int num_calls, num_updates;

} mod_corr_t;
