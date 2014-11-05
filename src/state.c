#include "state.h"


void state_init(state_t *s, const params_t *params, unsigned int seed)
{
    s->params = params;
    s->num_updates = 0;
    s->rng = RNG_ALLOC();
    RNG_SEED(s->rng, seed);

    exchange_init(&s->x1, &s->sample, params->beta, s->rng);
    exchange_init(&s->x2, &s->sample, params->beta, s->rng);
}

void state_update(state_t *s, int num_updates)
{
   int i;

   for (i = 0; i < num_updates; i++)
        exchange_update(&s->x1, s->rng);

   for (i = 0; i < num_updates; i++)
        exchange_update(&s->x2, s->rng);

    s->num_updates += num_updates;
}

void state_free(state_t *s)
{
    RNG_FREE(s->rng);
}
