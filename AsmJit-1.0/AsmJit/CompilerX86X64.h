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
#ifndef _ASMJIT_COMPILERX86X64_H
#define _ASMJIT_COMPILERX86X64_H

#if !defined(_ASMJIT_COMPILER_H)
#warning "AsmJit/CompilerX86X64.h can be only included by AsmJit/Compiler.h"
#endif // _ASMJIT_COMPILER_H

// [Dependencies]
#include "Build.h"
#include "Assembler.h"
#include "Defs.h"
#include "Operand.h"
#include "Util.h"

#include <string.h>

// A little bit C++.
#include <new>

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

//! @addtogroup AsmJit_Compiler
//! @{

// ============================================================================
// [AsmJit::TypeToId]
// ============================================================================

// Skip documenting this.
#if !defined(ASMJIT_DOXYGEN)

ASMJIT_DECLARE_TYPE_AS_ID(int8_t, VARIABLE_TYPE_GPD);
ASMJIT_DECLARE_TYPE_AS_ID(uint8_t, VARIABLE_TYPE_GPD);

ASMJIT_DECLARE_TYPE_AS_ID(int16_t, VARIABLE_TYPE_GPD);
ASMJIT_DECLARE_TYPE_AS_ID(uint16_t, VARIABLE_TYPE_GPD);

ASMJIT_DECLARE_TYPE_AS_ID(int32_t, VARIABLE_TYPE_GPD);
ASMJIT_DECLARE_TYPE_AS_ID(uint32_t, VARIABLE_TYPE_GPD);

#if defined(ASMJIT_X64)
ASMJIT_DECLARE_TYPE_AS_ID(int64_t, VARIABLE_TYPE_GPQ);
ASMJIT_DECLARE_TYPE_AS_ID(uint64_t, VARIABLE_TYPE_GPQ);
#endif // ASMJIT_X64

ASMJIT_DECLARE_TYPE_AS_ID(float, VARIABLE_TYPE_FLOAT);
ASMJIT_DECLARE_TYPE_AS_ID(double, VARIABLE_TYPE_DOUBLE);

#endif // !ASMJIT_DOXYGEN

// ============================================================================
// [AsmJit::FunctionPrototype]
// ============================================================================

//! @brief Calling convention and function argument handling.
struct ASMJIT_API FunctionPrototype
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FunctionPrototype() ASMJIT_NOTHROW;
  ~FunctionPrototype() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Argument]
  // --------------------------------------------------------------------------

  //! @brief Function argument location.
  struct Argument
  {
    //! @brief Variable type, see @c VARIABLE_TYPE.
    uint32_t variableType;
    //! @brief Register index if argument is passed through register, otherwise
    //! @c INVALID_VALUE.
    uint32_t registerIndex;
    //! @brief Stack offset if argument is passed through stack, otherwise
    //! @c INVALID_VALUE.
    int32_t stackOffset;

    inline bool isAssigned() const ASMJIT_NOTHROW
    { return registerIndex != INVALID_VALUE || stackOffset != (int32_t)INVALID_VALUE; }
  };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Set function prototype.
  //!
  //! This will set function calling convention and setup arguments variables.
  //!
  //! @note This function will allocate variables, it can be called only once.
  void setPrototype(uint32_t callingConvention, const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW;

  //! @brief Get function calling convention, see @c CALL_CONV.
  inline uint32_t getCallingConvention() const ASMJIT_NOTHROW { return _callingConvention; }

  //! @brief Get whether callee pops stack.
  inline uint32_t getCalleePopsStack() const ASMJIT_NOTHROW { return _calleePopsStack; }

  //! @brief Get function arguments.
  inline Argument* getArguments() ASMJIT_NOTHROW { return _arguments; }
  //! @brief Get function arguments (const version).
  inline const Argument* getArguments() const ASMJIT_NOTHROW { return _arguments; }

  //! @brief Get count of arguments.
  inline uint32_t getArgumentsCount() const ASMJIT_NOTHROW { return _argumentsCount; }

  //! @brief Get direction of arguments passed on the stack.
  //!
  //! Direction should be always @c ARGUMENT_DIR_RIGHT_TO_LEFT.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline uint32_t getArgumentsDirection() const ASMJIT_NOTHROW { return _argumentsDirection; }

  //! @brief Get stack size needed for function arguments passed on the stack.
  inline uint32_t getArgumentsStackSize() const ASMJIT_NOTHROW { return _argumentsStackSize; }

  //! @brief Get registers used to pass first integer parameters by current
  //! calling convention.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline const uint32_t* getArgumentsGP() const ASMJIT_NOTHROW { return _argumentsGP; }

  //! @brief Get registers used to pass first SP-FP or DP-FPparameters by
  //! current calling convention.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline const uint32_t* getArgumentsXMM() const ASMJIT_NOTHROW { return _argumentsXMM; }

  //! @brief Get bitmask of general purpose registers that's preserved
  //! (non-volatile).
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline uint32_t getPreservedGP() const ASMJIT_NOTHROW { return _preservedGP; }

  //! @brief Return bitmask of sse registers that's preserved (non-volatile).
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline uint32_t getPreservedXMM() const ASMJIT_NOTHROW { return _preservedXMM; }

private:

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  void _clear() ASMJIT_NOTHROW;
  void _setCallingConvention(uint32_t callingConvention) ASMJIT_NOTHROW;
  void _setPrototype(const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW;
  void _setReturnValue(uint32_t valueId) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Calling convention.
  uint32_t _callingConvention;
  //! @brief Whether callee pops stack.
  uint32_t _calleePopsStack;

  //! @brief List of arguments, their register codes or stack locations.
  Argument _arguments[32];

  //! @brief Count of arguments (in @c _argumentsList).
  uint32_t _argumentsCount;
  //! @brief Direction for arguments passed on the stack, see @c ARGUMENT_DIR.
  uint32_t _argumentsDirection;
  //! @brief Count of bytes consumed by arguments on the stack.
  uint32_t _argumentsStackSize;

  //! @brief List of registers that's used for first INT arguments (GP registers) instead of stack.
  uint32_t _argumentsGP[16];
  //! @brief List of registers that's used for first FPU arguments (XMM registers - SSE2) instead of stack.
  uint32_t _argumentsXMM[16];

  //! @brief Bitmask for preserved general purpose registers.
  uint32_t _preservedGP;
  //! @brief Bitmask for preserved sse registers.
  uint32_t _preservedXMM;
};

// ============================================================================
// [AsmJit::VarData]
// ============================================================================

//! @brief Variable data.
struct VarData
{
  // --------------------------------------------------------------------------
  // [Scope]
  // --------------------------------------------------------------------------

  //! @brief Scope (NULL if variable is global).
  EFunction* scope;

  Emittable* firstEmittable;
  Emittable* lastEmittable;

  // --------------------------------------------------------------------------
  // [Id / Name]
  // --------------------------------------------------------------------------

  //! @brief Variable name.
  const char* name;
  //! @brief Variable id.
  uint32_t id;
  //! @brief Variable type.
  uint32_t type;
  //! @brief Variable size.
  uint32_t size;

  // --------------------------------------------------------------------------
  // [Home]
  // --------------------------------------------------------------------------

  //! @brief Home register index or @c INVALID_VALUE (used by register allocator).
  uint32_t homeRegisterIndex;
  //! @brief Preferred register index.
  uint32_t prefRegisterIndex;

  //! @brief Home memory address register index (in most cases ESP/RSP).
  uint32_t homeMemoryRegister;
  //! @brief Home memory address offset (valid only if homeMemoryRegister is set).
  int32_t homeMemoryOffset;

  //! @brief Used by CompilerContext, do not touch (NULL when created).
  void* homeMemoryData;

  // --------------------------------------------------------------------------
  // [Actual]
  // --------------------------------------------------------------------------

  //! @brief Actual register index (connected with actual @c StateData).
  uint32_t registerIndex;
  //! @brief Actual working offset. This member is set before register allocator
  //! is called. If workOffset is same as CompilerContext::_currentOffset then
  //! this variable is probably used in next instruction and can't be spilled.
  uint32_t workOffset;

  //! @brief Next active variable in circullar double-linked list.
  VarData* nextActive;
  //! @brief Previous active variable in circullar double-linked list.
  VarData* prevActive;

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @brief Variable priority.
  uint8_t priority;
  //! @brief Whether variable content can be calculated by simple instruction
  //!
  //! This is used mainly by mmx or sse2 code and variable allocator will
  //! never reserve space for this variable. Calculated variables are for
  //! example all zeros, all ones, etc.
  uint8_t calculated;
  //! @brief Whether variable is argument passed through register.
  uint8_t isRegArgument;
  //! @brief Whether variable is argument passed through memory.
  uint8_t isMemArgument;

  //! @brief Variable state (connected with actual @c StateData).
  uint8_t state;
  //! @brief Whether variable was changed (connected with actual @c StateData).
  uint8_t changed;

  // --------------------------------------------------------------------------
  // [Statistics]
  // --------------------------------------------------------------------------

  //! @brief Register read statistics (used by instructions where this variable needs
  //! to be read only).
  uint32_t registerReadCount;
  //! @brief Register write statistics (used by instructions where this variable needs
  //! to be write only).
  uint32_t registerWriteCount;
  //! @brief Register read+write statistics (used by instructions where this variable
  //! needs to be read and write).
  uint32_t registerRWCount;

  //! @brief Register GPB.LO statistics (for code generator).
  uint32_t registerGPBLoCount;
  //! @brief Register GPB.HI statistics (for code generator).
  uint32_t registerGPBHiCount;

  //! @brief Memory read statistics.
  uint32_t memoryReadCount;
  //! @brief Memory write statistics.
  uint32_t memoryWriteCount;
  //! @brief Memory read+write statistics.
  uint32_t memoryRWCount;
};

// ============================================================================
// [AsmJit::VarMemBlock]
// ============================================================================

struct VarMemBlock
{
  int32_t offset;
  uint32_t size;

  VarMemBlock* nextUsed;
  VarMemBlock* nextFree;
};

// ============================================================================
// [AsmJit::VarAllocRecord]
// ============================================================================

//! @brief Variable record (for each instruction that uses variables).
//!
//! Variable record contains pointer to variable data and register allocation
//! flags. These flags are important to determine the best alloc instruction.
struct VarAllocRecord
{
  //! @brief Variable data (the structure owned by @c Compiler).
  VarData* vdata;
  //! @brief Variable alloc flags, see @c VARIABLE_ALLOC.
  uint32_t vflags;
};

// ============================================================================
// [AsmJit::VarHintRecord]
// ============================================================================

struct VarHintRecord
{
  VarData* vdata;
  uint32_t hint;
};

// ============================================================================
// [AsmJit::StateData]
// ============================================================================

//! @brief State data.
struct StateData
{
  union
  {
    //! @brief All variables in one array.
    VarData* regs[16 + 8 + 16];

    struct
    {
      //! @brief Regeral purpose registers.
      VarData* gp[16];
      //! @brief MM registers.
      VarData* mm[8];
      //! @brief XMM registers.
      VarData* xmm[16];
    };
  };

  //! @brief Used GP registers bitmask.
  uint32_t usedGP;
  //! @brief Used MM registers bitmask.
  uint32_t usedMM;
  //! @brief Used XMM registers bitmask.
  uint32_t usedXMM;

  //! @brief Changed GP registers bitmask.
  uint32_t changedGP;
  //! @brief Changed MM registers bitmask.
  uint32_t changedMM;
  //! @brief Changed XMM registers bitmask.
  uint32_t changedXMM;

  inline void clear() ASMJIT_NOTHROW
  {
    memset(this, 0, sizeof(*this));
  }
};

// ============================================================================
// [AsmJit::EVariableHint]
// ============================================================================

struct ASMJIT_API EVariableHint : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EVariableHint(Compiler* c, VarData* vdata, uint32_t hintId, uint32_t hintValue) ASMJIT_NOTHROW;
  virtual ~EVariableHint() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;
  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Hint]
  // --------------------------------------------------------------------------

  inline VarData* getVar() const ASMJIT_NOTHROW { return _vdata; }

  inline uint32_t getHintId() const ASMJIT_NOTHROW { return _hintId; }
  inline uint32_t getHintValue() const ASMJIT_NOTHROW { return _hintValue; }

  inline void setHintId(uint32_t hintId) ASMJIT_NOTHROW { _hintId = hintId; }
  inline void setHintValue(uint32_t hintValue) ASMJIT_NOTHROW { _hintValue = hintValue; }
/*
  TODO: REMOVE
  udelat to takto, pricitat ID behem translate(), ale u spicialnich typu to nedelat.
  Timto zpusobem nebude nutne prochazet promenne, ktere nelze presunout (v zavislosti
  na instrukci, ...). No a toto ID se pak muze pouzit i dale :) Vyhoda = Nebudou
  potreba tyto struktury, staci hintId, hintValue, a zdar.
*/
  VarData* _vdata;
  uint32_t _hintId;
  uint32_t _hintValue;
};

// ============================================================================
// [AsmJit::EInstruction]
// ============================================================================

//! @brief Emittable that represents single instruction and its operands.
struct ASMJIT_API EInstruction : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EInstruction(Compiler* c, uint32_t code, Operand* operandsData, uint32_t operandsCount) ASMJIT_NOTHROW;
  virtual ~EInstruction() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;
  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;

  virtual void emit(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Instruction Code]
  // --------------------------------------------------------------------------

  //! @brief Get instruction code, see @c INST_CODE.
  inline uint32_t getCode() const ASMJIT_NOTHROW { return _code; }

  //! @brief Set instruction code to @a code.
  //!
  //! Please do not modify instruction code if you are not know what you are
  //! doing. Incorrect instruction code or operands can raise assertion() at
  //! runtime.
  inline void setCode(uint32_t code) ASMJIT_NOTHROW { _code = code; }

  // --------------------------------------------------------------------------
  // [Operands]
  // --------------------------------------------------------------------------

  //! @brief Get count of operands in operands array (number between 0 to 2 inclusive).
  inline uint32_t getOperandsCount() const ASMJIT_NOTHROW { return _operandsCount; }

  //! @brief Get operands array (3 operands total).
  inline Operand* getOperands() ASMJIT_NOTHROW { return _operands; }
  //! @brief Get operands array (3 operands total).
  inline const Operand* getOperands() const ASMJIT_NOTHROW { return _operands; }

  inline Mem* getMemOp() ASMJIT_NOTHROW { return _memOp; }
  inline void setMemOp(Mem* op) ASMJIT_NOTHROW { _memOp = op; }

  // --------------------------------------------------------------------------
  // [Variables]
  // --------------------------------------------------------------------------

  //! @brief Get count of variables in instruction operands (and in variables array).
  inline uint32_t getVariablesCount() const ASMJIT_NOTHROW { return _variablesCount; }

  //! @brief Get operands array (3 operands total).
  inline VarAllocRecord* getVariables() ASMJIT_NOTHROW { return _variables; }
  //! @brief Get operands array (3 operands total).
  inline const VarAllocRecord* getVariables() const ASMJIT_NOTHROW { return _variables; }

  // --------------------------------------------------------------------------
  // [Jump]
  // --------------------------------------------------------------------------

  //! @brief Get possible jump target.
  //!
  //! If this instruction is conditional or normal jump then return value is
  //! label location (ETarget instance), otherwise return value is @c NULL.
  virtual ETarget* getJumpTarget() const ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Instruction code, see @c INST_CODE.
  uint32_t _code;

  //! @brief Operands count.
  uint32_t _operandsCount;

  //! @brief Variables count.
  uint32_t _variablesCount;

  //! @brief Operands.
  Operand* _operands;
  //! @brief Memory operand (if instruction contains any).
  Mem* _memOp;

  //! @brief Variables (extracted from operands).
  VarAllocRecord* _variables;

  friend struct EFunction;
  friend struct CompilerContext;
  friend struct CompilerCore;

private:
  ASMJIT_DISABLE_COPY(EInstruction)
};

// ============================================================================
// [AsmJit::EJmpInstruction]
// ============================================================================

struct ASMJIT_API EJmpInstruction : public EInstruction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EJmpInstruction(Compiler* c, uint32_t code, Operand* operandsData, uint32_t operandsCount) ASMJIT_NOTHROW;
  virtual ~EJmpInstruction() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;
  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Jump]
  // --------------------------------------------------------------------------

  virtual ETarget* getJumpTarget() const ASMJIT_NOTHROW;
  inline EJmpInstruction* getJumpNext() const ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  ETarget* _jumpTarget;
  EJmpInstruction *_jumpNext;

  friend struct EFunction;
  friend struct CompilerContext;
  friend struct CompilerCore;

private:
  ASMJIT_DISABLE_COPY(EJmpInstruction)
};

// ============================================================================
// [AsmJit::EFunction]
// ============================================================================

//! @brief Function emittable used to generate C/C++ functions.
//!
//! Functions are base blocks for generating assembler output. Each generated
//! assembler stream needs standard entry and leave sequences thats compatible 
//! to operating system conventions - Application Binary Interface (ABI).
//!
//! Function class can be used to generate entry (prolog) and leave (epilog)
//! sequences that is compatible to a given calling convention and to allocate
//! and manage variables that can be allocated to registers or spilled.
//!
//! @note To create function use @c AsmJit::Compiler::newFunction() method, do
//! not create @c Function instances by different ways.
//!
//! @sa @c State, @c StateRef, @c Variable, @c VariableRef.
struct ASMJIT_API EFunction : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create new @c Function instance.
  //!
  //! @note Always use @c AsmJit::Compiler::newFunction() to create @c Function
  //! instance.
  EFunction(Compiler* c) ASMJIT_NOTHROW;
  //! @brief Destroy @c Function instance.
  virtual ~EFunction() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Function Prototype (Calling Convention + Arguments) / Return Value]
  // --------------------------------------------------------------------------

  inline const FunctionPrototype& getPrototype() const ASMJIT_NOTHROW { return _functionPrototype; }
  inline uint32_t getHint(uint32_t hint) ASMJIT_NOTHROW { return _hints[hint]; }

  void setPrototype(uint32_t callingConvention, const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW;
  void setHint(uint32_t hint, uint32_t value) ASMJIT_NOTHROW;

  //! @brief Create variables from FunctionPrototype declaration. This is just
  //! parsing what FunctionPrototype generated for current function calling
  //! convention and arguments.
  void _createVariables() ASMJIT_NOTHROW;

  //! @brief Prepare variables (ids, names, scope, registers).
  void _prepareVariables(Emittable* first) ASMJIT_NOTHROW;

  //! @brief Allocate variables (setting correct state, changing masks, etc).
  void _allocVariables(CompilerContext& c) ASMJIT_NOTHROW;

  void _preparePrologEpilog(CompilerContext& c) ASMJIT_NOTHROW;
  void _dumpFunction(CompilerContext& c) ASMJIT_NOTHROW;
  void _emitProlog(CompilerContext& c) ASMJIT_NOTHROW;
  void _emitEpilog(CompilerContext& c) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Labels]
  // --------------------------------------------------------------------------

  //! @brief Get function entry label.
  //!
  //! Entry label can be used to call this function from another code that's
  //! being generated.
  inline const Label& getEntryLabel() const ASMJIT_NOTHROW { return _entryLabel; }

  //! @brief Get function exit label.
  //!
  //! Use exit label to jump to function epilog.
  inline const Label& getExitLabel() const ASMJIT_NOTHROW { return _exitLabel; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  //! @brief Function prototype.
  FunctionPrototype _functionPrototype;
  //! @brief Function arguments (variable IDs).
  VarData** _argumentVariables;
  //! @brief Function hints.
  uint32_t _hints[16];

  bool _isStackAlignedTo16Bytes;
  bool _isNaked;
  bool _prologEpilogPushPop;
  bool _emitEMMS;
  bool _emitSFence;
  bool _emitLFence;

  uint32_t _modifiedAndPreservedGP;
  uint32_t _modifiedAndPreservedXMM;
  uint32_t _movDqaInstruction;

  uint32_t _prologEpilogStackSize;
  uint32_t _prologEpilogStackSizeAligned16;
  uint32_t _memStackSize;
  uint32_t _memStackSizeAligned16;
  uint32_t _stackAdjust;

  //! @brief Function entry label.
  Label _entryLabel;
  //! @brief Function exit label.
  Label _exitLabel;

  EProlog* _prolog;
  EEpilog* _epilog;

  friend struct CompilerContext;
  friend struct CompilerCore;
  friend struct EProlog;
  friend struct EEpilog;
};

