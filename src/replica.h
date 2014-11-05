#ifndef REPLICA_H
#define REPLICA_H

#include "sample.h"

typedef struct
{
    const sample_t *sample;
    const int *S;
    const double *h2;
    double u;

} replica_t;

#endif
