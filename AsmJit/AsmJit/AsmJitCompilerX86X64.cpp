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

// We are using sprintf() here.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "AsmJitAssembler.h"
#include "AsmJitCompiler.h"
#include "AsmJitLogger.h"
#include "AsmJitUtil.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// VARIABLE_TYPE_INT64 not declared in 32 bit mode, in future this should 
// change
#if defined(ASMJIT_X86)
#define VARIABLE_TYPE_INT64 2
#endif // ASMJIT_X86

namespace AsmJit {

// ============================================================================
// [Helpers]
// ============================================================================

static void delAll(Compiler::EmittableList& buf)
{
  Emittable** emitters = buf.data();
  SysUInt i, len = buf.length();
  for (i = 0; i < len; i++) emitters[i]->~Emittable();
}

static void memset32(UInt32* p, UInt32 c, SysUInt len)
{
  SysUInt i;
  for (i = 0; i < len; i++) p[i] = c;
}

static bool isIntegerArgument(UInt32 arg)
{
  return 
    arg == VARIABLE_TYPE_INT32 ||
    arg == VARIABLE_TYPE_INT64 ;
}

static bool isFloatArgument(UInt32 arg)
{
  return arg == VARIABLE_TYPE_FLOAT || arg == VARIABLE_TYPE_DOUBLE;
}

static const UInt32 variableSizeData[] = {
  0,               // VARIABLE_TYPE_NONE
  4,               // VARIABLE_TYPE_INT32, VARIABLE_TYPE_UINT32
  8,               // VARIABLE_TYPE_INT64, VARIABLE_TYPE_UINT64
  4,               // VARIABLE_TYPE_FLOAT
  8,               // VARIABLE_TYPE_DOUBLE
  8,               // VARIABLE_TYPE_MM
  16               // VARIABLE_TYPE_XMM
};

static const UInt8 variableRegCodeData[] = 
{
  NO_REG,
  REG_GPD,
  REG_GPQ,
  REG_X87,
  REG_X87,
  REG_MM,
  REG_XMM
};

static UInt32 getVariableSize(UInt32 type)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableSizeData));
  return variableSizeData[type];
}

// ============================================================================
// [AsmJit::Variable]
// ============================================================================

Variable::Variable(Compiler* c, Function* f, UInt8 type) :
  _compiler(c),
  _function(f),
  _refCount(0),
  _spillCount(0),
  _reuseCount(0),
  _registerAccessCount(0),
  _memoryAccessCount(0),
  _type(type),
  _size(getVariableSize(type)),
  _state(VARIABLE_STATE_UNUSED),
  _priority(10),
  _registerCode(NO_REG),
  _preferredRegister(0xFF),
  _changed(false),
  _stackOffset(0),
  _allocFn(NULL),
  _spillFn(NULL),
  _data(NULL)
{
  ASMJIT_ASSERT(f != NULL);

  _memoryOperand = new(c->_zoneAlloc(sizeof(Mem))) Mem(ebp, 0, 0);
  c->_registerOperand(_memoryOperand);
}

Variable::~Variable()
{
}

void Variable::setPriority(UInt8 priority)
{
  _priority = priority;
  
  // Alloc if priority is set to 0
  if (priority == 0) _function->alloc(this);
}

Variable* Variable::ref()
{
  _refCount++;
  return this;
}

void Variable::deref()
{
  if (--_refCount == 0) unuse();
}

// ============================================================================
// [AsmJit::State]
// ============================================================================

State::State(Compiler* c, Function* f) :
  _compiler(c),
  _function(f)
{
  _clear();
}

State::~State()
{
}

void State::_clear()
{
  memset(&_data, 0, sizeof(_data));
}

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

Emittable::Emittable(Compiler* c, UInt32 type) : _compiler(c), _type(type)
{
}

Emittable::~Emittable()
{
}

void Emittable::prepare()
{
  // ...nothing...
}

void Emittable::postEmit(Assembler& a)
{
  // ...nothing...
}

// ============================================================================
// [AsmJit::Comment]
// ============================================================================

Comment::Comment(Compiler* c, const char* str) : Emittable(c, EMITTABLE_COMMENT)
{
  SysUInt len = strlen(str);

  // Alloc string, but round it up
  _str = (char*)c->_zoneAlloc((len + sizeof(SysInt)) & ~(sizeof(SysInt)-1));
  memcpy((char*)_str, str, len + 1);
}

Comment::~Comment()
{
}

void Comment::emit(Assembler& a)
{
  if (a.logger()) a.logger()->log(str());
}

// ============================================================================
// [AsmJit::EmbeddedData]
// ============================================================================

EmbeddedData::EmbeddedData(Compiler* c, SysUInt capacity, const void* data, SysUInt size) :
  Emittable(c, EMITTABLE_EMBEDDED_DATA)
{
  _size = size;
  _capacity = capacity;
  memcpy(_data, data, size);
}

EmbeddedData::~EmbeddedData()
{
}

void EmbeddedData::emit(Assembler& a)
{
  a._embed(data(), size());
}

// ============================================================================
// [AsmJit::Align]
// ============================================================================

Align::Align(Compiler* c, SysInt size) : 
  Emittable(c, EMITTABLE_ALIGN), _size(size) 
{
}

Align::~Align()
{
}

void Align::emit(Assembler& a)
{
  a.align(size());
}

// ============================================================================
// [AsmJit::Instruction]
// ============================================================================

Instruction::Instruction(Compiler* c) :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
  _o[0] = &_ocache[0];
  _o[1] = &_ocache[1];
  _o[2] = &_ocache[2];
  memset(_ocache, 0, sizeof(_ocache));
}

Instruction::Instruction(Compiler* c, UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3) :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
  _code = code;

  UInt32 oid;
  if ((oid = o1->operandId()) != 0) { ASMJIT_ASSERT(oid < c->_operands.length()); _o[0] = c->_operands[oid]; } else { _o[0] = &_ocache[0]; _ocache[0] = *o1; }
  if ((oid = o2->operandId()) != 0) { ASMJIT_ASSERT(oid < c->_operands.length()); _o[1] = c->_operands[oid]; } else { _o[1] = &_ocache[1]; _ocache[1] = *o2; }
  if ((oid = o3->operandId()) != 0) { ASMJIT_ASSERT(oid < c->_operands.length()); _o[2] = c->_operands[oid]; } else { _o[2] = &_ocache[2]; _ocache[2] = *o3; }
}

