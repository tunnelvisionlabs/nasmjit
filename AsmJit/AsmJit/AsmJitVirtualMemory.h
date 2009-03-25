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
#ifndef _ASMJITVIRTUALMEMORY_H
#define _ASMJITVIRTUALMEMORY_H

// [Dependencies]
#include "AsmJitBuild.h"

namespace AsmJit {

//! @addtogroup AsmJit_MemoryManagement
//! @{

// ============================================================================
// [AsmJit::VirtualMemory]
// ============================================================================

//! @brief Class that helps with allocating memory for executing code
//! generated by JIT compiler.
//!
//! There are defined functions that provides facility to allocate and free
//! memory where can be executed code. If processor and operating system
//! supports execution protection then you can't run code from normally
//! malloc()'ed memory.
//!
//! Functions are internally implemented by operating system dependent way.
//! VirtualAlloc() function is used for Windows operating system and mmap()
//! for posix ones. If you want to study or create your own functions, look
//! at VirtualAlloc() or mmap() documentation (depends on you target OS).
//!
//! Under posix operating systems is also useable mprotect() function, that
//! can enable execution protection to malloc()'ed memory block.
struct ASMJIT_API VirtualMemory
{
  //! @brief Allocate virtual memory. 
  //!
  //! Pages are readable/writeable, but they are not guaranteed to be 
  //! executable unless 'canExecute' is true. Returns the address of 
  //! allocated memory, or NULL if failed.
  static void* alloc(SysUInt length, SysUInt* allocated, bool canExecute) ASMJIT_NOTHROW;

  //! @brief Free memory allocated by @c alloc()
  static void free(void* addr, SysUInt length) ASMJIT_NOTHROW;

  //! @brief Get the Alignment guaranteed by alloc().
  static SysUInt alignment() ASMJIT_NOTHROW;

  //! @brief Returns size of one page.
  static SysUInt pageSize() ASMJIT_NOTHROW;
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITVIRTUALMEMORY_H
