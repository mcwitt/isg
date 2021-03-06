#include "exchange.h"
#include <math.h>

void exchange_init(exchange *x,
                   sample_data const *s,
                   double const beta[NUM_REPLICAS],
                   rng *rand)
{
    int i;

    x->sample = s;
    x->beta = beta;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        x->b2r[i] = i;
        x->r2b[i] = i;
        x->swapped[i] = 0;
        init_replica(s->n, s->J4, s->z, s->h2m, s->um,
                     x->S[i], x->h2[i], &x->u[i], rand);
    }
}

void exchange_update(exchange *x, rng *rand)
{
    int ir;

    for (ir = 0; ir < NUM_REPLICAS; ir++)
        update_replica(x->sample->n, x->sample->J4, x->sample->z,
                       x->beta[x->r2b[ir]],
                       x->S[ir], x->h2[ir], &x->u[ir], rand);

#if REPLICA_EXCHANGE
    update_exchange(x->beta, x->u, x->b2r, x->r2b, x->swapped, rand);
#endif
}

void exchange_get_replica(exchange const *x, int ib, replica *r)
{
    int ir = x->b2r[ib];

    r->sample = x->sample;
    r->S  = x->S[ir];
    r->h2 = x->h2[ir];
    r->u  = x->u[ir];
}

void update_exchange(double const beta[NUM_REPLICAS],
                     double const u[NUM_REPLICAS],
                     int b2r[NUM_REPLICAS],
                     int r2b[NUM_REPLICAS],
                     int swapped[NUM_REPLICAS],
                     rng *rand)
{
    int ib2;

    for (ib2 = 1; ib2 < NUM_REPLICAS; ib2++)
    {
        int ib1 = ib2 - 1,
            ir1 = b2r[ib1],
            ir2 = b2r[ib2];

        double db, du, r;

        db = beta[ib2] - beta[ib1]; /* negative */
        du = u[ir2] - u[ir1];

        if (du < 0. || (r = RNG_UNIFORM(rand), r < exp(db*du)))
        {
#define SET_TEMP(ir, ib) { b2r[ib] = ir; r2b[ir] = ib; }
            SET_TEMP(ir1, ib2); /* link replica 1 to temperature 2 */
            SET_TEMP(ir2, ib1); /* etc. */
#undef SET_TEMP
            swapped[ib2] = 1;
        }
        else swapped[ib2] = 0;
    }
}

