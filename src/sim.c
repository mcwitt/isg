#include "sim.h"
#include "macros.h"
#include <math.h>

enum
{
    U = 0,
    Q2,
    Q4,
    QL,
    RX,
    NUM_MEAS_KINDS
};

/* use constant names to label output columns */
#define S(x) [x] = #x
char *meas_name[] = {
    S(U),
    S(Q2),
    S(Q4),
    S(QL),
    S(RX)
};
#undef S

double spin_overlap(const int S1[N], const int S2[N])
{
    int i, sum = 0;

    for (i = 0; i < N; i++)
        sum += S1[i]*S2[i];

    return (double) sum/N;
}

double link_overlap(const int bond_i[MAX_BONDS],
                    const int bond_j[MAX_BONDS],
                    int num_bonds,
                    const int S1[N],
                    const int S2[N])
{
    int k, sum = 0;

    for (k = 0; k < num_bonds; k++)
    {
        int i, j;
        i = bond_i[k];
        j = bond_j[k];
        sum += S1[i]*S1[j]*S2[i]*S2[j];
    }

    return (double) sum/num_bonds;
}

void accumulate(state_t *s, double sum[][NUM_MEAS_KINDS])
{
    int ib;

    for (ib = 0; ib < NUM_REPLICAS; ib++)
    {
        int ir = s->x->b2r[ib];
        int   *S1 = s->x[0].S[ir], *S2 = s->x[1].S[ir];
        double u1 = s->x[0].u[ir],  u2 = s->x[1].u[ir];
        double q = spin_overlap(S1, S2);

        sum[ib][U] += 0.5*(u1 + u2)/N;
        sum[ib][Q2] += q*q;
        sum[ib][Q4] += q*q*q*q;

        sum[ib][QL] += link_overlap(s->sample.bond_i,
                                    s->sample.bond_j,
                                    s->sample.num_bonds,
                                    S1, S2);

        sum[ib][RX] += 0.5*(s->x[0].swapped[ib] + s->x[1].swapped[ib]);
    }
}

void print_header(FILE *fp)
{
    int i;

    fprintf(fp, "%*s", COL_WIDTH, "B");
    fprintf(fp, "%*s", COL_WIDTH, "T");

    for (i = 0; i < NUM_MEAS_KINDS; i++)
        fprintf(fp, "%*s", COL_WIDTH, meas_name[i]);

    fprintf(fp, "\n");
}

void print_averages(FILE *fp, int d, double T[NUM_REPLICAS],
                    double sum[][NUM_MEAS_KINDS], int num_meas)
{
    int i, j;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        fprintf(fp, "%*d", COL_WIDTH, d);
        fprintf(fp, "%*g", COL_WIDTH, T[i]);

        for (j = 0; j < NUM_MEAS_KINDS; j++)
            fprintf(fp, "%*g", COL_WIDTH, sum[i][j]/num_meas);

        fprintf(fp, "\n");
    }

    fflush(fp);
}

void run_sim(state_t *s,
             params_t *p,
             double T[NUM_REPLICAS],
             unsigned int seed,
             int dec_max,
             FILE *fp)
{
    int d, i, num_meas;
    double sum[NUM_REPLICAS][NUM_MEAS_KINDS];

    state_init(s, p, seed);
    state_update(s, pow(2, DEC_WARMUP));
    print_header(fp);

    for (d = DEC_WARMUP; d <= dec_max; d++)
    {
        num_meas = pow(2, d - DEC_PER_MEAS);
        ZERO(&sum[0][0], NUM_REPLICAS * NUM_MEAS_KINDS);

        for (i = 0; i < num_meas; i++)
        {
            state_update(s, UPDATES_PER_MEAS);
            accumulate(s, sum);
        }

        print_averages(fp, d, T, sum, num_meas);
    }

    state_free(s);
}

