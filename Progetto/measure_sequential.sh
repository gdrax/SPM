#!/usr/bin/env bash

PARTICLES=$1
EPOCHS=$2
TIME=0
OUTPUT=./sequential_time.txt

echo > $OUTPUT

#mean computation time of PARTICLES and EPOCHS with THREADS over 100 trials
echo Computing $PARTICLES particles over $EPOCHS epochs...

for n in {0..10}; do
  TIME=$(($TIME+$(./sequential sphere random $PARTICLES $EPOCHS )))
done;
echo sequential time: $((TIME/10)) usec >> $OUTPUT