Instruction::~Instruction()
{
}

void Instruction::emit(Assembler& a)
{
  a._emitX86(code(), _o[0], _o[1], _o[2]);
}

// ============================================================================
// [AsmJit::Function]
// ============================================================================

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4355) // this used in base member initializer list
#endif // _MSC_VER

Function::Function(Compiler* c) : 
  Emittable(c, EMITTABLE_FUNCTION),
  _stackAlignmentSize(0),
  _variablesStackSize(0),
  _cconv(CALL_CONV_NONE),
  _calleePopsStack(false),
  _naked(false),
  _allocableEbp(false),
  _emms(false),
  _cconvArgumentsDirection(ARGUMENT_DIR_RIGHT_TO_LEFT),
  _argumentsStackSize(0),
  _usedGpRegisters(0),
  _usedMmRegisters(0),
  _usedXmmRegisters(0),
  _modifiedGpRegisters(0),
  _modifiedMmRegisters(0),
  _modifiedXmmRegisters(0),
  _cconvPreservedGp(0),
  _cconvPreservedXmm(0),
  _lastUsedRegister(NULL),
  _entryLabel(c->newLabel()),
  _prologLabel(c->newLabel()),
  _exitLabel(c->newLabel()),
  _state(c, this)
{
  memset32(_cconvArgumentsGp, 0xFFFFFFFF, 16);
  memset32(_cconvArgumentsXmm, 0xFFFFFFFF, 16);
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // _MSC_VER

Function::~Function()
{
}

void Function::prepare()
{
  // Prepare variables
  static const UInt32 sizes[] = { 16, 8, 4, 2, 1 };

  SysUInt i, v;

  SysInt sp = 0;       // Stack offset
  SysInt pe = 0;       // Prolog / epilog size

  UInt8 argMemBase;    // Address base register for function arguments
  UInt8 varMemBase;    // Address base register for function variables
  SysInt argDisp = 0;  // Displacement for arguments
  SysInt varDisp = 0;  // Displacement for variables

  UInt32 alignSize = 0;// Maximum alignment stack size

  // This is simple optimization to do 16 byte aligned variables first and
  // all others next.
  for (i = 0; i < ASMJIT_ARRAY_SIZE(sizes); i++)
  {
    UInt32 size = sizes[i];

    for (v = 0; v < _variables.length(); v++)
    {
      Variable* var = _variables[v];

      // Use only variable with size 'size' and variable not mapped to the 
      // function arguments
      if (var->size() == size && var->_stackOffset <= 0 && var->_memoryAccessCount > 0)
      {
        // X86 stack is aligned to 32 bits (4 bytes). For MMX and SSE 
        // programming we need 8 or 16 bytes alignment. For MMX memory
        // operands we can use 4 bytes and for SSE 12 bytes.
#if defined(ASMJIT_X86)
        if (size ==  8 && alignSize <  8) alignSize =  8;
        if (size == 16 && alignSize < 16) alignSize = 16;
#endif // ASMJIT_X86

        sp += size;
        _variables[v]->_stackOffset = sp;
      }
    }
  }

  // Align to 16 bytes
  sp = (sp + 15) & ~15;

  // Get prolog/epilog push/pop size on the stack
  for (i = 0; i < NUM_REGS; i++)
  {
    if ((modifiedGpRegisters () & (1U << i)) && 
        (cconvPreservedGp() & (1U << i)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      pe += sizeof(SysInt);
    }
  }

  // push/pop ebp/rpb
  // if (!naked()) pe += sizeof(SysInt);

  _prologEpilogStackSize = pe;
  _variablesStackSize = sp;
  _stackAlignmentSize = alignSize;

  // Calculate displacement
  if (!naked())
  {
    // Functions with prolog/epilog are using ebp/rbp for variables
    argMemBase = RID_EBP | 0x10;
    // push ebp/rpb size (return address is already in arguments stack offset)
    argDisp = sizeof(SysInt);

    varMemBase = RID_ESP | 0x10;
    varDisp = pe;
  }
  else
  {
    // Naked functions are using always esp/rsp
    argMemBase = RID_ESP | 0x10;
    argDisp = pe;

    varMemBase = RID_ESP | 0x10;
    varDisp = -sp;
  }

  // Patch all variables to point to correct address in memory
  for (v = 0; v < _variables.length(); v++)
  {
    Variable* var = _variables[v];
    Mem* memop = var->_memoryOperand;

    if (v < argumentsCount())
    {
      // Arguments
      memop->_mem.base = argMemBase;
      memop->_mem.displacement = var->stackOffset() + argDisp;
    }
    else
    {
      // Variables
      memop->_mem.base = varMemBase;
      memop->_mem.displacement = var->stackOffset() + varDisp;
    }
  }
}

void Function::emit(Assembler& a)
{
  a.bind(_entryLabel);
}

void Function::setPrototype(UInt32 cconv, const UInt32* args, SysUInt count)
{
  _setCallingConvention(cconv);
  _setArguments(args, count);
}

void Function::setNaked(UInt8 naked)
{
  if (_naked == naked) return;

  _naked = naked;
}

void Function::_setCallingConvention(UInt32 cconv)
{
  // Safe defaults
  _cconv = cconv;
  _calleePopsStack = false;

  memset32(_cconvArgumentsGp, 0xFFFFFFFF, 16);
  memset32(_cconvArgumentsXmm, 0xFFFFFFFF, 16);

  _cconvArgumentsDirection = ARGUMENT_DIR_RIGHT_TO_LEFT;
  _argumentsStackSize = 0;

#if defined(ASMJIT_X86)
  // [X86 calling conventions]
  _cconvPreservedGp =
    (1 << (REG_EBX & REGCODE_MASK)) |
    (1 << (REG_ESP & REGCODE_MASK)) |
    (1 << (REG_EBP & REGCODE_MASK)) |
    (1 << (REG_ESI & REGCODE_MASK)) |
    (1 << (REG_EDI & REGCODE_MASK)) ;
  _cconvPreservedXmm = 0;

  switch (cconv)
  {
    case CALL_CONV_CDECL:
      break;
    case CALL_CONV_STDCALL:
      _calleePopsStack = true;
      break;
    case CALL_CONV_MSTHISCALL:
      _cconvArgumentsGp[0] = (REG_ECX & REGCODE_MASK);
      _calleePopsStack = true;
      break;
    case CALL_CONV_MSFASTCALL:
      _cconvArgumentsGp[0] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_EDX & REGCODE_MASK);
      _calleePopsStack = true;
      break;
    case CALL_CONV_BORLANDFASTCALL:
      _cconvArgumentsGp[0] = (REG_EAX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_EDX & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsDirection = ARGUMENT_DIR_LEFT_TO_RIGHT;
      _calleePopsStack = true;
      break;
    case CALL_CONV_GCCFASTCALL_2:
      _cconvArgumentsGp[0] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_EDX & REGCODE_MASK);
      _calleePopsStack = false;
      break;
    case CALL_CONV_GCCFASTCALL_3:
      _cconvArgumentsGp[0] = (REG_EDX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_EAX & REGCODE_MASK);
      _calleePopsStack = false;
      break;

    default:
      ASMJIT_CRASH();
  }
#else
  // [X64 calling conventions]
  switch(cconv)
  {
    case CALL_CONV_X64W:
      _cconvPreservedGp =
        (1 << (REG_RBX   & REGCODE_MASK)) |
        (1 << (REG_RSP   & REGCODE_MASK)) |
        (1 << (REG_RBP   & REGCODE_MASK)) |
        (1 << (REG_RSI   & REGCODE_MASK)) |
        (1 << (REG_RDI   & REGCODE_MASK)) |
        (1 << (REG_R12   & REGCODE_MASK)) |
        (1 << (REG_R13   & REGCODE_MASK)) |
        (1 << (REG_R14   & REGCODE_MASK)) |
        (1 << (REG_R15   & REGCODE_MASK)) ;
      _cconvPreservedXmm = 
        (1 << (REG_XMM6  & REGCODE_MASK)) |
        (1 << (REG_XMM7  & REGCODE_MASK)) |
        (1 << (REG_XMM8  & REGCODE_MASK)) |
        (1 << (REG_XMM9  & REGCODE_MASK)) |
        (1 << (REG_XMM10 & REGCODE_MASK)) |
        (1 << (REG_XMM11 & REGCODE_MASK)) |
        (1 << (REG_XMM12 & REGCODE_MASK)) |
        (1 << (REG_XMM13 & REGCODE_MASK)) |
        (1 << (REG_XMM14 & REGCODE_MASK)) |
        (1 << (REG_XMM15 & REGCODE_MASK)) ;

      _cconvArgumentsGp[0] = (REG_RCX  & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_RDX  & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_R8   & REGCODE_MASK);
      _cconvArgumentsGp[3] = (REG_R9   & REGCODE_MASK);
      _cconvArgumentsXmm[0] = (REG_XMM0 & REGCODE_MASK);
      _cconvArgumentsXmm[1] = (REG_XMM1 & REGCODE_MASK);
      _cconvArgumentsXmm[2] = (REG_XMM2 & REGCODE_MASK);
      _cconvArgumentsXmm[3] = (REG_XMM3 & REGCODE_MASK);
      break;

    case CALL_CONV_X64U:
      _cconvPreservedGp =
        (1 << (REG_RBX   & REGCODE_MASK)) |
        (1 << (REG_RSP   & REGCODE_MASK)) |
        (1 << (REG_RBP   & REGCODE_MASK)) |
        (1 << (REG_R12   & REGCODE_MASK)) |
        (1 << (REG_R13   & REGCODE_MASK)) |
        (1 << (REG_R14   & REGCODE_MASK)) |
        (1 << (REG_R15   & REGCODE_MASK)) ;
      _cconvPreservedXmm = 0;

      _cconvArgumentsGp[0] = (REG_RDI  & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_RSI  & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_RDX  & REGCODE_MASK);
      _cconvArgumentsGp[3] = (REG_RCX  & REGCODE_MASK);
      _cconvArgumentsGp[4] = (REG_R8   & REGCODE_MASK);
      _cconvArgumentsGp[5] = (REG_R9   & REGCODE_MASK);
      _cconvArgumentsXmm[0] = (REG_XMM0 & REGCODE_MASK);
      _cconvArgumentsXmm[1] = (REG_XMM1 & REGCODE_MASK);
      _cconvArgumentsXmm[2] = (REG_XMM2 & REGCODE_MASK);
      _cconvArgumentsXmm[3] = (REG_XMM3 & REGCODE_MASK);
      _cconvArgumentsXmm[4] = (REG_XMM4 & REGCODE_MASK);
      _cconvArgumentsXmm[5] = (REG_XMM5 & REGCODE_MASK);
      _cconvArgumentsXmm[6] = (REG_XMM6 & REGCODE_MASK);
      _cconvArgumentsXmm[7] = (REG_XMM7 & REGCODE_MASK);
      break;

    default:
      ASMJIT_CRASH();
  }
#endif
}

void Function::_setArguments(const UInt32* _args, SysUInt count)
{
  ASMJIT_ASSERT(count <= 32);

  SysInt i;

  SysInt gpnPos = 0;
  SysInt xmmPos = 0;
  SysInt stackOffset = 0;

  UInt32 args[32];
  memcpy(args, _args, count * sizeof(UInt32));

  _variables.clear();
  _state._clear();

  for (i = 0; i < (SysInt)count; i++)
  {
    Variable* v = _compiler->newObject<Variable>(this, args[i]);
    v->_refCount = 1; // Arguments are never freed or reused.
    _variables.append(v);
  }

  _argumentsCount = count;
  if (!_args) return;

#if defined(ASMJIT_X86)
  // ==========================================================================
  // [X86 Calling Conventions]

  // Register arguments (Integer), always left-to-right
  for (i = 0; i != count; i++)
  {
    UInt32 a = args[i];
    if (isIntegerArgument(a) && gpnPos < 32 && _cconvArgumentsGp[gpnPos] != 0xFFFFFFFF)
    {
      UInt8 reg = _cconvArgumentsGp[gpnPos++] | REG_GPN;
      Variable* v = _variables[i];

      v->setAll(a, 0, VARIABLE_STATE_REGISTER, 10, reg, NO_REG, 0);
      _allocReg(reg, v);

      _state._data._gp[reg & 0x0F] = v;
      args[i] = VARIABLE_TYPE_NONE;
    }
  }

  // Stack arguments
  bool ltr = _cconvArgumentsDirection == ARGUMENT_DIR_LEFT_TO_RIGHT;
  SysInt istart = ltr ? 0 : count-1;
  SysInt iend   = ltr ? count : -1;
  SysInt istep  = ltr ? 1 : -1;

  for (i = istart; i != iend; i += istep)
  {
    UInt32 a = args[i];
    if (isIntegerArgument(a))
    {
      stackOffset -= 4;

      _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, 20, NO_REG, NO_REG, stackOffset);
      args[i] = VARIABLE_TYPE_NONE;
    }
    else if (isFloatArgument(a))
    {
      if (a == VARIABLE_TYPE_FLOAT) 
        stackOffset -= 4;
      else
        stackOffset -= 8;

      _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, 20, NO_REG, NO_REG, stackOffset);
      args[i] = VARIABLE_TYPE_NONE;
    }
  }
  // ==========================================================================
