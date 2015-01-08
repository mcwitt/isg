#include "state.h"

void state_init(state *s, const params *pm, unsigned int seed)
{
    s->pm = pm;
    s->num_updates = 0;
    s->rand = RNG_ALLOC();
    RNG_SEED(s->rand, seed);

    exchange_init(&s->x1, &s->sample, pm->beta, s->rand);
    exchange_init(&s->x2, &s->sample, pm->beta, s->rand);
}

void state_update(state *s, int num_updates)
{
   int i;

   for (i = 0; i < num_updates; i++)
        exchange_update(&s->x1, s->rand);

   for (i = 0; i < num_updates; i++)
        exchange_update(&s->x2, s->rand);

    s->num_updates += num_updates;
}

void state_free(state *s)
{
    RNG_FREE(s->rand);
}
