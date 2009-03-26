cmake_minimum_required(VERSION 2.6)

Project(AsmJit C CXX)

Set(ASMJIT_DIR ".")

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
  ${ASMJIT_DIR}/AsmJit/AsmJitWarningsPop.h
  ${ASMJIT_DIR}/AsmJit/AsmJitWarningsPush.h
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

If(${CMAKE_BUILD_TYPE} STREQUAL "debug")
  Add_Definitions(-DDEBUG)
EndIF(${CMAKE_BUILD_TYPE} STREQUAL "debug")

Add_Library(AsmJit SHARED
  ${ASMJIT_BASE_SOURCES}
  ${ASMJIT_BASE_HEADERS}
  ${ASMJIT_COMPILER_SOURCES}
  ${ASMJIT_COMPILER_HEADERS})

Link_Libraries(AsmJit)

Add_Executable(testcompiler        ${ASMJIT_DIR}/test/testcompiler.cpp      )
Add_Executable(testcpu             ${ASMJIT_DIR}/test/testcpu.cpp           )
Add_Executable(testjit             ${ASMJIT_DIR}/test/testjit.cpp           )
Add_Executable(testjumptable       ${ASMJIT_DIR}/test/testjumptable.cpp     )
Add_Executable(testmemorymanager   ${ASMJIT_DIR}/test/testmemorymanager.cpp )
Add_Executable(testopcode          ${ASMJIT_DIR}/test/testopcode.cpp        )
Add_Executable(testrewrite         ${ASMJIT_DIR}/test/testrewrite.cpp       )
Add_Executable(teststate           ${ASMJIT_DIR}/test/teststate.cpp         )
Add_Executable(testvariables       ${ASMJIT_DIR}/test/testvariables.cpp     )
