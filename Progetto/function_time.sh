#!/usr/bin/env bash

SPHERE=0
MATYAS=0
HIMMEL=0

#mean computation time of the three functions over 10k trials
for n in {0..100}; do SPHERE=$((${SPHERE}+$(./sequential sphere random 1 10000))); done
echo sphere: $((${SPHERE}/10000)) usec;
for n in {0..100}; do MATYAS=$((${MATYAS}+$(./sequential matyas random 1 10000))); done
echo matyas: $((${MATYAS}/10000)) usec;
for n in {0..100}; do HIMMEL=$((${HIMMEL}+$(./sequential himmel random 1 10000))); done
echo himmel: $((${HIMMEL}/10000)) usec;
