#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include "exchange.h"
#include "params.h"
#include "rng.h"
#include "sample.h"

typedef struct
{
    exchange x1, x2;
    sample_data sample;
    rng *rand;
    params const *pm;
    int num_updates;

} state;

void state_init(state *s, params const *pm, unsigned int seed);

void state_update(state *s, int num_updates);

void state_checkpoint(state const *s, FILE *fp);

void state_restore(state *s, FILE *fp);

void state_free(state *s);

#endif
