#include "mod_spinstate.h"
#include "state.h"
#include "output.h"

void mod_spinstate_init(mod_spinstate *self, FILE *fp)
{
    print_index_header(fp);
    fprintf(fp, "%*s", 3, "c");
    fprintf(fp, "%*s", N+2, "S");
    fprintf(fp, "\n");
}

void mod_spinstate_reset(mod_spinstate *self) {}
void mod_spinstate_update(mod_spinstate *self, state_t const *s) {}

static void print_spins(int const S[], FILE *fp)
{
    int i;

    for (i = 0; i < N; i++)
    {
        if (S[i] == 1)
            fprintf(fp, "1");
        else
            fprintf(fp, "0");
    }

    fprintf(fp, "\n");
}

void mod_spinstate_output(mod_spinstate const *self,
                          state_t const *s,
                          index_t const *idx,
                          int num_updates,
                          FILE *fp)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica_t r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        index_print(idx, s->params->T[i], fp);
        fprintf(fp, "%3d  ", 0);
        print_spins(r1.S, fp);
        
        index_print(idx, s->params->T[i], fp);
        fprintf(fp, "%3d  ", 1);
        print_spins(r2.S, fp);
    }

    fflush(fp);
}

void mod_spinstate_cleanup(mod_spinstate *self) {}

