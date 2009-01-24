cmake_minimum_required(VERSION 2.6)

Project(jittest CXX)

Set(jittest_SOURCES
  jittest.cpp
  ../AsmJit/AsmJitVM.cpp
  ../AsmJit/AsmJitX86.cpp
)

Set(jittest_HEADERS
  ../AsmJit/AsmJit.h
  ../AsmJit/AsmJitConfig.h
  ../AsmJit/AsmJitVM.h
  ../AsmJit/AsmJitX86.h
)

Add_Executable(jittest ${jittest_SOURCES} ${jittest_HEADERS})
