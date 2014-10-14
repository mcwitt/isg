#!/bin/bash

root="$(dirname "$0")"

jobscript="$root/job-mpipks.sh.in"
hw="x86_64"
h_fsize="1G"

function askp {
    ival=$(eval echo \$$1)
    if [ -z $ival ]; then
        echo -n "$3=? [$2] "; read $1
        eval export $1=\${$1:-$2}
    fi
}

askp N 128 "N"
askp Z 6 "z"
askp sigma "0.784" "sigma"
askp dec_max 16 "number of decades"
askp REPLICA_EXCHANGE 0 "enable replica exchange"
askp num_jobs 12 "number of jobs"
askp num_cores 8 "number of cores per job"
askp h_rss "8G" "reserve memory"
askp h_cpu "12:00:00" "reserve CPU time"
askp h_fsize "1G" "reserve storage"
askp hw "x86_64" "CPU arch"
askp output_dir "$(pwd)" "output directory"

qsub -V -l h_rss=$h_rss,h_fsize=$h_fsize,h_cpu=$h_cpu,hw=$hw \
     -pe "smp" $num_cores -t 1-$num_jobs $jobscript

