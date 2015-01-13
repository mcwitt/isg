from numpy.random import random_integers

if __name__=='__main__':
    import sys
    num_seeds = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    seeds = random_integers(0, 2**24, num_seeds)

    for seed in seeds:
        print '{:06x}'.format(seed)

        
