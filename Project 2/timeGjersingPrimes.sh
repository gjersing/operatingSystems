#!/bin/bash

#Christopher Gjersing
#CS 370 1002 & 1003 - 2021 Sprg
#Project #2: Executable file
#Useage:

gcc -pthread -o gjersingPrimes gjersingPrimes.c
time ./gjersingPrimes -t $1 -l 10000000
time ./gjersingPrimes -t $1 -l 10000000
time ./gjersingPrimes -t $1 -l 10000000

