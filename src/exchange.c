#include "exchange.h"
#include <math.h>

#define SHARED_RANDOM 1

void init_replicas(const int n[NZ_MAX],
                   const double J4[NZ_MAX],
                   const double h2m[N],
                   double um,
                   int S[N][NUM_REPLICAS],
                   double h2[N][NUM_REPLICAS],
                   double u[NUM_REPLICAS],
                   rng_t *rng)
{
    int i, j;

    /* initialize with spins up */

    for (j = 0; j < NUM_REPLICAS; j++)
        u[j] = um;

    for (i = 0; i < N; i++)
        for (j = 0; j < NUM_REPLICAS; j++)
            h2[i][j] = h2m[i];

    /* randomize spins */

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < NUM_REPLICAS; j++)
        {
            int k;

            if (RNG_UNIFORM(rng) < 0.5)
            {
                S[i][j] = 1;
            }
            else
            {
                S[i][j] = -1;
                u[j] += h2[i][j];

                for (k = 0; k < Z_MAX; k++)
                {
                    int ik = i*Z_MAX + k;
                    if (n[ik] == -1) break;
                    h2[n[ik]][j] -= J4[ik];
                }
            }
        }
    }
}

#if SHARED_RANDOM
#define TOSS(p, r, rng) (r < p)
#else
#define TOSS(p, r, rng) (r = RNG_UNIFORM(rng), r < p)
#endif

void update_sweep(const int n[NZ_MAX],
                  const double J4[NZ_MAX],
                  const double beta[NUM_REPLICAS],
                  const int ib[NUM_REPLICAS],
                  int S[N][NUM_REPLICAS],
                  double h2[N][NUM_REPLICAS],
                  double u[NUM_REPLICAS],
                  rng_t *rng)
{
    int i;

    for (i = 0; i < N; i++)
    {
        int j;
        double r;

#if SHARED_RANDOM
        r = RNG_UNIFORM(rng);
#endif

        for (j = 0; j < NUM_REPLICAS; j++)
        {
            int k;
            double b = beta[ib[j]];

            if (S[i][j] == 1)
            {
                if (h2[i][j] < 0. || TOSS(exp(-b*h2[i][j]), r, rng))
                {
                    S[i][j] = -1;
                    u[j] += h2[i][j];

                    for (k = 0; k < Z_MAX; k++)
                    {
                        int ik = i*Z_MAX + k;
                        if (n[ik] == -1) break;
                        h2[n[ik]][j] -= J4[ik];
                    }
                }
            }
            else    /* S[i][j] == -1 */
            {
                if (h2[i][j] > 0. || TOSS(exp(b*h2[i][j]), r, rng))
                {
                    S[i][j] = 1;
                    u[j] -= h2[i][j];

                    for (k = 0; k < Z_MAX; k++)
                    {
                        int ik = i*Z_MAX + k;
                        if (n[ik] == -1) break;
                        h2[n[ik]][j] += J4[ik];
                    }
                }
            }
        }
    }
}

void update_exchange(const double beta[NUM_REPLICAS],
                     const double u[NUM_REPLICAS],
                     int ir[NUM_REPLICAS],
                     int ib[NUM_REPLICAS],
                     int swapped[NUM_REPLICAS],
                     rng_t *rng)
{
    int ib2;

    for (ib2 = 1; ib2 < NUM_REPLICAS; ib2++)
    {
        int ib1 = ib2 - 1,
            ir1 = ir[ib1],
            ir2 = ir[ib2];

        double db, du, r;

        db = beta[ib2] - beta[ib1]; /* negative */
        du = u[ir2] - u[ir1];

        if (du < 0. || (r = RNG_UNIFORM(rng), r < exp(db*du)))
        {
#define SET_TEMP(irx, ibx) { ir[ibx] = ir; ib[irx] = ib; }
            SET_TEMP(ir1, ib2);     /* link replica 1 to temperature 2 */
            SET_TEMP(ir2, ib1);     /* etc. */
#undef SET_TEMP
            swapped[ib2] = 1;
        }
        else swapped[ib2] = 0;
    }
}

void exchange_init(exchange_t *x,
                   const sample_t *s,
                   const double beta[NUM_REPLICAS],
                   rng_t *rng)
{
    int i;

    x->sample = s;
    x->beta = beta;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        x->ir[i] = i;
        x->ib[i] = i;
        x->swapped[i] = 0;
    }

    init_replicas(s->n, s->J4, s->h2m, s->um,
                  x->S, x->h2, x->u, rng);
}

void exchange_update(exchange_t *x, rng_t *rng)
{
    update_sweep(x->sample->n, x->sample->J4, x->beta,
                 x->ib, x->S, x->h2, x->u, rng);

#if REPLICA_EXCHANGE
    update_exchange(x->beta, x->u, x->ir, x->ib, x->swapped, rng);
#endif
}

