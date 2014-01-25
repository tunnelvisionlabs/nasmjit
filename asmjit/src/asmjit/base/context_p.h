// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_BASE_CONTEXT_H
#define _ASMJIT_BASE_CONTEXT_H

// [Dependencies - AsmJit]
#include "../base/compiler.h"
#include "../base/zone.h"

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {

// ============================================================================
// [asmjit::BaseContext]
// ============================================================================

struct BaseContext {
  ASMJIT_NO_COPY(BaseContext)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BaseContext(BaseCompiler* compiler);
  virtual ~BaseContext();

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset the whole context.
  virtual void reset();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get compiler.
  ASMJIT_INLINE BaseCompiler* getCompiler() const { return _compiler; }

  //! @brief Get function.
  ASMJIT_INLINE FuncNode* getFunc() const { return _func; }
  //! @brief Get stop node.
  ASMJIT_INLINE BaseNode* getStop() const { return _stop; }

  //! @brief Get start of the current scope.
  ASMJIT_INLINE BaseNode* getStart() const { return _start; }
  //! @brief Get end of the current scope.
  ASMJIT_INLINE BaseNode* getEnd() const { return _end; }

  //! @brief Get extra block.
  ASMJIT_INLINE BaseNode* getExtraBlock() const { return _extraBlock; }
  //! @brief Set extra block.
  ASMJIT_INLINE void setExtraBlock(BaseNode* node) { _extraBlock = node; }

  // --------------------------------------------------------------------------
  // [Error]
  // --------------------------------------------------------------------------

  //! @brief Set last error code and propagate it through the error handler.
  ASMJIT_INLINE Error setError(Error error, const char* message = NULL)
  { return getCompiler()->setError(error, message); }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Get current state.
  ASMJIT_INLINE BaseVarState* getState() const { return _state; }

  //! @brief Load current state from @a target state.
  virtual void loadState(BaseVarState* src) = 0;
  //! @brief Save current state, returning new @ref BaseVarState instance.
  virtual BaseVarState* saveState() = 0;

  //! @brief Change the current state to @a target state.
  virtual void switchState(BaseVarState* src) = 0;

  //! @brief Change the current state to the intersection of two states @a a
  //! and @a b.
  virtual void intersectStates(BaseVarState* a, BaseVarState* b) = 0;

  // --------------------------------------------------------------------------
  // [Memory]
  // --------------------------------------------------------------------------

  MemCell* _newMemCell(uint32_t size, uint32_t alignment);
  MemCell* _newVarCell(VarData* vd);

  ASMJIT_INLINE MemCell* getVarCell(VarData* vd) {
    MemCell* cell = vd->getMemCell();
    if (cell != NULL)
      return cell;
    else
      return _newVarCell(vd);
  }

  // --------------------------------------------------------------------------
  // [Bits]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE VarBits* newBits(uint32_t len) {
    return static_cast<VarBits*>(
      _zoneAllocator.calloc(static_cast<size_t>(len) * VarBits::kEntitySize));
  }

  ASMJIT_INLINE VarBits* copyBits(const VarBits* src, uint32_t len) {
    return static_cast<VarBits*>(
      _zoneAllocator.dup(src, static_cast<size_t>(len) * VarBits::kEntitySize));
  }

  // --------------------------------------------------------------------------
  // [Fetch]
  // --------------------------------------------------------------------------

  //! @brief Fetch.
  //!
  //! TODO: Documentation.
  virtual Error fetch() = 0;

  // --------------------------------------------------------------------------
  // [RemoveUnreachableCode]
  // --------------------------------------------------------------------------

  //! @brief Remove unreachable code.
  //!
  //! TODO: Documentation.
  virtual Error removeUnreachableCode();

  // --------------------------------------------------------------------------
  // [Analyze]
  // --------------------------------------------------------------------------

  //! @brief Preform variable liveness analysis.
  //!
  //! Analysis phase iterates over nodes in reverse order and generates a bit
  //! array describing variables that are alive at every node in the function.
  //! When the analysis start all variables are assumed dead, when a read or
  //! read/write operations of a variable is detected, it's assumed alive and
  //! when write-only operation is detected it's assumed dead.
  //!
  //! When a label is found, analysis will go through all nodes that jump to
  //! the label and to a previous node if it's not a direct jump or it's not
  //! part of a binary-data section.
  virtual Error analyze() = 0;

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  //! @brief Translate code by allocating registers and handling state changes.
  //!
  //! TODO: Documentation.
  virtual Error translate() = 0;

  // --------------------------------------------------------------------------
  // [Cleanup]
  // --------------------------------------------------------------------------

  virtual void cleanup();

  // --------------------------------------------------------------------------
  // [Compile]
  // --------------------------------------------------------------------------

  virtual Error compile(FuncNode* func);

  // --------------------------------------------------------------------------
  // [Assemble]
  // --------------------------------------------------------------------------

  virtual Error serialize(BaseAssembler* assembler, BaseNode* start, BaseNode* stop) = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Compiler.
  BaseCompiler* _compiler;
  //! @brief Function.
  FuncNode* _func;

  //! @brief Zone allocator.
  Zone _zoneAllocator;

  //! @brief Start of the current active scope.
  BaseNode* _start;
  //! @brief End of the current active scope.
  BaseNode* _end;

  //! @brief Node that is used to insert extra code after the function body.
  BaseNode* _extraBlock;
  //! @brief Stop node.
  BaseNode* _stop;

  //! @brief Unreachable nodes.
  PodList<BaseNode*> _unreachableList;
  //! @brief Jump nodes.
  PodList<BaseNode*> _jccList;

  //! @brief All variables used by the current function.
  PodVector<VarData*> _contextVd;

  //! @brief Memory cells.
  MemCell* _memCells;

  //! @brief Count of stack memory cells (not assigned to variables).
  uint32_t _numStackCells;
  //! @brief Count of 64-byte cells.
  uint32_t _num64ByteCells;
  //! @brief Count of 32-byte cells.
  uint32_t _num32ByteCells;
  //! @brief Count of 16-byte cells.
  uint32_t _num16ByteCells;
  //! @brief Count of 8-byte cells.
  uint32_t _num8ByteCells;
  //! @brief Count of 4-byte cells.
  uint32_t _num4ByteCells;
  //! @brief Count of 2-byte cells.
  uint32_t _num2ByteCells;
  //! @brief Count of 1-byte cells.
  uint32_t _num1ByteCells;

  //! @brief Count of bytes required for all cells.
  uint32_t _memSize;
  //! @brief Maximum size of one cell (used to adjust function alignment).
  uint32_t _maxCellSize;

  //! @brief Current state (used by register allocator).
  BaseVarState* _state;
};

} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"

// [Guard]
#endif // _ASMJIT_BASE_CONTEXT_H
