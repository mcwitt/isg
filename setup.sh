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
getp GENERATE_SAMPLE 0
getp SIGMA "0.6"
getp DILUTE 1
getp Z 6
getp Z_MAX 24

getp NUM_REPLICAS 50
getp REPLICA_EXCHANGE 1

getp LOG_WARMUP_UPDATES 3
getp LOG_NUM_UPDATES 10
getp LOG_UPDATES_PER_MEAS 3
getp FULL_OUTPUT 0
getp RNG "RNG_MT19937"

log_min_meas_per_bin=${log_min_meas_per_bin:-7}

# compute derived constants

setp N $((2**LOG_N))
setp NUM_UPDATES $((2**LOG_NUM_UPDATES))
setp UPDATES_PER_MEAS $((2**LOG_UPDATES_PER_MEAS))

log_num_meas=$((LOG_NUM_UPDATES - LOG_UPDATES_PER_MEAS))
setp LOG_MAX_HIST_SIZE $(((LOG_N + log_num_meas - log_min_meas_per_bin)/2))
setp MAX_HIST_SIZE $((2**LOG_MAX_HIST_SIZE))

for f in $srcdir/*.in; do
    $cmd $f > $(basename "${f%.in}")
done

