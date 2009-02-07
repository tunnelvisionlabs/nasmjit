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

// The AsmJit compiler is also based on V8 JIT compiler:
//
// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// [Guard]
#ifndef _ASMJITVM_H
#define _ASMJITVM_H

// [Dependencies]
#include "AsmJitConfig.h"

namespace AsmJit {

//! @addtogroup AsmJit_VM
//! @{

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
struct ASMJIT_API VM
{
  //! @brief Allocate virtual memory. 
  //!
  //! Pages are readable/writeable, but they are not guaranteed to be 
  //! executable unless 'canExecute' is true. Returns the address of 
  //! allocated memory, or NULL if failed.
  static void* alloc(SysUInt length, SysUInt* allocated, bool canExecute);

  //! @brief Free memory allocated by @c alloc()
  static void free(void* addr, SysUInt length);

  //! @brief Get the Alignment guaranteed by alloc().
  static SysUInt alignment();

  //! @brief Returns size of one page.
  static SysUInt pageSize();
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITVM_H
