#!/bin/sh

SHARED_LIB=./TokenUDF
export "LD_LIBRARY_PATH=$SHARED_LIB"

g++ -fPIC -shared "$SHARED_LIB/TokenBank1.cpp" -o "$SHARED_LIB/TokenBank1.so"
g++ -fPIC -shared "$SHARED_LIB/TokenBankM.cpp" -o "$SHARED_LIB/TokenBankM.so"
g++ -fPIC -shared "$SHARED_LIB/ConditionBank1.cpp" -o "$SHARED_LIB/ConditionBank1.so"
g++ -fPIC -shared "$SHARED_LIB/ConditionBankM.cpp" -o "$SHARED_LIB/ConditionBankM.so"

g++  Driver.cpp -ldl -o a.out


