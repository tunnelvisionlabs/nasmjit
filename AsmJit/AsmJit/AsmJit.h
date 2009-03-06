// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// [Guard]
#ifndef _ASMJIT_H
#define _ASMJIT_H

// [Documentation]

//! @mainpage
//!
//! AsmJit is complete JIT assembler for X86/X64 platform. It contains complete
//! x86/x64 intrinsics (included is FPU, MMX, 3dNow, SSE, SSE2, SSE3, SSE4) and 
//! powerful compiler that helps to write portable functions for 32 bit (x86) and
//! 64 bit (x64) architectures. AsmJit can be used to compile functions at runtime
//! that can be called from C/C++ code.
//! 
//! AsmJit can be compiled as a static or dynamically linked library. If you are 
//! building dynamically linked library, go to AsmJitConfig.h file and setup 
//! exporting macros (see wiki in AsmJit homepage).
//!
//! Everything in AsmJit library is in @c AsmJit namespace.
//!
//! Useful links:
//! - http://www.mark.masmcode.com/ (Tips)
//! - http://www.ragestorm.net/distorm/ (BSD licenced disassembler)

//! @defgroup AsmJit_Config AsmJit configuration.
//!
//! Contains macros that can be redefined to fit to any project.

//! @defgroup AsmJit_Assembler Assembler - low level code generation.
//!
//! Contains classes related to AsmJit::Assembler. It's directly used 
//! to generate binary code stream.

//! @defgroup AsmJit_Compiler Compiler - high level code generation.
//!
//! Contains classes related to AsmJit::Compiler.

//! @defgroup AsmJit_CpuInfo Get informations about host CPU.
//!
//! Contains structures and functions to call cpuid() and get advanced CPU
//! informations.

//! @defgroup AsmJit_Logging Logging - logging and error handling.
//!
//! Contains classes related to loging assembler output.

//! @defgroup AsmJit_Serializer Serializer - code generation abstraction.
//!
//! Serializer implements assembler intrinsics that's used by @c Assembler
//! and @c Compiler classes.

//! @defgroup AsmJit_VM Virtual memory alloc / free.
//!
//! Contains virtual memory management functions internally implemented
//! by OS dependent way.

//! @addtogroup AsmJit_Config
//! @{

//! @def ASMJIT_WINDOWS
//! @brief Macro that is declared if AsmJit is compiled for Windows.

//! @def ASMJIT_POSIX
//! @brief Macro that is declared if AsmJit is compiled for unix like 
//! operating system.

//! @def ASMJIT_API
//! @brief Attribute that's added to classes that can be exported if AsmJit
//! is compiled as a dll library.

//! @def ASMJIT_MALLOC
//! @brief Function to call to allocate dynamic memory.

//! @def ASMJIT_REALLOC
//! @brief Function to call to reallocate dynamic memory.

//! @def ASMJIT_FREE
//! @brief Function to call to free dynamic memory.

//! @def ASMJIT_CRASH
//! @brief Code that is execute if an one or more operands are invalid.

//! @def ASMJIT_ASSERT
//! @brief Assertion macro. Default implementation calls @c ASMJIT_CRASH
//! if assert fails.

//! @}

//! @namespace AsmJit
//! @brief Main AsmJit library namespace.
//!
//! There are not other namespaces used in AsmJit library.

// [Includes]
#include "AsmJitBuild.h"
#include "AsmJitAssembler.h"
#include "AsmJitCompiler.h"
#include "AsmJitCpuInfo.h"
#include "AsmJitDefs.h"
#include "AsmJitPrettyPrinter.h"
#include "AsmJitSerializer.h"
#include "AsmJitUtil.h"
#include "AsmJitVM.h"

// [Guard]
#endif // _ASMJIT_H