#else
  // ==========================================================================
  // [X64 Calling Conventions]

  // Windows 64-bit specific
  if (cconv() == CALL_CONV_X64W)
  {
    SysInt max = count < 4 ? count : 4;

    // Register arguments (Integer / FP), always left to right
    for (i = 0; i != max; i++)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        UInt8 reg = _cconvArgumentsGp[i] | REG_GPN;
        Variable* v = _variables[i];

        v->setAll(a, 0, VARIABLE_STATE_REGISTER, 20, reg, NO_REG, 0);
        _allocReg(reg, v);

        _state._data._gp[reg & 0x0F] = v;
        args[i] = VARIABLE_TYPE_NONE;
      }
      else if (isFloatArgument(a))
      {
        UInt8 reg = _cconvArgumentsXmm[i] | REG_XMM;
        Variable* v = _variables[i];

        v->setAll(a, 0, VARIABLE_STATE_REGISTER, 20, reg, NO_REG, 0);
        _allocReg(reg, v);

        _state._data._xmm[reg & 0x0F] = v;
        args[i] = VARIABLE_TYPE_NONE;
      }
    }

    // Stack arguments
    for (i = count-1; i != -1; i--)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, 20, NO_REG, NO_REG, stackOffset);
        args[i] = VARIABLE_TYPE_NONE;
      }
    }
  }
  // All others
  else
  {
    // Register arguments (Integer), always left to right
    for (i = 0; i != count; i++)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a) && gpnPos < 32 && _cconvArgumentsGp[gpnPos] != 0xFFFFFFFF)
      {
        UInt8 reg = _cconvArgumentsGp[gpnPos++] | REG_GPN;
        Variable* v = _variables[i];

        v->setAll(a, 0, VARIABLE_STATE_REGISTER, 20, reg, NO_REG, 0);
        _allocReg(reg, v);

        _state._data._gp[reg & 0x0F] = v;
        args[i] = VARIABLE_TYPE_NONE;
      }
    }

    // Register arguments (FP), always left to right
    for (i = 0; i != count; i++)
    {
      UInt32 a = args[i];
      if (isFloatArgument(a))
      {
        UInt8 reg = _cconvArgumentsXmm[xmmPos++] | REG_XMM;
        Variable* v = _variables[i];

        v->setAll(a, 0, VARIABLE_STATE_REGISTER, 20, reg, NO_REG, 0);
        _allocReg(reg, v);

        _state._data._xmm[reg & 0x0F] = v;
        args[i] = VARIABLE_TYPE_NONE;
      }
    }

    // Stack arguments
    for (i = count-1; i != -1; i--)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, 20, NO_REG, NO_REG, stackOffset);
        args[i] = VARIABLE_TYPE_NONE;
      }
    }
  }
  // ==========================================================================
