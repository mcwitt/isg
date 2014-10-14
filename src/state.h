/* struct containing minimum information needed for checkpointing */

#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include "exchange.h"
#include "params.h"
#include "rng.h"
#include "sample.h"


#define OVERLAP_DEG 2   /* 2-point correlations */

typedef struct
{
    exchange_t x[OVERLAP_DEG];
    sample_t sample;
    rng_t *rng;
    params_t *params;
    int num_updates;

} state_t;

void state_init(state_t *s, params_t *params, unsigned int seed);

void state_update(state_t *s, int num_updates);

void state_checkpoint(state_t *s, FILE *fp);

void state_restore(state_t *s, FILE *fp);

void state_free(state_t *s);

#endif
