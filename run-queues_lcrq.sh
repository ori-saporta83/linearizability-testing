#!/bin/bash

echo "running generated tests for LCRQ"

for t in ./tests/generated/generic_10_0_10_True_4_lcrq/*.c; do echo $t; date; timeout 3600 ../genmc-tool/src/genmc $t; done
for t in ./tests/generated/generic_10_2_10_True_4_lcrq/*.c; do echo $t; date; timeout 3600 ../genmc-tool/src/genmc $t; done