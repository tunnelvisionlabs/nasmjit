// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2006-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
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

#include "AsmJitVM.h"

// helpers
static bool isAligned(size_t base, size_t alignment)
{
  return base % alignment == 0;
}

static size_t roundUp(size_t base, size_t pageSize)
{
  size_t over = base % pageSize;
  return base + (over > 0 ? pageSize - over : 0);
}

// Implementation is from "Hacker's Delight" by Henry S. Warren, Jr.,
// figure 3-3, page 48, where the function is called clp2.
static size_t roundUpToPowerOf2(size_t base)
{
  base -= 1;

  base = base | (base >> 1);
  base = base | (base >> 2);
  base = base | (base >> 4);
  base = base | (base >> 8);
  base = base | (base >> 16);

  // 64 bit
  if (sizeof(size_t) == 8) base = base | (base >> 32);

  return base + 1;
}

// ============================================================================
// [Windows]
// ============================================================================

#if (ASMJIT_OS == ASMJIT_WINDOWS)

#include <windows.h>

namespace AsmJit {

struct VMLocal
{
  VMLocal()
  {
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    alignment = info.dwAllocationGranularity;
    pageSize = roundUpToPowerOf2(info.dwPageSize);
  }

  size_t alignment;
  size_t pageSize;
};

static VMLocal& vm()
{
  static VMLocal vm;
  return vm;
};

void* VM::alloc(size_t length, size_t* allocated, bool canExecute)
{
  // VirtualAlloc rounds allocated size to page size automatically.
  size_t msize = roundUp(length, vm().pageSize);

  // Windows XP SP2 / Vista allows Data Excution Prevention (DEP).
  WORD protect = canExecute ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
  LPVOID mbase = VirtualAlloc(NULL, msize, MEM_COMMIT | MEM_RESERVE, protect);
  if (mbase == NULL) return NULL;

  ASMJIT_ASSERT(isAligned(reinterpret_cast<size_t>(mbase), vm().alignment));

  if (allocated) *allocated = msize;
  return mbase;
}

void VM::free(void* addr, size_t /* length */)
{
  VirtualFree(addr, 0, MEM_RELEASE);
}

size_t VM::alignment()
{
  return vm().alignment;
}

size_t VM::pageSize()
{
  return vm().pageSize;
}

} // AsmJit

#endif // ASMJIT_WINDOWS

// ============================================================================
// [Posix]
// ============================================================================

#if ASMJIT_OS == ASMJIT_POSIX

#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

// MacOS uses MAP_ANON instead of MAP_ANONYMOUS
#ifndef MAP_ANONYMOUS
# define MAP_ANONYMOUS MAP_ANON
#endif

namespace AsmJit {

struct VMLocal
{
  VMLocal()
  {
    alignment = pageSize = getpagesize();
  }

  size_t alignment;
  size_t pageSize;
};

static VMLocal& vm()
{
  static VMLocal vm;
  return vm;
}

void* VM::alloc(size_t length, size_t* allocated, bool canExecute)
{
  size_t msize = roundUp(length, vm().pageSize);
  int protection = PROT_READ | PROT_WRITE | (canExecute ? PROT_EXEC : 0);
  void* mbase = mmap(NULL, msize, protection, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mbase == MAP_FAILED) return NULL;
  if (allocated) *allocated = msize;
  return mbase;
}

void VM::free(void* addr, size_t length)
{
  munmap(addr, length);
}

size_t VM::alignment()
{
  return vm().alignment;
}

size_t VM::pageSize()
{
  return vm().pageSize;
}

} // AsmJit

#endif // ASMJIT_POSIX
