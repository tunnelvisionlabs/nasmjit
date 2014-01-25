# =============================================================================
# [AsmJit - CMakeLists.txt]
# =============================================================================

CMake_Minimum_Required(VERSION 2.8.8)

# =============================================================================
# [AsmJit - Configuration]
# =============================================================================

# Whether to build static library (default FALSE).
# Set(ASMJIT_STATIC FALSE)

# Whether to build samples (default FALSE).
# Set(ASMJIT_BUILD_SAMPLES FALSE)

# =============================================================================
# [AsmJit - Build]
# =============================================================================

If(NOT CMAKE_PROJECT_NAME)
  Project(asmjit C CXX)
  Set(ASMJIT_PROJECT_STR "AsmJit|Project")

  # Build options when making a library.
  If(MSVC)
    Set(ASMJIT_LD_FLAGS "/OPT:REF /OPT:ICF")
    Set(ASMJIT_CXX_FLAGS_DEBUG "/GF /GS /GR-")
    Set(ASMJIT_CXX_FLAGS_RELEASE "/Oi /Oy /GF /GS- /GR-")
    Set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ASMJIT_CXX_FLAGS_DEBUG}")
    Set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ASMJIT_CXX_FLAGS_RELEASE}")
    Set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${ASMJIT_CXX_FLAGS_RELEASE}")
    Set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${ASMJIT_CXX_FLAGS_RELEASE}")
  EndIf()
Else()
  # Do not create a project if this CMakeLists.txt is included by a different
  # project. This allows easy static library build including debugger support.
  Set(ASMJIT_PROJECT_STR "AsmJit|Include")
EndIf()

If(ASMJIT_STATIC)
  Set(ASMJIT_PROJECT_STR "${ASMJIT_PROJECT_STR}|Static")
Else()
  Set(ASMJIT_PROJECT_STR "${ASMJIT_PROJECT_STR}|Shared")
EndIf()

Message("")
Message("== ====================================================")
Message("== [AsmJit ${ASMJIT_PROJECT_STR}]")
Message("== ====================================================")
Message("")

# =============================================================================
# [AsmJit - Directories]
# =============================================================================

If(NOT ASMJIT_DIR)
  Set(ASMJIT_DIR ${CMAKE_CURRENT_LIST_DIR})
  Message("-- Initializing ASMJIT_DIR=${ASMJIT_DIR}")
Else()
  Message("-- Using Custom ASMJIT_DIR=${ASMJIT_DIR}")
EndIf()

Set(ASMJIT_SRC_DIR "${ASMJIT_DIR}/src")
Set(ASMJIT_INCLUDE_DIR "${ASMJIT_SRC_DIR}")

# =============================================================================
# [AsmJit - Dependencies]
# =============================================================================

Set(ASMJIT_DEPS "")

# Pthread.
If(NOT WIN32)
  List(APPEND ASMJIT_DEPS pthread)
EndIf()

# =============================================================================
# [AsmJit - Include]
# =============================================================================

Include_Directories(${ASMJIT_SRC_DIR})

# =============================================================================
# [AsmJit - Definitions]
# =============================================================================

