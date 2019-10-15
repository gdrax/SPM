#!/usr/bin/env bash

PARTICLES=$1
EPOCHS=$2
THREADS=$3
TIME=0
T=1

#mean computation time of the three functions over 10k trials
echo Computing $PARTICLES particles over $EPOCHS epochs...

for ((i=0; i<THREADS; i++)); do
  for n in {0..1000}; do
    TIME=$(($TIME+$(./thread_pool sphere random $PARTICLES $EPOCHS )))
  done;
  echo 1 thread: $((TIME/100)) usec
  TIME=0
done
#for n in {0..1000}; do TIME=$(($TIME+$(./thread_pool sphere random $PARTICLES $EPOCHS 2))); done
#echo 2 thread: $((TIME/100)) usec;
#for n in {0..1000}; do TIME=$(($TIME+$(./thread_pool sphere random $PARTICLES $EPOCHS 3))); done
#echo 3 thread: $((TIME/100)) usec;
#for n in {0..1000}; do TIME=$(($TIME+$(./thread_pool sphere random $PARTICLES $EPOCHS 4))); done
#echo 4 thread: $((TIME/100)) usec;