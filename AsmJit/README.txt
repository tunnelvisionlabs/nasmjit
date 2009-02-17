AsmJit - Complete JIT Assembler for C++ Language
================================================

http://code.google.com/p/asmjit/

AsmJit is complete JIT assembly compiler for X86/X64 platform. It can compile 
nearly complete (most useful) instruction set with very friendly way. AsmJit
was designed to be embeddable to any project and this is the reason for MIT
licence that allows to do everything what you want with source code. For 
complete licence look to COPYING.txt file.

Directory structure
===================

AsmJit - Directory where are sources needed to compile AsmJit. This directory
is designed to be embeddable to your application as easy as possible. There is
also AsmJitConfig.h header where you can configure platform (if autodetection 
now works) and application specific features. Look at platform macros to change
some backends to your preferences.

test - Directory with cmake project to test AsmJit library. It generates simple
command line applications for testing AsmJit functionality. It's only here as a
demonstration how easy this can be done. These applications are also examples 
how to use AsmJit API. For example look at testjit for simple function 
generation, testcpu application for cpuid() and cpuInfo() demonstration, etc...

Supported compilers
===================

AsmJit is successfully tested for following compilers:
- MSVC (VC7.1 and VC8.0)
- GCC (3.4.X+ including MinGW and 4.3.X+)

If you are using different compiler and you have troubles, please use AsmJit
mailing list or create an Issue (see project home page).

Supported platforms
===================

Fully supported platforms at this time are X86 (32-bit) and X64 (64 bit). Other
platforms needs volunteers to support them. AsmJit is designed to generate 
assembler binary only for host CPU, don't try to generate 64 bit assembler in
32 bit mode and vica versa - this is not designed to work.

Examples
========

Examples can be found on these places:
- AsmJit home page <http://code.google.com/p/asmjit/>
- AsmJit wiki <http://code.google.com/p/asmjit/w/list>
- AsmJit test directory

Contact Author
==============

Petr Kobalicek <kobalicek.petr@gmail.com>
