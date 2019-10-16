#!/usr/bin/env bash

PARTICLES=$1
EPOCHS=$2
THREADS=$3
TIME=0

#mean computation time of PARTICLES and EPOCHS with THREADS over 100 trials
echo Computing $PARTICLES particles over $EPOCHS epochs...

for ((i=1; i<THREADS+1; i++)); do
  for n in {0..100}; do
    TIME=$(($TIME+$(./thread_pool sphere random $PARTICLES $EPOCHS $((i)))))
  done;
  echo $((i)) thread: $((TIME/100)) usec
  TIME=0
done