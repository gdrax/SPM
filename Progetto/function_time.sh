#!/usr/bin/env bash

SPHERE=0
MATYAS=0
HIMMEL=0

for n in {0..100}; do SPHERE=$((${SPHERE}+$(./sequential sphere random 1000 1))); done
echo sphere $((${SPHERE}/1000));
for n in {0..100}; do MATYAS=$((${MATYAS}+$(./sequential matyas random 1000 1))); done
echo matyas: $((${MATYAS}/1000));
for n in {0..100}; do HIMMEL=$((${HIMMEL}+$(./sequential himmel random 1000 1))); done
echo himmel $((${HIMMEL}/1000));
