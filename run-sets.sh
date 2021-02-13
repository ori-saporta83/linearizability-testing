#!/bin/bash

echo "running generated tests for "

for t in ./tests/generated/generic-set_3_1_0_True_3/*.c; do echo $t; date; timeout 3600 ../genmc/src/genmc -disable-race-detection $t; done
for t in ./tests/generated/generic-set_3_0_1_True_3/*.c; do echo $t; date; timeout 3600 ../genmc/src/genmc -disable-race-detection $t; done