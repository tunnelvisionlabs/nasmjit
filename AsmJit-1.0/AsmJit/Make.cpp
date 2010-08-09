// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2010, Petr Kobalicek <kobalicek.petr@gmail.com>
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

// [Dependencies]
#include "Defs.h"
#include "Make.h"
#include "MemoryManager.h"

namespace AsmJit {

MakeOptions::MakeOptions() :
  _memoryManager(MemoryManager::getGlobal()),
  _allocType(MEMORY_ALLOC_FREEABLE)
{
}

MakeOptions::~MakeOptions()
{
}

uint32_t MakeOptions::alloc(void** addressPtr, sysuint_t* addressBase, sysuint_t codeSize)
{
  ASMJIT_ASSERT(addressPtr != NULL);
  ASMJIT_ASSERT(addressBase != NULL);
  ASMJIT_ASSERT(codeSize > 0);

  MemoryManager* memmgr = getMemoryManager();
  // Switch to global memory manager if not provided.
  if (memmgr == NULL) memmgr = MemoryManager::getGlobal();

  void* p = memmgr->alloc(codeSize, getAllocType());

  *addressPtr = p;
  *addressBase = (sysuint_t)p;

  return p ? ERROR_NONE : ERROR_NO_VIRTUAL_MEMORY;
}

} // AsmJit namespace
