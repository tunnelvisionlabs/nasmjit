mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DASMJIT_BUILD_LIBRARY=1 -DASMJIT_BUILD_TEST=1 -G"Borland Makefiles"
cd ..
