// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_X86_X86COMPILERFUNC_H
#define _ASMJIT_X86_X86COMPILERFUNC_H

// [Dependencies - AsmJit]
#include "../X86/X86Assembler.h"
#include "../X86/X86Compiler.h"
#include "../X86/X86CompilerItem.h"

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

//! @addtogroup AsmJit_X86
//! @{

// ============================================================================
// [AsmJit::X86CompilerFuncDecl]
// ============================================================================

//! @brief @ref X86Compiler specific function declaration item.
struct X86CompilerFuncDecl : public CompilerFuncDecl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new @ref X86CompilerFuncDecl instance.
  ASMJIT_API X86CompilerFuncDecl(X86Compiler* x86Compiler) ASMJIT_NOTHROW;
  //! @brief Destroy the @ref X86CompilerFuncDecl instance.
  ASMJIT_API virtual ~X86CompilerFuncDecl() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get compiler as @ref X86Compiler.
  inline X86Compiler* getCompiler() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86Compiler*>(_compiler); }

  //! @brief Get function end item as @ref X86CompilerFuncEnd.
  inline X86CompilerFuncEnd* getEnd() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86CompilerFuncEnd*>(_end); }

  //! @brief Get function declaration as @ref X86FuncDecl.
  inline X86FuncDecl* getDecl() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86FuncDecl*>(_decl); }

  //! @brief Get function arguments as variables as @ref X86CompilerVar.
  inline X86CompilerVar** getVars() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86CompilerVar**>(_vars); }

  //! @brief Get function argument at @a index.
  inline X86CompilerVar* getVar(uint32_t index) const ASMJIT_NOTHROW
  {
    ASMJIT_ASSERT(index < _x86Decl.getArgumentsCount());
    return reinterpret_cast<X86CompilerVar**>(_vars)[index];
  }

  //! @brief Get whether it's assumed that stack is aligned to 16 bytes.
  inline bool isAssumed16ByteAlignment() const ASMJIT_NOTHROW
  { return hasFuncFlag(kX86FuncFlagAssume16ByteAlignment); }

  //! @brief Get whether it's required to align stack to 16 bytes by function.
  inline bool isPerformed16ByteAlignment() const ASMJIT_NOTHROW
  { return hasFuncFlag(kX86FuncFlagPerform16ByteAlignment); }

  //! @brief Get whether the ESP is adjusted.
  inline bool isEspAdjusted() const ASMJIT_NOTHROW
  { return hasFuncFlag(kX86FuncFlagIsEspAdjusted); }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual void prepare(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API virtual CompilerItem* translate(CompilerContext& cc) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual int getMaxSize() const ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Prototype]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual void setPrototype(
    uint32_t convention, 
    uint32_t returnType,
    const uint32_t* arguments, 
    uint32_t argumentsCount) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Get required stack offset needed to subtract/add Esp/Rsp in 
  //! prolog/epilog.
  inline int32_t _getRequiredStackOffset() const ASMJIT_NOTHROW
  { return _funcCallStackSize + _memStackSize16 + _peMovStackSize + _peAdjustStackSize; }

  //! @brief Create variables from FunctionPrototype declaration. This is just
  //! parsing what FunctionPrototype generated for current function calling
  //! convention and arguments.
  ASMJIT_API void _createVariables() ASMJIT_NOTHROW;

  //! @brief Prepare variables (ids, names, scope, registers).
  ASMJIT_API void _prepareVariables(CompilerItem* first) ASMJIT_NOTHROW;

  //! @brief Allocate variables (setting correct state, changing masks, etc).
  ASMJIT_API void _allocVariables(CompilerContext& cc) ASMJIT_NOTHROW;

  ASMJIT_API void _preparePrologEpilog(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API void _dumpFunction(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API void _emitProlog(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API void _emitEpilog(CompilerContext& cc) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Function-Call]
  // --------------------------------------------------------------------------

  //! @brief Reserve stack for calling other function and mark function as
  //! callee.
  ASMJIT_API void reserveStackForFunctionCall(int32_t size);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief X86 function decl.
  X86FuncDecl _x86Decl;

  //! @brief Modified and preserved GP registers mask.
  uint32_t _gpModifiedAndPreserved;
  //! @brief Modified and preserved MM registers mask.
  uint32_t _mmModifiedAndPreserved;
  //! @brief Modified and preserved XMM registers mask.
  uint32_t _xmmModifiedAndPreserved;

  //! @brief Id of MovDQWord instruction (@c kX86InstMovDQA or @c kX86InstMovDQU).
  //!
  //! The value is based on stack alignment. If it's guaranteed that stack
  //! is aligned to 16-bytes then @c kX86InstMovDQA instruction is used, otherwise
  //! the @c kX86InstMovDQU instruction is used for 16-byte mov.
  uint32_t _movDqInstCode;

  //! @brief Prolog / epilog stack size for PUSH/POP sequences.
  int32_t _pePushPopStackSize;
  //! @brief Prolog / epilog stack size for MOV sequences.
  int32_t _peMovStackSize;
  //! @brief Prolog / epilog stack adjust size (to make it 16-byte aligned).
  int32_t _peAdjustStackSize;

  //! @brief Memory stack size (for all variables and temporary memory).
  int32_t _memStackSize;
  //! @brief Like @c _memStackSize, but aligned to 16-bytes.
  int32_t _memStackSize16;

  ASMJIT_NO_COPY(X86CompilerFuncDecl)
};

// ============================================================================
// [AsmJit::X86CompilerFuncEnd]
// ============================================================================

//! @brief @ref X86Compiler function end item.
struct X86CompilerFuncEnd : public CompilerFuncEnd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new @ref X86CompilerFuncEnd instance.
  ASMJIT_API X86CompilerFuncEnd(X86Compiler* x86Compiler, X86CompilerFuncDecl* func) ASMJIT_NOTHROW;
  //! @brief Destroy the @ref X86CompilerFuncEnd instance.
  ASMJIT_API virtual ~X86CompilerFuncEnd() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get compiler as @ref X86Compiler.
  inline X86Compiler* getCompiler() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86Compiler*>(_compiler); }

  //! @brief Get related function as @ref X86CompilerFuncDecl.
  inline X86CompilerFuncDecl* getFunc() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86CompilerFuncDecl*>(_func); }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual void prepare(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API virtual CompilerItem* translate(CompilerContext& cc) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  ASMJIT_NO_COPY(X86CompilerFuncEnd)
};

