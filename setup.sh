#!/bin/bash

srcdir="$(dirname "$0")/src"
cmd=sed

function addp {
    eval $1=\${$1:-$2}
    val=$(eval echo \$$1)
    cmd="$cmd -e s|@$1@|$val|"
}

addp srcdir $srcdir

addp N 128
addp Z 6
addp Z_MAX 24
addp MAX_BONDS 6144

addp NUM_REPLICAS 50
addp REPLICA_EXCHANGE 1

addp DEC_PER_MEAS 3
addp RNG "RNG_MT19937"

# compute derived constants
addp NZ_MAX $((N*Z_MAX))
addp NUM_BONDS $((N*Z/2))
addp UPDATES_PER_MEAS $((2**DEC_PER_MEAS))

for f in $srcdir/*.in; do
    $cmd $f > $(basename "${f%.in}")
done