#endif // ASMJIT_X86, ASMJIT_X64

  // Modify stack offset (all function parameters will be in positive stack
  // offset that is never zero).
  for (i = 0; i < (SysInt)count; i++)
  {
    _variables[i]->_stackOffset += sizeof(SysInt) - stackOffset;
  }

  _argumentsStackSize = (UInt32)(-stackOffset);
}

Variable* Function::newVariable(UInt8 type, UInt8 priority, UInt8 preferredRegister)
{
  Variable* v;

  // First look to unused variables
  SysUInt i;
  for (i = 0; i < _variables.length(); i++)
  {
    v = _variables[i];
    if (v->refCount() == 0 && v->type() == type) 
    {
      v->_spillCount = 0;
      v->_reuseCount++;
      v->_registerAccessCount = 0;
      v->_memoryAccessCount = 0;

      v->_preferredRegister = preferredRegister;
      v->_priority = priority;
      v->_changed = 0;

      v->_allocFn = NULL;
      v->_spillFn = NULL;
      v->_data = NULL;

      return v;
    }
  }

  // If variable can't be reused, create new one.
  v = compiler()->newObject<Variable>(this, type);
  v->_preferredRegister = preferredRegister;
  v->_priority = priority;
  _variables.append(v);

  // Alloc register if priority is zero
  if (priority == 0) alloc(v);

  return v;
}

