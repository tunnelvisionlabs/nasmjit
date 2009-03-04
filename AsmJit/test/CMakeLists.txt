cmake_minimum_required(VERSION 2.6)

Project(test CXX)

Set(ASMJIT_DIR "..")

Set(ASMJIT_BASE_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitAssemblerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitSerializerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitVM.cpp
)

Set(ASMJIT_BASE_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJit.h
  ${ASMJIT_DIR}/AsmJit/AsmJitAssembler.h
  ${ASMJIT_DIR}/AsmJit/AsmJitAssemblerX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitConfig.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.h
  ${ASMJIT_DIR}/AsmJit/AsmJitDefs.h
  ${ASMJIT_DIR}/AsmJit/AsmJitDefsX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitSerializer.h
  ${ASMJIT_DIR}/AsmJit/AsmJitSerializerX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.h
  ${ASMJIT_DIR}/AsmJit/AsmJitVM.h
)

Set(ASMJIT_COMPILER_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitCompilerX86X64.cpp
)

Set(ASMJIT_COMPILER_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJitCompiler.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCompilerX86X64.h
)

Set(ASMJIT_PRETTYPRINTER_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitPrettyPrinter.cpp
)

Set(ASMJIT_PRETTYPRINTER_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJitPrettyPrinter.h
)

Include_Directories(${ASMJIT_DIR})

Add_Definitions(-DDEBUG)

Add_Executable(testcpu       testcpu.cpp       ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testjit       testjit.cpp       ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testopcode    testopcode.cpp    ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testreloc     testreloc.cpp     ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testrewrite   testrewrite.cpp   ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})

Add_Executable(testcompiler  testcompiler.cpp
  ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS} 
  ${ASMJIT_COMPILER_SOURCES} ${ASMJIT_COMPILER_HEADERS} 
  ${ASMJIT_PRETTYPRINTER_SOURCES} ${ASMJIT_PRETTYPRINTER_HEADERS})

Add_Executable(testvariables testvariables.cpp
  ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS}
  ${ASMJIT_COMPILER_SOURCES} ${ASMJIT_COMPILER_HEADERS} 
  ${ASMJIT_PRETTYPRINTER_SOURCES} ${ASMJIT_PRETTYPRINTER_HEADERS})
