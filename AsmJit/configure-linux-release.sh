#!/bin/sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DASMJIT_BUILD_LIBRARY=1 -GKDevelop3
cd ..