void Function::alloc(Variable* v, UInt8 mode, UInt8 preferredRegister)
{
  ASMJIT_ASSERT(compiler() == v->compiler());
  if (v->state() == VARIABLE_STATE_REGISTER) { _lastUsedRegister = v; return; }

  UInt32 i;

  // Preferred register code
  UInt8 pref = (preferredRegister != NO_REG) 
    ? preferredRegister 
    : v->_preferredRegister;

  // New register code.
  UInt8 code = NO_REG;

  // First try to find unused registers
  if (v->type() == VARIABLE_TYPE_INT32 || v->type() == VARIABLE_TYPE_INT64)
  {
    // preferred register
    if (pref != NO_REG)
    {
      UInt8 last = _lastUsedRegister ? _lastUsedRegister->registerCode() : NO_REG;

      // esp/rsp can't be never allocated
      ASMJIT_ASSERT((pref & REGCODE_MASK) != RID_ESP);

      if ((_usedGpRegisters & (1U << (pref & REGCODE_MASK))) == 0)
      {
        code = pref;
      }
      else
      {
        if ((last & REGTYPE_MASK) <= REG_GPQ && ((pref & REGCODE_MASK) == (last & REGCODE_MASK)))
        {
          // Fatal error, need to spill register that is used in current 
          // instruction
          ASMJIT_ASSERT(0);
        }
        else
        {
          // Spill register we need
          Variable* candidate = _state._data._gp[pref & REGCODE_MASK];
          // Candidate must exists if it's marked as used
          ASMJIT_ASSERT(candidate);
          // Candidate priority must be larger than zero
          ASMJIT_ASSERT(candidate->priority() > 0);
          // Spill it (everything should be ok)
          spill(candidate);
          code = pref;
        }
      }
    }

    // We start from 1, because EAX/RAX registers are sometimes explicitly 
    // needed
    if (code == NO_REG)
    {
      for (i = 1; i < NUM_REGS; i++)
      {
        if ((_usedGpRegisters & (1U << i)) == 0 && (i != RID_EBP || allocableEbp()) && i != RID_ESP) 
        {
          if (v->type() == VARIABLE_TYPE_INT32)
            code = i | REG_GPD;
          else
            code = i | REG_GPQ;
          break;
        }
      }
    }

    // If not found, try EAX/RAX
    if (code == NO_REG && (_usedGpRegisters & 1) == 0) 
    {
      if (v->type() == VARIABLE_TYPE_INT32)
        code = RID_EAX | REG_GPD;
      else
        code = RID_EAX | REG_GPQ;
    }
  }
  else if (v->type() == VARIABLE_TYPE_MM)
  {
    // preferred register
    if (pref != NO_REG)
    {
      UInt8 last = _lastUsedRegister ? _lastUsedRegister->registerCode() : NO_REG;

      if ((_usedMmRegisters & (1U << (pref & 0x7))) == 0)
      {
        code = pref;
      }
      else
      {
        if ((last & 0x7) == REG_MM && ((pref & 0x7) == (last & 0x7)))
        {
          // Fatal error, need to spill register that is used in current 
          // instruction
          ASMJIT_ASSERT(0);
        }
        else
        {
          // Spill register we need
          Variable* candidate = _state._data._mm[pref & 0x7 /* limit it to 0-7 */];
          // Candidate must exists if it's marked as used
          ASMJIT_ASSERT(candidate);
          // Candidate priority must be larger than zero
          ASMJIT_ASSERT(candidate->priority() > 0);
          // Spill it (everything should be ok)
          spill(candidate);
          code = pref;
        }
      }
    }

    if (code == NO_REG)
    {
      for (i = 0; i < 8; i++)
      {
        if ((_usedMmRegisters & (1U << i)) == 0)
        {
          code = i | REG_MM;
          break;
        }
      }
    }
  }
  else if (v->type() == VARIABLE_TYPE_XMM)
  {
    // preferred register
    if (pref != NO_REG)
    {
      UInt8 last = _lastUsedRegister ? _lastUsedRegister->registerCode() : NO_REG;

      if ((_usedMmRegisters & (1U << (pref & REGCODE_MASK))) == 0)
      {
        code = pref;
      }
      else
      {
        if ((last & REGTYPE_MASK) == REG_XMM && ((pref & REGCODE_MASK) == (last & REGCODE_MASK)))
        {
          // Fatal error, need to spill register that is used in current 
          // instruction
          ASMJIT_ASSERT(0);
        }
        else
        {
          // Spill register we need
          Variable* candidate = _state._data._xmm[pref & REGCODE_MASK];
          // Candidate must exists if it's marked as used
          ASMJIT_ASSERT(candidate);
          // Candidate priority must be larger than zero
          ASMJIT_ASSERT(candidate->priority() > 0);
          // Spill it (everything should be ok)
          spill(candidate);
          code = pref;
        }
      }
    }

    if (code == NO_REG)
    {
      for (i = 0; i < NUM_REGS; i++)
      {
        if ((_usedXmmRegisters & (1U << i)) == 0)
        {
          code = i | REG_XMM;
          break;
        }
      }
    }
  }

  // If register is still not found, spill some variable
  if (code == NO_REG)
  {
    Variable* candidate = _getSpillCandidate(v->type());
    ASMJIT_ASSERT(candidate);

    code = candidate->_registerCode;
    spill(candidate);
  }

  _allocAs(v, mode, code);
}

void Function::spill(Variable* v)
{
  ASMJIT_ASSERT(compiler() == v->compiler());

  if (_lastUsedRegister == v) _lastUsedRegister = NULL;

  if (v->state() == VARIABLE_STATE_UNUSED) return;
  if (v->state() == VARIABLE_STATE_MEMORY) return;
  if (v->state() == VARIABLE_STATE_REGISTER)
  {
    if (v->changed())
    {
      if (v->isCustom())
      {
        if (v->_spillFn) v->_spillFn(v);
      }
      else
      {
        switch (v->type())
        {
          case VARIABLE_TYPE_INT32:
            compiler()->mov(*v->_memoryOperand, mk_gpd(v->registerCode()));
            break;
#if defined(ASMJIT_X64)
          case VARIABLE_TYPE_INT64:
            compiler()->mov(*v->_memoryOperand, mk_gpq(v->registerCode()));
            break;
#endif // ASMJIT_X64
          case VARIABLE_TYPE_FLOAT:
            // TODO: NOT IMPLEMENTED
            break;
          case VARIABLE_TYPE_DOUBLE:
            // TODO: NOT IMPLEMENTED
            break;
          case VARIABLE_TYPE_MM:
            compiler()->movq(*v->_memoryOperand, mk_mm(v->registerCode()));
            break;
          case VARIABLE_TYPE_XMM:
            // Alignment is not guaranted for naked functions in 32 bit mode
            if (naked())
              compiler()->movdqu(*v->_memoryOperand, mk_xmm(v->registerCode()));
            else
              compiler()->movdqa(*v->_memoryOperand, mk_xmm(v->registerCode()));
            break;
        }
        v->_memoryAccessCount++;
      }

      v->setChanged(false);
    }

    _freeReg(v->registerCode());
    v->_registerCode = NO_REG;

    v->_state = VARIABLE_STATE_MEMORY;
    v->_spillCount++;
  }
}

void Function::unuse(Variable* v)
{
  ASMJIT_ASSERT(compiler() == v->compiler());
  if (v->state() == VARIABLE_STATE_UNUSED) return;

  if (v->state() == VARIABLE_STATE_REGISTER)
  {
    _freeReg(v->registerCode());
    v->_registerCode = NO_REG;
  }

  v->_state = VARIABLE_STATE_UNUSED;
  v->_reuseCount++;
}

