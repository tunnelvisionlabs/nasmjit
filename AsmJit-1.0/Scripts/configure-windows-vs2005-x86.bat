mkdir ..\Build
cd ..\Build
cmake .. -G"Visual Studio 8 2005" -DASMJIT_BUILD_LIBRARY=1 -DASMJIT_BUILD_TEST=1
cd ..\Scripts
pause
