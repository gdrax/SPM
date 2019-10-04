#!/usr/bin/env bash

SPHERE=0
MATYAS=0
HIMMEL=0

EPOCHS=$1

#mean computation time of the three functions over 10k trials
for n in {0..100}; do SPHERE=$((${SPHERE}+$(./sequential sphere random 1 $  EPOCHS -s))); done
echo sphere: $((SPHERE/EPOCHS)) usec;
for n in {0..100}; do MATYAS=$((${MATYAS}+$(./sequential matyas random 1 $EPOCHS -s))); done
echo matyas: $((MATYAS/EPOCHS)) usec;
for n in {0..100}; do HIMMEL=$((${HIMMEL}+$(./sequential himmel random 1 $EPOCHS -s))); done
echo himmel: $((HIMMEL/EPOCHS)) usec;
