# Reguire minimum version of CMake
CMake_Minimum_Required(VERSION 2.6)

# AsmJit project - Need to use both C and C++
Project(AsmJit C CXX)

# If ASMJIT_DIR is not specified, assume that we are building it from here
If(NOT ASMJIT_DIR)
  Set(ASMJIT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
EndIf(NOT ASMJIT_DIR)

# AsmJit C++ sources
Set(ASMJIT_SOURCES
  ${ASMJIT_DIR}/AsmJit/AssemblerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/CompilerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/CpuInfo.cpp
  ${ASMJIT_DIR}/AsmJit/Lock.cpp
  ${ASMJIT_DIR}/AsmJit/Logger.cpp
  ${ASMJIT_DIR}/AsmJit/LoggerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/LoggerX86X64Dump.cpp
  ${ASMJIT_DIR}/AsmJit/MemoryManager.cpp
  ${ASMJIT_DIR}/AsmJit/SerializerX86X64.cpp
  ${ASMJIT_DIR}/AsmJit/Util.cpp
  ${ASMJIT_DIR}/AsmJit/VirtualMemory.cpp
)

# AsmJit C++ headers
Set(ASMJIT_HEADERS
  ${ASMJIT_DIR}/AsmJit/AsmJit.h
  ${ASMJIT_DIR}/AsmJit/Assembler.h
  ${ASMJIT_DIR}/AsmJit/AssemblerX86X64.h
  ${ASMJIT_DIR}/AsmJit/Build.h
  ${ASMJIT_DIR}/AsmJit/Compiler.h
  ${ASMJIT_DIR}/AsmJit/CompilerX86X64.h
  ${ASMJIT_DIR}/AsmJit/Config.h
  ${ASMJIT_DIR}/AsmJit/CpuInfo.h
  ${ASMJIT_DIR}/AsmJit/Defs.h
  ${ASMJIT_DIR}/AsmJit/DefsX86X64.h
  ${ASMJIT_DIR}/AsmJit/Lock.h
  ${ASMJIT_DIR}/AsmJit/Logger.h
  ${ASMJIT_DIR}/AsmJit/LoggerX86X64.h
  ${ASMJIT_DIR}/AsmJit/MemoryManager.h
  ${ASMJIT_DIR}/AsmJit/Serializer.h
  ${ASMJIT_DIR}/AsmJit/SerializerX86X64.h
  ${ASMJIT_DIR}/AsmJit/Util.h
  ${ASMJIT_DIR}/AsmJit/VirtualMemory.h
  ${ASMJIT_DIR}/AsmJit/WarningsPop.h
  ${ASMJIT_DIR}/AsmJit/WarningsPush.h
)

# Include AsmJit to be able to use #include <AsmJit/...>
Include_Directories(${ASMJIT_DIR})

# pthread library is needed for non-windows OSes.
If(NOT WIN32)
  Link_Libraries(pthread)
EndIf(NOT WIN32)

# Debugging support is indicated through _DEBUG macro, add it if needed
If(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  Add_Definitions(-D_DEBUG)
EndIf(${CMAKE_BUILD_TYPE} MATCHES "Debug")

# Build AsmJit shared library?
If(ASMJIT_BUILD_LIBRARY)
  Add_Library(AsmJit SHARED ${ASMJIT_SOURCES} ${ASMJIT_HEADERS})
  Install(TARGETS AsmJit DESTINATION lib)

  ForEach(i ${ASMJIT_HEADERS})
    Get_Filename_Component(file ${i} NAME)
    Install(FILES ${i} DESTINATION "include/AsmJit")
  EndForEach(i)
EndIf(ASMJIT_BUILD_LIBRARY)

# Build AsmJit test executables?
If(ASMJIT_BUILD_TEST)
  Set(ASMJIT_TEST_FILES
    testcompiler
    testcpu
    testdummy
    testfunclargeargs
    testjit
    testjumptable
    testmemorymanager
    testopcode
    testrewrite
    teststate
    testvariables
    testvarmemalloc
    testvarspill
  )

  ForEach(file ${ASMJIT_TEST_FILES})
    Add_Executable(${file} ${ASMJIT_DIR}/test/${file}.cpp)
    Target_Link_Libraries(${file} AsmJit)
  EndForEach(file)
EndIf(ASMJIT_BUILD_TEST)
