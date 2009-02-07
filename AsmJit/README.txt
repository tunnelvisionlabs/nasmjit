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
command line application that generates jit code on the fly and runs it. It's
only as a demonstration how easy this can be done. There is also cputest 
application that demonstrates how to use cpuid() and cpuInfo() functions.

Supported platforms
===================

Full supported platforms at this time are X86 (32-bit) and X64 (64 bit). Other
platforms needs volunteers to support them.

Examples
========

For examples look at http://code.google.com/p/asmjit/

Contact
=======

Petr Kobalicek <kobalicek.petr@gmail.com>