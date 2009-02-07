cmake_minimum_required(VERSION 2.6)

Project(asmjittest CXX)

Set(ASMJIT_DIR "..")

Set(ASMJIT_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitVM.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitX86.cpp
)

Set(ASMJIT_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJit.h
  ${ASMJIT_DIR}/AsmJit/AsmJitConfig.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.h
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.h
  ${ASMJIT_DIR}/AsmJit/AsmJitVM.h
  ${ASMJIT_DIR}/AsmJit/AsmJitX86.h
)

Add_Executable(jittest jittest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
Add_Executable(cputest cputest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
Add_Executable(reloctest reloctest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
