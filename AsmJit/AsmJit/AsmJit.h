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
//! AsmJit? is complete JIT assembler compiler for X86/X64 platform. It can 
//! compile nearly complete (most useful) X86/X64 instruction set with very 
//! friendly way. AsmJit? was designed to be embeddable to any C++ project
//! and this is the reason for MIT licence.
//!
//! AsmJit? is based on Google's V8 code, but modified to fit different code 
//! generation concepts and of course added support for X64. The unmodified
//! code from V8 is probably only Label and Displacement implementation 
//! (that is very good). Main reasons for developers to choose AsmJit? is very 
//! clean C++ design (but no stl or exceptions are used) that allows to write 
//! code by very similar way that most assembler developers do. 
//!
//! Everything in AsmJit library is in @c AsmJit namespace.
//!
//! Useful links:
//! - http://www.mark.masmcode.com/ (Tips)
//! - http://www.ragestorm.net/distorm/ (BSD licenced disassembler)

//! @defgroup AsmJit_Config AsmJit configuration.
//!
//! Contains macros that can be redefined to fit to any project.

//! @defgroup AsmJit_Main AsmJit code generation.
//!
//! Contains classes that's directly used to generate binary code.

//! @defgroup AsmJit_VM Virtual AsmJit virtual memory.
//!
//! Contains virtual memory management functions internally implemented
//! by OS dependent way.

//! @addtogroup AsmJit_Config
//! @{

//! @def ASMJIT_OS
//! @brief Macro that contains information about operating system.
//!
//! Supported values are @c ASMJIT_WINDOWS (1) for Windows or 
//! @c ASMJIT_POSIX (2) for Posix operating systems (Linux, BSD, MacOS).

//! @def ASMJIT_WINDOWS
//! @brief Used in @c ASMJIT_OS macro.
//!
//! Value is 1.

//! @def ASMJIT_POSIX
//! @brief Used in @c ASMJIT_OS macro.
//!
//! Value is 2.

//! @def ASMJIT_API
//! @brief Attribute that's added to classes that can be exported if AsmJit
//! is compiled as a dll library.

//! @def ASMJIT_VAR
//! @brief Attribute that's added to variables that can be exported if AsmJit
//! is compiled as a dll library.
//!
//! @note Default value is extern, because registers are declared in .cpp file
//! and not in headers.

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

// [Includes]
#include "AsmJitConfig.h"

#include "AsmJitAssembler.h"
#include "AsmJitCompiler.h"
#include "AsmJitCpuInfo.h"
#include "AsmJitUtil.h"
#include "AsmJitVM.h"

// [Guard]
#endif // _ASMJIT_H
