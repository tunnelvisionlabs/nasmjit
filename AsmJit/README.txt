AsmJit - Complete JIT Assembler for C++ Language
================================================

http://code.google.com/p/asmjit/

AsmJit is complete JIT assembly compiler for X86 platform. It can compile 
nearly complete (most useful) instruction set with very friendly way. AsmJit
was designed to be embeddable to any project and this is the reason for MIT
licence that allows to do everything what you want with source code. For 
complete licence look to COPYING.txt file.

Directory structure
===================

AsmJit - Directory where are sources needed to compile AsmJit. This directory
is designed to be embeddable to your application as easy as possible. There is
also AsmJitConfig.h header where you can configure platform and application
specific features. Look at platform macros and change them to correct operating
system (this should be maximum to make it working).

test - Directory with cmake project to test AsmJit library. It generates simple
command line application that generates jit code on the fly and runs it. It's
only as a demonstration how easy this can be done.

Supported platforms
===================

Only supported platform at this time is X86 32-bit. X86 64-bit platform (X64)
will be supported in the future, but other platforms needs volunteers, because
author have no access to different architectures than X86 (32 and 64 bit).

[x] X86 32/64 bit
    [ ] FPU instructions and format is not well defined. Some instructions
        allows to store result in st(i) and others to get st(0) and st(i).
        This should be clear in the future.
    [ ] Add SSE4a instruction set (AMD).
    [ ] Add AMD 3dNow.
    [ ] Add AMD 3dNow extensions.
    [ ] X64 instructions not ported: j, jmp, call

[ ] ARM

Examples
========

For examples look at http://code.google.com/p/asmjit/

Contact
=======

Petr Kobalicek <kobalicek.petr@gmail.com>