#!/bin/sh

export LD_LIBRARY_PATH='.'

g++ -fPIC -shared TokenBank.cpp -o TokenBank.so

g++  Driver.cpp -ldl -o a.out

