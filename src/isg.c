#include "sim.h"
#include <ctype.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define DEC_MAX 10
#define TEMP_FILE "temps.txt"
#define SAMPLE_FILENAME_FMT "sample_%06x.txt"
#define OUTPUT_FILENAME_FMT "out_%06x.txt"

#define SAFE_OPEN(fp, filename, mode) \
    if ((fp = fopen(filename, mode)) == NULL) {\
        fprintf(stderr, "%s: error opening file `%s'\n", argv[0], filename);\
        exit(EXIT_FAILURE);\
    }

int main(int argc, char *argv[])
{
    FILE *fp;
    char buf[50], *filename = TEMP_FILE;
    double T[NUM_REPLICAS];
    int i, c, num_seeds, dec_max = DEC_MAX;
    int num_bonds, num_tasks, rank;
    unsigned int seed;
    params_t p;
    state_t *s;

    /* get options */
    while ((c = getopt(argc, argv, "d:t:")) != -1)
    {
        switch (c)
        {
            case 'd':
                dec_max = atoi(optarg);
                break;
            case 't':
                filename = optarg;
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
    SAFE_OPEN(fp, filename, "r");

    for (i = 0; i < NUM_REPLICAS; i++)
    {
        if (fscanf(fp, "%lf", &T[i]) != EOF)
            p.beta[i] = 1./T[i];
        else
        {
            fprintf(stderr,
                    "%s: error: not enough temperatures in %s\n",
                    argv[0], filename);

            return EXIT_FAILURE;
        }
    }

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
    sprintf(buf, SAMPLE_FILENAME_FMT, seed);
    SAFE_OPEN(fp, buf, "r");

    if (sample_read(&s->sample, fp, &num_bonds) != 0 || num_bonds != NUM_BONDS)
    {
        fprintf(stderr,
                "%s: error loading sample from file `%s'\n",
                argv[0], buf);

        exit(EXIT_FAILURE);
    }

    /* open output file and run simulation */
    sprintf(buf, OUTPUT_FILENAME_FMT, seed);
    SAFE_OPEN(fp, buf, "w");
    run_sim(s, &p, T, seed, dec_max, fp);

    free(s);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
