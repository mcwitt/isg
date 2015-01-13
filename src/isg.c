#include "isg.h"
#include "bonds.h"
#include "bond_dist.h"
#include "modules.h"
#include "rng.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define SAFE_OPEN(fp, filename, mode) \
    if ((fp = fopen(filename, mode)) == NULL) {\
        fprintf(stderr, "%s: error opening file `%s'\n", argv[0], filename);\
        exit(EXIT_FAILURE);\
    }

#define OPEN_FILE(fmt, seed, buf, fp, mode) {\
    sprintf(buf, fmt, seed); \
    SAFE_OPEN(fp, buf, mode); \
}

void run_sim(state *s,
             params const *p,
             unsigned int seed,
             output_files const *of)
{
    int b, i, num_updates;
    modules mods;

    state_init(s, p, seed);
    state_update(s, pow(2, LOG_WARMUP_UPDATES));
    MODULES(MODULE_INIT, mods, of);

    for (b = LOG_WARMUP_UPDATES + 1; b <= LOG_NUM_UPDATES; b++)
    {
        output_index idx = {b};
        num_updates = pow(2, b - LOG_UPDATES_PER_MEAS);
        MODULES(MODULE_RESET, mods);

        for (i = 0; i < num_updates; i++)
        {
            state_update(s, UPDATES_PER_MEAS);
            MODULES(MODULE_UPDATE, mods, s);
        }

        MODULES(MODULE_OUTPUT, mods, s, &idx, num_updates, of);
    }

    MODULES(MODULE_CLEANUP, mods);
    state_free(s);
}

int main(int argc, char *argv[])
{
    FILE *fp_in;
    char buf[50], **seeds;
    int c, i, num_seeds, num_tasks, rank;
    unsigned int seed;
    params p;
    state *s;
    output_files of;

    num_seeds = argc - 1;
    seeds = argv + 1;

    if (num_seeds < 1)
    {
        fprintf(stderr, "%s: error: must specify at least 1 seed\n", argv[0]);
        return EXIT_FAILURE;
    }
        
    /* read temperatures */

    SAFE_OPEN(fp_in, TEMP_FILE, "r");

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        if (fscanf(fp_in, "%lf", p.T + i) != EOF)
            p.beta[i] = 1./p.T[i];
        else
        {
            fprintf(stderr, "%s: error: not enough temperatures in %s\n",
                    argv[0], TEMP_FILE);

            return EXIT_FAILURE;
        }
    }

    fclose(fp_in);

    /* start MPI */
    if ((c = MPI_Init(&argc, &argv)) != MPI_SUCCESS)
    {
        fprintf(stderr, "%s: error starting MPI\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, c);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (num_seeds > num_tasks)
        fprintf(stderr, "%s: warning: specified %d seeds with only %d MPI tasks\n",
                argv[0], num_seeds, num_tasks);
    else if (rank >= num_seeds)
    {
        fprintf(stderr, "%s: error: no seed specified for task %d\n", argv[0], rank);
        exit(EXIT_FAILURE);
    }

    if ((s = malloc(sizeof(state))) == NULL)
    {
        fprintf(stderr, "%s: error allocating memory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    seed = strtol(seeds[rank], NULL, 16);

#if GENERATE_SAMPLE
    /* generate a new sample */
    {
        bonds *b = malloc(sizeof(bonds));
        rng *rand = RNG_ALLOC();
        int *bond_site;
        double *bond_value;

        bonds_init(b);
        RNG_SEED(rand, seed);
        bond_dist_dilute(b, SIGMA, rand);
        RNG_FREE(rand);

        bond_site =  malloc(2*NUM_BONDS*sizeof(int));
        bond_value = malloc(  NUM_BONDS*sizeof(double));

        bonds_to_list(b, bond_site, bond_value);
        bonds_free(b);
        free(b);

        sample_init(&s->sample, bond_site, bond_value);

        free(bond_site);
        free(bond_value);
    }
#else
    /* load sample from text file */
    OPEN_FILE("samp" SUFFIX, seed, buf, fp_in, "r");
    sample_init_read(&s->sample, fp_in);
    fclose(fp_in);
#endif

    /* open output files */
#define OPEN_OUTPUT(name, dummy) \
    OPEN_FILE(#name SUFFIX, seed, buf, of.name, "w");

    MODULES(OPEN_OUTPUT)

    /* run the simulation */
    run_sim(s, &p, seed, &of);

    /* clean up */

#define CLOSE_OUTPUT(name, dummy) fclose(of.name);
    MODULES(CLOSE_OUTPUT)

    free(s);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
