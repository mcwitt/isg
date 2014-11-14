#!/bin/bash -i
#$ -S /bin/bash
#$ -cwd
#$ -N isg
#$ -j y

set -x  # echo commands

# Copy files to /scratch
scratch="$scratch_root/$$"
rm -rf $scratch
mkdir -p $scratch
num_temps=$(wc -l < $tempset)
cp $tempset "$scratch/temps.txt"

# Get source and compile
cd $scratch
git init
git remote add origin $root
git fetch origin
git checkout $commit
NUM_REPLICAS=$num_temps ./setup.sh
make

# Generate samples
seeds=$(python bonds.py $((2**LOG_N)) -z $Z --sigma $sigma --ns $num_cores)

# Run
# Don't read or write to /home from here
time mpirun ./isg $seeds

# Move output to home directory, clean up
[ "$KEEP_SAMPLES" -eq 1 ] || rm samp_*.txt
bzip2 -9 *.txt
mv *.bz2 $output_dir
mv *.h $output_dir
cd
rm -rf $scratch

