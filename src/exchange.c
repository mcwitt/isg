#include "exchange.h"
#include "replica.h"
#include <math.h>


void exchange_init(exchange_t *x, sample_t *s, const double beta[NUM_REPLICAS])
{
    int i;

    x->sample = s;
    x->beta = beta;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        x->b2r[i] = i;
        x->r2b[i] = i;
        x->swapped[i] = 0;
        init_replica(s->h2m, s->um, x->S[i], x->h2[i], &x->u[i]);
    }
}

void exchange_update(exchange_t *x, rng_t *rng)
{
    int ir;

    for (ir = 0; ir < NUM_REPLICAS; ir++)
        update_replica(x->sample->n, x->sample->J4,
                       x->beta[x->r2b[ir]],
                       x->S[ir], x->h2[ir], &x->u[ir], rng);

#if REPLICA_EXCHANGE
    update_exchange(x->beta, x->u, x->b2r, x->r2b, x->swapped, rng);
#endif
}

void update_exchange(const double beta[NUM_REPLICAS],
                     const double u[NUM_REPLICAS],
                     int b2r[NUM_REPLICAS],
                     int r2b[NUM_REPLICAS],
                     int swapped[NUM_REPLICAS],
                     rng_t *rng)
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

        if (du < 0. || (r = RNG_UNIFORM(rng), r < exp(db*du)))
        {
#define SET_BETA(ir, ib) { b2r[ib] = ir; r2b[ir] = ib; }
            SET_BETA(ir1, ib2);     /* move replica 1 to temperature slot 2 */
            SET_BETA(ir2, ib1);     /* etc. */
#undef SET_BETA
            swapped[ib2] = 1;
        }
        else swapped[ib2] = 0;
    }
}

