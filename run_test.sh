#!/usr/bin/env bash

cd build
export GTEST_COLOR=1
ctest --output-on-failure
