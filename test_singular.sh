#!/bin/bash
input=$1

./sim "$input" < run.txt > my_results.txt
./ref_sim_lab "$input" < run.txt > ref_results.txt

diff my_results.txt ref_results.txt
echo "FINITO"
exit
