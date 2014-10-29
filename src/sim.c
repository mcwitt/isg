#include "sim.h"
#include "output.h"
#include <math.h>
#include <string.h>

#define X(_) mod_##_##_t _;
typedef struct { MODULES } modules_t;
#undef X

#define X(_) mod_##_##_init(&mods->_, f->_);
void init(modules_t *mods, const output_files_t *f) { MODULES } 
#undef X

#define X(_) mod_##_##_reset(&mods->_);
void reset(modules_t *mods) { MODULES }
#undef X

#define X(_) mod_##_##_update(&mods->_, s);
void update(modules_t *mods, const state_t *s) { MODULES }
#undef X

void output(const modules_t *mods,
            const state_t *s,
            const index_t *idx,
            int num_updates)
{
#define X(_) mod_##_##_output(&mods->_, idx, s, num_updates);
        MODULES
#undef X
}

void run_sim(state_t *s,
             const params_t *p,
             unsigned int seed,
             int dec_warmup,
             int dec_max,
             const output_files_t *of)
{
    int d, i, num_updates;
    modules_t mods;

    state_init(s, p, seed);
    state_update(s, pow(2, dec_warmup));
    init(&mods, of);

    for (d = dec_warmup + 1; d <= dec_max; d++)
    {
        index_t idx = {d};
        num_updates = pow(2, d - DEC_PER_MEAS);
        reset(&mods);

        for (i = 0; i < num_updates; i++)
        {
            state_update(s, UPDATES_PER_MEAS);
            update(&mods, s);
        }

        output(&mods, s, &idx, num_updates);
    }

    state_free(s);
}