// ============================================================================
// [AsmJit::EProlog]
// ============================================================================

//! @brief Prolog emittable.
struct ASMJIT_API EProlog : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EProlog(Compiler* c, EFunction* f) ASMJIT_NOTHROW;
  virtual ~EProlog() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;
  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Get function associated with this prolog.
  inline EFunction* getFunction() const ASMJIT_NOTHROW { return _function; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  //! @brief Prolog owner function.
  EFunction* _function;

  friend struct CompilerCore;
  friend struct EFunction;
};

// ============================================================================
// [AsmJit::EEpilog]
// ============================================================================

//! @brief Epilog emittable.
struct ASMJIT_API EEpilog : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EEpilog(Compiler* c, EFunction* f) ASMJIT_NOTHROW;
  virtual ~EEpilog() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;
  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Get function associated with this epilog.
  inline EFunction* getFunction() const ASMJIT_NOTHROW { return _function; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  //! @brief Epilog owner function.
  EFunction* _function;

  friend struct CompilerCore;
  friend struct EFunction;
};

// ============================================================================
// [AsmJit::CompilerContext]
// ============================================================================

struct ASMJIT_API CompilerContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  CompilerContext(Compiler* compiler) ASMJIT_NOTHROW;
  ~CompilerContext() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  void _clear() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Register Allocator]
  // --------------------------------------------------------------------------

  void allocVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW;
  void spillVar(VarData* vdata) ASMJIT_NOTHROW;
  void unuseVar(VarData* vdata, uint32_t toState) ASMJIT_NOTHROW;

  void allocGPVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW;
  void spillGPVar(VarData* vdata) ASMJIT_NOTHROW;

  void allocMMVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW;
  void spillMMVar(VarData* vdata) ASMJIT_NOTHROW;

  void allocXMMVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW;
  void spillXMMVar(VarData* vdata) ASMJIT_NOTHROW;

  void emitLoadVar(VarData* vdata, uint32_t regIndex) ASMJIT_NOTHROW;
  void emitSaveVar(VarData* vdata, uint32_t regIndex) ASMJIT_NOTHROW;

  void emitMoveVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW;
  void emitExchangeVar(VarData* vdata, uint32_t regIndex, uint32_t vflags, VarData* other) ASMJIT_NOTHROW;

  void _postAlloc(VarData* vdata, uint32_t vflags) ASMJIT_NOTHROW;
  void _markMemoryUsed(VarData* vdata) ASMJIT_NOTHROW;

  VarData* _getSpillCandidateGP() ASMJIT_NOTHROW;
  VarData* _getSpillCandidateMM() ASMJIT_NOTHROW;
  VarData* _getSpillCandidateXMM() ASMJIT_NOTHROW;
  VarData* _getSpillCandidateGeneric(VarData** varArray, uint32_t count) ASMJIT_NOTHROW;

  inline bool _isActive(VarData* vdata) ASMJIT_NOTHROW { return vdata->nextActive != NULL; }
  void _addActive(VarData* vdata) ASMJIT_NOTHROW;
  void _freeActive(VarData* vdata) ASMJIT_NOTHROW;
  void _freeAllActive() ASMJIT_NOTHROW;

  void _allocatedVariable(VarData* vdata) ASMJIT_NOTHROW;

  inline void _allocatedGPRegister(uint32_t index) ASMJIT_NOTHROW { _state.usedGP |= (1 << index); _modifiedGPRegisters |= (1 << index); }
  inline void _allocatedMMRegister(uint32_t index) ASMJIT_NOTHROW { _state.usedMM |= (1 << index); _modifiedMMRegisters |= (1 << index); }
  inline void _allocatedXMMRegister(uint32_t index) ASMJIT_NOTHROW { _state.usedXMM |= (1 << index); _modifiedXMMRegisters |= (1 << index); }

  inline void _freedGPRegister(uint32_t index) ASMJIT_NOTHROW { _state.usedGP &= ~(1 << index); }
  inline void _freedMMRegister(uint32_t index) ASMJIT_NOTHROW { _state.usedMM &= ~(1 << index); }
  inline void _freedXMMRegister(uint32_t index) ASMJIT_NOTHROW { _state.usedXMM &= ~(1 << index); }

  inline void _markChangedGPRegister(uint32_t index) ASMJIT_NOTHROW { _modifiedGPRegisters |= (1 << index); }
  inline void _markChangedMMRegister(uint32_t index) ASMJIT_NOTHROW { _modifiedMMRegisters |= (1 << index); }
  inline void _markChangedXMMRegister(uint32_t index) ASMJIT_NOTHROW { _modifiedXMMRegisters |= (1 << index); }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  StateData* _saveState() ASMJIT_NOTHROW;
  void _restoreState(StateData* state) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Memory Allocator]
  // --------------------------------------------------------------------------

  VarMemBlock* _allocMemBlock(uint32_t size) ASMJIT_NOTHROW;
  void _freeMemBlock(VarMemBlock* mem) ASMJIT_NOTHROW;

  void _allocMemoryOperands() ASMJIT_NOTHROW;
  void _patchMemoryOperands() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Zone memory manager.
  Zone _zone;

  //! @brief Compiler instance.
  Compiler* _compiler;
  //! @brief Function emittable.
  EFunction* _function;

  //! @brief Current active scope start emittable.
  Emittable* _start;
  //! @brief Current active scope end emittable.
  Emittable* _stop;

  //! @brief Current state (register allocator).
  StateData _state;
  //! @brief Link to circullar double-linked list containing all active variables
  //! (for current state).
  VarData* _active;

  //! @brief Current offset, used in prepare() stage and each emittable can
  //! increment it.
  uint32_t _currentOffset;

  //! @brief Whether current code is unrecheable.
  uint32_t _unrecheable;

  //! @brief Global modified GP registers mask (per function).
  uint32_t _modifiedGPRegisters;
  //! @brief Global modified MM registers mask (per function).
  uint32_t _modifiedMMRegisters;
  //! @brief Global modified XMM registers mask (per function).
  uint32_t _modifiedXMMRegisters;

  //! @brief Whether EBP/RBP register can be used by register allocator.
  uint32_t _allocableEBP;
  //! @brief Whether ESP/RSP register can be used by register allocator.
  //!
  //! Experimental, can be used only in cases that variables are never spilled
  //! into memory and function memory is not used (allocated).
  uint32_t _allocableESP;

  //! @brief Function arguments base pointer (register).
  uint32_t _argumentsBaseReg;
  //! @brief Function arguments base offset.
  int32_t _argumentsBaseOffset;
  //! @brief Function arguments displacement.
  int32_t _argumentsActualDisp;

  //! @brief Function variables base pointer (register).
  uint32_t _variablesBaseReg;
  //! @brief Function variables base offset.
  int32_t _variablesBaseOffset;
  //! @brief Function variables displacement.
  int32_t _variablesActualDisp;

  //! @brief Used memory blocks (for variables, here is each created mem block
  //! that can be also in _memFree list).
  VarMemBlock* _memUsed;
  //! @brief Free memory blocks (freed, prepared for another allocation).
  VarMemBlock* _memFree;
  //! @brief Count of 4-bytes memory blocks used by the function.
  uint32_t _mem4BlocksCount;
  //! @brief Count of 8-bytes memory blocks used by the function.
  uint32_t _mem8BlocksCount;
  //! @brief Count of 16-bytes memory blocks used by the function.
  uint32_t _mem16BlocksCount;
  //! @brief Count of total bytes of stack memory used by the function.
  uint32_t _memBytesTotal;
};

// ============================================================================
// [AsmJit::CompilerCore]
// ============================================================================

//! @brief Compiler core.
//!
//! @sa @c AsmJit::Compiler.
struct ASMJIT_API CompilerCore
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create new (empty) instance of @c Compiler.
  CompilerCore() ASMJIT_NOTHROW;
  //! @brief Destroy @c Compiler instance.
  virtual ~CompilerCore() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Memory Management]
  // --------------------------------------------------------------------------

  //! @brief Get zone memory manager.
  inline Zone& getZone() { return _zone; }

  // --------------------------------------------------------------------------
  // [Logging]
  // --------------------------------------------------------------------------

  //! @brief Get logger.
  inline Logger* getLogger() const ASMJIT_NOTHROW { return _logger; }

  //! @brief Set logger to @a logger.
  virtual void setLogger(Logger* logger) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Error Handling]
  // --------------------------------------------------------------------------

  //! @brief Get error code.
  inline uint32_t getError() const ASMJIT_NOTHROW { return _error; }

  //! @brief Set error code.
  //!
  //! This method is virtual, because higher classes can use it to catch all
  //! errors.
  virtual void setError(uint32_t error) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  uint32_t getProperty(uint32_t propertyId);
  void setProperty(uint32_t propertyId, uint32_t value);

  // --------------------------------------------------------------------------
  // [Buffer]
  // --------------------------------------------------------------------------

  //! @brief Clear everything, but not deallocate buffers.
  //!
  //! @note This method will destroy your code.
  void clear() ASMJIT_NOTHROW;

  //! @brief Free internal buffer, all emitters and NULL all pointers.
  //!
  //! @note This method will destroy your code.
  void free() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emittables]
  // --------------------------------------------------------------------------

  //! @brief Get first emittable.
  inline Emittable* getFirstEmittable() const ASMJIT_NOTHROW { return _first; }

  //! @brief Get last emittable.
  inline Emittable* getLastEmittable() const ASMJIT_NOTHROW { return _last; }

  //! @brief Get current emittable.
  //!
  //! @note If this method return @c NULL, it means that nothing emitted yet.
  inline Emittable* getCurrentEmittable() const ASMJIT_NOTHROW { return _current; }

  //! @brief Set new current emittable and return previous one.
  Emittable* setCurrentEmittable(Emittable* current) ASMJIT_NOTHROW;

  //! @brief Add emittable after current and set current to @a emittable.
  void addEmittable(Emittable* emittable) ASMJIT_NOTHROW;

  //! @brief Add emittable after @a ref.
  void addEmittableAfter(Emittable* emittable, Emittable* ref) ASMJIT_NOTHROW;

  //! @brief Remove emittable (and if needed set current to previous).
  void removeEmittable(Emittable* emittable) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Comment]
  // --------------------------------------------------------------------------

  //! @brief Emit a single comment line that will be logged.
  //!
  //! Emitting comments are useful to log something. Because assembler can be
  //! generated from AST or other data structures, you may sometimes need to
  //! log data characteristics or statistics.
  //!
  //! @note Emitting comment is not directly sent to logger, but instead it's
  //! stored in @c AsmJit::Compiler and emitted when @c serialize() method is
  //! called. Each comment keeps correct order.
  void comment(const char* fmt, ...) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Function Builder]
  // --------------------------------------------------------------------------

  //! @brief Create a new function (low level version).
  //!
  //! @param cconv Function calling convention (see @c AsmJit::CALL_CONV).
  //! @param args Function arguments (see @c AsmJit::VARIABLE_TYPE).
  //! @param count Arguments count.
  //!
  //! This method is internally called from @c newFunction() method and 
  //! contains arguments thats used internally by @c AsmJit::Compiler.
  //!
  //! @note To get current function use @c currentFunction() method.
  EFunction* newFunction_(uint32_t cconv, const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW;

  //! @brief Get current function.
  //!
  //! This method can be called within @c newFunction() and @c endFunction()
  //! block to get current function you are working with. It's recommended
  //! to store @c AsmJit::Function pointer returned by @c newFunction<> method,
  //! because this allows you in future implement function sections outside of
  //! function itself (yeah, this is possible!).
  inline EFunction* getFunction() const ASMJIT_NOTHROW { return _function; }

  //! @brief End of current function scope and all variables.
  EFunction* endFunction() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Memory Management]
  // --------------------------------------------------------------------------

  inline EInstruction* newInstruction(uint32_t code, Operand* operandsData, uint32_t operandsCount) ASMJIT_NOTHROW
  {
    if (code >= INST_J && code <= INST_JMP)
    {
      void* addr = _zone.zalloc(sizeof(EJmpInstruction));
      return new(addr) EJmpInstruction(reinterpret_cast<Compiler*>(this),
        code, operandsData, operandsCount);
    }
    else
    {
      void* addr = _zone.zalloc(sizeof(EInstruction) + operandsCount * sizeof(Operand));
      return new(addr) EInstruction(reinterpret_cast<Compiler*>(this),
        code, operandsData, operandsCount);
    }
  }

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  //! @brief Emit instruction with no operand.
  void _emitInstruction(uint32_t code) ASMJIT_NOTHROW;
  //! @brief Emit instruction with one operand.
  void _emitInstruction(uint32_t code, const Operand* o0) ASMJIT_NOTHROW;
  //! @brief Emit instruction with two operands.
  void _emitInstruction(uint32_t code, const Operand* o0, const Operand* o1) ASMJIT_NOTHROW;
  //! @brief Emit instruction with three operands.
  void _emitInstruction(uint32_t code, const Operand* o0, const Operand* o1, const Operand* o2) ASMJIT_NOTHROW;
  //! @brief Emit instruction with four operands (Compiler specific).
  void _emitInstruction(uint32_t code, const Operand* o0, const Operand* o1, const Operand* o2, const Operand* o3) ASMJIT_NOTHROW;

  //! @brief Private method for emitting jcc.
  //! @internal This should be probably private.
  void _emitJcc(uint32_t code, const Label* label, uint32_t hint) ASMJIT_NOTHROW;

  void _emitReturn(const Operand* val);

  // --------------------------------------------------------------------------
  // [Embed]
  // --------------------------------------------------------------------------

  void embed(const void* data, sysuint_t len) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Align]
  // --------------------------------------------------------------------------

  void align(uint32_t m) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Label]
  // --------------------------------------------------------------------------

  Label newLabel() ASMJIT_NOTHROW;
  void bind(const Label& label) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Variables]
  // --------------------------------------------------------------------------

  VarData* _newVarData(const char* name, uint32_t type, uint32_t size) ASMJIT_NOTHROW;
  VarData* _getVarData(uint32_t id) const ASMJIT_NOTHROW { return _varData[id & OPERAND_ID_VALUE_MASK]; }

  GPVar newGP(uint32_t variableType = VARIABLE_TYPE_GPN, const char* name = NULL) ASMJIT_NOTHROW;
  GPVar argGP(uint32_t index) ASMJIT_NOTHROW;

  MMVar newMM(uint32_t variableType = VARIABLE_TYPE_MM, const char* name = NULL) ASMJIT_NOTHROW;
  MMVar argMM(uint32_t index) ASMJIT_NOTHROW;

  XMMVar newXMM(uint32_t variableType = VARIABLE_TYPE_XMM, const char* name = NULL) ASMJIT_NOTHROW;
  XMMVar argXMM(uint32_t index) ASMJIT_NOTHROW;

  void _vhint(BaseVar& var, uint32_t hintId, uint32_t hintValue) ASMJIT_NOTHROW;

  void alloc(BaseVar& var) ASMJIT_NOTHROW;
  void alloc(BaseVar& var, uint32_t regIndex) ASMJIT_NOTHROW;
  void alloc(BaseVar& var, const BaseReg& reg) ASMJIT_NOTHROW;
  void spill(BaseVar& var) ASMJIT_NOTHROW;
  void unuse(BaseVar& var) ASMJIT_NOTHROW;

  uint32_t getPriority(BaseVar& var) const ASMJIT_NOTHROW;
  void setPriority(BaseVar& var, uint32_t priority) ASMJIT_NOTHROW;

  void rename(BaseVar& var, const char* name) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  StateData* _newStateData() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Make]
  // --------------------------------------------------------------------------

  //! @brief Make is convenience method to make currently serialized code and
  //! return pointer to generated function.
  //!
  //! What you need is only to cast this pointer to your function type and call
  //! it. Note that if there was an error and calling @c error() method not
  //! returns @c ERROR_NONE (zero) then this function always return @c NULL and
  //! error will value remains the same.
  //!
  //! As a convenience you can pass your own memory manager that will be used
  //! to allocate virtual memory, default NULL value means to use default memory
  //! manager that can be get by @c AsmJit::MemoryManager::getGlobal() method.
  //!
  //! Default allocation type is @c MEMORY_ALLOC_FREEABLE. If you need pernament
  //! allocation you should use @c MEMORY_ALLOC_PERNAMENT instead.
  virtual void* make(
    MemoryManager* memoryManager = NULL,
    uint32_t allocType = MEMORY_ALLOC_FREEABLE) ASMJIT_NOTHROW;

  //! @brief Method that will emit everything to @c Assembler instance @a a.
  virtual void serialize(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  inline ETarget* _getTarget(uint32_t id)
  {
    ASMJIT_ASSERT((id & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_LABEL);
    return _targetData[id & OPERAND_ID_VALUE_MASK];
  }

  // --------------------------------------------------------------------------
  // [Variables]
  // --------------------------------------------------------------------------

protected:

  //! @brief Zone memory management.
  Zone _zone;

  //! @brief Logger.
  Logger* _logger;

  //! @brief Last error code.
  uint32_t _error;

  //! @brief Properties.
  uint32_t _properties;

  //! @brief Whether compiler was finished the job (register allocator, etc...).
  uint32_t _finished;

  //! @brief First emittable.
  Emittable* _first;
  //! @brief Last emittable.
  Emittable* _last;
  //! @brief Current emittable.
  Emittable* _current;

  //! @brief Current function.
  EFunction* _function;

  //! @brief Label data.
  PodVector<ETarget*> _targetData;

  //! @brief Variable data.
  PodVector<VarData*> _varData;

  friend struct BaseVar;
  friend struct CompilerContext;
  friend struct EFunction;
  friend struct EInstruction;
};

// ============================================================================
// [AsmJit::CompilerIntrinsics]
// ============================================================================

//! @brief Implementation of @c Compiler intrinsics.
//!
//! Methods in this class are implemented here, because we wan't to hide them
//! in shared libraries. These methods should be never exported by C++ compiler.
//!
//! @sa @c AsmJit::Compiler.
struct ASMJIT_HIDDEN CompilerIntrinsics : public CompilerCore
{
  // Special X86 instructions:
  // - cbw, cwde, cdqe,
  // - cmpxchg8b, cmpxchg16b,
  // - cpuid,
  // - daa, das,
  // - div, idiv,
  // - mul, imul,
  //
  // Special X87 instructions:
  // - fisttp

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create @c CompilerIntrinsics instance. Always use @c AsmJit::Compiler.
  inline CompilerIntrinsics() ASMJIT_NOTHROW {}

  // --------------------------------------------------------------------------
  // [Function Builder]
  // --------------------------------------------------------------------------

  //! @brief Create a new function.
  //!
  //! @param cconv Calling convention to use (see @c CALL_CONV enum)
  //! @param params Function arguments prototype.
  //!
  //! This method is usually used as a first step when generating functions
  //! by @c Compiler. First parameter @a cconv specifies function calling
  //! convention to use. Second parameter @a params specifies function
  //! arguments. To create function arguments are used templates 
  //! @c BuildFunction0<>, @c BuildFunction1<...>, @c BuildFunction2<...>, 
  //! etc...
  //!
  //! Templates with BuildFunction prefix are used to generate argument IDs
  //! based on real C++ types. See next example how to generate function with
  //! two 32 bit integer arguments.
  //!
  //! @code
  //! // Building function using AsmJit::Compiler example.
  //!
  //! // Compiler instance
  //! Compiler c;
  //!
  //! // Begin of function (also emits function @c Prolog)
  //! Function& f = *c.newFunction(
  //!   // Default calling convention (32 bit cdecl or 64 bit for host OS)
  //!   CALL_CONV_DEFAULT,
  //!   // Using function builder to generate arguments list
  //!   BuildFunction2<int, int>());
  //!
  //! // End of function (also emits function @c Epilog)
  //! c.endFunction();
  //! @endcode
  //!
  //! You can see that building functions is really easy. Previous code snipped
  //! will generate code for function with two 32 bit integer arguments. You 
  //! can access arguments by @c AsmJit::Function::argument() method. Arguments
  //! are indexed from 0 (like everything in C).
  //!
  //! @code
  //! // Accessing function arguments through AsmJit::Function example.
  //!
  //! // Compiler instance
  //! Compiler c;
  //!
  //! // Begin of function (also emits function @c Prolog)
  //! Function& f = *c.newFunction(
  //!   // Default calling convention (32 bit cdecl or 64 bit for host OS)
  //!   CALL_CONV_DEFAULT,
  //!   // Using function builder to generate arguments list
  //!   BuildFunction2<int, int>());
  //!
  //! // Arguments are like other variables, you need to reference them by
  //! // VariableRef types:
  //! Int32Ref a0 = f.argument(0);
  //! Int32Ref a1 = f.argument(1);
  //!
  //! // To allocate them to registers just use .alloc(), .r(), .x() or .c() 
  //! // variable methods:
  //! c.add(a0.r(), a1.r());
  //!
  //! // End of function (also emits function @c Epilog)
  //! c.endFunction();
  //! @endcode
  //!
  //! Arguments are like variables. How to manipulate with variables is
  //! documented in @c AsmJit::Compiler detail and @c AsmJit::VariableRef 
  //! class.
  //!
  //! @note To get current function use @c currentFunction() method or save
  //! pointer to @c AsmJit::Function returned by @c AsmJit::Compiler::newFunction<>
  //! method. Recommended is to save the pointer.
  //!
  //! @sa @c BuildFunction0, @c BuildFunction1, @c BuildFunction2, ...
  template<typename T>
  inline EFunction* newFunction(uint32_t cconv, ASMJIT_TYPE_TO_TYPE(T) params) ASMJIT_NOTHROW
  { return newFunction_(cconv, params.getArgs(), params.getCount()); }

  // --------------------------------------------------------------------------
  // [Embed]
  // --------------------------------------------------------------------------

  inline void db(uint8_t  x) ASMJIT_NOTHROW { embed(&x, 1); }
  inline void dw(uint16_t x) ASMJIT_NOTHROW { embed(&x, 2); }
  inline void dd(uint32_t x) ASMJIT_NOTHROW { embed(&x, 4); }
  inline void dq(uint64_t x) ASMJIT_NOTHROW { embed(&x, 8); }

  inline void dint8(int8_t x) ASMJIT_NOTHROW { embed(&x, sizeof(int8_t)); }
  inline void duint8(uint8_t x) ASMJIT_NOTHROW { embed(&x, sizeof(uint8_t)); }

  inline void dint16(int16_t x) ASMJIT_NOTHROW { embed(&x, sizeof(int16_t)); }
  inline void duint16(uint16_t x) ASMJIT_NOTHROW { embed(&x, sizeof(uint16_t)); }

  inline void dint32(int32_t x) ASMJIT_NOTHROW { embed(&x, sizeof(int32_t)); }
  inline void duint32(uint32_t x) ASMJIT_NOTHROW { embed(&x, sizeof(uint32_t)); }

  inline void dint64(int64_t x) ASMJIT_NOTHROW { embed(&x, sizeof(int64_t)); }
  inline void duint64(uint64_t x) ASMJIT_NOTHROW { embed(&x, sizeof(uint64_t)); }

  inline void dsysint(sysint_t x) ASMJIT_NOTHROW { embed(&x, sizeof(sysint_t)); }
  inline void dsysuint(sysuint_t x) ASMJIT_NOTHROW { embed(&x, sizeof(sysuint_t)); }

  inline void dfloat(float x) ASMJIT_NOTHROW { embed(&x, sizeof(float)); }
  inline void ddouble(double x) ASMJIT_NOTHROW { embed(&x, sizeof(double)); }

  inline void dptr(void* x) ASMJIT_NOTHROW { embed(&x, sizeof(void*)); }

  inline void dmm(const MMData& x) ASMJIT_NOTHROW { embed(&x, sizeof(MMData)); }
  inline void dxmm(const XMMData& x) ASMJIT_NOTHROW { embed(&x, sizeof(XMMData)); }

  inline void data(const void* data, sysuint_t size) ASMJIT_NOTHROW { embed(data, size); }

  template<typename T>
  inline void dstruct(const T& x) ASMJIT_NOTHROW { embed(&x, sizeof(T)); }

  // --------------------------------------------------------------------------
  // [Custom Instructions]
  // --------------------------------------------------------------------------

  // These emitters are used by custom compiler code (register alloc / spill,
  // prolog / epilog generator, ...).

  inline void emit(uint32_t code) ASMJIT_NOTHROW
  {
    _emitInstruction(code);
  }

  inline void emit(uint32_t code, const Operand& o0) ASMJIT_NOTHROW
  {
    _emitInstruction(code, &o0);
  }

  inline void emit(uint32_t code, const Operand& o0, const Operand& o1) ASMJIT_NOTHROW
  {
    _emitInstruction(code, &o0, &o1);
  }

  inline void emit(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2) ASMJIT_NOTHROW
  {
    _emitInstruction(code, &o0, &o1, &o2);
  }

  // --------------------------------------------------------------------------
  // [X86 Instructions]
  // --------------------------------------------------------------------------

  //! @brief Add with Carry.
  inline void adc(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_ADC, &dst, &src);
  }

  //! @brief Add.
  inline void add(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_ADD, &dst, &src);
  }

  //! @brief Logical And.
  inline void and_(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_AND, &dst, &src);
  }

  //! @brief Bit Scan Forward.
  inline void bsf(const GPVar& dst, const GPVar& src)
  {
    ASMJIT_ASSERT(!dst.isGPB());
    _emitInstruction(INST_BSF, &dst, &src);
  }
  //! @brief Bit Scan Forward.
  inline void bsf(const GPVar& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isGPB());
    _emitInstruction(INST_BSF, &dst, &src);
  }

  //! @brief Bit Scan Reverse.
  inline void bsr(const GPVar& dst, const GPVar& src)
  {
    ASMJIT_ASSERT(!dst.isGPB());
    _emitInstruction(INST_BSR, &dst, &src);
  }
  //! @brief Bit Scan Reverse.
  inline void bsr(const GPVar& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isGPB());
    _emitInstruction(INST_BSR, &dst, &src);
  }

  //! @brief Byte swap (32 bit or 64 bit registers only) (i486).
  inline void bswap(const GPVar& dst)
  {
    // ASMJIT_ASSERT(dst.getRegType() == REG_GPD || dst.getRegType() == REG_GPQ);
    _emitInstruction(INST_BSWAP, &dst);
  }

  //! @brief Bit test.
  inline void bt(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_BT, &dst, &src);
  }
  //! @brief Bit test.
  inline void bt(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_BT, &dst, &src);
  }
  //! @brief Bit test.
  inline void bt(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_BT, &dst, &src);
  }
  //! @brief Bit test.
  inline void bt(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_BT, &dst, &src);
  }

  //! @brief Bit test and complement.
  inline void btc(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_BTC, &dst, &src);
  }
  //! @brief Bit test and complement.
  inline void btc(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_BTC, &dst, &src);
  }
  //! @brief Bit test and complement.
  inline void btc(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_BTC, &dst, &src);
  }
  //! @brief Bit test and complement.
  inline void btc(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_BTC, &dst, &src);
  }

  //! @brief Bit test and reset.
  inline void btr(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_BTR, &dst, &src);
  }
  //! @brief Bit test and reset.
  inline void btr(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_BTR, &dst, &src);
  }
  //! @brief Bit test and reset.
  inline void btr(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_BTR, &dst, &src);
  }
  //! @brief Bit test and reset.
  inline void btr(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_BTR, &dst, &src);
  }

  //! @brief Bit test and set.
  inline void bts(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_BTS, &dst, &src);
  }
  //! @brief Bit test and set.
  inline void bts(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_BTS, &dst, &src);
  }
  //! @brief Bit test and set.
  inline void bts(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_BTS, &dst, &src);
  }
  //! @brief Bit test and set.
  inline void bts(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_BTS, &dst, &src);
  }

  //! @brief Call Procedure.
  inline void call(const GPVar& dst)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_TYPE_GPN));
    _emitInstruction(INST_CALL, &dst);
  }
  //! @brief Call Procedure.
  inline void call(const Mem& dst)
  {
    _emitInstruction(INST_CALL, &dst);
  }
  //! @brief Call Procedure.
  inline void call(const Imm& dst)
  {
    _emitInstruction(INST_CALL, &dst);
  }
  //! @brief Jump.
  //! @overload
  inline void call(void* dst)
  {
    Imm imm((sysint_t)dst);
    _emitInstruction(INST_CALL, &imm);
  }

  //! @brief Call Procedure.
  inline void call(const Label& label)
  {
    _emitInstruction(INST_CALL, &label);
  }

  //! @brief Convert Byte to Word (Sign Extend).
  inline void cbw(const GPVar& dst)
  {
    _emitInstruction(INST_CBW);
  }

  //! @brief Convert Word to DWord (Sign Extend).
  inline void cwde(const GPVar& dst)
  {
    _emitInstruction(INST_CWDE);
  }