If(${CMAKE_BUILD_TYPE})
  If(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    Add_Definitions(-DASMJIT_DEBUG)
  Else()
    Add_Definitions(-DASMJIT_NO_DEBUG)
  EndIf()
EndIf()

# =============================================================================
# [AsmJit - Macros]
# =============================================================================

Macro(AsmJitAddSrc dst_var base_path)
  Set(dst_src "")
  Set(dst_hdr "")
  Set(dst_path "${ASMJIT_SRC_DIR}/${base_path}")

  ForEach(file ${ARGN})
    Set(file_p "${dst_path}/${file}")

    If(file MATCHES "\\.c|\\.cc|\\.cpp|\\.cxx|\\.m|\\.mm")
      Set(file_cflags "")
      If(optimize)
        Set_Source_Files_Properties(${file_p} PROPERTIES COMPILE_FLAGS ${file_cflags})
      EndIf()
      List(APPEND dst_src ${file_p})
    EndIf()

    If(file MATCHES "\\.h|\\.hh|\\.hpp|\\.hxx|\\.inc")
      List(APPEND dst_hdr ${file_p})
    EndIf()
  EndForEach()

  List(APPEND "${dst_var}" ${dst_src} ${dst_hdr})
  Source_Group(${base_path} FILES ${dst_src} ${dst_hdr})
EndMacro()

# =============================================================================
# [AsmJit - Sources]
# =============================================================================

Set(ASMJIT_SRC "")

AsmJitAddSrc(ASMJIT_SRC asmjit
  asmjit.h
  base.h
  build.h
  config.h
  host.h
  x86.h
)

AsmJitAddSrc(ASMJIT_SRC asmjit/base
  apibegin.h
  apiend.h

  assembler.cpp
  assembler.h
  assert.cpp
  assert.h
  codegen.cpp
  codegen.h
  compiler.cpp
  compiler.h
  context.cpp
  context_p.h
  cpu.cpp
  cpu.h
  defs.cpp
  defs.h
  error.cpp
  error.h
  func.cpp
  func.h
  globals.h
  intutil.h
  lock.h
  logger.cpp
  logger.h
  memorymanager.cpp
  memorymanager.h
  podlist.h
  podvector.cpp
  podvector.h
  runtime.cpp
  runtime.h
  string.cpp
  string.h
  vectypes.h
  vmem.cpp
  vmem.h
  zone.cpp
  zone.h
)

AsmJitAddSrc(ASMJIT_SRC asmjit/x86
  x86assembler.cpp
  x86assembler.h
  x86compiler.cpp
  x86compiler.h
  x86context.cpp
  x86context_p.h
  x86cpu.cpp
  x86cpu.h
  x86defs.cpp
  x86defs.h
  x86func.cpp
  x86func.h
)

# =============================================================================
# [AsmJit - Headers]
# =============================================================================

If(NOT ASMJIT_STATIC)
  ForEach(i ${ASMJIT_SRC})
    Get_Filename_Component(path ${i} PATH)
    Get_Filename_Component(name ${i} NAME)
    String(REGEX REPLACE "^${ASMJIT_SRC_DIR}/" "" targetpath "${path}")
    If(${name} MATCHES ".h$")
      If(NOT "${name}" MATCHES "_p.h$")
        Install(FILES ${i} DESTINATION "include/${targetpath}")
      EndIf()
    EndIf()
  EndForEach()
EndIf()

# =============================================================================
# [Asmjit - Library]
# =============================================================================

If(NOT ASMJIT_STATIC)
  Add_Library(asmjit SHARED ${ASMJIT_SRC})
  If(ASMJIT_LD_FLAGS)
    Set_Target_Properties(asmjit PROPERTIES LINK_FLAGS ${ASMJIT_LD_FLAGS})
  EndIf()
  Target_Link_Libraries(asmjit ${ASMJIT_DEPS})
  Install(TARGETS asmjit DESTINATION lib)
Else()
  Add_Library(asmjit STATIC ${ASMJIT_SRC})
  Target_Link_Libraries(asmjit ${ASMJIT_DEPS})
EndIf()

# =============================================================================
# [Asmjit - Samples]
# =============================================================================

If(ASMJIT_BUILD_SAMPLES)
  Set(ASMJIT_SRC_SAMPLES
    benchx86
    testcpu
    testdummy
    testmem
    testopcode
    testsizeof
    testx86
  )

  ForEach(file ${ASMJIT_SRC_SAMPLES})
    Add_Executable(${file} src/app/test/${file}.cpp)
    Target_Link_Libraries(${file} asmjit ${ASMJIT_DEPS})
  EndForEach(file)
EndIf()
