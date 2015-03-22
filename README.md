Monte Carlo simulation of one-dimensional Ising spin glasses
-------------------------------------------------------------------------------

`isg` is an implementation of replica-exchange Monte Carlo for one-dimensional
Ising spin glasses with long range interactions. It has a modular measurement
system which makes it possible to add new types of measurements without
changing the core simulation code.

Basic usage
-------------------------------------------------------------------------------

Simulation parameters are defined as macro constants to allow for effective
compiler optimization. Use the script `setup.sh` to prepare a build for a given
set of parameters. Binaries with different sets of parameters should be built
in separate directories.

For example, to build a simulation with the following parameters

- 2^10 spins
- diluted bonds with average coordination z=6
- 50 replicas
- 2^6 warmup sweeps
- 2^24 measurement sweeps

we use

```bash
mkdir -p build/z6-N2p10
cd build/z6-N2p10
export LOG_N=10
export Z=6
export NUM_REPLICAS=50
export LOG_WARMUP_UPDATES=6
export LOG_NUM_UPDATES=24
 ../../setup.sh
make
```

The `ifm` binary takes a single argument, which is used to seed the random
number generator. Additionally, the following files should exist in the current
directory:

- `temps.txt`, a list of temperatures. Length must match `NUM_REPLICAS`.
- `samp_*.txt`, bond configuration for each sample.
  This is only required if `GENERATE_SAMPLES=0` (the default value).
  There must be a sample file corresponding to each seed passed to `isg`.
  See "sample generation" below.

Module system
-------------------------------------------------------------------------------
The code to do various measurements on the system is separated from the core
simulation code into modules. The header `modules.h` contains the interface
that extension modules should implement. Modules are prefixed with "mod\_" and
consist of a header file declaring the data structures used by the module and a
source file implementing the functions in `modules.h`. The output from each
module should go into a separate file, named according to the type of
measurement and the seed of the measured sample.

Some included modules are listed below:

- `meas`: basic averages including energy, spin overlap, link overlap
- `hist`: distribution of the spin overlap
- `corr` (`corr_fft`): spin correlation function at logarithmically
  (linearly)-spaced points

Sample generation
-------------------------------------------------------------------------------

There are two methods for sample generation.

1. Provide a list of bonds in a text file. Each line of the file represents a
   coupling ("bond") and should be formatted as 3 comma-separated values where
   the first two are integers representing site indices and the third is the
   coupling strength. The script `bonds.py` generates samples in this format.

2. Set `GENERATE_SAMPLES=1`. The samples are then generated at run-time in the
   simulation code. This is useful when studying very large sizes since the
   bond configurations are not written to disk.

See the help message for `bonds.py` for more usage information.
If using `GENERATE_SAMPLES=1`, the range parameter is controlled by setting the
`SIGMA` parameter before calling `setup.h`.

