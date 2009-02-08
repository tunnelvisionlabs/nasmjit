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
#ifndef _ASMJITCOMPILERX86X64_H
#define _ASMJITCOMPILERX86X64_H

// [Dependencies]
#include "AsmJitConfig.h"

#include "AsmJitAssembler.h"
#include "AsmJitUtil.h"

namespace AsmJit {

//! @addtogroup AsmJit_Compiler
//! @{

//! @brief Compiler.
//!
//! This class is used to store instruction stream and allows to modify
//! it on the fly. It uses different concept than @c AsmJit::Assembler class
//! and in fact @c AsmJit::Assembler is only used as a backend.
//!
//! Using of this class will generate slower code than using 
//! @c AsmJit::Assembler, but in some situations it can be more powerful and 
//! /it can result in more optimized code, because @c AsmJit::AdvX86 contains 
//! also register allocator and you can use variables instead of hardcoding 
//! registers.
struct ASMJIT_API Compiler
{
private:
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITCOMPILERX86X64_H
