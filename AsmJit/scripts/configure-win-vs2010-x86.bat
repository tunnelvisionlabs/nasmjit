mkdir ..\build
cd ..\build
cmake .. -G"Visual Studio 10" -DASMJIT_BUILD_LIBRARY=1 -DASMJIT_BUILD_TEST=1
cd ..\scripts
pause
