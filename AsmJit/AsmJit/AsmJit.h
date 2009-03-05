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
//! AsmJit is complete JIT assembler compiler for X86/X64 platform. It can 
//! compile nearly complete (most useful) X86/X64 instruction set with very 
//! friendly way. AsmJit was designed to be embeddable to any C++ project
//! and this is the reason for MIT licence.
//!
//! First versions of AsmJit was based on Google's V8 code, but now the code
//! is totally different and V8 generation code or concepts are not used anymore.
//! Main reasons for developers to choose AsmJit? is very clean C++ design (but 
//! no stl or exceptions are used) that allows to write code by very similar 
//! way that most assembler developers do. 
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
//! Contains classes that's directly used to generate binary code stream.

//! @defgroup AsmJit_VM Virtual AsmJit virtual memory.
//!
//! Contains virtual memory management functions internally implemented
//! by OS dependent way.

//! @defgroup AsmJit_CpuInfo Get informations about host CPU.
//!
//! Contains structures and functions to call cpuid() and get advanced CPU
//! informations

//! @addtogroup AsmJit_Config
//! @{

//! @def ASMJIT_WINDOWS
//! @brief Macro that is declared if AsmJit is compiler for Windows.

//! @def ASMJIT_POSIX
//! @brief Macro that is declared if AsmJit is compiler for unix like 
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
