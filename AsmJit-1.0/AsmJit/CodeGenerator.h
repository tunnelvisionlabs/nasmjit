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
#ifndef _ASMJIT_CODEGENERATOR_H
#define _ASMJIT_CODEGENERATOR_H

// [Dependencies]
#include "Build.h"

namespace AsmJit {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct JitCodeGenerator;
struct MemoryManager;

// ============================================================================
// [AsmJit::CodeGenerator]
// ============================================================================

//! @brief Code generator is core class for changing behavior of code generated
//! by @c Assembler or @c Compiler.
struct ASMJIT_API CodeGenerator
{
  // --------------------------------------------------------------------------  // [Construction / Destruction]  // --------------------------------------------------------------------------  //! @brief Create a @c CodeGenerator instance.
  CodeGenerator();
  //! @brief Destroy the @c CodeGenerator instance.
  virtual ~CodeGenerator();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  //! @brief Allocate memory for code of @a codeSize size.
  //!
  //! @param addressPtr Target address where to store the code. This address
  //! is used to store generated assembler into.
  //! @param addressBase Base address used for displacement. AsmJit default
  //! memory manager will always return the same value as @a addressPtr, but
  //! when patching already running process this might be different.
  //! @param codeSize Size of generated code (bytes needed to alloc).
  //!
  //! This method was designed in extensibility in mind. It can be used to
  //! allocate memory used by JIT compiler, remote process patcher or linker.
  //!
  //! @retrurn Error value, see @c ERROR_CODE.
  virtual uint32_t alloc(void** addressPtr, sysuint_t* addressBase, sysuint_t codeSize) = 0;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static CodeGenerator* getGlobal();

private:
  ASMJIT_DISABLE_COPY(CodeGenerator)
};

// ============================================================================
// [AsmJit::JitCodeGenerator]
// ============================================================================

struct JitCodeGenerator : public CodeGenerator
{
  // --------------------------------------------------------------------------  // [Construction / Destruction]  // --------------------------------------------------------------------------  //! @brief Create a @c JitCodeGenerator instance.
  JitCodeGenerator();
  //! @brief Destroy the @c JitCodeGenerator instance.
  virtual ~JitCodeGenerator();

  // --------------------------------------------------------------------------
  // [Memory Manager and Alloc Type]
  // --------------------------------------------------------------------------

  // Note: These members can be ignored by all derived classes. They are here
  // only to privide default implementation. All other implementations (remote
  // code patching or making dynamic loadable libraries/executables) ignore
  // members accessed by these accessors.

  //! @brief Get the @c MemoryManager instance.
  inline MemoryManager* getMemoryManager() const { return _memoryManager; }
  //! @brief Set the @c MemoryManager instance.
  inline void setMemoryManager(MemoryManager* memoryManager) { _memoryManager = memoryManager; }

  //! @brief Get the type of allocation.
  inline uint32_t getAllocType() const { return _allocType; }
  //! @brief Set the type of allocation.
  inline void setAllocType(uint32_t allocType) { _allocType = allocType; }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual uint32_t alloc(void** addressPtr, sysuint_t* addressBase, sysuint_t codeSize);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Memory manager.
  MemoryManager* _memoryManager;
  //! @brief Type of allocation.
  uint32_t _allocType;

private:
  ASMJIT_DISABLE_COPY(JitCodeGenerator)
};

} // AsmJit namespace

// [Guard]
#endif // _ASMJIT_CODEGENERATOR_H