#if defined(ASMJIT_X64)
  //! @brief Convert DWord to QWord (Sign Extend).
  inline void cdqe(const GPVar& dst)
  {
    _emitInstruction(INST_CDQE);
  }
#endif // ASMJIT_X64

  //! @brief Clear Carry flag
  //!
  //! This instruction clears the CF flag in the EFLAGS register.
  inline void clc()
  {
    _emitInstruction(INST_CLC);
  }

  //! @brief Clear Direction flag
  //!
  //! This instruction clears the DF flag in the EFLAGS register.
  inline void cld()
  {
    _emitInstruction(INST_CLD);
  }

  //! @brief Complement Carry Flag.
  //!
  //! This instruction complements the CF flag in the EFLAGS register.
  //! (CF = NOT CF)
  inline void cmc()
  {
    _emitInstruction(INST_CMC);
  }

  //! @brief Conditional Move.
  inline void cmov(CONDITION cc, const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(ConditionToInstruction::toCMovCC(cc), &dst, &src);
  }

  //! @brief Conditional Move.
  inline void cmov(CONDITION cc, const GPVar& dst, const Mem& src)
  {
    _emitInstruction(ConditionToInstruction::toCMovCC(cc), &dst, &src);
  }

  //! @brief Conditional Move.
  inline void cmova  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVA  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmova  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVA  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovae (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVAE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovae (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVAE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovb  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVB  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovb  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVB  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovbe (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVBE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovbe (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVBE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovc  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVC  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovc  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVC  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmove  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVE  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmove  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVE  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovg  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVG  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovg  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVG  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovge (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVGE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovge (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVGE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovl  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVL  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovl  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVL  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovle (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVLE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovle (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVLE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovna (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNA , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovna (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNA , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnae(const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNAE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnae(const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNAE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnb (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNB , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnb (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNB , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnbe(const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNBE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnbe(const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNBE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnc (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNC , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnc (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNC , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovne (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovne (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovng (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNG , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovng (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNG , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnge(const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNGE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnge(const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNGE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnl (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNL , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnl (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNL , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnle(const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNLE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnle(const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNLE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovno (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNO , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovno (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNO , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnp (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNP , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnp (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNP , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovns (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNS , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovns (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNS , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnz (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVNZ , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnz (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVNZ , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovo  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVO  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovo  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVO  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovp  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVP  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovp  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVP  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovpe (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVPE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovpe (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVPE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovpo (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVPO , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovpo (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVPO , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovs  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVS  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovs  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVS  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovz  (const GPVar& dst, const GPVar& src) { _emitInstruction(INST_CMOVZ  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovz  (const GPVar& dst, const Mem& src)   { _emitInstruction(INST_CMOVZ  , &dst, &src); }

  //! @brief Compare Two Operands.
  inline void cmp(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_CMP, &dst, &src);
  }

  //! @brief Compare and Exchange (i486).
  inline void cmpxchg(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_CMPXCHG, &dst, &src);
  }
  //! @brief Compare and Exchange (i486).
  inline void cmpxchg(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_CMPXCHG, &dst, &src);
  }

#if 0
  // TODO: NOT IMPLEMENTED BY THE COMPILER.
  //! @brief Compares the 64-bit value in EDX:EAX with the memory operand (Pentium).
  //!
  //! If the values are equal, then this instruction stores the 64-bit value
  //! in ECX:EBX into the memory operand and sets the zero flag. Otherwise,
  //! this instruction copies the 64-bit memory operand into the EDX:EAX
  //! registers and clears the zero flag.
  inline void cmpxchg8b(const Mem& dst)
  {
    _emitInstruction(INST_CMPXCHG8B, &dst);
  }
#endif

#if 0
  // TODO: NOT IMPLEMENTED BY THE COMPILER.
#if defined(ASMJIT_X64)
  //! @brief Compares the 128-bit value in RDX:RAX with the memory operand (X64).
  //!
  //! If the values are equal, then this instruction stores the 128-bit value
  //! in RCX:RBX into the memory operand and sets the zero flag. Otherwise,
  //! this instruction copies the 128-bit memory operand into the RDX:RAX
  //! registers and clears the zero flag.
  inline void cmpxchg16b(const Mem& dst)
  {
    _emitInstruction(INST_CMPXCHG16B, &dst);
  }
#endif // ASMJIT_X64
#endif

  //! @brief CPU Identification (i486).
  inline void cpuid()
  {
    _emitInstruction(INST_CPUID);
  }

#if 0 && defined(ASMJIT_X86)
  // TODO: NOT IMPLEMENTED BY THE COMPILER.
  inline void daa();
#endif // ASMJIT_X86

#if 0 && defined(ASMJIT_X86)
  // TODO: NOT IMPLEMENTED BY THE COMPILER.
  inline void das();
#endif // ASMJIT_X86

  //! @brief Decrement by 1.
  //! @note This instruction can be slower than sub(dst, 1)
  inline void dec(const GPVar& dst)
  {
    _emitInstruction(INST_DEC, &dst);
  }
  //! @brief Decrement by 1.
  //! @note This instruction can be slower than sub(dst, 1)
  inline void dec(const Mem& dst)
  {
    _emitInstruction(INST_DEC, &dst);
  }

  //! @brief Unsigned divide.
  //!
  //! This instruction divides (unsigned) the value in the AL, AX, or EAX
  //! register by the source operand and stores the result in the AX,
  //! DX:AX, or EDX:EAX registers.
  inline void div(const GPVar& dst_lo, const GPVar& dst_hi, const GPVar& src)
  {
    _emitInstruction(INST_DIV, &dst_lo, &dst_hi, &src);
  }
  //! @brief Unsigned divide.
  //! @overload
  inline void div(const GPVar& dst_lo, const GPVar& dst_hi, const Mem& src)
  {
    _emitInstruction(INST_DIV, &dst_lo, &dst_hi, &src);
  }

  //! @brief Make Stack Frame for Procedure Parameters.
  inline void enter(const Imm& imm16, const Imm& imm8)
  {
    _emitInstruction(INST_ENTER, &imm16, &imm8);
  }

  //! @brief Signed divide.
  //!
  //! This instruction divides (signed) the value in the AL, AX, or EAX
  //! register by the source operand and stores the result in the AX,
  //! DX:AX, or EDX:EAX registers.
  inline void idiv(const GPVar& dst_lo, const GPVar& dst_hi, const GPVar& src)
  {
    _emitInstruction(INST_IDIV, &dst_lo, &dst_hi, &src);
  }
  //! @brief Signed divide.
  //! @overload
  inline void idiv(const GPVar& dst_lo, const GPVar& dst_hi, const Mem& src)
  {
    _emitInstruction(INST_IDIV, &dst_lo, &dst_hi, &src);
  }

  //! @brief Signed multiply.
  //!
  //! Source operand (in a general-purpose register or memory location)
  //! is multiplied by the value in the AL, AX, or EAX register (depending
  //! on the operand size) and the product is stored in the AX, DX:AX, or
  //! EDX:EAX registers, respectively.
  inline void imul(const GPVar& dst_lo, const GPVar& dst_hi, const GPVar& src)
  {
    _emitInstruction(INST_IMUL, &src);
  }
  //! @overload
  inline void imul(const GPVar& dst_lo, const GPVar& dst_hi, const Mem& src)
  {
    _emitInstruction(INST_IMUL, &src);
  }

  //! @brief Signed multiply.
  //!
  //! Destination operand (the first operand) is multiplied by the source
  //! operand (second operand). The destination operand is a generalpurpose
  //! register and the source operand is an immediate value, a general-purpose
  //! register, or a memory location. The product is then stored in the
  //! destination operand location.
  inline void imul(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_IMUL, &dst, &src);
  }
  //! @brief Signed multiply.
  //! @overload
  inline void imul(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_IMUL, &dst, &src);
  }
  //! @brief Signed multiply.
  //! @overload
  inline void imul(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_IMUL, &dst, &src);
  }

  //! @brief Signed multiply.
  //!
  //! source operand (which can be a general-purpose register or a memory
  //! location) is multiplied by the second source operand (an immediate
  //! value). The product is then stored in the destination operand
  //! (a general-purpose register).
  inline void imul(const GPVar& dst, const GPVar& src, const Imm& imm)
  {
    _emitInstruction(INST_IMUL, &dst, &src, &imm);
  }
  //! @overload
  inline void imul(const GPVar& dst, const Mem& src, const Imm& imm)
  {
    _emitInstruction(INST_IMUL, &dst, &src, &imm);
  }

  //! @brief Increment by 1.
  //! @note This instruction can be slower than add(dst, 1)
  inline void inc(const GPVar& dst)
  {
    _emitInstruction(INST_INC, &dst);
  }
  //! @brief Increment by 1.
  //! @note This instruction can be slower than add(dst, 1)
  inline void inc(const Mem& dst)
  {
    _emitInstruction(INST_INC, &dst);
  }

  //! @brief Interrupt 3 - trap to debugger.
  inline void int3()
  {
    _emitInstruction(INST_INT3);
  }

  //! @brief Jump to label @a label if condition @a cc is met.
  //!
  //! This instruction checks the state of one or more of the status flags in
  //! the EFLAGS register (CF, OF, PF, SF, and ZF) and, if the flags are in the
  //! specified state (condition), performs a jump to the target instruction
  //! specified by the destination operand. A condition code (cc) is associated
  //! with each instruction to indicate the condition being tested for. If the
  //! condition is not satisfied, the jump is not performed and execution
  //! continues with the instruction following the Jcc instruction.
  inline void j(CONDITION cc, const Label& label, uint32_t hint = HINT_NONE)
  {
    _emitJcc(ConditionToInstruction::toJCC(cc), &label, hint);
  }

  //! @brief Jump to label @a label if condition is met.
  inline void ja  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JA  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jae (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JAE , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jb  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JB  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jbe (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JBE , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jc  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JC  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void je  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JE  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jg  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JG  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jge (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JGE , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jl  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JL  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jle (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JLE , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jna (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNA , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnae(const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNAE, &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnb (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNB , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnbe(const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNBE, &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnc (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNC , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jne (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNE , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jng (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNG , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnge(const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNGE, &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnl (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNL , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnle(const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNLE, &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jno (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNO , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnp (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNP , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jns (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNS , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnz (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JNZ , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jo  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JO  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jp  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JP  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jpe (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JPE , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jpo (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JPO , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void js  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JS  , &label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jz  (const Label& label, uint32_t hint = HINT_NONE) { _emitJcc(INST_JZ  , &label, hint); }

  //! @brief Jump.
  //! @overload
  inline void jmp(const GPVar& dst)
  {
    _emitInstruction(INST_JMP, &dst);
  }
  //! @brief Jump.
  //! @overload
  inline void jmp(const Mem& dst)
  {
    _emitInstruction(INST_JMP, &dst);
  }
  //! @brief Jump.
  //! @overload
  inline void jmp(const Imm& dst)
  {
    _emitInstruction(INST_JMP, &dst);
  }

  //! @brief Jump.
  //! @overload
  inline void jmp(void* dst)
  {
    Imm imm((sysint_t)dst);
    _emitInstruction(INST_JMP, &imm);
  }

  //! @brief Jump.
  //!
  //! This instruction transfers program control to a different point
  //! in the instruction stream without recording return information.
  //! The destination (target) operand specifies the label of the
  //! instruction being jumped to.
  inline void jmp(const Label& label)
  {
    _emitInstruction(INST_JMP, &label);
  }
  //! @brief Load Effective Address
  //!
  //! This instruction computes the effective address of the second
  //! operand (the source operand) and stores it in the first operand
  //! (destination operand). The source operand is a memory address
  //! (offset part) specified with one of the processors addressing modes.
  //! The destination operand is a general-purpose register.
  inline void lea(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_LEA, &dst, &src);
  }

  //! @brief High Level Procedure Exit.
  inline void leave()
  {
    _emitInstruction(INST_LEAVE);
  }

  //! @brief Assert LOCK# Signal Prefix.
  //!
  //! This instruction causes the processor�s LOCK# signal to be asserted
  //! during execution of the accompanying instruction (turns the
  //! instruction into an atomic instruction). In a multiprocessor environment,
  //! the LOCK# signal insures that the processor has exclusive use of any shared
  //! memory while the signal is asserted.
  //!
  //! The LOCK prefix can be prepended only to the following instructions and
  //! to those forms of the instructions that use a memory operand: ADD, ADC,
  //! AND, BTC, BTR, BTS, CMPXCHG, DEC, INC, NEG, NOT, OR, SBB, SUB, XOR, XADD,
  //! and XCHG. An undefined opcode exception will be generated if the LOCK
  //! prefix is used with any other instruction. The XCHG instruction always
  //! asserts the LOCK# signal regardless of the presence or absence of the LOCK
  //! prefix.
  inline void lock()
  {
    _emitInstruction(INST_LOCK);
  }

  //! @brief Move.
  //!
  //! This instruction copies the second operand (source operand) to the first
  //! operand (destination operand). The source operand can be an immediate
  //! value, general-purpose register, segment register, or memory location.
  //! The destination register can be a general-purpose register, segment
  //! register, or memory location. Both operands must be the same size, which
  //! can be a byte, a word, or a DWORD.
  //!
  //! @note To move MMX or SSE registers to/from GP registers or memory, use
  //! corresponding functions: @c movd(), @c movq(), etc. Passing MMX or SSE
  //! registers to @c mov() is illegal.
  inline void mov(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_MOV, &dst, &src);
  }

  //! @brief Move byte, word, dword or qword from absolute address @a src to
  //! AL, AX, EAX or RAX register.
  inline void mov_ptr(const GPVar& dst, void* src)
  {
    Imm imm((sysint_t)src);
    _emitInstruction(INST_MOV_PTR, &dst, &imm);
  }

  //! @brief Move byte, word, dword or qword from AL, AX, EAX or RAX register
  //! to absolute address @a dst.
  inline void mov_ptr(void* dst, const GPVar& src)
  {
    Imm imm((sysint_t)dst);
    _emitInstruction(INST_MOV_PTR, &imm, &src);
  }

  //! @brief Move with Sign-Extension.
  //!
  //! This instruction copies the contents of the source operand (register
  //! or memory location) to the destination operand (register) and sign
  //! extends the value to 16, 32 or 64 bits.
  //!
  //! @sa movsxd().
  void movsx(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVSX, &dst, &src);
  }
  //! @brief Move with Sign-Extension.
  //! @overload
  void movsx(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVSX, &dst, &src);
  }

#if defined(ASMJIT_X64)
  //! @brief Move DWord to QWord with sign-extension.
  inline void movsxd(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVSXD, &dst, &src);
  }
  //! @brief Move DWord to QWord with sign-extension.
  //! @overload
  inline void movsxd(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVSXD, &dst, &src);
  }
#endif // ASMJIT_X64

  //! @brief Move with Zero-Extend.
  //!
  //! This instruction copies the contents of the source operand (register
  //! or memory location) to the destination operand (register) and zero
  //! extends the value to 16 or 32 bits. The size of the converted value
  //! depends on the operand-size attribute.
  inline void movzx(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVZX, &dst, &src);
  }
  //! @brief Move with Zero-Extend.
  //! @brief Overload
  inline void movzx(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVZX, &dst, &src);
  }

  //! @brief Unsigned multiply.
  //!
  //! Source operand (in a general-purpose register or memory location)
  //! is multiplied by the value in the AL, AX, or EAX register (depending
  //! on the operand size) and the product is stored in the AX, DX:AX, or
  //! EDX:EAX registers, respectively.
  inline void mul(const GPVar& dst_lo, const GPVar& dst_hi, const GPVar& src)
  {
    _emitInstruction(INST_MUL, &src);
  }
  //! @brief Unsigned multiply.
  //! @overload
  inline void mul(const GPVar& dst_lo, const GPVar& dst_hi, const Mem& src)
  {
    _emitInstruction(INST_MUL, &src);
  }

  //! @brief Two's Complement Negation.
  inline void neg(const GPVar& dst)
  {
    _emitInstruction(INST_NEG, &dst);
  }
  //! @brief Two's Complement Negation.
  inline void neg(const Mem& dst)
  {
    _emitInstruction(INST_NEG, &dst);
  }

  //! @brief No Operation.
  //!
  //! This instruction performs no operation. This instruction is a one-byte
  //! instruction that takes up space in the instruction stream but does not
  //! affect the machine context, except the EIP register. The NOP instruction
  //! is an alias mnemonic for the XCHG (E)AX, (E)AX instruction.
  inline void nop()
  {
    _emitInstruction(INST_NOP);
  }

  //! @brief One's Complement Negation.
  inline void not_(const GPVar& dst)
  {
    _emitInstruction(INST_NOT, &dst);
  }
  //! @brief One's Complement Negation.
  inline void not_(const Mem& dst)
  {
    _emitInstruction(INST_NOT, &dst);
  }

  //! @brief Logical Inclusive OR.
  inline void or_(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_OR, &dst, &src);
  }

  //! @brief Pop a Value from the Stack.
  //!
  //! This instruction loads the value from the top of the stack to the location
  //! specified with the destination operand and then increments the stack pointer.
  //! The destination operand can be a general purpose register, memory location,
  //! or segment register.
  inline void pop(const GPVar& dst)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_TYPE_GPW) || dst.isRegType(REG_TYPE_GPN));
    _emitInstruction(INST_POP, &dst);
  }

  inline void pop(const Mem& dst)
  {
    ASMJIT_ASSERT(dst.getSize() == 2 || dst.getSize() == sizeof(sysint_t));
    _emitInstruction(INST_POP, &dst);
  }

#if defined(ASMJIT_X86)
  //! @brief Pop All General-Purpose Registers.
  //!
  //! Pop EDI, ESI, EBP, EBX, EDX, ECX, and EAX.
  inline void popad()
  {
    _emitInstruction(INST_POPAD);
  }
#endif // ASMJIT_X86

  //! @brief Pop Stack into EFLAGS Register (32 bit or 64 bit).
  inline void popf()
  {
#if defined(ASMJIT_X86)
    popfd();
#else
    popfq();
#endif
  }

#if defined(ASMJIT_X86)
  //! @brief Pop Stack into EFLAGS Register (32 bit).
  inline void popfd() { _emitInstruction(INST_POPFD); }
#else
  //! @brief Pop Stack into EFLAGS Register (64 bit).
  inline void popfq() { _emitInstruction(INST_POPFQ); }
#endif

  //! @brief Push WORD/DWORD/QWORD Onto the Stack.
  //!
  //! @note 32 bit architecture pushed DWORD while 64 bit
  //! pushes QWORD. 64 bit mode not provides instruction to
  //! push 32 bit register/memory.
  inline void push(const GPVar& src)
  {
    ASMJIT_ASSERT(src.isRegType(REG_TYPE_GPW) || src.isRegType(REG_TYPE_GPN));
    _emitInstruction(INST_PUSH, &src);
  }
  //! @brief Push WORD/DWORD/QWORD Onto the Stack.
  inline void push(const Mem& src)
  {
    ASMJIT_ASSERT(src.getSize() == 2 || src.getSize() == sizeof(sysint_t));
    _emitInstruction(INST_PUSH, &src);
  }
  //! @brief Push WORD/DWORD/QWORD Onto the Stack.
  inline void push(const Imm& src)
  {
    _emitInstruction(INST_PUSH, &src);
  }

#if defined(ASMJIT_X86)
  //! @brief Push All General-Purpose Registers.
  //!
  //! Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI.
  inline void pushad()
  {
    _emitInstruction(INST_PUSHAD);
  }
#endif // ASMJIT_X86

  //! @brief Push EFLAGS Register (32 bit or 64 bit) onto the Stack.
  inline void pushf()
  {
#if defined(ASMJIT_X86)
    pushfd();
#else
    pushfq();
#endif
  }

#if defined(ASMJIT_X86)
  //! @brief Push EFLAGS Register (32 bit) onto the Stack.
  inline void pushfd() { _emitInstruction(INST_PUSHFD); }
#else
  //! @brief Push EFLAGS Register (64 bit) onto the Stack.
  inline void pushfq() { _emitInstruction(INST_PUSHFQ); }
#endif // ASMJIT_X86

  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rcl(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_RCL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rcl(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_RCL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rcl(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_RCL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rcl(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_RCL, &dst, &src);
  }

  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void rcr(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_RCR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void rcr(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_RCR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void rcr(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_RCR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void rcr(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_RCR, &dst, &src);
  }

  //! @brief Read Time-Stamp Counter (Pentium).
  inline void rdtsc(const GPVar& dst_edx, const GPVar& dst_eax)
  {
    _emitInstruction(INST_RDTSC, &dst_edx, &dst_eax);
  }

  //! @brief Read Time-Stamp Counter and Processor ID (New).
  inline void rdtscp(const GPVar& dst_edx, const GPVar& dst_eax, const GPVar& dst_ecx)
  {
    _emitInstruction(INST_RDTSCP, &dst_edx, &dst_eax, &dst_ecx);
  }

  //! @brief Return from Procedure.
  inline void ret()
  {
    _emitReturn(NULL);
  }

  //! @brief Return from Procedure.
  inline void ret(const GPVar& val)
  {
    _emitReturn(&val);
  }

  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rol(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_ROL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rol(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_ROL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rol(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_ROL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rol(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_ROL, &dst, &src);
  }

  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void ror(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_ROR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void ror(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_ROR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void ror(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_ROR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void ror(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_ROR, &dst, &src);
  }

#if defined(ASMJIT_X86)
  //! @brief Store @a var (allocated to AH/AX/EAX/RAX) into Flags.
  inline void sahf(const GPVar& var)
  {
    _emitInstruction(INST_SAHF, &var);
  }
#endif // ASMJIT_X86

  //! @brief Integer subtraction with borrow.
  inline void sbb(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_SBB, &dst, &src);
  }

  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void sal(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_SAL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void sal(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_SAL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void sal(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_SAL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void sal(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_SAL, &dst, &src);
  }

  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void sar(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_SAR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void sar(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_SAR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void sar(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_SAR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void sar(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_SAR, &dst, &src);
  }

  //! @brief Set Byte on Condition.
  inline void set(CONDITION cc, const GPVar& dst)
  {
    _emitInstruction(ConditionToInstruction::toSetCC(cc), &dst);
  }

  //! @brief Set Byte on Condition.
  inline void set(CONDITION cc, const Mem& dst)
  {
    _emitInstruction(ConditionToInstruction::toSetCC(cc), &dst);
  }

  //! @brief Set Byte on Condition.
  inline void seta  (const GPVar& dst) { _emitInstruction(INST_SETA  , &dst); }
  //! @brief Set Byte on Condition.
  inline void seta  (const Mem& dst)   { _emitInstruction(INST_SETA  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setae (const GPVar& dst) { _emitInstruction(INST_SETAE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setae (const Mem& dst)   { _emitInstruction(INST_SETAE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setb  (const GPVar& dst) { _emitInstruction(INST_SETB  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setb  (const Mem& dst)   { _emitInstruction(INST_SETB  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setbe (const GPVar& dst) { _emitInstruction(INST_SETBE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setbe (const Mem& dst)   { _emitInstruction(INST_SETBE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setc  (const GPVar& dst) { _emitInstruction(INST_SETC  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setc  (const Mem& dst)   { _emitInstruction(INST_SETC  , &dst); }
  //! @brief Set Byte on Condition.
  inline void sete  (const GPVar& dst) { _emitInstruction(INST_SETE  , &dst); }
  //! @brief Set Byte on Condition.
  inline void sete  (const Mem& dst)   { _emitInstruction(INST_SETE  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setg  (const GPVar& dst) { _emitInstruction(INST_SETG  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setg  (const Mem& dst)   { _emitInstruction(INST_SETG  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setge (const GPVar& dst) { _emitInstruction(INST_SETGE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setge (const Mem& dst)   { _emitInstruction(INST_SETGE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setl  (const GPVar& dst) { _emitInstruction(INST_SETL  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setl  (const Mem& dst)   { _emitInstruction(INST_SETL  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setle (const GPVar& dst) { _emitInstruction(INST_SETLE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setle (const Mem& dst)   { _emitInstruction(INST_SETLE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setna (const GPVar& dst) { _emitInstruction(INST_SETNA , &dst); }
  //! @brief Set Byte on Condition.
  inline void setna (const Mem& dst)   { _emitInstruction(INST_SETNA , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnae(const GPVar& dst) { _emitInstruction(INST_SETNAE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnae(const Mem& dst)   { _emitInstruction(INST_SETNAE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnb (const GPVar& dst) { _emitInstruction(INST_SETNB , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnb (const Mem& dst)   { _emitInstruction(INST_SETNB , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnbe(const GPVar& dst) { _emitInstruction(INST_SETNBE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnbe(const Mem& dst)   { _emitInstruction(INST_SETNBE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnc (const GPVar& dst) { _emitInstruction(INST_SETNC , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnc (const Mem& dst)   { _emitInstruction(INST_SETNC , &dst); }
  //! @brief Set Byte on Condition.
  inline void setne (const GPVar& dst) { _emitInstruction(INST_SETNE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setne (const Mem& dst)   { _emitInstruction(INST_SETNE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setng (const GPVar& dst) { _emitInstruction(INST_SETNG , &dst); }
  //! @brief Set Byte on Condition.
  inline void setng (const Mem& dst)   { _emitInstruction(INST_SETNG , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnge(const GPVar& dst) { _emitInstruction(INST_SETNGE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnge(const Mem& dst)   { _emitInstruction(INST_SETNGE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnl (const GPVar& dst) { _emitInstruction(INST_SETNL , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnl (const Mem& dst)   { _emitInstruction(INST_SETNL , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnle(const GPVar& dst) { _emitInstruction(INST_SETNLE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setnle(const Mem& dst)   { _emitInstruction(INST_SETNLE, &dst); }
  //! @brief Set Byte on Condition.
  inline void setno (const GPVar& dst) { _emitInstruction(INST_SETNO , &dst); }
  //! @brief Set Byte on Condition.
  inline void setno (const Mem& dst)   { _emitInstruction(INST_SETNO , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnp (const GPVar& dst) { _emitInstruction(INST_SETNP , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnp (const Mem& dst)   { _emitInstruction(INST_SETNP , &dst); }
  //! @brief Set Byte on Condition.
  inline void setns (const GPVar& dst) { _emitInstruction(INST_SETNS , &dst); }
  //! @brief Set Byte on Condition.
  inline void setns (const Mem& dst)   { _emitInstruction(INST_SETNS , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnz (const GPVar& dst) { _emitInstruction(INST_SETNZ , &dst); }
  //! @brief Set Byte on Condition.
  inline void setnz (const Mem& dst)   { _emitInstruction(INST_SETNZ , &dst); }
  //! @brief Set Byte on Condition.
  inline void seto  (const GPVar& dst) { _emitInstruction(INST_SETO  , &dst); }
  //! @brief Set Byte on Condition.
  inline void seto  (const Mem& dst)   { _emitInstruction(INST_SETO  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setp  (const GPVar& dst) { _emitInstruction(INST_SETP  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setp  (const Mem& dst)   { _emitInstruction(INST_SETP  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setpe (const GPVar& dst) { _emitInstruction(INST_SETPE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setpe (const Mem& dst)   { _emitInstruction(INST_SETPE , &dst); }
  //! @brief Set Byte on Condition.
  inline void setpo (const GPVar& dst) { _emitInstruction(INST_SETPO , &dst); }
  //! @brief Set Byte on Condition.
  inline void setpo (const Mem& dst)   { _emitInstruction(INST_SETPO , &dst); }
  //! @brief Set Byte on Condition.
  inline void sets  (const GPVar& dst) { _emitInstruction(INST_SETS  , &dst); }
  //! @brief Set Byte on Condition.
  inline void sets  (const Mem& dst)   { _emitInstruction(INST_SETS  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setz  (const GPVar& dst) { _emitInstruction(INST_SETZ  , &dst); }
  //! @brief Set Byte on Condition.
  inline void setz  (const Mem& dst)   { _emitInstruction(INST_SETZ  , &dst); }

  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void shl(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_SHL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void shl(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_SHL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void shl(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_SHL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void shl(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_SHL, &dst, &src);
  }

  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void shr(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_SHR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void shr(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_SHR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void shr(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_SHR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void shr(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_SHR, &dst, &src);
  }

  //! @brief Double Precision Shift Left.
  //! @note src2 register can be only @c cl register.
  inline void shld(const GPVar& dst, const GPVar& src1, const GPVar& src2)
  {
    _emitInstruction(INST_SHLD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Left.
  inline void shld(const GPVar& dst, const GPVar& src1, const Imm& src2)
  {
    _emitInstruction(INST_SHLD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Left.
  //! @note src2 register can be only @c cl register.
  inline void shld(const Mem& dst, const GPVar& src1, const GPVar& src2)
  {
    _emitInstruction(INST_SHLD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Left.
  inline void shld(const Mem& dst, const GPVar& src1, const Imm& src2)
  {
    _emitInstruction(INST_SHLD, &dst, &src1, &src2);
  }

  //! @brief Double Precision Shift Right.
  //! @note src2 register can be only @c cl register.
  inline void shrd(const GPVar& dst, const GPVar& src1, const GPVar& src2)
  {
    _emitInstruction(INST_SHRD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Right.
  inline void shrd(const GPVar& dst, const GPVar& src1, const Imm& src2)
  {
    _emitInstruction(INST_SHRD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Right.
  //! @note src2 register can be only @c cl register.
  inline void shrd(const Mem& dst, const GPVar& src1, const GPVar& src2)
  {
    _emitInstruction(INST_SHRD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Right.
  inline void shrd(const Mem& dst, const GPVar& src1, const Imm& src2)
  {
    _emitInstruction(INST_SHRD, &dst, &src1, &src2);
  }

  //! @brief Set Carry Flag to 1.
  inline void stc()
  {
    _emitInstruction(INST_STC);
  }

  //! @brief Set Direction Flag to 1.
  inline void std()
  {
    _emitInstruction(INST_STD);
  }

  //! @brief Subtract.
  inline void sub(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_SUB, &dst, &src);
  }

  //! @brief Logical Compare.
  inline void test(const GPVar& op1, const GPVar& op2)
  {
    _emitInstruction(INST_TEST, &op1, &op2);
  }
  //! @brief Logical Compare.
  inline void test(const GPVar& op1, const Imm& op2)
  {
    _emitInstruction(INST_TEST, &op1, &op2);
  }
  //! @brief Logical Compare.
  inline void test(const Mem& op1, const GPVar& op2)
  {
    _emitInstruction(INST_TEST, &op1, &op2);
  }
  //! @brief Logical Compare.
  inline void test(const Mem& op1, const Imm& op2)
  {
    _emitInstruction(INST_TEST, &op1, &op2);
  }

  //! @brief Undefined instruction - Raise invalid opcode exception.
  inline void ud2()
  {
    _emitInstruction(INST_UD2);
  }

  //! @brief Exchange and Add.
  inline void xadd(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_XADD, &dst, &src);
  }
  //! @brief Exchange and Add.
  inline void xadd(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_XADD, &dst, &src);
  }

  //! @brief Exchange Register/Memory with Register.
  inline void xchg(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_XCHG, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xchg(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_XCHG, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xchg(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_XCHG, &src, &dst);
  }

  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const GPVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const GPVar& dst, const Imm& src)
  {
    _emitInstruction(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Mem& dst, const Imm& src)
  {
    _emitInstruction(INST_XOR, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [MMX]
  // --------------------------------------------------------------------------

  //! @brief Empty MMX state.
  inline void emms()
  {
    _emitInstruction(INST_EMMS);
  }

  //! @brief Move DWord (MMX).
  inline void movd(const Mem& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord (MMX).
  inline void movd(const GPVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord (MMX).
  inline void movd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord (MMX).
  inline void movd(const MMVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }

  //! @brief Move QWord (MMX).
  inline void movq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
  //! @brief Move QWord (MMX).
  inline void movq(const Mem& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#if defined(ASMJIT_X64)
  //! @brief Move QWord (MMX).
  inline void movq(const GPVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#endif
  //! @brief Move QWord (MMX).
  inline void movq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#if defined(ASMJIT_X64)
  //! @brief Move QWord (MMX).
  inline void movq(const MMVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#endif

  //! @brief Pack with Unsigned Saturation (MMX).
  inline void packuswb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PACKUSWB, &dst, &src);
  }
  //! @brief Pack with Unsigned Saturation (MMX).
  inline void packuswb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PACKUSWB, &dst, &src);
  }

  //! @brief Packed BYTE Add (MMX).
  inline void paddb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDB, &dst, &src);
  }
  //! @brief Packed BYTE Add (MMX).
  inline void paddb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDB, &dst, &src);
  }

  //! @brief Packed WORD Add (MMX).
  inline void paddw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDW, &dst, &src);
  }
  //! @brief Packed WORD Add (MMX).
  inline void paddw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDW, &dst, &src);
  }

  //! @brief Packed DWORD Add (MMX).
  inline void paddd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDD, &dst, &src);
  }
  //! @brief Packed DWORD Add (MMX).
  inline void paddd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDD, &dst, &src);
  }

  //! @brief Packed Add with Saturation (MMX).
  inline void paddsb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDSB, &dst, &src);
  }
  //! @brief Packed Add with Saturation (MMX).
  inline void paddsb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDSB, &dst, &src);
  }

  //! @brief Packed Add with Saturation (MMX).
  inline void paddsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDSW, &dst, &src);
  }
  //! @brief Packed Add with Saturation (MMX).
  inline void paddsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDSW, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDUSB, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDUSB, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDUSW, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDUSW, &dst, &src);
  }

  //! @brief Logical AND (MMX).
  inline void pand(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PAND, &dst, &src);
  }
  //! @brief Logical AND (MMX).
  inline void pand(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PAND, &dst, &src);
  }

  //! @brief Logical AND Not (MMX).
  inline void pandn(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PANDN, &dst, &src);
  }
  //! @brief Logical AND Not (MMX).
  inline void pandn(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PANDN, &dst, &src);
  }

  //! @brief Packed Compare for Equal (BYTES) (MMX).
  inline void pcmpeqb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PCMPEQB, &dst, &src);
  }
  //! @brief Packed Compare for Equal (BYTES) (MMX).
  inline void pcmpeqb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQB, &dst, &src);
  }

  //! @brief Packed Compare for Equal (WORDS) (MMX).
  inline void pcmpeqw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PCMPEQW, &dst, &src);
  }
  //! @brief Packed Compare for Equal (WORDS) (MMX).
  inline void pcmpeqw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQW, &dst, &src);
  }

  //! @brief Packed Compare for Equal (DWORDS) (MMX).
  inline void pcmpeqd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PCMPEQD, &dst, &src);
  }
  //! @brief Packed Compare for Equal (DWORDS) (MMX).
  inline void pcmpeqd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQD, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (BYTES) (MMX).
  inline void pcmpgtb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PCMPGTB, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (BYTES) (MMX).
  inline void pcmpgtb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTB, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (WORDS) (MMX).
  inline void pcmpgtw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PCMPGTW, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (WORDS) (MMX).
  inline void pcmpgtw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTW, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (DWORDS) (MMX).
  inline void pcmpgtd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PCMPGTD, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (DWORDS) (MMX).
  inline void pcmpgtd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTD, &dst, &src);
  }

  //! @brief Packed Multiply High (MMX).
  inline void pmulhw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMULHW, &dst, &src);
  }
  //! @brief Packed Multiply High (MMX).
  inline void pmulhw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULHW, &dst, &src);
  }

  //! @brief Packed Multiply Low (MMX).
  inline void pmullw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMULLW, &dst, &src);
  }
  //! @brief Packed Multiply Low (MMX).
  inline void pmullw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULLW, &dst, &src);
  }

  //! @brief Bitwise Logical OR (MMX).
  inline void por(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_POR, &dst, &src);
  }
  //! @brief Bitwise Logical OR (MMX).
  inline void por(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_POR, &dst, &src);
  }

  //! @brief Packed Multiply and Add (MMX).
  inline void pmaddwd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMADDWD, &dst, &src);
  }
  //! @brief Packed Multiply and Add (MMX).
  inline void pmaddwd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMADDWD, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  inline void pslld(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void pslld(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void pslld(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLD, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  inline void psllq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllq(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLQ, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  inline void psllw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllw(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLW, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psrad(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psrad(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psrad(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRAD, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psraw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psraw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psraw(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRAW, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (MMX).
  inline void psrld(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrld(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrld(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLD, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlq(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLQ, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlw(const MMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLW, &dst, &src);
  }

  //! @brief Packed Subtract (MMX).
  inline void psubb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBB, &dst, &src);
  }
  //! @brief Packed Subtract (MMX).
  inline void psubb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBB, &dst, &src);
  }

  //! @brief Packed Subtract (MMX).
  inline void psubw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBW, &dst, &src);
  }
  //! @brief Packed Subtract (MMX).
  inline void psubw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBW, &dst, &src);
  }

  //! @brief Packed Subtract (MMX).
  inline void psubd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBD, &dst, &src);
  }
  //! @brief Packed Subtract (MMX).
  inline void psubd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBD, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBSB, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBSB, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBSW, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBSW, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBUSB, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBUSB, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBUSW, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBUSW, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhbw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PUNPCKHBW, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhbw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHBW, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhwd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PUNPCKHWD, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhwd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHWD, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhdq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PUNPCKHDQ, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhdq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHDQ, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklbw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PUNPCKLBW, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklbw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLBW, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklwd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PUNPCKLWD, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklwd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLWD, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckldq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PUNPCKLDQ, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckldq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLDQ, &dst, &src);
  }

  //! @brief Bitwise Exclusive OR (MMX).
  inline void pxor(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PXOR, &dst, &src);
  }
  //! @brief Bitwise Exclusive OR (MMX).
  inline void pxor(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PXOR, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [3dNow]
  // --------------------------------------------------------------------------

  //! @brief Faster EMMS (3dNow!).
  //!
  //! @note Use only for early AMD processors where is only 3dNow! or SSE. If
  //! CPU contains SSE2, it's better to use @c emms() ( @c femms() is mapped
  //! to @c emms() ).
  inline void femms()
  {
    _emitInstruction(INST_FEMMS);
  }

  //! @brief Packed SP-FP to Integer Convert (3dNow!).
  inline void pf2id(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PF2ID, &dst, &src);
  }
  //! @brief Packed SP-FP to Integer Convert (3dNow!).
  inline void pf2id(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PF2ID, &dst, &src);
  }

  //! @brief  Packed SP-FP to Integer Word Convert (3dNow!).
  inline void pf2iw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PF2IW, &dst, &src);
  }
  //! @brief  Packed SP-FP to Integer Word Convert (3dNow!).
  inline void pf2iw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PF2IW, &dst, &src);
  }

  //! @brief Packed SP-FP Accumulate (3dNow!).
  inline void pfacc(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFACC, &dst, &src);
  }
  //! @brief Packed SP-FP Accumulate (3dNow!).
  inline void pfacc(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFACC, &dst, &src);
  }

  //! @brief Packed SP-FP Addition (3dNow!).
  inline void pfadd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFADD, &dst, &src);
  }
  //! @brief Packed SP-FP Addition (3dNow!).
  inline void pfadd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFADD, &dst, &src);
  }

  //! @brief Packed SP-FP Compare - dst == src (3dNow!).
  inline void pfcmpeq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFCMPEQ, &dst, &src);
  }
  //! @brief Packed SP-FP Compare - dst == src (3dNow!).
  inline void pfcmpeq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFCMPEQ, &dst, &src);
  }

  //! @brief Packed SP-FP Compare - dst >= src (3dNow!).
  inline void pfcmpge(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFCMPGE, &dst, &src);
  }
  //! @brief Packed SP-FP Compare - dst >= src (3dNow!).
  inline void pfcmpge(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFCMPGE, &dst, &src);
  }

  //! @brief Packed SP-FP Compare - dst > src (3dNow!).
  inline void pfcmpgt(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFCMPGT, &dst, &src);
  }
  //! @brief Packed SP-FP Compare - dst > src (3dNow!).
  inline void pfcmpgt(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFCMPGT, &dst, &src);
  }

  //! @brief Packed SP-FP Maximum (3dNow!).
  inline void pfmax(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFMAX, &dst, &src);
  }
  //! @brief Packed SP-FP Maximum (3dNow!).
  inline void pfmax(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFMAX, &dst, &src);
  }

  //! @brief Packed SP-FP Minimum (3dNow!).
  inline void pfmin(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFMIN, &dst, &src);
  }
  //! @brief Packed SP-FP Minimum (3dNow!).
  inline void pfmin(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFMIN, &dst, &src);
  }

  //! @brief Packed SP-FP Multiply (3dNow!).
  inline void pfmul(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFMUL, &dst, &src);
  }
  //! @brief Packed SP-FP Multiply (3dNow!).
  inline void pfmul(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFMUL, &dst, &src);
  }

  //! @brief Packed SP-FP Negative Accumulate (3dNow!).
  inline void pfnacc(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFNACC, &dst, &src);
  }
  //! @brief Packed SP-FP Negative Accumulate (3dNow!).
  inline void pfnacc(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFNACC, &dst, &src);
  }

  //! @brief Packed SP-FP Mixed Accumulate (3dNow!).
  inline void pfpnaxx(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFPNACC, &dst, &src);
  }
  //! @brief Packed SP-FP Mixed Accumulate (3dNow!).
  inline void pfpnacc(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFPNACC, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal Approximation (3dNow!).
  inline void pfrcp(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFRCP, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal Approximation (3dNow!).
  inline void pfrcp(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFRCP, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal, First Iteration Step (3dNow!).
  inline void pfrcpit1(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFRCPIT1, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal, First Iteration Step (3dNow!).
  inline void pfrcpit1(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFRCPIT1, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal, Second Iteration Step (3dNow!).
  inline void pfrcpit2(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFRCPIT2, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal, Second Iteration Step (3dNow!).
  inline void pfrcpit2(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFRCPIT2, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal Square Root, First Iteration Step (3dNow!).
  inline void pfrsqit1(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFRSQIT1, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal Square Root, First Iteration Step (3dNow!).
  inline void pfrsqit1(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFRSQIT1, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal Square Root Approximation (3dNow!).
  inline void pfrsqrt(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFRSQRT, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal Square Root Approximation (3dNow!).
  inline void pfrsqrt(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFRSQRT, &dst, &src);
  }

  //! @brief Packed SP-FP Subtract (3dNow!).
  inline void pfsub(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFSUB, &dst, &src);
  }
  //! @brief Packed SP-FP Subtract (3dNow!).
  inline void pfsub(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFSUB, &dst, &src);
  }

  //! @brief Packed SP-FP Reverse Subtract (3dNow!).
  inline void pfsubr(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PFSUBR, &dst, &src);
  }
  //! @brief Packed SP-FP Reverse Subtract (3dNow!).
  inline void pfsubr(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PFSUBR, &dst, &src);
  }

  //! @brief Packed DWords to SP-FP (3dNow!).
  inline void pi2fd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PI2FD, &dst, &src);
  }
  //! @brief Packed DWords to SP-FP (3dNow!).
  inline void pi2fd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PI2FD, &dst, &src);
  }

  //! @brief Packed Words to SP-FP (3dNow!).
  inline void pi2fw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PI2FW, &dst, &src);
  }
  //! @brief Packed Words to SP-FP (3dNow!).
  inline void pi2fw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PI2FW, &dst, &src);
  }

  //! @brief Packed swap DWord (3dNow!)
  inline void pswapd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSWAPD, &dst, &src);
  }
  //! @brief Packed swap DWord (3dNow!)
  inline void pswapd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSWAPD, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [SSE]
  // --------------------------------------------------------------------------

  //! @brief Packed SP-FP Add (SSE).
  inline void addps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ADDPS, &dst, &src);
  }
  //! @brief Packed SP-FP Add (SSE).
  inline void addps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADDPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Add (SSE).
  inline void addss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ADDSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Add (SSE).
  inline void addss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADDSS, &dst, &src);
  }

  //! @brief Bit-wise Logical And Not For SP-FP (SSE).
  inline void andnps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ANDNPS, &dst, &src);
  }
  //! @brief Bit-wise Logical And Not For SP-FP (SSE).
  inline void andnps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ANDNPS, &dst, &src);
  }

  //! @brief Bit-wise Logical And For SP-FP (SSE).
  inline void andps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ANDPS, &dst, &src);
  }
  //! @brief Bit-wise Logical And For SP-FP (SSE).
  inline void andps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ANDPS, &dst, &src);
  }

  //! @brief Packed SP-FP Compare (SSE).
  inline void cmpps(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPPS, &dst, &src, &imm8);
  }
  //! @brief Packed SP-FP Compare (SSE).
  inline void cmpps(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPPS, &dst, &src, &imm8);
  }

  //! @brief Compare Scalar SP-FP Values (SSE).
  inline void cmpss(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPSS, &dst, &src, &imm8);
  }
  //! @brief Compare Scalar SP-FP Values (SSE).
  inline void cmpss(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPSS, &dst, &src, &imm8);
  }

  //! @brief Scalar Ordered SP-FP Compare and Set EFLAGS (SSE).
  inline void comiss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_COMISS, &dst, &src);
  }
  //! @brief Scalar Ordered SP-FP Compare and Set EFLAGS (SSE).
  inline void comiss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_COMISS, &dst, &src);
  }

  //! @brief Packed Signed INT32 to Packed SP-FP Conversion (SSE).
  inline void cvtpi2ps(const XMMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_CVTPI2PS, &dst, &src);
  }
  //! @brief Packed Signed INT32 to Packed SP-FP Conversion (SSE).
  inline void cvtpi2ps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPI2PS, &dst, &src);
  }

  //! @brief Packed SP-FP to Packed INT32 Conversion (SSE).
  inline void cvtps2pi(const MMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTPS2PI, &dst, &src);
  }
  //! @brief Packed SP-FP to Packed INT32 Conversion (SSE).
  inline void cvtps2pi(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPS2PI, &dst, &src);
  }

  //! @brief Scalar Signed INT32 to SP-FP Conversion (SSE).
  inline void cvtsi2ss(const XMMVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_CVTSI2SS, &dst, &src);
  }
  //! @brief Scalar Signed INT32 to SP-FP Conversion (SSE).
  inline void cvtsi2ss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTSI2SS, &dst, &src);
  }

  //! @brief Scalar SP-FP to Signed INT32 Conversion (SSE).
  inline void cvtss2si(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTSS2SI, &dst, &src);
  }
  //! @brief Scalar SP-FP to Signed INT32 Conversion (SSE).
  inline void cvtss2si(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTSS2SI, &dst, &src);
  }

  //! @brief Packed SP-FP to Packed INT32 Conversion (truncate) (SSE).
  inline void cvttps2pi(const MMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTTPS2PI, &dst, &src);
  }
  //! @brief Packed SP-FP to Packed INT32 Conversion (truncate) (SSE).
  inline void cvttps2pi(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTTPS2PI, &dst, &src);
  }

  //! @brief Scalar SP-FP to Signed INT32 Conversion (truncate) (SSE).
  inline void cvttss2si(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTTSS2SI, &dst, &src);
  }
  //! @brief Scalar SP-FP to Signed INT32 Conversion (truncate) (SSE).
  inline void cvttss2si(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTTSS2SI, &dst, &src);
  }

  //! @brief Packed SP-FP Divide (SSE).
  inline void divps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_DIVPS, &dst, &src);
  }
  //! @brief Packed SP-FP Divide (SSE).
  inline void divps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_DIVPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Divide (SSE).
  inline void divss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_DIVSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Divide (SSE).
  inline void divss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_DIVSS, &dst, &src);
  }

  //! @brief Load Streaming SIMD Extension Control/Status (SSE).
  inline void ldmxcsr(const Mem& src)
  {
    _emitInstruction(INST_LDMXCSR, &src);
  }

  //! @brief Byte Mask Write (SSE).
  //!
  //! @note The default memory location is specified by DS:EDI.
  inline void maskmovq(const MMVar& data, const MMVar& mask)
  {
    _emitInstruction(INST_MASKMOVQ, &data, &mask);
  }

  //! @brief Packed SP-FP Maximum (SSE).
  inline void maxps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MAXPS, &dst, &src);
  }
  //! @brief Packed SP-FP Maximum (SSE).
  inline void maxps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MAXPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Maximum (SSE).
  inline void maxss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MAXSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Maximum (SSE).
  inline void maxss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MAXSS, &dst, &src);
  }

  //! @brief Packed SP-FP Minimum (SSE).
  inline void minps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MINPS, &dst, &src);
  }
  //! @brief Packed SP-FP Minimum (SSE).
  inline void minps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MINPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Minimum (SSE).
  inline void minss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MINSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Minimum (SSE).
  inline void minss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MINSS, &dst, &src);
  }

  //! @brief Move Aligned Packed SP-FP Values (SSE).
  inline void movaps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVAPS, &dst, &src);
  }
  //! @brief Move Aligned Packed SP-FP Values (SSE).
  inline void movaps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVAPS, &dst, &src);
  }

  //! @brief Move Aligned Packed SP-FP Values (SSE).
  inline void movaps(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVAPS, &dst, &src);
  }

  //! @brief Move DWord.
  inline void movd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord.
  inline void movd(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord.
  inline void movd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord.
  inline void movd(const XMMVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVD, &dst, &src);
  }

  //! @brief Move QWord (SSE).
  inline void movq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
  //! @brief Move QWord (SSE).
  inline void movq(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#if defined(ASMJIT_X64)
  //! @brief Move QWord (SSE).
  inline void movq(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#endif // ASMJIT_X64
  //! @brief Move QWord (SSE).
  inline void movq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#if defined(ASMJIT_X64)
  //! @brief Move QWord (SSE).
  inline void movq(const XMMVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVQ, &dst, &src);
  }
#endif // ASMJIT_X64

  //! @brief Move 64 Bits Non Temporal (SSE).
  inline void movntq(const Mem& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVNTQ, &dst, &src);
  }

  //! @brief High to Low Packed SP-FP (SSE).
  inline void movhlps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVHLPS, &dst, &src);
  }

  //! @brief Move High Packed SP-FP (SSE).
  inline void movhps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVHPS, &dst, &src);
  }

  //! @brief Move High Packed SP-FP (SSE).
  inline void movhps(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVHPS, &dst, &src);
  }

  //! @brief Move Low to High Packed SP-FP (SSE).
  inline void movlhps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVLHPS, &dst, &src);
  }

  //! @brief Move Low Packed SP-FP (SSE).
  inline void movlps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVLPS, &dst, &src);
  }

  //! @brief Move Low Packed SP-FP (SSE).
  inline void movlps(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVLPS, &dst, &src);
  }

  //! @brief Move Aligned Four Packed SP-FP Non Temporal (SSE).
  inline void movntps(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVNTPS, &dst, &src);
  }

  //! @brief Move Scalar SP-FP (SSE).
  inline void movss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVSS, &dst, &src);
  }

  //! @brief Move Scalar SP-FP (SSE).
  inline void movss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVSS, &dst, &src);
  }

  //! @brief Move Scalar SP-FP (SSE).
  inline void movss(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVSS, &dst, &src);
  }

  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  inline void movups(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVUPS, &dst, &src);
  }
  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  inline void movups(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVUPS, &dst, &src);
  }

  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  inline void movups(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVUPS, &dst, &src);
  }

  //! @brief Packed SP-FP Multiply (SSE).
  inline void mulps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MULPS, &dst, &src);
  }
  //! @brief Packed SP-FP Multiply (SSE).
  inline void mulps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MULPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Multiply (SSE).
  inline void mulss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MULSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Multiply (SSE).
  inline void mulss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MULSS, &dst, &src);
  }

  //! @brief Bit-wise Logical OR for SP-FP Data (SSE).
  inline void orps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ORPS, &dst, &src);
  }
  //! @brief Bit-wise Logical OR for SP-FP Data (SSE).
  inline void orps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ORPS, &dst, &src);
  }

  //! @brief Packed Average (SSE).
  inline void pavgb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PAVGB, &dst, &src);
  }
  //! @brief Packed Average (SSE).
  inline void pavgb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PAVGB, &dst, &src);
  }

  //! @brief Packed Average (SSE).
  inline void pavgw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PAVGW, &dst, &src);
  }
  //! @brief Packed Average (SSE).
  inline void pavgw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PAVGW, &dst, &src);
  }

  //! @brief Extract Word (SSE).
  inline void pextrw(const GPVar& dst, const MMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRW, &dst, &src, &imm8);
  }

  //! @brief Insert Word (SSE).
  inline void pinsrw(const MMVar& dst, const GPVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRW, &dst, &src, &imm8);
  }
  //! @brief Insert Word (SSE).
  inline void pinsrw(const MMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRW, &dst, &src, &imm8);
  }

  //! @brief Packed Signed Integer Word Maximum (SSE).
  inline void pmaxsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMAXSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Maximum (SSE).
  inline void pmaxsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Maximum (SSE).
  inline void pmaxub(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMAXUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Maximum (SSE).
  inline void pmaxub(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXUB, &dst, &src);
  }

  //! @brief Packed Signed Integer Word Minimum (SSE).
  inline void pminsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMINSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Minimum (SSE).
  inline void pminsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Minimum (SSE).
  inline void pminub(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMINUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Minimum (SSE).
  inline void pminub(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINUB, &dst, &src);
  }

  //! @brief Move Byte Mask To Integer (SSE).
  inline void pmovmskb(const GPVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMOVMSKB, &dst, &src);
  }

  //! @brief Packed Multiply High Unsigned (SSE).
  inline void pmulhuw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMULHUW, &dst, &src);
  }
  //! @brief Packed Multiply High Unsigned (SSE).
  inline void pmulhuw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULHUW, &dst, &src);
  }

  //! @brief Packed Sum of Absolute Differences (SSE).
  inline void psadbw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSADBW, &dst, &src);
  }
  //! @brief Packed Sum of Absolute Differences (SSE).
  inline void psadbw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSADBW, &dst, &src);
  }

  //! @brief Packed Shuffle word (SSE).
  inline void pshufw(const MMVar& dst, const MMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFW, &dst, &src, &imm8);
  }
  //! @brief Packed Shuffle word (SSE).
  inline void pshufw(const MMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFW, &dst, &src, &imm8);
  }

  //! @brief Packed SP-FP Reciprocal (SSE).
  inline void rcpps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_RCPPS, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal (SSE).
  inline void rcpps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_RCPPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Reciprocal (SSE).
  inline void rcpss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_RCPSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Reciprocal (SSE).
  inline void rcpss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_RCPSS, &dst, &src);
  }

  //! @brief Prefetch (SSE).
  inline void prefetch(const Mem& mem, const Imm& hint)
  {
    _emitInstruction(INST_PREFETCH, &mem, &hint);
  }

  //! @brief Compute Sum of Absolute Differences (SSE).
  inline void psadbw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSADBW, &dst, &src);
  }
  //! @brief Compute Sum of Absolute Differences (SSE).
  inline void psadbw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSADBW, &dst, &src);
  }

  //! @brief Packed SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_RSQRTPS, &dst, &src);
  }
  //! @brief Packed SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_RSQRTPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_RSQRTSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_RSQRTSS, &dst, &src);
  }

  //! @brief Store fence (SSE).
  inline void sfence()
  {
    _emitInstruction(INST_SFENCE);
  }

  //! @brief Shuffle SP-FP (SSE).
  inline void shufps(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_SHUFPS, &dst, &src, &imm8);
  }
  //! @brief Shuffle SP-FP (SSE).
  inline void shufps(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_SHUFPS, &dst, &src, &imm8);
  }

  //! @brief Packed SP-FP Square Root (SSE).
  inline void sqrtps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SQRTPS, &dst, &src);
  }
  //! @brief Packed SP-FP Square Root (SSE).
  inline void sqrtps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SQRTPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Square Root (SSE).
  inline void sqrtss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SQRTSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Square Root (SSE).
  inline void sqrtss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SQRTSS, &dst, &src);
  }

  //! @brief Store Streaming SIMD Extension Control/Status (SSE).
  inline void stmxcsr(const Mem& dst)
  {
    _emitInstruction(INST_STMXCSR, &dst);
  }

  //! @brief Packed SP-FP Subtract (SSE).
  inline void subps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SUBPS, &dst, &src);
  }
  //! @brief Packed SP-FP Subtract (SSE).
  inline void subps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SUBPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Subtract (SSE).
  inline void subss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SUBSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Subtract (SSE).
  inline void subss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SUBSS, &dst, &src);
  }

  //! @brief Unordered Scalar SP-FP compare and set EFLAGS (SSE).
  inline void ucomiss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_UCOMISS, &dst, &src);
  }
  //! @brief Unordered Scalar SP-FP compare and set EFLAGS (SSE).
  inline void ucomiss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_UCOMISS, &dst, &src);
  }

  //! @brief Unpack High Packed SP-FP Data (SSE).
  inline void unpckhps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_UNPCKHPS, &dst, &src);
  }
  //! @brief Unpack High Packed SP-FP Data (SSE).
  inline void unpckhps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_UNPCKHPS, &dst, &src);
  }

  //! @brief Unpack Low Packed SP-FP Data (SSE).
  inline void unpcklps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_UNPCKLPS, &dst, &src);
  }
  //! @brief Unpack Low Packed SP-FP Data (SSE).
  inline void unpcklps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_UNPCKLPS, &dst, &src);
  }

  //! @brief Bit-wise Logical Xor for SP-FP Data (SSE).
  inline void xorps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_XORPS, &dst, &src);
  }
  //! @brief Bit-wise Logical Xor for SP-FP Data (SSE).
  inline void xorps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_XORPS, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [SSE2]
  // --------------------------------------------------------------------------

  //! @brief Packed DP-FP Add (SSE2).
  inline void addpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ADDPD, &dst, &src);
  }
  //! @brief Packed DP-FP Add (SSE2).
  inline void addpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADDPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Add (SSE2).
  inline void addsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ADDSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Add (SSE2).
  inline void addsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADDSD, &dst, &src);
  }

  //! @brief Bit-wise Logical And Not For DP-FP (SSE2).
  inline void andnpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ANDNPD, &dst, &src);
  }
  //! @brief Bit-wise Logical And Not For DP-FP (SSE2).
  inline void andnpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ANDNPD, &dst, &src);
  }

  //! @brief Bit-wise Logical And For DP-FP (SSE2).
  inline void andpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ANDPD, &dst, &src);
  }
  //! @brief Bit-wise Logical And For DP-FP (SSE2).
  inline void andpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ANDPD, &dst, &src);
  }

  //! @brief Flush Cache Line (SSE2).
  inline void clflush(const Mem& mem)
  {
    _emitInstruction(INST_CLFLUSH, &mem);
  }

  //! @brief Packed DP-FP Compare (SSE2).
  inline void cmppd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPPD, &dst, &src, &imm8);
  }
  //! @brief Packed DP-FP Compare (SSE2).
  inline void cmppd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPPD, &dst, &src, &imm8);
  }

  //! @brief Compare Scalar SP-FP Values (SSE2).
  inline void cmpsd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPSD, &dst, &src, &imm8);
  }
  //! @brief Compare Scalar SP-FP Values (SSE2).
  inline void cmpsd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_CMPSD, &dst, &src, &imm8);
  }

  //! @brief Scalar Ordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void comisd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_COMISD, &dst, &src);
  }
  //! @brief Scalar Ordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void comisd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_COMISD, &dst, &src);
  }

  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtdq2pd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTDQ2PD, &dst, &src);
  }
  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtdq2pd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTDQ2PD, &dst, &src);
  }

  //! @brief Convert Packed Dword Integers to Packed SP-FP Values (SSE2).
  inline void cvtdq2ps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTDQ2PS, &dst, &src);
  }
  //! @brief Convert Packed Dword Integers to Packed SP-FP Values (SSE2).
  inline void cvtdq2ps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTDQ2PS, &dst, &src);
  }

  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2dq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTPD2DQ, &dst, &src);
  }
  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2dq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPD2DQ, &dst, &src);
  }

  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2pi(const MMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTPD2PI, &dst, &src);
  }
  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2pi(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPD2PI, &dst, &src);
  }

  //! @brief Convert Packed DP-FP Values to Packed SP-FP Values (SSE2).
  inline void cvtpd2ps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTPD2PS, &dst, &src);
  }
  //! @brief Convert Packed DP-FP Values to Packed SP-FP Values (SSE2).
  inline void cvtpd2ps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPD2PS, &dst, &src);
  }

  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtpi2pd(const XMMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_CVTPI2PD, &dst, &src);
  }
  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtpi2pd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPI2PD, &dst, &src);
  }

  //! @brief Convert Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtps2dq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTPS2DQ, &dst, &src);
  }
  //! @brief Convert Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtps2dq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPS2DQ, &dst, &src);
  }

  //! @brief Convert Packed SP-FP Values to Packed DP-FP Values (SSE2).
  inline void cvtps2pd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTPS2PD, &dst, &src);
  }
  //! @brief Convert Packed SP-FP Values to Packed DP-FP Values (SSE2).
  inline void cvtps2pd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTPS2PD, &dst, &src);
  }

  //! @brief Convert Scalar DP-FP Value to Dword Integer (SSE2).
  inline void cvtsd2si(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTSD2SI, &dst, &src);
  }
  //! @brief Convert Scalar DP-FP Value to Dword Integer (SSE2).
  inline void cvtsd2si(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTSD2SI, &dst, &src);
  }

  //! @brief Convert Scalar DP-FP Value to Scalar SP-FP Value (SSE2).
  inline void cvtsd2ss(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTSD2SS, &dst, &src);
  }
  //! @brief Convert Scalar DP-FP Value to Scalar SP-FP Value (SSE2).
  inline void cvtsd2ss(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTSD2SS, &dst, &src);
  }

  //! @brief Convert Dword Integer to Scalar DP-FP Value (SSE2).
  inline void cvtsi2sd(const XMMVar& dst, const GPVar& src)
  {
    _emitInstruction(INST_CVTSI2SD, &dst, &src);
  }
  //! @brief Convert Dword Integer to Scalar DP-FP Value (SSE2).
  inline void cvtsi2sd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTSI2SD, &dst, &src);
  }

  //! @brief Convert Scalar SP-FP Value to Scalar DP-FP Value (SSE2).
  inline void cvtss2sd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTSS2SD, &dst, &src);
  }
  //! @brief Convert Scalar SP-FP Value to Scalar DP-FP Value (SSE2).
  inline void cvtss2sd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTSS2SD, &dst, &src);
  }

  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2pi(const MMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTTPD2PI, &dst, &src);
  }
  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2pi(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTTPD2PI, &dst, &src);
  }

  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2dq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTTPD2DQ, &dst, &src);
  }
  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2dq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTTPD2DQ, &dst, &src);
  }

  //! @brief Convert with Truncation Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttps2dq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTTPS2DQ, &dst, &src);
  }
  //! @brief Convert with Truncation Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttps2dq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTTPS2DQ, &dst, &src);
  }

  //! @brief Convert with Truncation Scalar DP-FP Value to Signed Dword Integer (SSE2).
  inline void cvttsd2si(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_CVTTSD2SI, &dst, &src);
  }
  //! @brief Convert with Truncation Scalar DP-FP Value to Signed Dword Integer (SSE2).
  inline void cvttsd2si(const GPVar& dst, const Mem& src)
  {
    _emitInstruction(INST_CVTTSD2SI, &dst, &src);
  }

  //! @brief Packed DP-FP Divide (SSE2).
  inline void divpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_DIVPD, &dst, &src);
  }
  //! @brief Packed DP-FP Divide (SSE2).
  inline void divpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_DIVPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Divide (SSE2).
  inline void divsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_DIVSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Divide (SSE2).
  inline void divsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_DIVSD, &dst, &src);
  }

  //! @brief Load Fence (SSE2).
  inline void lfence()
  {
    _emitInstruction(INST_LFENCE);
  }

  //! @brief Store Selected Bytes of Double Quadword (SSE2).
  //!
  //! @note Target is DS:EDI.
  inline void maskmovdqu(const XMMVar& src, const XMMVar& mask)
  {
    _emitInstruction(INST_MASKMOVDQU, &src, &mask);
  }

  //! @brief Return Maximum Packed Double-Precision FP Values (SSE2).
  inline void maxpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MAXPD, &dst, &src);
  }
  //! @brief Return Maximum Packed Double-Precision FP Values (SSE2).
  inline void maxpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MAXPD, &dst, &src);
  }

  //! @brief Return Maximum Scalar Double-Precision FP Value (SSE2).
  inline void maxsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MAXSD, &dst, &src);
  }
  //! @brief Return Maximum Scalar Double-Precision FP Value (SSE2).
  inline void maxsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MAXSD, &dst, &src);
  }

  //! @brief Memory Fence (SSE2).
  inline void mfence()
  {
    _emitInstruction(INST_MFENCE);
  }

  //! @brief Return Minimum Packed DP-FP Values (SSE2).
  inline void minpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MINPD, &dst, &src);
  }
  //! @brief Return Minimum Packed DP-FP Values (SSE2).
  inline void minpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MINPD, &dst, &src);
  }

  //! @brief Return Minimum Scalar DP-FP Value (SSE2).
  inline void minsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MINSD, &dst, &src);
  }
  //! @brief Return Minimum Scalar DP-FP Value (SSE2).
  inline void minsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MINSD, &dst, &src);
  }

  //! @brief Move Aligned DQWord (SSE2).
  inline void movdqa(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVDQA, &dst, &src);
  }
  //! @brief Move Aligned DQWord (SSE2).
  inline void movdqa(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVDQA, &dst, &src);
  }

  //! @brief Move Aligned DQWord (SSE2).
  inline void movdqa(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVDQA, &dst, &src);
  }

  //! @brief Move Unaligned Double Quadword (SSE2).
  inline void movdqu(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVDQU, &dst, &src);
  }
  //! @brief Move Unaligned Double Quadword (SSE2).
  inline void movdqu(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVDQU, &dst, &src);
  }

  //! @brief Move Unaligned Double Quadword (SSE2).
  inline void movdqu(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVDQU, &dst, &src);
  }

  //! @brief Extract Packed SP-FP Sign Mask (SSE2).
  inline void movmskps(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVMSKPS, &dst, &src);
  }

  //! @brief Extract Packed DP-FP Sign Mask (SSE2).
  inline void movmskpd(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVMSKPD, &dst, &src);
  }

  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  inline void movsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVSD, &dst, &src);
  }
  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  inline void movsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVSD, &dst, &src);
  }

  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  inline void movsd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVSD, &dst, &src);
  }

  //! @brief Move Aligned Packed Double-Precision FP Values (SSE2).
  inline void movapd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVAPD, &dst, &src);
  }

  //! @brief Move Aligned Packed Double-Precision FP Values (SSE2).
  inline void movapd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVAPD, &dst, &src);
  }

  //! @brief Move Aligned Packed Double-Precision FP Values (SSE2).
  inline void movapd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVAPD, &dst, &src);
  }

  //! @brief Move Quadword from XMM to MMX Technology Register (SSE2).
  inline void movdq2q(const MMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVDQ2Q, &dst, &src);
  }

  //! @brief Move Quadword from MMX Technology to XMM Register (SSE2).
  inline void movq2dq(const XMMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_MOVQ2DQ, &dst, &src);
  }

  //! @brief Move High Packed Double-Precision FP Value (SSE2).
  inline void movhpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVHPD, &dst, &src);
  }

  //! @brief Move High Packed Double-Precision FP Value (SSE2).
  inline void movhpd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVHPD, &dst, &src);
  }

  //! @brief Move Low Packed Double-Precision FP Value (SSE2).
  inline void movlpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVLPD, &dst, &src);
  }

  //! @brief Move Low Packed Double-Precision FP Value (SSE2).
  inline void movlpd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVLPD, &dst, &src);
  }

  //! @brief Store Double Quadword Using Non-Temporal Hint (SSE2).
  inline void movntdq(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVNTDQ, &dst, &src);
  }

  //! @brief Store Store DWORD Using Non-Temporal Hint (SSE2).
  inline void movnti(const Mem& dst, const GPVar& src)
  {
    _emitInstruction(INST_MOVNTI, &dst, &src);
  }

  //! @brief Store Packed Double-Precision FP Values Using Non-Temporal Hint (SSE2).
  inline void movntpd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVNTPD, &dst, &src);
  }

  //! @brief Move Unaligned Packed Double-Precision FP Values (SSE2).
  inline void movupd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVUPD, &dst, &src);
  }

  //! @brief Move Unaligned Packed Double-Precision FP Values (SSE2).
  inline void movupd(const Mem& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVUPD, &dst, &src);
  }

  //! @brief Packed DP-FP Multiply (SSE2).
  inline void mulpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MULPD, &dst, &src);
  }
  //! @brief Packed DP-FP Multiply (SSE2).
  inline void mulpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MULPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Multiply (SSE2).
  inline void mulsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MULSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Multiply (SSE2).
  inline void mulsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MULSD, &dst, &src);
  }

  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void orpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ORPD, &dst, &src);
  }
  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void orpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ORPD, &dst, &src);
  }

  //! @brief Pack with Signed Saturation (SSE2).
  inline void packsswb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PACKSSWB, &dst, &src);
  }
  //! @brief Pack with Signed Saturation (SSE2).
  inline void packsswb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PACKSSWB, &dst, &src);
  }

  //! @brief Pack with Signed Saturation (SSE2).
  inline void packssdw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PACKSSDW, &dst, &src);
  }
  //! @brief Pack with Signed Saturation (SSE2).
  inline void packssdw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PACKSSDW, &dst, &src);
  }

  //! @brief Pack with Unsigned Saturation (SSE2).
  inline void packuswb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PACKUSWB, &dst, &src);
  }
  //! @brief Pack with Unsigned Saturation (SSE2).
  inline void packuswb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PACKUSWB, &dst, &src);
  }

  //! @brief Packed BYTE Add (SSE2).
  inline void paddb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDB, &dst, &src);
  }
  //! @brief Packed BYTE Add (SSE2).
  inline void paddb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDB, &dst, &src);
  }

  //! @brief Packed WORD Add (SSE2).
  inline void paddw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDW, &dst, &src);
  }
  //! @brief Packed WORD Add (SSE2).
  inline void paddw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDW, &dst, &src);
  }

  //! @brief Packed DWORD Add (SSE2).
  inline void paddd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDD, &dst, &src);
  }
  //! @brief Packed DWORD Add (SSE2).
  inline void paddd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDD, &dst, &src);
  }

  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PADDQ, &dst, &src);
  }
  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDQ, &dst, &src);
  }

  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDQ, &dst, &src);
  }
  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDQ, &dst, &src);
  }

  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDSB, &dst, &src);
  }
  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDSB, &dst, &src);
  }

  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDSW, &dst, &src);
  }
  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDSW, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDUSB, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDUSB, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PADDUSW, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PADDUSW, &dst, &src);
  }

  //! @brief Logical AND (SSE2).
  inline void pand(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PAND, &dst, &src);
  }
  //! @brief Logical AND (SSE2).
  inline void pand(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PAND, &dst, &src);
  }

  //! @brief Logical AND Not (SSE2).
  inline void pandn(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PANDN, &dst, &src);
  }
  //! @brief Logical AND Not (SSE2).
  inline void pandn(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PANDN, &dst, &src);
  }

  //! @brief Spin Loop Hint (SSE2).
  inline void pause()
  {
    _emitInstruction(INST_PAUSE);
  }

  //! @brief Packed Average (SSE2).
  inline void pavgb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PAVGB, &dst, &src);
  }
  //! @brief Packed Average (SSE2).
  inline void pavgb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PAVGB, &dst, &src);
  }

  //! @brief Packed Average (SSE2).
  inline void pavgw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PAVGW, &dst, &src);
  }
  //! @brief Packed Average (SSE2).
  inline void pavgw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PAVGW, &dst, &src);
  }

  //! @brief Packed Compare for Equal (BYTES) (SSE2).
  inline void pcmpeqb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPEQB, &dst, &src);
  }
  //! @brief Packed Compare for Equal (BYTES) (SSE2).
  inline void pcmpeqb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQB, &dst, &src);
  }

  //! @brief Packed Compare for Equal (WORDS) (SSE2).
  inline void pcmpeqw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPEQW, &dst, &src);
  }
  //! @brief Packed Compare for Equal (WORDS) (SSE2).
  inline void pcmpeqw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQW, &dst, &src);
  }

  //! @brief Packed Compare for Equal (DWORDS) (SSE2).
  inline void pcmpeqd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPEQD, &dst, &src);
  }
  //! @brief Packed Compare for Equal (DWORDS) (SSE2).
  inline void pcmpeqd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQD, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (BYTES) (SSE2).
  inline void pcmpgtb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPGTB, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (BYTES) (SSE2).
  inline void pcmpgtb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTB, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (WORDS) (SSE2).
  inline void pcmpgtw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPGTW, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (WORDS) (SSE2).
  inline void pcmpgtw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTW, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (DWORDS) (SSE2).
  inline void pcmpgtd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPGTD, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (DWORDS) (SSE2).
  inline void pcmpgtd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTD, &dst, &src);
  }

  //! @brief Packed Signed Integer Word Maximum (SSE2).
  inline void pmaxsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMAXSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Maximum (SSE2).
  inline void pmaxsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Maximum (SSE2).
  inline void pmaxub(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMAXUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Maximum (SSE2).
  inline void pmaxub(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXUB, &dst, &src);
  }

  //! @brief Packed Signed Integer Word Minimum (SSE2).
  inline void pminsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMINSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Minimum (SSE2).
  inline void pminsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Minimum (SSE2).
  inline void pminub(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMINUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Minimum (SSE2).
  inline void pminub(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINUB, &dst, &src);
  }

  //! @brief Move Byte Mask (SSE2).
  inline void pmovmskb(const GPVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVMSKB, &dst, &src);
  }

  //! @brief Packed Multiply High (SSE2).
  inline void pmulhw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULHW, &dst, &src);
  }
  //! @brief Packed Multiply High (SSE2).
  inline void pmulhw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULHW, &dst, &src);
  }

  //! @brief Packed Multiply High Unsigned (SSE2).
  inline void pmulhuw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULHUW, &dst, &src);
  }
  //! @brief Packed Multiply High Unsigned (SSE2).
  inline void pmulhuw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULHUW, &dst, &src);
  }

  //! @brief Packed Multiply Low (SSE2).
  inline void pmullw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULLW, &dst, &src);
  }
  //! @brief Packed Multiply Low (SSE2).
  inline void pmullw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULLW, &dst, &src);
  }

  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMULUDQ, &dst, &src);
  }
  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULUDQ, &dst, &src);
  }

  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULUDQ, &dst, &src);
  }
  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULUDQ, &dst, &src);
  }

  //! @brief Bitwise Logical OR (SSE2).
  inline void por(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_POR, &dst, &src);
  }
  //! @brief Bitwise Logical OR (SSE2).
  inline void por(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_POR, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslld(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslld(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslld(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLD, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllq(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLQ, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllw(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLW, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslldq(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSLLDQ, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psrad(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psrad(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psrad(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRAD, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psraw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psraw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psraw(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRAW, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBB, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBB, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBW, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBW, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBD, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBD, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubq(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSUBQ, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubq(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBQ, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBQ, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBQ, &dst, &src);
  }

  //! @brief Packed Multiply and Add (SSE2).
  inline void pmaddwd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMADDWD, &dst, &src);
  }
  //! @brief Packed Multiply and Add (SSE2).
  inline void pmaddwd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMADDWD, &dst, &src);
  }

  //! @brief Shuffle Packed DWORDs (SSE2).
  inline void pshufd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFD, &dst, &src, &imm8);
  }
  //! @brief Shuffle Packed DWORDs (SSE2).
  inline void pshufd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFD, &dst, &src, &imm8);
  }

  //! @brief Shuffle Packed High Words (SSE2).
  inline void pshufhw(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFHW, &dst, &src, &imm8);
  }
  //! @brief Shuffle Packed High Words (SSE2).
  inline void pshufhw(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFHW, &dst, &src, &imm8);
  }

  //! @brief Shuffle Packed Low Words (SSE2).
  inline void pshuflw(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFLW, &dst, &src, &imm8);
  }
  //! @brief Shuffle Packed Low Words (SSE2).
  inline void pshuflw(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PSHUFLW, &dst, &src, &imm8);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrld(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrld(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrld(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLD, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlq(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLQ, &dst, &src);
  }

  //! @brief DQWord Shift Right Logical (MMX).
  inline void psrldq(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLDQ, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlw(const XMMVar& dst, const Imm& src)
  {
    _emitInstruction(INST_PSRLW, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBSB, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBSB, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBSW, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBSW, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBUSB, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBUSB, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSUBUSW, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSUBUSW, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhbw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKHBW, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhbw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHBW, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhwd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKHWD, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhwd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHWD, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhdq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKHDQ, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhdq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHDQ, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhqdq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKHQDQ, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhqdq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKHQDQ, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpcklbw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKLBW, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpcklbw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLBW, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpcklwd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKLWD, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpcklwd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLWD, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpckldq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKLDQ, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpckldq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLDQ, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpcklqdq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PUNPCKLQDQ, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpcklqdq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PUNPCKLQDQ, &dst, &src);
  }

  //! @brief Bitwise Exclusive OR (SSE2).
  inline void pxor(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PXOR, &dst, &src);
  }
  //! @brief Bitwise Exclusive OR (SSE2).
  inline void pxor(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PXOR, &dst, &src);
  }

  //! @brief Shuffle DP-FP (SSE2).
  inline void shufpd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_SHUFPD, &dst, &src, &imm8);
  }
  //! @brief Shuffle DP-FP (SSE2).
  inline void shufpd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_SHUFPD, &dst, &src, &imm8);
  }

  //! @brief Compute Square Roots of Packed DP-FP Values (SSE2).
  inline void sqrtpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SQRTPD, &dst, &src);
  }
  //! @brief Compute Square Roots of Packed DP-FP Values (SSE2).
  inline void sqrtpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SQRTPD, &dst, &src);
  }

  //! @brief Compute Square Root of Scalar DP-FP Value (SSE2).
  inline void sqrtsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SQRTSD, &dst, &src);
  }
  //! @brief Compute Square Root of Scalar DP-FP Value (SSE2).
  inline void sqrtsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SQRTSD, &dst, &src);
  }

  //! @brief Packed DP-FP Subtract (SSE2).
  inline void subpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SUBPD, &dst, &src);
  }
  //! @brief Packed DP-FP Subtract (SSE2).
  inline void subpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SUBPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Subtract (SSE2).
  inline void subsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_SUBSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Subtract (SSE2).
  inline void subsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_SUBSD, &dst, &src);
  }

  //! @brief Scalar Unordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void ucomisd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_UCOMISD, &dst, &src);
  }
  //! @brief Scalar Unordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void ucomisd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_UCOMISD, &dst, &src);
  }

  //! @brief Unpack and Interleave High Packed Double-Precision FP Values (SSE2).
  inline void unpckhpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_UNPCKHPD, &dst, &src);
  }
  //! @brief Unpack and Interleave High Packed Double-Precision FP Values (SSE2).
  inline void unpckhpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_UNPCKHPD, &dst, &src);
  }

  //! @brief Unpack and Interleave Low Packed Double-Precision FP Values (SSE2).
  inline void unpcklpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_UNPCKLPD, &dst, &src);
  }
  //! @brief Unpack and Interleave Low Packed Double-Precision FP Values (SSE2).
  inline void unpcklpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_UNPCKLPD, &dst, &src);
  }

  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void xorpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_XORPD, &dst, &src);
  }
  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void xorpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_XORPD, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [SSE3]
  // --------------------------------------------------------------------------

  //! @brief Packed DP-FP Add/Subtract (SSE3).
  inline void addsubpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ADDSUBPD, &dst, &src);
  }
  //! @brief Packed DP-FP Add/Subtract (SSE3).
  inline void addsubpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADDSUBPD, &dst, &src);
  }

  //! @brief Packed SP-FP Add/Subtract (SSE3).
  inline void addsubps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_ADDSUBPS, &dst, &src);
  }
  //! @brief Packed SP-FP Add/Subtract (SSE3).
  inline void addsubps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_ADDSUBPS, &dst, &src);
  }

