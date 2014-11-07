#!/bin/bash

srcdir="$(dirname "$0")/src"
cmd=sed

function getp {
    eval $1=\${$1:-$2}
    val=$(eval echo \$$1)
    cmd="$cmd -e s|@$1@|$val|"
}

function setp {
    eval $1=$2
    cmd="$cmd -e s|@$1@|$2|"
}

getp srcdir $srcdir

getp LOG_N 7
getp Z 6
getp Z_MAX 24

getp NUM_REPLICAS 50
getp REPLICA_EXCHANGE 1

getp LOG_UPDATES_PER_MEAS 3
getp RNG "RNG_MT19937"

getp LOG_MIN_BLOCK_SIZE 4

# compute derived constants
setp N $((2**LOG_N))
setp UPDATES_PER_MEAS $((2**LOG_UPDATES_PER_MEAS))
setp MIN_BLOCK_SIZE $((2**LOG_MIN_BLOCK_SIZE))

for f in $srcdir/*.in; do
    $cmd $f > $(basename "${f%.in}")
done

