#!/usr/bin/env bash

PARTICLES=$1
EPOCHS=$2
THREADS=$3
TIME=0
OUTPUT=./phi_times.txt

echo > $OUTPUT

#mean computation time of PARTICLES and EPOCHS with THREADS over 100 trials
echo Computing $PARTICLES particles over $EPOCHS epochs...

for ((i=1; i<THREADS+1; i*=2)); do
  for n in {0..10}; do
    TIME=$(($TIME+$(./muulti_thread sphere random $PARTICLES $EPOCHS $((i)))))
  done;
  echo $((i)) thread: $((TIME/10)) usec >> $OUTPUT
  TIME=0
done