#if 0
  // TODO: NOT IMPLEMENTED BY THE COMPILER.
  //! @brief Store Integer with Truncation (SSE3).
  inline void fisttp(const Mem& dst)
  {
    _emitInstruction(INST_FISTTP, &dst);
  }
#endif

  //! @brief Packed DP-FP Horizontal Add (SSE3).
  inline void haddpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_HADDPD, &dst, &src);
  }
  //! @brief Packed DP-FP Horizontal Add (SSE3).
  inline void haddpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_HADDPD, &dst, &src);
  }

  //! @brief Packed SP-FP Horizontal Add (SSE3).
  inline void haddps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_HADDPS, &dst, &src);
  }
  //! @brief Packed SP-FP Horizontal Add (SSE3).
  inline void haddps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_HADDPS, &dst, &src);
  }

  //! @brief Packed DP-FP Horizontal Subtract (SSE3).
  inline void hsubpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_HSUBPD, &dst, &src);
  }
  //! @brief Packed DP-FP Horizontal Subtract (SSE3).
  inline void hsubpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_HSUBPD, &dst, &src);
  }

  //! @brief Packed SP-FP Horizontal Subtract (SSE3).
  inline void hsubps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_HSUBPS, &dst, &src);
  }
  //! @brief Packed SP-FP Horizontal Subtract (SSE3).
  inline void hsubps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_HSUBPS, &dst, &src);
  }

  //! @brief Load Unaligned Integer 128 Bits (SSE3).
  inline void lddqu(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_LDDQU, &dst, &src);
  }

  //! @brief Set Up Monitor Address (SSE3).
  inline void monitor()
  {
    _emitInstruction(INST_MONITOR);
  }

  //! @brief Move One DP-FP and Duplicate (SSE3).
  inline void movddup(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVDDUP, &dst, &src);
  }
  //! @brief Move One DP-FP and Duplicate (SSE3).
  inline void movddup(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVDDUP, &dst, &src);
  }

  //! @brief Move Packed SP-FP High and Duplicate (SSE3).
  inline void movshdup(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVSHDUP, &dst, &src);
  }
  //! @brief Move Packed SP-FP High and Duplicate (SSE3).
  inline void movshdup(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVSHDUP, &dst, &src);
  }

  //! @brief Move Packed SP-FP Low and Duplicate (SSE3).
  inline void movsldup(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_MOVSLDUP, &dst, &src);
  }
  //! @brief Move Packed SP-FP Low and Duplicate (SSE3).
  inline void movsldup(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVSLDUP, &dst, &src);
  }

  //! @brief Monitor Wait (SSE3).
  inline void mwait()
  {
    _emitInstruction(INST_MWAIT);
  }

  // --------------------------------------------------------------------------
  // [SSSE3]
  // --------------------------------------------------------------------------

  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSIGNB, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSIGNB, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSIGNB, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSIGNB, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSIGNW, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSIGNW, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSIGNW, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSIGNW, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSIGND, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSIGND, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSIGND, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSIGND, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PHADDW, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHADDW, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHADDW, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHADDW, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PHADDD, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHADDD, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHADDD, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHADDD, &dst, &src);
  }

  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PHADDSW, &dst, &src);
  }
  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHADDSW, &dst, &src);
  }

  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHADDSW, &dst, &src);
  }
  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHADDSW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PHSUBW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHSUBW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHSUBW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHSUBW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PHSUBD, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHSUBD, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHSUBD, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHSUBD, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PHSUBSW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHSUBSW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHSUBSW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHSUBSW, &dst, &src);
  }

  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMADDUBSW, &dst, &src);
  }
  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMADDUBSW, &dst, &src);
  }

  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMADDUBSW, &dst, &src);
  }
  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMADDUBSW, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PABSB, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PABSB, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PABSB, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PABSB, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PABSW, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PABSW, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PABSW, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PABSW, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PABSD, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PABSD, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PABSD, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PABSD, &dst, &src);
  }

  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PMULHRSW, &dst, &src);
  }
  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULHRSW, &dst, &src);
  }

  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULHRSW, &dst, &src);
  }
  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULHRSW, &dst, &src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const MMVar& dst, const MMVar& src)
  {
    _emitInstruction(INST_PSHUFB, &dst, &src);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const MMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSHUFB, &dst, &src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PSHUFB, &dst, &src);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PSHUFB, &dst, &src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const MMVar& dst, const MMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PALIGNR, &dst, &src, &imm8);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const MMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PALIGNR, &dst, &src, &imm8);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PALIGNR, &dst, &src, &imm8);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PALIGNR, &dst, &src, &imm8);
  }

  // --------------------------------------------------------------------------
  // [SSE4.1]
  // --------------------------------------------------------------------------

  //! @brief Blend Packed DP-FP Values (SSE4.1).
  inline void blendpd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_BLENDPD, &dst, &src, &imm8);
  }
  //! @brief Blend Packed DP-FP Values (SSE4.1).
  inline void blendpd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_BLENDPD, &dst, &src, &imm8);
  }

  //! @brief Blend Packed SP-FP Values (SSE4.1).
  inline void blendps(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_BLENDPS, &dst, &src, &imm8);
  }
  //! @brief Blend Packed SP-FP Values (SSE4.1).
  inline void blendps(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_BLENDPS, &dst, &src, &imm8);
  }

  //! @brief Variable Blend Packed DP-FP Values (SSE4.1).
  inline void blendvpd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_BLENDVPD, &dst, &src);
  }
  //! @brief Variable Blend Packed DP-FP Values (SSE4.1).
  inline void blendvpd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_BLENDVPD, &dst, &src);
  }

  //! @brief Variable Blend Packed SP-FP Values (SSE4.1).
  inline void blendvps(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_BLENDVPS, &dst, &src);
  }
  //! @brief Variable Blend Packed SP-FP Values (SSE4.1).
  inline void blendvps(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_BLENDVPS, &dst, &src);
  }

  //! @brief Dot Product of Packed DP-FP Values (SSE4.1).
  inline void dppd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_DPPD, &dst, &src, &imm8);
  }
  //! @brief Dot Product of Packed DP-FP Values (SSE4.1).
  inline void dppd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_DPPD, &dst, &src, &imm8);
  }

  //! @brief Dot Product of Packed SP-FP Values (SSE4.1).
  inline void dpps(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_DPPS, &dst, &src, &imm8);
  }
  //! @brief Dot Product of Packed SP-FP Values (SSE4.1).
  inline void dpps(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_DPPS, &dst, &src, &imm8);
  }

  //! @brief Extract Packed SP-FP Value (SSE4.1).
  inline void extractps(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_EXTRACTPS, &dst, &src, &imm8);
  }
  //! @brief Extract Packed SP-FP Value (SSE4.1).
  inline void extractps(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_EXTRACTPS, &dst, &src, &imm8);
  }

  //! @brief Load Double Quadword Non-Temporal Aligned Hint (SSE4.1).
  inline void movntdqa(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_MOVNTDQA, &dst, &src);
  }

  //! @brief Compute Multiple Packed Sums of Absolute Difference (SSE4.1).
  inline void mpsadbw(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_MPSADBW, &dst, &src, &imm8);
  }
  //! @brief Compute Multiple Packed Sums of Absolute Difference (SSE4.1).
  inline void mpsadbw(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_MPSADBW, &dst, &src, &imm8);
  }

  //! @brief Pack with Unsigned Saturation (SSE4.1).
  inline void packusdw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PACKUSDW, &dst, &src);
  }
  //! @brief Pack with Unsigned Saturation (SSE4.1).
  inline void packusdw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PACKUSDW, &dst, &src);
  }

  //! @brief Variable Blend Packed Bytes (SSE4.1).
  inline void pblendvb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PBLENDVB, &dst, &src);
  }
  //! @brief Variable Blend Packed Bytes (SSE4.1).
  inline void pblendvb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PBLENDVB, &dst, &src);
  }

  //! @brief Blend Packed Words (SSE4.1).
  inline void pblendw(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PBLENDW, &dst, &src, &imm8);
  }
  //! @brief Blend Packed Words (SSE4.1).
  inline void pblendw(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PBLENDW, &dst, &src, &imm8);
  }

  //! @brief Compare Packed Qword Data for Equal (SSE4.1).
  inline void pcmpeqq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPEQQ, &dst, &src);
  }
  //! @brief Compare Packed Qword Data for Equal (SSE4.1).
  inline void pcmpeqq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPEQQ, &dst, &src);
  }

  //! @brief Extract Byte (SSE4.1).
  inline void pextrb(const GPVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRB, &dst, &src, &imm8);
  }
  //! @brief Extract Byte (SSE4.1).
  inline void pextrb(const Mem& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRB, &dst, &src, &imm8);
  }

  //! @brief Extract Dword (SSE4.1).
  inline void pextrd(const GPVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRD, &dst, &src, &imm8);
  }
  //! @brief Extract Dword (SSE4.1).
  inline void pextrd(const Mem& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRD, &dst, &src, &imm8);
  }

  //! @brief Extract Dword (SSE4.1).
  inline void pextrq(const GPVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRQ, &dst, &src, &imm8);
  }
  //! @brief Extract Dword (SSE4.1).
  inline void pextrq(const Mem& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRQ, &dst, &src, &imm8);
  }

  //! @brief Extract Word (SSE4.1).
  inline void pextrw(const GPVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRW, &dst, &src, &imm8);
  }
  //! @brief Extract Word (SSE4.1).
  inline void pextrw(const Mem& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PEXTRW, &dst, &src, &imm8);
  }

  //! @brief Packed Horizontal Word Minimum (SSE4.1).
  inline void phminposuw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PHMINPOSUW, &dst, &src);
  }
  //! @brief Packed Horizontal Word Minimum (SSE4.1).
  inline void phminposuw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PHMINPOSUW, &dst, &src);
  }

  //! @brief Insert Byte (SSE4.1).
  inline void pinsrb(const XMMVar& dst, const GPVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRB, &dst, &src, &imm8);
  }
  //! @brief Insert Byte (SSE4.1).
  inline void pinsrb(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRB, &dst, &src, &imm8);
  }

  //! @brief Insert Dword (SSE4.1).
  inline void pinsrd(const XMMVar& dst, const GPVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRD, &dst, &src, &imm8);
  }
  //! @brief Insert Dword (SSE4.1).
  inline void pinsrd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRD, &dst, &src, &imm8);
  }

  //! @brief Insert Dword (SSE4.1).
  inline void pinsrq(const XMMVar& dst, const GPVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRQ, &dst, &src, &imm8);
  }
  //! @brief Insert Dword (SSE4.1).
  inline void pinsrq(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRQ, &dst, &src, &imm8);
  }

  //! @brief Insert Word (SSE2).
  inline void pinsrw(const XMMVar& dst, const GPVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRW, &dst, &src, &imm8);
  }
  //! @brief Insert Word (SSE2).
  inline void pinsrw(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PINSRW, &dst, &src, &imm8);
  }

  //! @brief Maximum of Packed Word Integers (SSE4.1).
  inline void pmaxuw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMAXUW, &dst, &src);
  }
  //! @brief Maximum of Packed Word Integers (SSE4.1).
  inline void pmaxuw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXUW, &dst, &src);
  }

  //! @brief Maximum of Packed Signed Byte Integers (SSE4.1).
  inline void pmaxsb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMAXSB, &dst, &src);
  }
  //! @brief Maximum of Packed Signed Byte Integers (SSE4.1).
  inline void pmaxsb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXSB, &dst, &src);
  }

  //! @brief Maximum of Packed Signed Dword Integers (SSE4.1).
  inline void pmaxsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMAXSD, &dst, &src);
  }
  //! @brief Maximum of Packed Signed Dword Integers (SSE4.1).
  inline void pmaxsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXSD, &dst, &src);
  }

  //! @brief Maximum of Packed Unsigned Dword Integers (SSE4.1).
  inline void pmaxud(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMAXUD, &dst, &src);
  }
  //! @brief Maximum of Packed Unsigned Dword Integers (SSE4.1).
  inline void pmaxud(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMAXUD, &dst, &src);
  }

  //! @brief Minimum of Packed Signed Byte Integers (SSE4.1).
  inline void pminsb(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMINSB, &dst, &src);
  }
  //! @brief Minimum of Packed Signed Byte Integers (SSE4.1).
  inline void pminsb(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINSB, &dst, &src);
  }

  //! @brief Minimum of Packed Word Integers (SSE4.1).
  inline void pminuw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMINUW, &dst, &src);
  }
  //! @brief Minimum of Packed Word Integers (SSE4.1).
  inline void pminuw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINUW, &dst, &src);
  }

  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminud(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMINUD, &dst, &src);
  }
  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminud(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINUD, &dst, &src);
  }

  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminsd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMINSD, &dst, &src);
  }
  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminsd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMINSD, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVSXBW, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVSXBW, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVSXBD, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVSXBD, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVSXBQ, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVSXBQ, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxwd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVSXWD, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxwd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVSXWD, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovsxwq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVSXWQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovsxwq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVSXWQ, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovsxdq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVSXDQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovsxdq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVSXDQ, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbw(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVZXBW, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbw(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVZXBW, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVZXBD, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVZXBD, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVZXBQ, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVZXBQ, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxwd(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVZXWD, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxwd(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVZXWD, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovzxwq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVZXWQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovzxwq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVZXWQ, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovzxdq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMOVZXDQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovzxdq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMOVZXDQ, &dst, &src);
  }

  //! @brief Multiply Packed Signed Dword Integers (SSE4.1).
  inline void pmuldq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULDQ, &dst, &src);
  }
  //! @brief Multiply Packed Signed Dword Integers (SSE4.1).
  inline void pmuldq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULDQ, &dst, &src);
  }

  //! @brief Multiply Packed Signed Integers and Store Low Result (SSE4.1).
  inline void pmulld(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PMULLD, &dst, &src);
  }
  //! @brief Multiply Packed Signed Integers and Store Low Result (SSE4.1).
  inline void pmulld(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PMULLD, &dst, &src);
  }

  //! @brief Logical Compare (SSE4.1).
  inline void ptest(const XMMVar& op1, const XMMVar& op2)
  {
    _emitInstruction(INST_PTEST, &op1, &op2);
  }
  //! @brief Logical Compare (SSE4.1).
  inline void ptest(const XMMVar& op1, const Mem& op2)
  {
    _emitInstruction(INST_PTEST, &op1, &op2);
  }

  //! Round Packed SP-FP Values @brief (SSE4.1).
  inline void roundps(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDPS, &dst, &src, &imm8);
  }
  //! Round Packed SP-FP Values @brief (SSE4.1).
  inline void roundps(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDPS, &dst, &src, &imm8);
  }

  //! @brief Round Scalar SP-FP Values (SSE4.1).
  inline void roundss(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDSS, &dst, &src, &imm8);
  }
  //! @brief Round Scalar SP-FP Values (SSE4.1).
  inline void roundss(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDSS, &dst, &src, &imm8);
  }

  //! @brief Round Packed DP-FP Values (SSE4.1).
  inline void roundpd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDPD, &dst, &src, &imm8);
  }
  //! @brief Round Packed DP-FP Values (SSE4.1).
  inline void roundpd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDPD, &dst, &src, &imm8);
  }

  //! @brief Round Scalar DP-FP Values (SSE4.1).
  inline void roundsd(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDSD, &dst, &src, &imm8);
  }
  //! @brief Round Scalar DP-FP Values (SSE4.1).
  inline void roundsd(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_ROUNDSD, &dst, &src, &imm8);
  }

  // --------------------------------------------------------------------------
  // [SSE4.2]
  // --------------------------------------------------------------------------

  //! @brief Accumulate CRC32 Value (polynomial 0x11EDC6F41) (SSE4.2).
  inline void crc32(const GPVar& dst, const GPVar& src)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_TYPE_GPD) || dst.isRegType(REG_TYPE_GPQ));
    _emitInstruction(INST_CRC32, &dst, &src);
  }
  //! @brief Accumulate CRC32 Value (polynomial 0x11EDC6F41) (SSE4.2).
  inline void crc32(const GPVar& dst, const Mem& src)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_TYPE_GPD) || dst.isRegType(REG_TYPE_GPQ));
    _emitInstruction(INST_CRC32, &dst, &src);
  }

  //! @brief Packed Compare Explicit Length Strings, Return Index (SSE4.2).
  inline void pcmpestri(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPESTRI, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Explicit Length Strings, Return Index (SSE4.2).
  inline void pcmpestri(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPESTRI, &dst, &src, &imm8);
  }

  //! @brief Packed Compare Explicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpestrm(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPESTRM, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Explicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpestrm(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPESTRM, &dst, &src, &imm8);
  }

  //! @brief Packed Compare Implicit Length Strings, Return Index (SSE4.2).
  inline void pcmpistri(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPISTRI, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Implicit Length Strings, Return Index (SSE4.2).
  inline void pcmpistri(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPISTRI, &dst, &src, &imm8);
  }

  //! @brief Packed Compare Implicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpistrm(const XMMVar& dst, const XMMVar& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPISTRM, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Implicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpistrm(const XMMVar& dst, const Mem& src, const Imm& imm8)
  {
    _emitInstruction(INST_PCMPISTRM, &dst, &src, &imm8);
  }

  //! @brief Compare Packed Data for Greater Than (SSE4.2).
  inline void pcmpgtq(const XMMVar& dst, const XMMVar& src)
  {
    _emitInstruction(INST_PCMPGTQ, &dst, &src);
  }
  //! @brief Compare Packed Data for Greater Than (SSE4.2).
  inline void pcmpgtq(const XMMVar& dst, const Mem& src)
  {
    _emitInstruction(INST_PCMPGTQ, &dst, &src);
  }

  //! @brief Return the Count of Number of Bits Set to 1 (SSE4.2).
  inline void popcnt(const GPVar& dst, const GPVar& src)
  {
    // ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    // ASMJIT_ASSERT(src.getRegType() == dst.getRegType());
    _emitInstruction(INST_POPCNT, &dst, &src);
  }
  //! @brief Return the Count of Number of Bits Set to 1 (SSE4.2).
  inline void popcnt(const GPVar& dst, const Mem& src)
  {
    // ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    _emitInstruction(INST_POPCNT, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [AMD only]
  // --------------------------------------------------------------------------

  //! @brief Prefetch (3dNow - Amd).
  //!
  //! Loads the entire 64-byte aligned memory sequence containing the
  //! specified memory address into the L1 data cache. The position of
  //! the specified memory address within the 64-byte cache line is
  //! irrelevant. If a cache hit occurs, or if a memory fault is detected,
  //! no bus cycle is initiated and the instruction is treated as a NOP.
  inline void amd_prefetch(const Mem& mem)
  {
    _emitInstruction(INST_AMD_PREFETCH, &mem);
  }

  //! @brief Prefetch and set cache to modified (3dNow - Amd).
  //!
  //! The PREFETCHW instruction loads the prefetched line and sets the
  //! cache-line state to Modified, in anticipation of subsequent data
  //! writes to the line. The PREFETCH instruction, by contrast, typically
  //! sets the cache-line state to Exclusive (depending on the hardware
  //! implementation).
  inline void amd_prefetchw(const Mem& mem)
  {
    _emitInstruction(INST_AMD_PREFETCHW, &mem);
  }

  // --------------------------------------------------------------------------
  // [Intel only]
  // --------------------------------------------------------------------------

  //! @brief Move Data After Swapping Bytes (SSE3 - Intel Atom).
  inline void movbe(const GPVar& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isGPB());
    _emitInstruction(INST_MOVBE, &dst, &src);
  }

  //! @brief Move Data After Swapping Bytes (SSE3 - Intel Atom).
  inline void movbe(const Mem& dst, const GPVar& src)
  {
    ASMJIT_ASSERT(!src.isGPB());
    _emitInstruction(INST_MOVBE, &dst, &src);
  }
};

