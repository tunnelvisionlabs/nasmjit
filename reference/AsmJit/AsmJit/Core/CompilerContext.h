// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_CORE_COMPILERCONTEXT_H
#define _ASMJIT_CORE_COMPILERCONTEXT_H

// [Dependencies - AsmJit]
#include "../Core/Compiler.h"
#include "../Core/CompilerFunc.h"
#include "../Core/CompilerItem.h"
#include "../Core/ZoneMemory.h"

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::CompilerContext]
// ============================================================================

struct CompilerContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ASMJIT_API CompilerContext(Compiler* compiler) ASMJIT_NOTHROW;
  ASMJIT_API virtual ~CompilerContext() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Accessor]
  // --------------------------------------------------------------------------

  inline Compiler* getCompiler() const ASMJIT_NOTHROW
  { return _compiler; }
  
  inline CompilerFuncDecl* getFunc() const ASMJIT_NOTHROW
  { return _func; }

  inline CompilerItem* getExtraBlock() const ASMJIT_NOTHROW
  { return _extraBlock; }
  
  inline void setExtraBlock(CompilerItem* item) ASMJIT_NOTHROW
  { _extraBlock = item; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief ZoneMemory manager.
  ZoneMemory _zoneMemory;

  //! @brief Compiler.
  Compiler* _compiler;
  //! @brief Function.
  CompilerFuncDecl* _func;

  //! @brief Start of the current active scope.
  CompilerItem* _start;
  //! @brief End of the current active scope.
  CompilerItem* _stop;
  //! @brief Item that is used to insert some code after the function body.
  CompilerItem* _extraBlock;

  //! @brief Current state (used by register allocator).
  CompilerState* _state;
  //! @brief Link to circular double-linked list containing all active variables
  //! of the current state.
  CompilerVar* _active;

  //! @brief Current offset, used in prepare() stage. Each item should increment it.
  uint32_t _currentOffset;
  //! @brief Whether current code is unreachable.
  uint32_t _isUnreachable;

  ASMJIT_NO_COPY(CompilerContext)
};

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"

// [Guard]
#endif // _ASMJIT_CORE_COMPILERCONTEXT_H