static UInt32 getSpillScore(Variable* v)
{
  if (v->priority() == 0) return 0;

  // Priority is main factor.
  UInt32 p = ((UInt32)v->priority() << 24) - ((1U << 24) / 2);

  // Each register access means lower probability of spilling
  p -= (UInt32)v->registerAccessCount();

  // Each memory access means higher probability of spilling
  p += (UInt32)v->memoryAccessCount();

  return p;
}

Variable* Function::_getSpillCandidate(UInt8 type)
{
  Variable* candidate = NULL;
  Variable* v;
  SysUInt i, len = _variables.length();

  UInt32 candidateScore = 0;
  UInt32 variableScore;

  switch (type)
  {
    case VARIABLE_TYPE_INT32:
    case VARIABLE_TYPE_INT64:
      for (i = 0; i < len; i++)
      {
        v = _variables[i];
        if ((v->type() == VARIABLE_TYPE_INT32 || v->type() == VARIABLE_TYPE_INT64) &&
            (v->state() == VARIABLE_STATE_REGISTER && v->priority() > 0) &&
            (v != _lastUsedRegister))
        {
          variableScore = getSpillScore(v);
          if (variableScore > candidateScore) { candidateScore = variableScore; candidate = v; }
        }
      }
      break;
    case VARIABLE_TYPE_FLOAT:
      // TODO: NOT IMPLEMENTED
      break;
    case VARIABLE_TYPE_DOUBLE:
      // TODO: NOT IMPLEMENTED
      break;
    case VARIABLE_TYPE_MM:
      for (i = 0; i < len; i++)
      {
        v = _variables[i];
        if ((v->type() == VARIABLE_TYPE_MM) &&
            (v->state() == VARIABLE_STATE_REGISTER && v->priority() > 0) &&
            (v != _lastUsedRegister))
        {
          variableScore = getSpillScore(v);
          if (variableScore > candidateScore) { candidateScore = variableScore; candidate = v; }
        }
      }
      break;
    case VARIABLE_TYPE_XMM:
      for (i = 0; i < len; i++)
      {
        v = _variables[i];
        if ((v->type() == VARIABLE_TYPE_XMM) &&
            (v->state() == VARIABLE_STATE_REGISTER && v->priority() > 0) &&
            (v != _lastUsedRegister))
        {
          variableScore = getSpillScore(v);
          if (variableScore > candidateScore) { candidateScore = variableScore; candidate = v; }
        }
      }
      break;
  }

  return candidate;
}

void Function::_allocAs(Variable* v, UInt8 mode, UInt32 code)
{
  // true if we must copy content from memory to register before we can use it
  bool copy = (v->state() == VARIABLE_STATE_MEMORY);

  v->_state = VARIABLE_STATE_REGISTER;
  v->_registerCode = code;

  _allocReg(code, v);

  if (copy && mode != VARIABLE_ALLOC_WRITE)
  {
    if (v->isCustom())
    {
      if (v->_allocFn) v->_allocFn(v);
    }
    else
    {
      switch (v->type())
      {
        case VARIABLE_TYPE_INT32:
          compiler()->mov(mk_gpd(v->_registerCode), *v->_memoryOperand);
          break;
#if defined(ASMJIT_X64)
        case VARIABLE_TYPE_INT64:
          compiler()->mov(mk_gpq(v->_registerCode), *v->_memoryOperand);
          break;
#endif // ASMJIT_X64
        case VARIABLE_TYPE_FLOAT:
          // TODO: NOT IMPLEMENTED
          break;
        case VARIABLE_TYPE_DOUBLE:
          // TODO: NOT IMPLEMENTED
          break;
        case VARIABLE_TYPE_MM:
          compiler()->movq(mk_mm(v->_registerCode), *v->_memoryOperand);
          break;
        case VARIABLE_TYPE_XMM:
          // Alignment is not guaranted for naked functions in 32 bit mode
          if (naked())
            compiler()->movdqu(mk_xmm(v->_registerCode), *v->_memoryOperand);
          else
            compiler()->movdqa(mk_xmm(v->_registerCode), *v->_memoryOperand);
          break;
      }
      v->_memoryAccessCount++;
    }
  }

  if ((mode & VARIABLE_ALLOC_WRITE) != 0)
  {
    v->_changed = true;
  }

  _lastUsedRegister = v;
}

void Function::_allocReg(UInt8 code, Variable* v)
{
  UInt32 type = code & REGTYPE_MASK;
  UInt32 mask = 1U << (code & REGCODE_MASK);

  switch (type)
  {
    case REG_GPB:
    case REG_GPW:
    case REG_GPD:
    case REG_GPQ:
      useGpRegisters(mask);
      modifyGpRegisters(mask);
      _state._data._gp[code & 0x0F] = v;
      break;
    case REG_MM:
      useMmRegisters(mask);
      modifyMmRegisters(mask);
      _state._data._mm[code & 0x0F] = v;
      break;
    case REG_XMM:
      useXmmRegisters(mask);
      modifyXmmRegisters(mask);
      _state._data._xmm[code & 0x0F] = v;
      break;
  }
}

void Function::_freeReg(UInt8 code)
{
  UInt32 type = code & REGTYPE_MASK;
  UInt32 mask = 1U << (code & REGCODE_MASK);

  switch (type)
  {
    case REG_GPB:
    case REG_GPW:
    case REG_GPD:
    case REG_GPQ:
      unuseGpRegisters(mask);
      _state._data._gp[code & 0x0F] = NULL;
      break;
    case REG_MM:
      unuseMmRegisters(mask);
      _state._data._mm[code & 0x0F] = NULL;
      break;
    case REG_XMM:
      unuseXmmRegisters(mask);
      _state._data._xmm[code & 0x0F] = NULL;
      break;
  }
}

State *Function::saveState()
{
  State* s = compiler()->newObject<State>(this);
  memcpy(&s->_data, &_state._data, sizeof(State::Data));
  return s;
}

