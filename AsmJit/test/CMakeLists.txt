cmake_minimum_required(VERSION 2.6)

Project(test C CXX)

Set(ASMJIT_DIR "..")

Set(ASMJIT_BASE_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitAssemblerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitLock.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitLogger.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitLoggerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitLoggerX86X64Dump.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitMemoryManager.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitSerializerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.cpp
  ${ASMJIT_DIR}/AsmJit/AsmJitVirtualMemory.cpp
)

Set(ASMJIT_BASE_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJit.h
  ${ASMJIT_DIR}/AsmJit/AsmJitAssembler.h
  ${ASMJIT_DIR}/AsmJit/AsmJitAssemblerX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitBuild.h
  ${ASMJIT_DIR}/AsmJit/AsmJitConfig.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCpuInfo.h
  ${ASMJIT_DIR}/AsmJit/AsmJitDefs.h
  ${ASMJIT_DIR}/AsmJit/AsmJitDefsX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitLock.h
  ${ASMJIT_DIR}/AsmJit/AsmJitLogger.h
  ${ASMJIT_DIR}/AsmJit/AsmJitLoggerX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitMemoryManager.h
  ${ASMJIT_DIR}/AsmJit/AsmJitSerializer.h
  ${ASMJIT_DIR}/AsmJit/AsmJitSerializerX86X64.h
  ${ASMJIT_DIR}/AsmJit/AsmJitUtil.h
  ${ASMJIT_DIR}/AsmJit/AsmJitVirtualMemory.h
)

Set(ASMJIT_COMPILER_SOURCES
  ${ASMJIT_DIR}/AsmJit/AsmJitCompilerX86X64.cpp
)

Set(ASMJIT_COMPILER_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJitCompiler.h
  ${ASMJIT_DIR}/AsmJit/AsmJitCompilerX86X64.h
)

Include(FindThreads)
If(CMAKE_THREAD_LIBS_INIT)
  Link_Libraries(${CMAKE_THREAD_LIBS_INIT})
EndIf(CMAKE_THREAD_LIBS_INIT)

Include_Directories(${ASMJIT_DIR})

Add_Definitions(-DDEBUG)

Add_Executable(testcpu            testcpu.cpp           ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testjit            testjit.cpp           ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testmemorymanager  testmemorymanager.cpp ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testopcode         testopcode.cpp        ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})
Add_Executable(testrewrite        testrewrite.cpp       ${ASMJIT_BASE_SOURCES} ${ASMJIT_BASE_HEADERS})

Add_Executable(testcompiler       testcompiler.cpp
  ${ASMJIT_BASE_SOURCES}          ${ASMJIT_BASE_HEADERS} 
  ${ASMJIT_COMPILER_SOURCES}      ${ASMJIT_COMPILER_HEADERS})

Add_Executable(testjumptable      testjumptable.cpp
  ${ASMJIT_BASE_SOURCES}          ${ASMJIT_BASE_HEADERS} 
  ${ASMJIT_COMPILER_SOURCES}      ${ASMJIT_COMPILER_HEADERS})

Add_Executable(testvariables      testvariables.cpp
  ${ASMJIT_BASE_SOURCES}          ${ASMJIT_BASE_HEADERS}
  ${ASMJIT_COMPILER_SOURCES}      ${ASMJIT_COMPILER_HEADERS})
