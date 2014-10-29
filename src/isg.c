#include "sim.h"
#include <ctype.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define DEC_MAX 12
#define DEC_WARMUP 7
#define TEMP_FILE "temps.txt"
#define SUFFIX "_%06x.txt"

#define SAFE_OPEN(fp, filename, mode) \
    if ((fp = fopen(filename, mode)) == NULL) {\
        fprintf(stderr, "%s: error opening file `%s'\n", argv[0], filename);\
        exit(EXIT_FAILURE);\
    }

#define OPEN_FILE(fmt, seed, buf, fp, mode) {\
    sprintf(buf, fmt, seed); \
    SAFE_OPEN(fp, buf, mode); \
}

int main(int argc, char *argv[])
{
    FILE *fp_in;
    char buf[50], *fname_temps = TEMP_FILE;

    int i, c, num_seeds,
        num_bonds, num_tasks, rank,
        dec_max = DEC_MAX,
        dec_warmup = DEC_WARMUP;

    unsigned int seed;
    params_t p;
    state_t *s;
    output_files_t of;

    /* get options */
    while ((c = getopt(argc, argv, "d:t:w:")) != -1)
    {
        switch (c)
        {
            case 'd':
                dec_max = atoi(optarg);
                break;
            case 'w':
                dec_warmup = atoi(optarg);
                break;
            case 't':
                fname_temps = optarg;
                break;
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    /* non-option args are seeds */
    num_seeds = argc - optind;

    if (num_seeds < 1)
    {
        fprintf(stderr, "%s: error: must specify at least 1 seed\n", argv[0]);
        return EXIT_FAILURE;
    }
        
    /* read temperatures */

    SAFE_OPEN(fp_in, fname_temps, "r");

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        if (fscanf(fp_in, "%lf", p.T + i) != EOF)
            p.beta[i] = 1./p.T[i];
        else
        {
            fprintf(stderr, "%s: error: not enough temperatures in %s\n",
                    argv[0], fname_temps);

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

    if ((s = malloc(sizeof(state_t))) == NULL)
    {
        fprintf(stderr, "%s: error allocating memory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* load sample */
    seed = strtol(argv[optind + rank], NULL, 16); 
    OPEN_FILE("samp" SUFFIX, seed, buf, fp_in, "r");

    if (sample_read(&s->sample, fp_in, &num_bonds) != 0 || num_bonds != MAX_BONDS)
    {
        fprintf(stderr, "%s: error loading sample from file `%s'\n",
                argv[0], buf);

        exit(EXIT_FAILURE);
    }

    fclose(fp_in);

    /* open output files */
#define X(_) OPEN_FILE(#_ SUFFIX, seed, buf, of._, "w");
    MODULES
#undef X

    /* run the simulation */
    run_sim(s, &p, seed, dec_warmup, dec_max, &of);

    /* clean up */

#define X(_) fclose(of._);
    MODULES
#undef X

    free(s);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
