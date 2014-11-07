#!/bin/bash

root="$(dirname "$0")"

# don't prompt the user for parameters defined here
jobscript="$root/job-mpipks.sh.in"
hw="x86_64"
num_cores=1

function askp {
    if eval [ -z "\$$1" ]; then
        read -e -p "$3: " -i $2 $1
    fi
    export $1
}

askp jobname "job$$" "job name"
askp jobscript "$root/job-mpipks.sh.in" "template script"
askp LOG_N 7 "log_2 N"

askp Z 6 "z (0 for undiluted)"
if [ ! "$Z" -eq 0 ]; then
    askp Z_MAX 24 "maximum z"
fi

askp sigma "0.600" "sigma"

askp tempset \
    "$root/data/tempsets/temps_s$(echo $sigma | tr . _).txt" \
    "temperature set"

askp dec_warmup 10 "number of warmup decades"
askp dec_max 16 "number of decades"
askp REPLICA_EXCHANGE 0 "enable replica exchange"
askp num_cores 1 "number of cores per job"
askp num_jobs 100 "number of jobs"
askp h_rss "8G" "reserve memory"
askp h_cpu "12:00:00" "reserve CPU time"
askp h_fsize "1G" "reserve storage"
askp hw "x86_64" "CPU arch"
askp output_dir "$(pwd)" "output directory"

qsub -N $jobname -V -pe "smp" $num_cores -t 1-$num_jobs $jobscript \
     -l h_rss=$h_rss,h_fsize=$h_fsize,h_cpu=$h_cpu,hw=$hw
