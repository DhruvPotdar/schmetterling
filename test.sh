
#!/usr/bin/env bash

./compile.sh
echo "============================================================================================================"
export GTEST_COLOR=1
cd build
ctest --output-on-failure
