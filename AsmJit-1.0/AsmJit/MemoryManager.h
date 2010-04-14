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

// [Guard]
#ifndef _ASMJIT_MEMORYMANAGER_H
#define _ASMJIT_MEMORYMANAGER_H

// [Dependencies]
#include "Build.h"
#include "Defs.h"

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

//! @addtogroup AsmJit_MemoryManagement
//! @{

// ============================================================================
// [AsmJit::MemoryManager]
// ============================================================================

//! @brief Virtual memory manager interface.
//!
//! This class is pure virtual. You can get default virtual memory manager using
//! @c getGlobal() method. If you want to create more memory managers with same
//! functionality as global memory manager use @c DefaultMemoryManager class.
struct ASMJIT_API MemoryManager
{
  //! @brief Create memory manager instance.
  MemoryManager() ASMJIT_NOTHROW;
  //! @brief Destroy memory manager instance, this means also to free all memory
  //! blocks.
  virtual ~MemoryManager() ASMJIT_NOTHROW;

  //! @brief Allocate a @a size bytes of virtual memory.
  //!
  //! Note that if you are implementing your own virtual memory manager then you
  //! can quitly ignore type of allocation. This is mainly for AsmJit to memory
  //! manager that allocated memory will be never freed.
  virtual void* alloc(sysuint_t size, uint32_t type = MEMORY_ALLOC_FREEABLE) ASMJIT_NOTHROW = 0;
  //! @brief Free previously allocated memory at a given @a address.
  virtual bool free(void* address) ASMJIT_NOTHROW = 0;

  //! @brief Tell how many bytes are currently used.
  virtual sysuint_t getUsedBytes() ASMJIT_NOTHROW = 0;
  //! @brief Tell how many bytes are currently allocated.
  virtual sysuint_t getAllocatedBytes() ASMJIT_NOTHROW = 0;

  //! @brief Get global memory manager instance.
  //!
  //! Global instance is instance of @c DefaultMemoryManager class.
  static MemoryManager* getGlobal() ASMJIT_NOTHROW;
};

//! @brief Reference implementation of memory manager that uses
//! AsmJit::VirtualMemory class to allocate chunks of virtual memory and bit
//! arrays to manage it.
struct ASMJIT_API DefaultMemoryManager : public MemoryManager
{
  //! @brief Create memory manager instance.
  DefaultMemoryManager() ASMJIT_NOTHROW;
  //! @brief Destroy memory manager instance, this means also to free all blocks.
  virtual ~DefaultMemoryManager() ASMJIT_NOTHROW;

  virtual void* alloc(sysuint_t size, uint32_t type = MEMORY_ALLOC_FREEABLE) ASMJIT_NOTHROW;
  virtual bool free(void* address) ASMJIT_NOTHROW;

  virtual sysuint_t getUsedBytes() ASMJIT_NOTHROW;
  virtual sysuint_t getAllocatedBytes() ASMJIT_NOTHROW;

private:
  void* _d;
};

//! @}

} // AsmJit namespace

// [Api-End]
#include "ApiEnd.h"

// [Guard]
#endif // _ASMJIT_MEMORYMANAGER_H
