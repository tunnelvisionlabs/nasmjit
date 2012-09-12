#!/bin/sh
mkdir ../build
cd ../build
cmake .. -G"Xcode" -DASMJIT_BUILD_LIBRARY=1 -DASMJIT_BUILD_TEST=1
cd ../scripts
