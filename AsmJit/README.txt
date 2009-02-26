AsmJit - Complete x86/x64 JIT Assembler for C++ Language
========================================================

http://code.google.com/p/asmjit/

Introduction
============

AsmJit is complete JIT assembler for X86/X64 platform. It contains complete
x86/x64 intrinsics (included is FPU, MMX, 3dNow, SSE, SSE2, SSE3, SSE4) and 
powerful compiler that helps to write portable functions for 32 bit (x86) and
64 bit (x64) architectures. AsmJit can be used to compile functions in dynamic
way that can be called from C/C++ code.

AsmJit can be compiled as a static or dynamically linked library. If you are 
building dynamically linked library, go to AsmJitConfig.h file and setup 
exporting stuff (see wiki in AsmJit homepage).

Assembler / Compiler
====================

Assembler is basic (and also oldest) class that can be used for asm code 
generation. It directly emits binary stream that represents encoded 
x86/x64 assembler opcodes. Together with operands and labels it can be used
to generate complete code. Registers and stack allocation must be done manually
when using pure Assembler class.

Compiler on the other side is higher level class that allows to develop 
crossplatform assembler code without worring about function calling conventions
and registers allocation. It can be also used to write 32 bit and 64 bit 
portable code.

Directory structure
===================

AsmJit - Directory where are sources needed to compile AsmJit. This directory
is designed to be embeddable to your application as easy as possible. There is
also AsmJitConfig.h header where you can configure platform (if autodetection 
now works for you) and application specific features. Look at platform macros 
to change some backends to your preferences.

test - Directory with cmake project to test AsmJit library. It generates simple
command line applications for testing AsmJit functionality. It's only here as a
demonstration how easy this can be done. These applications are also examples 
how to use AsmJit API. For example look at testjit for simple code generation, 
testcpu for cpuid() and cpuInfo() demonstration, testcompiler for compiler 
example, etc...

Supported compilers
===================

AsmJit is successfully tested by following compilers:
- MSVC (VC7.1 and VC8.0)
- GCC (3.4.X+ including MinGW and 4.3.X+)

If you are using different compiler and you have troubles, please use AsmJit
mailing list or create an Issue (see project home page).

Supported platforms
===================

Fully supported platforms at this time are X86 (32-bit) and X64 (64 bit). Other
platforms needs volunteers to support them. AsmJit is designed to generate 
assembler binary only for host CPU, don't try to generate 64 bit assembler in
32 bit mode and vica versa - this is not designed to work and will not work.

Examples
========

Examples can be found on these places:
- AsmJit home page <http://code.google.com/p/asmjit/>
- AsmJit wiki <http://code.google.com/p/asmjit/w/list>
- AsmJit test directory

Licence
=======

AsmJit can be distributed under MIT licence:
<http://www.opensource.org/licenses/mit-license.php>

Older versions of AsmJit also contained Google's V8 licence and Sun licence
(where original Assembler class from Google's V8 is). But because AsmJit were
completely rewritten and contains complete new codebase, these licences were
removed (code from Google's V8 is not used in new versions of AsmJit).

Contact Author
==============

Petr Kobalicek <kobalicek.petr@gmail.com>
