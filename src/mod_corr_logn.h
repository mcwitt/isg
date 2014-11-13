#include "exchange.h"
#include "output.h"
#include "sample.h"

#define COL_WIDTH_R (NUM_DIGITS_N + COL_PAD)

typedef struct { double C4[NUM_REPLICAS][LOG_N]; } mod_corr_logn_t;
