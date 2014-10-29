#include "mod_spinstate.h"

void mod_spinstate_init(mod_spinstate_t *self, FILE *fp)
{
    self->fp = fp;
    print_index_header(fp);
    fprintf(fp, "%*s", 3, "c");
    fprintf(fp, "%*s", N+2, "S");
    fprintf(fp, "\n");
}

void mod_spinstate_reset(mod_spinstate_t *self) {}
void mod_spinstate_update(mod_spinstate_t *self, const state_t *s) {}

static void print_spins(const int S[], FILE *fp)
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

void mod_spinstate_output(const mod_spinstate_t *self,
                          const index_t *idx,
                          const state_t *s,
                          int num_updates)
{
    int i;

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        replica_t r1, r2;

        exchange_get_replica(&s->x1, i, &r1);
        exchange_get_replica(&s->x2, i, &r2);

        index_print(idx, s->params->T[i], self->fp);
        fprintf(self->fp, "%3d  ", 0);
        print_spins(r1.S, self->fp);
        
        index_print(idx, s->params->T[i], self->fp);
        fprintf(self->fp, "%3d  ", 1);
        print_spins(r2.S, self->fp);
    }
}