void Function::restoreState(State* s)
{
  ASMJIT_ASSERT(s->_function == this);

  // make local copy of function state
  State::Data f_d;
  State::Data& s_d = s->_data;

  memcpy(&f_d, &_state._data, sizeof(State::Data));

  SysInt base;
  SysInt i;

  // Spill registers
  for (base = 0, i = 0; i < 16+8+16; i++)
  {
    if (i == 16 || i == 24) base = i;

    Variable* from = f_d._v[i];
    Variable* to   = s_d._v[i];

    if (from != to)
    {
      UInt8 regIndex = (UInt8)(i - base);

      // Spill register
      if (from != NULL) 
      {
        spill(from);
      }
    }
  }

  // Alloc registers
  for (base = 0, i = 0; i < 16+8+16; i++)
  {
    if (i == 16 || i == 24) base = i;

    Variable* from = f_d._v[i];
    Variable* to   = s_d._v[i];

    if (from != to)
    {
      UInt8 regIndex = (UInt8)(i - base);

      // Alloc register
      if (to != NULL) 
      {
        UInt8 code = regIndex | variableRegCodeData[to->type()];
        _allocAs(to, VARIABLE_ALLOC_READWRITE, code);
      }
    }
  }
}

void Function::setState(State* s)
{
  ASMJIT_ASSERT(s->_function == this);
  memcpy(&_state._data, &s->_data, sizeof(State::Data));
}

// ============================================================================
// [AsmJit::Prolog]
// ============================================================================

Prolog::Prolog(Compiler* c, Function* f) : 
  Emittable(c, EMITTABLE_PROLOGUE), 
  _function(f)
{
}

Prolog::~Prolog()
{
}

