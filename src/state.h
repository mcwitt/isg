#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include "exchange.h"
#include "params.h"
#include "rng.h"
#include "sample.h"

/* struct containing minimum information needed for checkpointing */

typedef struct
{
    exchange_t x1, x2;
    sample_t sample;
    rng_t *rng;
    const params_t *params;
    int num_updates;

} state_t;

void state_init(state_t *s, const params_t *params, unsigned int seed);

void state_update(state_t *s, int num_updates);

void state_overlap(state_t *s,
                   int w,
                   double q2[NUM_REPLICAS],
                   double q4[NUM_REPLICAS]);

void state_link_overlap(state_t *s, double ql[NUM_REPLICAS]);

void state_checkpoint(state_t *s, FILE *fp);

void state_restore(state_t *s, FILE *fp);

void state_free(state_t *s);

#endif
