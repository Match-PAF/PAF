#!/bin/bash
ulimit -s unlimited
#cd SCSLwithPrediction
g++ -std=c++11 -O3 *.cpp *.h -o suibian -lpthread
echo "finish compile"

