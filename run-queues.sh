#!/bin/bash

echo "running generated tests for " $1

for t in ./tests/generated/generic_10_0_10_True_4/*.c; do echo $t; date; timeout 3600 ../genmc/src/genmc -disable-race-detection -- -D$1 $t; done
for t in ./tests/generated/generic_10_2_10_True_4/*.c; do echo $t; date; timeout 3600 ../genmc/src/genmc -disable-race-detection -- -D$1 $t; done
for t in ./tests/generated/generic_10_0_10_True_5/*.c; do echo $t; date; timeout 3600 ../genmc/src/genmc -disable-race-detection -- -D$1 $t; done
for t in ./tests/generated/generic_10_2_10_True_5/*.c; do echo $t; date; timeout 3600 ../genmc/src/genmc -disable-race-detection -- -D$1 $t; done