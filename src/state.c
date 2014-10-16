#include "state.h"


void state_init(state_t *s, const params_t *params, unsigned int seed)
{
    int i;

    s->params = params;
    s->num_updates = 0;
    s->rng = RNG_ALLOC();
    RNG_SEED(s->rng, seed);

    for (i = 0; i < OVERLAP_DEG; i++)
        exchange_init(&s->x[i], &s->sample, params->beta);
}

void state_update(state_t *s, int num_updates)
{
    int i, j;

    for (i = 0; i < OVERLAP_DEG; i++)
       for (j = 0; j < num_updates; j++)
            exchange_update(&s->x[i], s->rng);

    s->num_updates += num_updates;
}

void state_free(state_t *s)
{
    RNG_FREE(s->rng);
}
