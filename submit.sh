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
askp dec_warmup 3 "number of warmup decades"
askp dec_max 16 "number of decades"
askp REPLICA_EXCHANGE 0 "enable replica exchange"
askp BIG_MEASUREMENTS 1 "big measurements"
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

echo 'qsub -N $jobname -V $PE_OPT -t 1-$num_jobs $jobscript \
     -l h_rss=$h_rss,h_fsize=$h_fsize,h_cpu=$h_cpu,hw=$hw' \
     >> $tmp

output="$jobname-submit.sh"
mv $tmp $output
chmod +x $output

