#!/bin/bash

echo "running generated tests for "

for t in ./tests/generated/generic-set_3_1_0_True_2/*.c; do echo $t; date; timeout 3600 ../genmc-tool/src/genmc --disable-race-detection $t; done
for t in ./tests/generated/generic-set_3_0_1_True_2/*.c; do echo $t; date; timeout 3600 ../genmc-tool/src/genmc --disable-race-detection $t; done