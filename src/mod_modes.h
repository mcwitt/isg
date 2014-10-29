#include "module.h"
#include "exchange.h"
#include "sample.h"

typedef struct
{
    double coskr[N], sinkr[N];
    double q2k1[NUM_REPLICAS];
    FILE *fp;
} mod_modes_t;

MODULE_INTERFACE(modes);