// ============================================================================
// [AsmJit::Compiler]
// ============================================================================

//! @brief Compiler - high level code generation.
//!
//! This class is used to store instruction stream and allows to modify
//! it on the fly. It uses different concept than @c AsmJit::Assembler class
//! and in fact @c AsmJit::Assembler is only used as a backend. Compiler never
//! emits machine code and each instruction you use is stored to instruction
//! array instead. This allows to modify instruction stream later and for 
//! example to reorder instructions to make better performance.
//!
//! Using @c AsmJit::Compiler moves code generation to higher level. Higher 
//! level constructs allows to write more abstract and extensible code that
//! is not possible with pure @c AsmJit::Assembler class. Because 
//! @c AsmJit::Compiler needs to create many objects and lifetime of these 
//! objects is small (same as @c AsmJit::Compiler lifetime itself) it uses 
//! very fast memory management model. This model allows to create object 
//! instances in nearly zero time (compared to @c malloc() or @c new() 
//! operators) so overhead by creating machine code by @c AsmJit::Compiler
//! is minimized.
//!
//! <b>Code Generation</b>
//! 
//! First that is needed to know about compiler is that compiler never emits
//! machine code. It's used as a middleware between @c AsmJit::Assembler and
//! your code. There is also convenience method @c make() that allows to
//! generate machine code directly without creating @c AsmJit::Assembler
//! instance.
//!
//! Example how to generate machine code using @c Assembler and @c Compiler:
//! 
//! @code
//! // Assembler instance is low level code generation class that emits 
//! // machine code.
//! Assembler a;
//!
//! // Compiler instance is high level code generation class that stores all
//! // instructions in internal representation.
//! Compiler c;
//!
//! // ... put your code using Compiler instance ...
//!
//! // Final step - generate code. AsmJit::Compiler::serialize() will serialize
//! // all instructions into Assembler and this ensures generating real machine
//! // code.
//! c.serialize(a);
//!
//! // Your function
//! void* fn = a.make();
//! @endcode
//!
//! Example how to generate machine code using only @c Compiler (preferred):
//!
//! @code
//! // Compiler instance is enough.
//! Compiler c;
//!
//! // ... put your code using Compiler instance ...
//!
//! // Your function
//! void* fn = c.make();
//! @endcode
//!
//! You can see that there is @c AsmJit::Compiler::serialize() function that
//! emits instructions into @c AsmJit::Assembler(). This layered architecture
//! means that each class is used for something different and there is no code
//! duplication. For convenience there is also @c AsmJit::Compiler::make()
//! method that can create your function using @c AsmJit::Assembler, but 
//! internally (this is preffered bahavior when using @c AsmJit::Compiler).
//!
//! @c make() allocates memory using global memory manager instance, if your
//! function lifetime is over, you should free that memory by
//! @c AsmJit::MemoryManager::free() method.
//!
//! @code
//! // Compiler instance is enough.
//! Compiler c;
//!
//! // ... put your code using Compiler instance ...
//!
//! // Your function
//! void* fn = c.make();
//!
//! // Free it if you don't want it anymore
//! // (using global memory manager instance)
//! MemoryManager::getGlobal()->free(fn);
//! @endcode
//!
//! <b>Functions</b>
//!
//! To build functions with @c Compiler, see @c AsmJit::Compiler::newFunction()
//! method.
//!
//! <b>Variables</b>
//!
//! TODO: OBSOLETE.
//!
//! Compiler also manages your variables and function arguments. Using manual
//! register allocation is not recommended way and it must be done carefully.
//! See @c AsmJit::VariableRef and related classes how to work with variables
//! and next example how to use AsmJit API to create function and manage them:
//!
//! @code
//! // Compiler and function declaration - void f(int*);
//! Compiler c;
//! Function& f = *c.newFunction(CALL_CONV_DEFAULT, BuildFunction1<int*>());
//!
//! // Get argument variable (it's pointer)
//! PtrRef a1(f.argument(0));
//!
//! // Create your variables
//! Int32Ref x1(f.newVariable(VARIABLE_TYPE_INT32));
//! Int32Ref x2(f.newVariable(VARIABLE_TYPE_INT32));
//!
//! // Init your variables
//! c.mov(x1.r(), 1);
//! c.mov(x2.r(), 2);
//!
//! // ... your code ...
//! c.add(x1.r(), x2.r());
//! // ... your code ...
//!
//! // Store result to a given pointer in first argument
//! c.mov(dword_ptr(a1.c()), x1.c());
//!
//! // Make function
//! typedef void (*MyFn)(int*);
//! MyFn fn = function_cast<MyFn>(c.make());
//! @endcode
//!
//! There was presented small code snippet with variables, but it's needed to 
//! explain it more. You can see that there are more variable types that can 
//! be used. Most useful variables that can be allocated to general purpose 
//! registers are variables wrapped to @c Int32Ref, @c Int64Ref, @c SysIntRef 
//! and @c PtrRef. Only @c Int64Ref is limited to 64 bit architecture. 
//! @c SysIntRef and @c PtrRef variables are equal and it's size depends to 
//! architecture (32 or 64 bits).
//!
//! Compiler is not using variables directly, instead you need to create the
//! function and create variables through @c AsmJit::Function. In code you will
//! always work with @c AsmJit::Compiler and @c AsmJit::Function together.
//!
//! Each variable contains state that describes where it is currently allocated
//! and if it's used. Life of variables is based on reference counting and if
//! variable is dereferenced to zero its life ends.
//!
//! Variable states:
//!
//! - Unused (@c AsmJit::VARIABLE_STATE_UNUSED) - State that is assigned to
//!   newly created variables or to not used variables (dereferenced to zero).
//! - In register (@c AsmJit::VARIABLE_STATE_REGISTER) - State that means that
//!   variable is currently allocated in register.
//! - In memory (@c AsmJit::VARIABLE_STATE_MEMORY) - State that means that
//!   variable is currently only in memory location.
//! 
//! When you create new variable, its state is always @c VARIABLE_STATE_UNUSED,
//! allocating it to register or spilling to memory changes this state to 
//! @c VARIABLE_STATE_REGISTER or @c VARIABLE_STATE_MEMORY, respectively. 
//! During variable lifetime it's usual that its state is changed multiple
//! times. To generate better code, you can control allocating and spilling
//! by using up to four types of methods that allows it (see next list).
//!
//! Explicit variable allocating / spilling methods:
//!
//! - @c VariableRef::alloc() - Explicit method to alloc variable into 
//!      register. You can use this before loops or code blocks.
//!
//! - @c VariableRef::spill() - Explicit method to spill variable. If variable
//!      is in register and you call this method, it's moved to its home memory
//!      location. If variable is not in register no operation is performed.
//!
//! Implicit variable allocating / spilling methods:
//!
//! - @c VariableRef::r() - Method used to allocate (if it's not previously
//!      allocated) variable to register for read / write. In most cases
//!      this is the right method to use in your code. If variable is in
//!      memory and you use this method it's allocated and moved to register.
//!      If variable is already in register or it's marked as unused this 
//!      method does nothing.
//! 
//! - @c VariableRef::x() - Method used to allocate variable for write only.
//!      In AsmJit this means completely overwrite it without using it's value.
//!      This method is helpful when you want to prevent from copying variable
//!      from memory to register to save one mov() instruction. If you want
//!      to clear or set your variable to something it's recommended to use
//!      @c VariableRef::x().
//!
//! - @c VariableRef::c() - Method used to use variable as a constant. 
//!      Constants means that you will not change that variable or you don't
//!      want to mark variable as changed. If variable is not marked as changed
//!      and spill happens you will save one mov() instruction that is needed
//!      to copy variable from register to its home address.
//!
//! - @c VariableRef::m() - Method used to access variable memory address. If
//!      variable is allocated in register and you call this method, it's 
//!      spilled, in all other cases it does nothing.
//!
//! Next example shows how allocating and spilling works:
//!
//! @code
//! // Small example to show how variable allocating and spilling works
//!
//! // Your compiler
//! Compiler c;
//!
//! // Your variable
//! Int32Ref var = ...;
//! 
//! // Make sure variable is spilled
//! var.spill();
//! 
//! // 1. Example: using var.r()
//! c.mov(var.r(), imm(0));
//! var.spill();
//! // Generated code:
//! //    mov var.reg, [var.home]
//! //    mov var.reg, 0
//! //    mov [var.home], var.reg
//! 
//! // 2. Example: using var.x()
//! c.mov(var.x(), imm(0));
//! var.spill();
//! // Generated code:
//! //    --- no alloc, .x() inhibits it.
//! //    mov var.reg, 0
//! //    mov [var.home], var.reg
//! 
//! // 3. Example: using var.c()
//! c.mov(var.c(), imm(0));
//! var.spill();
//! // Generated code:
//! //    mov var.reg, [var.home]
//! //    mov var.reg, 0
//! //    --- no spill, .c() means that you are not changing it, it's 'c'onstant
//! 
//! // 4. Example: using var.m()
//! c.mov(var.m(), imm(0));
//! var.spill();
//! // Generated code:
//! //    --- no alloc, because we are not allocating it
//! //    mov [var.home], 0
//! //    --- no spill, because variable is not allocated
//!
//! // 5. Example: using var.x(), setChanged()
//! c.mov(var.x(),imm(0));
//! var.setChanged(false);
//! var.spill();
//! // Generated code:
//! //    --- no alloc, .x() inhibits it.
//! //    mov var.reg, 0
//! //    --- no spill, setChanged(false) marked variable as unmodified
//! @endcode
//!
//! Please see AsmJit tutorials (testcompiler.cpp and testvariables.cpp) for 
//! more complete examples.
//!
//! <b>Intrinsics Extensions</b>
//!
//! Compiler supports extensions to intrinsics implemented in 
//! @c AsmJit::Serializer that enables to use variables in instructions without
//! specifying to use it as register or as memory operand. Sometimes is better
//! not to alloc variable for each read or write. There is limitation that you
//! can use variable without specifying if it's in register or in memory 
//! location only for one operand. This is because x86/x64 architecture not
//! allows to use two memory operands in one instruction and this could 
//! happen without this restriction (two variables in memory).
//!
//! @code
//! // Small example to show how intrinsics extensions works
//!
//! // Your compiler
//! Compiler c;
//!
//! // Your variable
//! Int32Ref var = ...;
//! 
//! // Make sure variable is spilled
//! var.spill();
//! 
//! // 1. Example: Allocated variable
//! var.alloc()
//! c.mov(var, imm(0));
//! var.spill();
//! // Generated code:
//! //    mov var.reg, [var.home]
//! //    mov var.reg, 0
//! //    mov [var.home], var.reg
//! 
//! // 2. Example: Memory variable
//! c.mov(var, imm(0));
//! var.spill();
//! // Generated code:
//! //    --- no alloc, we want variable in memory
//! //    mov [var.home], 0
//! //    --- no spill, becuase variable in not allocated
//! @endcode
//!
//! <b>Memory Management</b>
//!
//! @c Compiler Memory management follows these rules:
//! - Everything created by @c Compiler is always freed by @c Compiler.
//! - To get decent performance, compiler always uses larger buffer for 
//!   objects to allocate and when compiler instance is destroyed, this 
//!   buffer is freed. Destructors of active objects are called when 
//!   destroying compiler instance. Destructors of abadonded compiler
//!   objects are called immediately after abadonding it.
//!
//! This means that you can't use any @c Compiler object after destructing it,
//! it also means that each object like @c Label, @c Variable nad others are
//! created and managed by @c Compiler itself.
//!
//! <b>Compiling process details</b>
//!
//! This section is here for people interested in the compiling process. There
//! are few steps that must be done for each compiled function (or your code).
//!
//! When your @c Compiler instance is ready, you can create function and add
//! emittables using intrinsics or higher level methods implemented in the
//! @c AsmJit::Compiler. When you are done serializing instructions you will 
//! usually call @c AsmJit::Compiler::make() method to serialize all emittables 
//! to @c AsmJit::Assembler. Next steps shows what's done internally before code
//! is serialized into @c AsmJit::Assembler
//!   (implemented in @c AsmJit::Compiler::serialize() method).
//! 
//! 1. All emittables are traversed (from first to last) and method 
//!    @c AsmJit::Emittable::prepare() is called. This signalizes to all 
//!    emittables that instruction generation step is over and now they
//!    should prepare to code generation. In this step can be processed
//!    variables, states, etc...
//! 2. All emittables are traversed (from first to last) and method 
//!    @c AsmJit::Emittable::emit() is called. In this step each emittable
//!    can serialize real assembler instructions into @c AsmJit::Assembler
//!    instance. This step also generates function prolog and epilog.
//! 3. All emittables are traversed (from first to last) and method 
//!    @c AsmJit::Emittable::post() is called. Post emitting is used
//!    to embed data after function body (not only user data, but also some
//!    helper data that can help generating jumps, variables restore / save
//!    sequences, condition blocks).
//! 4. Jump tables data are emitted.
//!
//! When everything here ends, @c AsmJit::Assembler contains binary stream
//! that needs only relocation to be callable.
//!
//! <b>Differences summary to @c AsmJit::Assembler</b>
//!
//! - Instructions are not translated to machine code immediately, they are
//!   stored as @c Emmitable's (see @c AsmJit::Instruction).
//! - Each @c Label must be allocated by @c AsmJit::Compiler::newLabel().
//! - Contains function builder.
//! - Contains register allocator / variables management.
//! - Contains a lot of helper methods to simplify code generation.
struct ASMJIT_API Compiler : public CompilerIntrinsics
{
  //! @brief Create a new @c Compiler instance.
  Compiler() ASMJIT_NOTHROW;
  //! @brief Destroy @c Compiler instance.
  virtual ~Compiler() ASMJIT_NOTHROW;
};

//! @}

} // AsmJit namespace

// [Api-End]
#include "ApiEnd.h"

// [Guard]
#endif // _ASMJIT_COMPILERX86X64_H