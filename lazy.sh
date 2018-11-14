#!/bin/bash
input=$1

./ref_sim_lab "$input" < run.txt > correct_results.txt

echo "FINITO"
exit
