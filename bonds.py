import numpy as np
import numpy.random
from bisect import bisect
from numpy import pi, sin, sqrt, sum
from numpy.random import rand, normal


def chord_distance(N, k):
    return abs(N/pi*sin(pi*k/N))


def bonds(N, sigma=0., distance=chord_distance):
    """Generate a random (undiluted) bond configuration.

    Parameters
    ----------
    N : int
        Number of sites

    sigma : float
        Interaction decay parameter

    Notes
    -----
    The variance falls off with distance as
    ..math:: \overline{J_{ij}^2} \sim 1/r_{ij}^{2 \sigma}
    """
    
    r = distance(N, np.arange(N))
    cc = 1./sum(r[1:]**(-2.*sigma))
    c = sqrt(cc)
    
    return [(i, j, normal(0, c/r[j-i]**sigma)) 
            for i in range(N) for j in range(i+1, N)]


def bonds_dilute(N, z=6, sigma=0., distance=chord_distance):
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
    .. math:: p_{ij} \sim 1/r_{ij}^{2 \sigma}
    at large distance.
    """

    assert z < N
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
            
    return [(i, j, v) for (i, j), v in sorted(bonds.items())]


if __name__=='__main__':
    import argparse
    import sys

    parser = argparse.ArgumentParser(description='generate a random dilute bond configuration')
    parser.add_argument('N', type=int, help='system size')
    parser.add_argument('-z', type=int, help='average coordination number for dilute bonds (0 is alias for undiluted model)')
    parser.add_argument('--sigma', type=float, default=0., help='interactions fall off with distance as r^(-2*sigma)')
    parser.add_argument('--ns', type=int, help='number of samples to generate')
    parser.add_argument('--seed', type=int, help='random seed')
    parser.add_argument('--fmt', type=str, help='format string for output filenames')

    args = parser.parse_args()

    def write_sample(bonds, output):
        for bond in bonds:
            output.write('{} {} {}\n'.format(*bond))

    if args.seed is not None:
        np.random.seed(args.seed)

    if args.z:
        func = bonds_dilute
        fargs = (args.N, args.z, args.sigma)
    else:
        func = bonds
        fargs = (args.N, args.sigma)


    if args.ns is None:
        write_sample(func(*fargs), sys.stdout)
    else:
        fmt = args.fmt if args.fmt else 'samp_{}.txt'

        for i in range(args.ns):
            seed = np.random.random_integers(0, 2**24)
            np.random.seed(seed)
            bonds = func(*fargs)
            s = '{:06x}'.format(seed)
            write_sample(bonds, open(fmt.format(s), 'w'))
            print s