// ============================================================================
// [AsmJit::X86CompilerFuncRet]
// ============================================================================

//! @brief Function return.
struct X86CompilerFuncRet : public CompilerFuncRet
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new @ref X86CompilerFuncRet instance.
  ASMJIT_API X86CompilerFuncRet(X86Compiler* c, X86CompilerFuncDecl* func,
    const Operand* first, const Operand* second) ASMJIT_NOTHROW;
  //! @brief Destroy the @ref X86CompilerFuncRet instance.
  ASMJIT_API virtual ~X86CompilerFuncRet() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get compiler as @ref X86Compiler.
  inline X86Compiler* getCompiler() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86Compiler*>(_compiler); }

  //! @Brief Get related function as @ref X86CompilerFuncDecl.
  inline X86CompilerFuncDecl* getFunc() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86CompilerFuncDecl*>(_func); }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual void prepare(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API virtual CompilerItem* translate(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API virtual void emit(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual int getMaxSize() const ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  ASMJIT_NO_COPY(X86CompilerFuncRet)
};

// ============================================================================
// [AsmJit::X86CompilerFuncCall]
// ============================================================================

//! @brief Compiler function call item.
struct X86CompilerFuncCall : public CompilerFuncCall
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new @ref X86CompilerFuncCall instance.
  ASMJIT_API X86CompilerFuncCall(X86Compiler* x86Compiler, X86CompilerFuncDecl* caller, const Operand* target) ASMJIT_NOTHROW;
  //! @brief Destroy the @ref X86CompilerFuncCall instance.
  ASMJIT_API virtual ~X86CompilerFuncCall() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get compiler as @ref X86Compiler.
  inline X86Compiler* getCompiler() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86Compiler*>(_compiler); }

  //! @brief Get caller as @ref X86CompilerFuncDecl.
  inline X86CompilerFuncDecl* getCaller() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86CompilerFuncDecl*>(_caller); }

  //! @brief Get function prototype.
  inline const X86FuncDecl* getDecl() const ASMJIT_NOTHROW
  { return reinterpret_cast<X86FuncDecl*>(_decl); }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual void prepare(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API virtual CompilerItem* translate(CompilerContext& cc) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual int getMaxSize() const ASMJIT_NOTHROW;
  ASMJIT_API virtual bool _tryUnuseVar(CompilerVar* v) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Prototype]
  // --------------------------------------------------------------------------

  ASMJIT_API virtual void setPrototype(uint32_t convention, uint32_t returnType, const uint32_t* arguments, uint32_t argumentsCount) ASMJIT_NOTHROW;

  //! @brief Set function prototype.
  inline void setPrototype(uint32_t convention, const FuncPrototype& func) ASMJIT_NOTHROW
  { setPrototype(convention, func.getReturnType(), func.getArguments(), func.getArgumentsCount()); }

  //! @brief Set return value.
  ASMJIT_API bool setReturn(const Operand& first, const Operand& second = Operand()) ASMJIT_NOTHROW;

  //! @brief Set function argument @a i to @a var.
  ASMJIT_API bool setArgument(uint32_t i, const Var& var) ASMJIT_NOTHROW;
  //! @brief Set function argument @a i to @a imm.
  ASMJIT_API bool setArgument(uint32_t i, const Imm& imm) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

  ASMJIT_API uint32_t _findTemporaryGpRegister(CompilerContext& cc) ASMJIT_NOTHROW;
  ASMJIT_API uint32_t _findTemporaryXmmRegister(CompilerContext& cc) ASMJIT_NOTHROW;

  ASMJIT_API X86CompilerVar* _getOverlappingVariable(CompilerContext& cc, const FuncArg& argType) const ASMJIT_NOTHROW;

  ASMJIT_API void _moveAllocatedVariableToStack(CompilerContext& cc, X86CompilerVar* vdata, const FuncArg& argType) ASMJIT_NOTHROW;
  ASMJIT_API void _moveSpilledVariableToStack(CompilerContext& cc, X86CompilerVar* vdata, const FuncArg& argType,
    uint32_t temporaryGpReg,
    uint32_t temporaryXmmReg) ASMJIT_NOTHROW;
  ASMJIT_API void _moveSrcVariableToRegister(CompilerContext& cc, X86CompilerVar* vdata, const FuncArg& argType) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief X86 declaration.
  X86FuncDecl _x86Decl;

  //! @brief Mask of GP registers used as function arguments.
  uint32_t _gpParams;
  //! @brief Mask of MM registers used as function arguments.
  uint32_t _mmParams;
  //! @brief Mask of XMM registers used as function arguments.
  uint32_t _xmmParams;

  //! @brief Variables count.
  uint32_t _variablesCount;

  //! @brief Variables (extracted from operands).
  VarCallRecord* _variables;
  //! @brief Argument index to @c VarCallRecord.
  VarCallRecord* _argumentToVarRecord[kFuncArgsMax];

  ASMJIT_NO_COPY(X86CompilerFuncCall)
};

//! @}

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"

// [Guard]
#endif // _ASMJIT_X86_X86COMPILERFUNC_H
