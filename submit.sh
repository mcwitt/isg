#!/bin/bash

tmp="$$.tmp"

# don't prompt the user for parameters defined here
root="$(cd $(dirname "$0") && pwd)"
jobscript="$root/job.sh"
commit=$(cd $root; git rev-parse HEAD)
hw="x86_64"
num_cores=1

function askp {
    if eval [ -z "\$$1" ]; then
        read -e -p "$3: " -i $2 $1
    fi
    eval setp $1 \$$1
}

function setp { echo export $1=$2 >> $tmp; }
function cleanup { rm -f $tmp; exit; }

trap cleanup INT

echo "#!/bin/sh" > $tmp

askp root "$(cd $(dirname "$0") && pwd)" "root directory"
askp jobname "job$$" "job name"
askp jobscript "$root/job.sh" "job script"
askp commit $(cd $root; git rev-parse HEAD) "commit"

askp LOG_N 7 "log_2 N"
askp sigma "0.600" "sigma"
askp DILUTE 1 "dilute model"

if [ "$DILUTE" -eq 1 ]; then
    askp Z 6 "average z"
    askp Z_MAX 24 "maximum z"
else setp Z 0
fi

askp tempset "./temps.txt" "temperature set"
askp LOG_WARMUP_UPDATES 3 "number of warmup decades"
askp LOG_NUM_UPDATES 16 "number of decades"
askp log_min_meas_per_bin 10 "log minimum measurements per histogram bin"
askp REPLICA_EXCHANGE 0 "enable replica exchange"
askp FULL_OUTPUT 0 "full output [O(N) space]"
askp KEEP_SAMPLES 0 "keep samples"
askp num_cores 1 "number of cores per job"
askp num_jobs 100 "number of jobs"
askp h_rss "4G" "reserve memory"
askp h_cpu "2:00:00" "reserve CPU time"
askp h_fsize "2G" "reserve storage"
askp hw "x86_64" "CPU arch"
askp scratch_root "/scratch/$USER" "scratch directory"
askp output_dir "$(pwd)" "output directory"

if [ "$num_cores" -gt 1 ]
then setp PE_OPT "-pe \"smp\" $num_cores"
else setp PE_OPT ""
fi

echo 'qsub -N $jobname -V $PE_OPT -t 1-$num_jobs \
     -l h_rss=$h_rss,h_fsize=$h_fsize,h_cpu=$h_cpu,hw=$hw \
     $jobscript'
     >> $tmp

output="$jobname-submit.sh"
mv $tmp $output
chmod +x $output