void Prolog::emit(Assembler& a)
{
  Function* f = function();
  ASMJIT_ASSERT(f);

  int i;

  // Emit prolog if needed (prolog is not emitted for pure naked functions)
  if (!f->naked())
  {
    a.push(nbp);
    a.mov(nbp, nsp);

    // Arguments size
    if (f->variablesStackSize())
    {
      SysInt ss = (
        f->variablesStackSize() + 
        f->prologEpilogStackSize() + 
        f->stackAlignmentSize() + 
        sizeof(SysInt) + 15) & ~15;
      a.sub(nsp, ss);

#if defined(ASMJIT_X86)
      // Alignment
      if (f->stackAlignmentSize())
      {
        // stackAlignmentSize can be 8 or 16
        a.and_(nsp, -((Int32)f->stackAlignmentSize()));
      }
#endif // ASMJIT_X86
    }
  }

  // Save registers if needed
  for (i = 0; i < NUM_REGS; i++)
  {
    if ((f->modifiedGpRegisters () & (1U << i)) && 
        (f->cconvPreservedGp() & (1U << i)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      a.push(mk_gpn(i));
    }
  }

  // After prolog, bind label
  if (_label) a.bind(_label);
}

// ============================================================================
// [AsmJit::Epilog]
// ============================================================================

Epilog::Epilog(Compiler* c, Function* f) : 
  Emittable(c, EMITTABLE_EPILOGUE),
  _function(f)
{
}

Epilog::~Epilog()
{
}

void Epilog::emit(Assembler& a)
{
  Function* f = function();
  ASMJIT_ASSERT(f);

  // First bind label (Function::_exitLabel) before the epilog
  if (_label) a.bind(_label);

  if (f->emms())
  {
    a.emms();
  }

  // Add variables and register cleanup code
  int i;
  for (i = NUM_REGS-1; i >= 0; i--)
  {
    if ((f->modifiedGpRegisters() & (1U << i)) && 
        (f->cconvPreservedGp() & (1U << i)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      a.pop(mk_gpn(i));
    }
  }

  // Use epilog code (if needed)
  if (!f->naked())
  {
    a.mov(nsp, nbp);
    a.pop(nbp);
  }

  // Return using correct instruction
  if (f->calleePopsStack())
  {
    a.ret((Int16)f->argumentsStackSize());
  }
  else
  {
    a.ret();
  }

  // Add end of line after epilog
  if (a.logger()) a.logger()->log("");
}

// ============================================================================
// [AsmJit::Target]
// ============================================================================

Target::Target(Compiler* c, Label* l) : 
  Emittable(c, EMITTABLE_TARGET), 
  _label(l)
{
}

Target::~Target()
{
}

void Target::emit(Assembler& a)
{
  a.bind(_label);
}

// ============================================================================
// [AsmJit::Compiler - Construction / Destruction]
// ============================================================================

Compiler::Compiler() :
  _currentPosition(0),
  _currentFunction(NULL),
  _labelIdCounter(1)
{
  _jumpTableLabel = newLabel();
}

Compiler::~Compiler()
{
  delAll(_buffer);
}

// ============================================================================
// [AsmJit::Compiler - Buffer]
// ============================================================================

void Compiler::clear()
{
  delAll(_buffer);
  _zone.freeAll();

  _buffer.clear();
  _operands.clear();
  _currentPosition = 0;
  _jumpTableLabel = newLabel();
  _jumpTableData.clear();
}

void Compiler::free()
{
  clear();
  _buffer.free();
  _operands.free();
  _jumpTableData.free();
}

// ============================================================================
// [AsmJit::Compiler - Logging]
// ============================================================================

void Compiler::comment(const char* fmt, ...)
{
  char buf[1024];

  if (fmt)
  {
    buf[0] = ';';
    buf[1] = ' ';

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf + 2, 1021, fmt, ap);
    va_end(ap);
  }
  else
  {
    buf[0] = '\n';
    buf[1] = '\0';
  }

  emit(newObject<Comment>(buf));
}

// ============================================================================
// [AsmJit::Compiler - Function Builder]
// ============================================================================

Function* Compiler::newFunction_(UInt32 cconv, const UInt32* args, SysUInt count)
{
  ASMJIT_ASSERT(_currentFunction == NULL);

  Function* f = _currentFunction = newObject<Function>();
  f->setPrototype(cconv, args, count);

  emit(f);

  Prolog* e = prolog(f);
  e->_label = f->_prologLabel;

  return f;
}

Function* Compiler::endFunction()
{
  ASMJIT_ASSERT(_currentFunction != NULL);
  Function* f = _currentFunction;

  Epilog* e = epilog(f);
  e->_label = f->_exitLabel;

  _currentFunction = NULL;
  return f;
}

Prolog* Compiler::prolog(Function* f)
{
  Prolog* e = newObject<Prolog>(f);
  emit(e);
  return e;
}

Epilog* Compiler::epilog(Function* f)
{
  Epilog* e = newObject<Epilog>(f);
  emit(e);
  return e;
}

// ============================================================================
// [AsmJit::Compiler - Labels]
// ============================================================================

Label* Compiler::newLabel()
{
  Label* label = new(_zoneAlloc(sizeof(Label))) Label((UInt16)(_labelIdCounter++));
  _registerOperand(label);
  return label;
}

// ============================================================================
// [AsmJit::Compiler - Emit]
// ============================================================================

// logger switcher used in Compiler::build().
struct LoggerSwitcher
{
  LoggerSwitcher(Assembler* a, Compiler* c) :
    a(a),
    logger(a->logger())
  {
    // Set compiler logger
    if (!logger && c->logger()) a->setLogger(c->logger());
  }

  ~LoggerSwitcher()
  {
    // Restore logger
    a->setLogger(logger);
  }

  Assembler* a;
  Logger* logger;
};

void Compiler::emit(Emittable* emittable, bool endblock)
{
  if (endblock)
  {
    _buffer.append(emittable);
  }
  else
  {
    _buffer.insert(_currentPosition, emittable);
    _currentPosition++;
  }
}

void Compiler::build(Assembler& a)
{
  LoggerSwitcher loggerSwitcher(&a, this);

  Emittable** emitters = _buffer.data();
  SysUInt i, len;

  // Prepare (prepare action can append emittable)
  len = _buffer.length();
  for (i = 0; i < len; i++) emitters[i]->prepare();

  // Emit and postEmit
  len = _buffer.length();
  for (i = 0; i < len; i++) emitters[i]->emit(a);
  for (i = 0; i < len; i++) emitters[i]->postEmit(a);

  // Jump table
  a.bind(_jumpTableLabel);

  len = _jumpTableData.length();
  for (i = 0; i < len; i++)
  {
    a.dptr(_jumpTableData[i]);
  }
}

// ============================================================================
// [AsmJit::Compiler - Memory Management]
// ============================================================================

void Compiler::_registerOperand(Operand* op)
{
  op->_operandId = _operands.length();
  _operands.append(op);
}

// ============================================================================
// [AsmJit::Compiler - Absolute Jumps / Calls]
// ============================================================================

void Compiler::jmp(void* target)
{
  jmp(ptr(_jumpTableLabel, _addTarget(target)));
}

void Compiler::call(void* target)
{
  call(ptr(_jumpTableLabel, _addTarget(target)));
}

SysInt Compiler::_addTarget(void* target)
{
  SysInt id = _jumpTableData.length() * sizeof(SysInt);
  _jumpTableData.append(target);
  return id;
}

// ============================================================================
// [AsmJit::Compiler - Intrinsics]
// ============================================================================

void Compiler::op_var32(UInt32 code, const Int32Ref& a)
{
  if (a.state() == VARIABLE_STATE_REGISTER)
  {
    Register ar = a.r32();
    __emitX86(code, &ar);
  }
  else
  {
    __emitX86(code, &a.m());
  }
}

void Compiler::op_reg32_var32(UInt32 code, const Register& a, const Int32Ref& b)
{
  if (b.state() == VARIABLE_STATE_REGISTER)
  {
    Register br = b.r32();
    __emitX86(code, &a, &br);
  }
  else
  {
    __emitX86(code, &a, &b.m());
  }
}

void Compiler::op_var32_reg32(UInt32 code, const Int32Ref& a, const Register& b)
{
  if (a.state() == VARIABLE_STATE_REGISTER)
  {
    Register ar = a.r32();
    __emitX86(code, &ar, &b);
  }
  else
  {
    __emitX86(code, &a.m(), &b);
  }
}

void Compiler::op_var32_imm(UInt32 code, const Int32Ref& a, const Immediate& b)
{
  if (a.state() == VARIABLE_STATE_REGISTER)
  {
    Register ar = a.r32();
    __emitX86(code, &ar, &b);
  }
  else
  {
    __emitX86(code, &a.m(), &b);
  }
}

#if defined(ASMJIT_X64)
void Compiler::op_var64(UInt32 code, const Int64Ref& a)
{
  if (a.state() == VARIABLE_STATE_REGISTER)
  {
    Register ar = a.r64();
    __emitX86(code, &ar);
  }
  else
  {
    __emitX86(code, &a.m());
  }
}

void Compiler::op_reg64_var64(UInt32 code, const Register& a, const Int64Ref& b)
{
  if (b.state() == VARIABLE_STATE_REGISTER)
  {
    Register br = b.r64();
    __emitX86(code, &a, &br);
  }
  else
  {
    __emitX86(code, &a, &b.m());
  }
}

void Compiler::op_var64_reg64(UInt32 code, const Int64Ref& a, const Register& b)
{
  if (a.state() == VARIABLE_STATE_REGISTER)
  {
    Register ar = a.r64();
    __emitX86(code, &ar, &b);
  }
  else
  {
    __emitX86(code, &a.m(), &b);
  }
}

void Compiler::op_var64_imm(UInt32 code, const Int64Ref& a, const Immediate& b)
{
  if (a.state() == VARIABLE_STATE_REGISTER)
  {
    Register ar = a.r64();
    __emitX86(code, &ar, &b);
  }
  else
  {
    __emitX86(code, &a.m(), &b);
  }
}
#endif // ASMJIT_X64

// ============================================================================
// [AsmJit::Compiler - EmitX86]
// ============================================================================

void Compiler::_emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  emit(newObject<Instruction>(code, o1, o2, o3));
}

// ============================================================================
// [AsmJit::Compiler - Embed]
// ============================================================================

void Compiler::_embed(const void* data, SysUInt size)
{
  // Align capacity to 16 bytes
  SysUInt capacity = (size + 15) & ~15;

  EmbeddedData* e = 
    new(_zoneAlloc(sizeof(EmbeddedData) - sizeof(void*) + capacity)) 
      EmbeddedData(this, capacity, data, size);
  emit(e);
}

// ============================================================================
// [AsmJit::Compiler - Align]
// ============================================================================

void Compiler::align(SysInt m)
{
  emit(newObject<Align>(m));
}

// ============================================================================
// [AsmJit::Compiler - Bind]
// ============================================================================

void Compiler::bind(Label* label)
{
  emit(newObject<Target>(label));
}

} // AsmJit namespace
