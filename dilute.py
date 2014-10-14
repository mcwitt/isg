import numpy as np
import numpy.random
from bisect import bisect
from numpy import pi, sin, sum
from numpy.random import rand, normal

def chord_distance(N, k):
    return abs(N/pi*sin(pi*k/N))

def random_bonds(N, z, sigma, distance=chord_distance):
    """Generate a random dilute bond configuration.

    Parameters
    ----------
    N : int
        Number of sites
    z : int
        Coordination number (number of bonds for each site)
    sigma : float
        Interaction decay parameter

    Notes
    -----
    The probability of a bond between falls off with distance as
    .. math:: p_{ij} \sim 1/r^{2 \sigma}
    at large distance.
    """

    s = np.arange(1, N)
    w = np.empty(N)
    w[0] = 0.
    w[1:] = 1./distance(N, s)**(2*sigma)
    f = np.cumsum(w)
    f /= f[-1]
    bonds = {}
    
    while len(bonds) < N*z/2:
        i = int(rand()*N)
        s = bisect(f, rand())
        j = i + s
        
        if j >= N:
            i, j = j - N, i

        # create a bond if one does not already exist
        if (i, j) not in bonds:
            bonds[i, j] = normal(0, 1)
            
    return bonds

if __name__=='__main__':
    import argparse
    import sys

    parser = argparse.ArgumentParser(description='generate a random dilute bond configuration')
    parser.add_argument('N', type=int, help='system size')
    parser.add_argument('z', type=int, help='average coordination number')
    parser.add_argument('sigma', type=float, help='interactions fall off with distance as r^(-2*sigma)')
    parser.add_argument('-n', type=int, help='number of samples to generate')
    parser.add_argument('--seed', type=int, help='random seed')
    parser.add_argument('--fmt', type=str, help='format string for output filenames')

    args = parser.parse_args()

    def write_sample(bonds, output):
        for (i, j), v in bonds.items():
            output.write('{} {} {}\n'.format(i, j, v))

    if args.seed:
        np.random.seed(args.seed)

    if args.n is None:
        write_sample(random_bonds(args.N, args.z, args.sigma), sys.stdout)
    else:
        fmt = args.fmt if args.fmt else 'sample_{}.txt'

        for i in range(args.n):
            seed = np.random.random_integers(0, 2**24)
            np.random.seed(seed)
            bonds = random_bonds(args.N, args.z, args.sigma)
            s = '{:06x}'.format(seed)
            write_sample(bonds, open(fmt.format(s), 'w'))
            print s

