#!/bin/sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=debug -DASMJIT_BUILD_LIBRARY=1 -DASMJIT_BUILD_TEST=1 -GKDevelop3
cd ..
