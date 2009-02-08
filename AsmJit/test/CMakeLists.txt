cmake_minimum_required(VERSION 2.6)

Project(asmjittest CXX)

Set(ASMJIT_DIR "..")

Set(ASMJIT_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitAssemblerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitCompilerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitVM.cpp
)

Set(ASMJIT_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJit.h
  ${ASMJIT_DIR}/AsmJit/AsmJitConfig.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCompiler.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCompilerX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.h
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.h
  ${ASMJIT_DIR}/AsmJit/AsmJitVM.h
  ${ASMJIT_DIR}/AsmJit/AsmJitAssembler.h
  ${ASMJIT_DIR}/AsmJit/AsmJitAssemblerX86X64.h
)

Add_Executable(compilertest compilertest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
Add_Executable(cputest cputest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
Add_Executable(jittest jittest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
Add_Executable(reloctest reloctest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
Add_Executable(rewritetest rewritetest.cpp ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
