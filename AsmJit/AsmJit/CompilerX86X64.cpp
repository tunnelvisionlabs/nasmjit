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
#include "Assembler.h"
#include "Compiler.h"
#include "CpuInfo.h"
#include "Logger.h"
#include "Util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// [Warnings-Push]
#include "WarningsPush.h"

// VARIABLE_TYPE_INT64 not declared in 32 bit mode, in future this should 
// change
#if defined(ASMJIT_X86)
#define VARIABLE_TYPE_INT64 2
#endif // ASMJIT_X86

namespace AsmJit {

// ============================================================================
// [Helpers]
// ============================================================================

static void delAll(Emittable* first)
{
  Emittable* cur;
  for (cur = first; cur; cur = cur->next()) cur->~Emittable();
}

static void memset32(UInt32* p, UInt32 c, SysUInt len)
{
  SysUInt i;
  for (i = 0; i < len; i++) p[i] = c;
}

static bool isIntegerArgument(UInt32 arg)
{
  return arg == VARIABLE_TYPE_INT32 ||
         arg == VARIABLE_TYPE_INT64 ;
}

static bool isFloatArgument(UInt32 arg)
{
  return arg == VARIABLE_TYPE_FLOAT || 
         arg == VARIABLE_TYPE_DOUBLE;
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

static const char* variableTypeName[] =
{
  "none",
  "int32",
  "int64",
  "float",
  "double",
  "mm",
  "xmm"
};

static UInt32 getVariableSize(UInt32 type)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableSizeData));
  return variableSizeData[type];
}

static UInt8 getVariableRegisterCode(UInt32 type, UInt8 index)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableRegCodeData));
  return variableRegCodeData[type] | index;
}

// ============================================================================
// [AsmJit::Variable]
// ============================================================================

Variable::Variable(Compiler* c, Function* f, UInt8 type) :
  _compiler(c),
  _function(f),
  _refCount(0),
  _spillCount(0),
  _registerAccessCount(0),
  _memoryAccessCount(0),
  _lifeId(0),
  _globalSpillCount(0),
  _globalRegisterAccessCount(0),
  _globalMemoryAccessCount(0),
  _type(type),
  _size(getVariableSize(type)),
  _state(VARIABLE_STATE_UNUSED),
  _priority(10),
  _registerCode(NO_REG),
  _preferredRegisterCode(0xFF),
  _homeRegisterCode(0xFF),
  _changed(false),
  _reusable(true),
  _customMemoryHome(false),
  _stackOffset(0),
  _allocFn(NULL),
  _spillFn(NULL),
  _dataPtr(NULL),
  _dataInt(0)
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

void Variable::setMemoryHome(const Mem& memoryHome) ASMJIT_NOTHROW
{
  _reusable = false;
  _customMemoryHome = true;
  *_memoryOperand = memoryHome;
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

void Variable::getReg(
  UInt8 mode, UInt8 preferredRegister,
  BaseReg* dest, UInt8 regType)
{
  alloc(mode, preferredRegister);

  // Setup register in dest
  UInt8 size = 1U << (regType >> 4);
  if (regType == REG_X87) size = 10;
  if (regType == REG_MM ) size = 8;
  if (regType == REG_XMM) size = 16;

  *dest = BaseReg((_registerCode & REGCODE_MASK) | regType, size);

  // Statistics
  _registerAccessCount++;
  _globalRegisterAccessCount++;
}


// ============================================================================
// [AsmJit::State]
// ============================================================================

// this helper structure is used for jumps that ensures that variable allocator
// state is updated to correct values. It's saved in Label operand as single
// linked list.
struct JumpAndRestore
{
  JumpAndRestore* next;
  Instruction* instruction;
  State* from;
  State* to;
};

State::State(Compiler* c, Function* f) :
  _compiler(c),
  _function(f)
{
  _clear();
}

State::~State()
{
}

void State::saveFunctionState(Data* dst, Function* f)
{
  for (SysUInt i = 0; i < 16+8+16; i++)
  {
    Variable* v = f->_state.regs[i];

    if (v)
    {
      dst->regs[i].v = v;
      dst->regs[i].lifeId = v->lifeId();
      dst->regs[i].state = v->state();
      dst->regs[i].changed = v->changed();
    }
    else
    {
      memset(&dst->regs[i], 0, sizeof(Entry));
    }
  }

  dst->usedGpRegisters  = f->usedGpRegisters ();
  dst->usedMmRegisters  = f->usedMmRegisters ();
  dst->usedXmmRegisters = f->usedXmmRegisters();
}

void State::_clear()
{
  memset(&_data, 0, sizeof(_data));
}

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

Emittable::Emittable(Compiler* c, UInt32 type) : 
  _compiler(c),
  _next(NULL),
  _prev(NULL),
  _type(type)
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
  _str = (char*)c->_zoneAlloc(
    (len + sizeof(SysInt)) & ~(sizeof(SysInt)-1));
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
  ASMJIT_ASSERT(capacity >= size);

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

Function::Function(Compiler* c) : 
  Emittable(c, EMITTABLE_FUNCTION),
  _stackAlignmentSize(0),
  _variablesStackSize(0),
  _cconv(CALL_CONV_NONE),
  _calleePopsStack(false),
  _naked(false),
  _allocableEbp(false),
  _emms(false),
  _sfence(false),
  _lfence(false),
  _optimizedPrologEpilog(true),
  _cconvArgumentsDirection(ARGUMENT_DIR_RIGHT_TO_LEFT),
  _cconvPreservedGp(0),
  _cconvPreservedXmm(0),
  _argumentsCount(0),
  _argumentsStackSize(0),
  _usedGpRegisters(0),
  _usedMmRegisters(0),
  _usedXmmRegisters(0),
  _modifiedGpRegisters(0),
  _modifiedMmRegisters(0),
  _modifiedXmmRegisters(0),
  _usePrevention(true),
  _entryLabel(c->newLabel()),
  _prologLabel(c->newLabel()),
  _exitLabel(c->newLabel())
{
  memset32(_cconvArgumentsGp, 0xFFFFFFFF, 16);
  memset32(_cconvArgumentsXmm, 0xFFFFFFFF, 16);
  memset(&_state, 0, sizeof(_state));
}

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
      if (var->size() == size && var->_stackOffset <= 0 && var->_globalMemoryAccessCount > 0)
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

  _prologEpilogStackSize = pe;
  _variablesStackSize = sp;
  _stackAlignmentSize = alignSize;

  // Calculate displacement
  if (!naked())
  {
    // Functions with prolog/epilog are using ebp/rbp for variables
    argMemBase = RID_EBP;
    // push ebp/rpb size (return address is already in arguments stack offset)
    argDisp = sizeof(SysInt);

    varMemBase = RID_ESP;
    varDisp = pe;
  }
  else
  {
    // Naked functions are using always esp/rsp
    argMemBase = RID_ESP;
    argDisp = pe;

    varMemBase = RID_ESP;
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
  // Dump function and statistics
  Logger* logger = a.logger();
  if (logger && logger->enabled())
  {
    char _buf[1024];
    char* p;
    const char* loc;

    SysUInt i;
    SysUInt varlen = _variables.length();

    // Log function and its parameters
    logger->log("; Function Prototype:\n");
    logger->log(";   (");
    for (i = 0; i < argumentsCount(); i++)
    {
      Variable* v = _variables[i];

      if (i != 0) logger->log(", ");
      logger->log(
        v->type() < _VARIABLE_TYPE_COUNT ? variableTypeName[v->type()] : "unknown");
    }
    logger->log(")\n");
    logger->log(";\n");
    logger->log("; Variables:\n");

    // Log variables
    for (i = 0; i < varlen; i++)
    {
      Variable* v = _variables[i];

      if (v->_globalMemoryAccessCount > 0)
      {
        loc = _buf;
        Logger::dumpOperand(_buf, v->_memoryOperand)[0] = '\0';
      }
      else
      {
        loc = "[None]";
      }

      logger->logFormat(";   %-2u (%-6s) at %-9s - reg access: %-3u, mem access: %-3u\n",
        // Variable ID
        (unsigned int)i,
        // Variable Type
        v->type() < _VARIABLE_TYPE_COUNT ? variableTypeName[v->type()] : "unknown",
        // Variable Memory Address
        loc,
        // Register Access Count
        (unsigned int)v->_globalRegisterAccessCount,
        // Memory Access Count
        (unsigned int)v->_globalMemoryAccessCount
      );
    }

    // Log Registers
    p = _buf;

    SysUInt r;
    SysUInt modifiedRegisters = 0;
    for (r = 0; r < 3; r++)
    {
      bool first = true;
      UInt32 regs;
      UInt32 type;

      memcpy(p, ";   ", 4); p += 4;

      switch (r)
      {
        case 0: regs = _modifiedGpRegisters ; type = REG_GPN; memcpy(p, "GP :", 4); p += 4; break;
        case 1: regs = _modifiedMmRegisters ; type = REG_MM ; memcpy(p, "MM :", 4); p += 4; break;
        case 2: regs = _modifiedXmmRegisters; type = REG_XMM; memcpy(p, "XMM:", 4); p += 4; break;
      }

      for (i = 0; i < NUM_REGS; i++)
      {
        if ((regs & (1 << i)) != 0)
        {
          if (!first) { *p++ = ','; *p++ = ' '; }
          p = Logger::dumpRegister(p, (UInt8)type, (UInt8)i);
          first = false;
          modifiedRegisters++;
        }
      }
      *p++ = '\n';
    }
    *p = '\0';

    logger->logFormat(";\n");
    logger->logFormat("; Modified registers (%u):\n",
      (unsigned int)modifiedRegisters);
    logger->log(_buf);
  }

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
  memset(&_state, 0, sizeof(_state));

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

      _state.gp[reg & 0x0F] = v;
      args[i] = VARIABLE_TYPE_NONE;
    }
  }

  // Stack arguments
  bool ltr = _cconvArgumentsDirection == ARGUMENT_DIR_LEFT_TO_RIGHT;
  SysInt istart = ltr ? 0 : (SysInt)count - 1;
  SysInt iend   = ltr ? (SysInt)count : -1;
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

        _state.gp[reg & 0x0F] = v;
        args[i] = VARIABLE_TYPE_NONE;
      }
      else if (isFloatArgument(a))
      {
        UInt8 reg = _cconvArgumentsXmm[i] | REG_XMM;
        Variable* v = _variables[i];

        v->setAll(a, 0, VARIABLE_STATE_REGISTER, 20, reg, NO_REG, 0);
        _allocReg(reg, v);

        _state.xmm[reg & 0x0F] = v;
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

        _state.gp[reg & 0x0F] = v;
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

        _state.xmm[reg & 0x0F] = v;
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

Variable* Function::newVariable(UInt8 type, UInt8 priority, UInt8 preferredRegisterCode)
{
  Variable* v;

  // First look to unused variables
  SysUInt i;
  for (i = 0; i < _variables.length(); i++)
  {
    v = _variables[i];
    if (v->refCount() == 0 && v->reusable() && v->type() == type) 
    {
      v->_preferredRegisterCode = preferredRegisterCode;
      v->_priority = priority;
      return v;
    }
  }

  // If variable can't be reused, create new one.
  v = compiler()->newObject<Variable>(this, type);
  v->_preferredRegisterCode = preferredRegisterCode;
  v->_priority = priority;
  _variables.append(v);

  // Alloc register if priority is zero
  if (priority == 0) alloc(v);

  return v;
}

bool Function::alloc(Variable* v, UInt8 mode, UInt8 preferredRegisterCode)
{
  ASMJIT_ASSERT(compiler() == v->compiler());

  UInt32 i;

  // Preferred register code
  UInt8 pref = (preferredRegisterCode != NO_REG) 
    ? preferredRegisterCode 
    : v->_preferredRegisterCode;

  // Last register code
  UInt8 home = v->homeRegisterCode();

  // New register code.
  UInt8 code = NO_REG;

  // Spill candidate.
  Variable* spillCandidate = NULL;

  // --------------------------------------------------------------------------
  // [Already Allocated]
  // --------------------------------------------------------------------------

  // Go away if variable is already allocated
  if (v->state() == VARIABLE_STATE_REGISTER)
  {
    UInt8 oldIndex = v->registerCode() & 0xF;
    UInt8 newIndex = pref & 0xF;

    // Preferred register is none or same as currently allocated one, this is
    // best case
    if (pref == NO_REG || oldIndex == newIndex)
    {
      _postAlloc(v, mode);
      return true;
    }

    if (v->type() == VARIABLE_TYPE_INT32 || v->type() == VARIABLE_TYPE_INT64)
    {
      Variable* other = _state.gp[newIndex];

      if (other->priority() == 0)
      {
        // TODO: Error handling
        ASMJIT_ASSERT(0);
      }

      // Exchange instead of spill/alloc
      _exchangeGp(v, mode, other);
      _postAlloc(v, mode);
      return true;
    }
  }

  // --------------------------------------------------------------------------
  // [Find Unused GP]
  // --------------------------------------------------------------------------

  if (v->type() == VARIABLE_TYPE_INT32 || v->type() == VARIABLE_TYPE_INT64)
  {
    // preferred register
    if (pref != NO_REG)
    {
      // esp/rsp can't be never allocated
      ASMJIT_ASSERT((pref & REGCODE_MASK) != RID_ESP);

      if ((_usedGpRegisters & (1U << (pref & REGCODE_MASK))) == 0)
      {
        code = pref;
      }
      else
      {
        // Spill register we need
        spillCandidate = _state.gp[pref & REGCODE_MASK];

        // Can't alloc register that is manually masked (marked as used but
        // no variable exists). Yeah, this is possible, but not recommended.
        if (spillCandidate == NULL)
        {
          // TODO: Error handling
          ASMJIT_ASSERT(0);
        }

        // Jump to spill part of allocation
        goto L_spill;
      }
    }

    // Home register code
    if (code == NO_REG && home != NO_REG)
    {
      if ((_usedGpRegisters & (1U << (home & REGCODE_MASK))) == 0)
      {
        code = home;
      }
    }

    // We start from 1, because EAX/RAX register is sometimes explicitly 
    // needed. So we trying to prevent register reallocation.
    if (code == NO_REG)
    {
      for (i = 1; i < NUM_REGS; i++)
      {
        UInt32 mask = (1U << i);
        if ((_usedGpRegisters & mask) == 0 && (i != RID_EBP || allocableEbp()) && i != RID_ESP)
        {
          // Convenience to alloc registers from positions 0 to 15
          if (code != NO_REG && (_cconvPreservedGp & mask) == 1) continue;

          if (v->type() == VARIABLE_TYPE_INT32)
            code = i | REG_GPD;
          else
            code = i | REG_GPQ;

          // If current register is preserved, we should try to find different
          // one that is not. This can save one push / pop in prolog / epilog.
          if ((_cconvPreservedGp & mask) == 0) break;
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

  // --------------------------------------------------------------------------
  // [Find Unused MM]
  // --------------------------------------------------------------------------

  else if (v->type() == VARIABLE_TYPE_MM)
  {
    // preferred register
    if (pref != NO_REG)
    {
      if ((_usedMmRegisters & (1U << (pref & 0x7))) == 0)
      {
        code = pref;
      }
      else
      {
        // Spill register we need
        spillCandidate = _state.mm[pref & REGCODE_MASK];

        // Can't alloc register that is manually masked (marked as used but
        // no variable exists). Yeah, this is possible, but not recommended.
        if (spillCandidate == NULL)
        {
          // TODO: Error handling
          ASMJIT_ASSERT(0);
        }

        // Jump to spill part of allocation
        goto L_spill;
      }
    }

    // Home register code
    if (code == NO_REG && home != NO_REG)
    {
      if ((_usedMmRegisters & (1U << (home & REGCODE_MASK))) == 0)
      {
        code = home;
      }
    }

    if (code == NO_REG)
    {
      for (i = 0; i < 8; i++)
      {
        UInt32 mask = (1U << i);
        if ((_usedMmRegisters & mask) == 0)
        {
          code = i | REG_MM;
          break;
        }
      }
    }
  }

  // --------------------------------------------------------------------------
  // [Find Unused XMM]
  // --------------------------------------------------------------------------

  else if (v->type() == VARIABLE_TYPE_XMM)
  {
    // preferred register
    if (pref != NO_REG)
    {
      if ((_usedXmmRegisters & (1U << (pref & REGCODE_MASK))) == 0)
      {
        code = pref;
      }
      else
      {
        // Spill register we need
        spillCandidate = _state.xmm[pref & REGCODE_MASK];

        // Can't alloc register that is manually masked (marked as used but
        // no variable exists). Yeah, this is possible, but not recommended.
        if (spillCandidate == NULL)
        {
          // TODO: Error handling
          ASMJIT_ASSERT(0);
        }

        // Jump to spill part of allocation
        goto L_spill;
      }
    }

    // Home register code
    if (code == NO_REG && home != NO_REG)
    {
      if ((_usedXmmRegisters & (1U << (home & REGCODE_MASK))) == 0)
      {
        code = home;
      }
    }

    if (code == NO_REG)
    {
      for (i = 0; i < NUM_REGS; i++)
      {
        UInt32 mask = (1U << i);
        if ((_usedXmmRegisters & mask) == 0)
        {
          // Convenience to alloc registers from positions 0 to 15
          if (code != NO_REG && (_cconvPreservedXmm & mask) == 1) continue;

          code = i | REG_XMM;

          // If current register is preserved, we should try to find different
          // one that is not. This can save one push / pop in prolog / epilog.
          if ((_cconvPreservedXmm & mask) == 0) break;
        }
      }
    }
  }

  // --------------------------------------------------------------------------
  // [Spill]
  // --------------------------------------------------------------------------

  // If register is still not found, spill some variable
  if (code == NO_REG)
  {
    if (!spillCandidate) spillCandidate = _getSpillCandidate(v->type());

    // Spill candidate not found.
    if (!spillCandidate)
    {
      // TODO: Error handling
      ASMJIT_ASSERT(0);
    }

L_spill:

    // Prevented variables can't be spilled. _getSpillCandidate() never returns
    // prevented variables, but when jumping to L_spill it can happen.
    if (isPrevented(spillCandidate))
    {
      // TODO: Error handling
      ASMJIT_ASSERT(0);
    }

    // Can't alloc register that is used in other variable and its priority is
    // zero. Zero priority variable can't be spilled.
    if (spillCandidate->priority() == 0)
    {
      // TODO: Error handling
      ASMJIT_ASSERT(0);
    }

    code = spillCandidate->registerCode();
    spill(spillCandidate);
  }

  // --------------------------------------------------------------------------
  // [Finish]
  // --------------------------------------------------------------------------

  _allocAs(v, mode, code);
  _postAlloc(v, mode);
  return true;
}

bool Function::spill(Variable* v)
{
  ASMJIT_ASSERT(compiler() == v->compiler());

  removePrevented(v);

  if (v->state() == VARIABLE_STATE_UNUSED) return true;
  if (v->state() == VARIABLE_STATE_MEMORY) return true;
  if (v->state() == VARIABLE_STATE_REGISTER)
  {
    if (v->priority() == 0)
    {
      return false;
    }

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
        v->_globalMemoryAccessCount++;
      }

      v->setChanged(false);
    }

    _freeReg(v->registerCode());
    v->_registerCode = NO_REG;

    v->_state = VARIABLE_STATE_MEMORY;
    v->_spillCount++;
    v->_globalSpillCount++;
  }

  return true;
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

  v->_spillCount = 0;
  v->_registerAccessCount = 0;
  v->_memoryAccessCount = 0;

  v->_lifeId++;

  v->_preferredRegisterCode = NO_REG;
  v->_homeRegisterCode = NO_REG;
  v->_priority = 10;
  v->_changed = 0;

  v->_allocFn = NULL;
  v->_spillFn = NULL;
  v->_dataPtr = NULL;
  v->_dataInt = 0;
}

void Function::spillAll()
{
  _spillAll(0, 16+8+16);
}

void Function::spillAllGp()
{
  _spillAll(0, 16);
}

void Function::spillAllMm()
{
  _spillAll(16, 8);
}

void Function::spillAllXmm()
{
  _spillAll(16+8, 16);
}

void Function::_spillAll(SysUInt start, SysUInt end)
{
  SysUInt i;

  for (i = start; i < end; i++)
  {
    Variable* v = _state.regs[i];
    if (v) spill(v);
  }
}

void Function::spillRegister(const BaseReg& reg)
{
  SysUInt i = reg.index();
  Variable *v;

  switch (reg.type())
  {
    case REG_GPB:
    case REG_GPW:
    case REG_GPD:
    case REG_GPQ:
      v = _state.gp[i];
      break;
    case REG_MM:
      v = _state.mm[i];
      break;
    case REG_XMM:
      v = _state.xmm[i];
      break;
    default:
      return;
  }

  if (v) spill(v);
}

static SysInt getFreeRegs(UInt32 regs, SysUInt max)
{
  SysUInt n = 0;
  SysUInt i;
  UInt32 mask = 1;

  for (i = 0; i < max; i++, mask <<= 1)
  {
    if ((regs & mask) == 0) n++;
  }
  return n;
}

SysInt Function::numFreeGp() const
{
  SysInt n = getFreeRegs(_usedGpRegisters, NUM_REGS);

  if ((_usedGpRegisters & (1 << RID_ESP)) == 0) n--;
  if ((_usedGpRegisters & (1 << RID_EBP)) == 0 && !allocableEbp()) n--;

  return n;
}

SysInt Function::numFreeMm() const
{
  return getFreeRegs(_usedMmRegisters, 8);
}

SysInt Function::numFreeXmm() const
{
  return getFreeRegs(_usedXmmRegisters, NUM_REGS);
}

bool Function::isPrevented(Variable* v)
{
  return _usePrevention && _prevented.indexOf(v) != (SysUInt)-1;
}

void Function::addPrevented(Variable* v)
{
  if (!_usePrevention) return;

  SysUInt i = _prevented.indexOf(v);
  if (i == (SysUInt)-1) _prevented.append(v);
}

void Function::removePrevented(Variable* v)
{
  if (!_usePrevention) return;

  SysUInt i = _prevented.indexOf(v);
  if (i != (SysUInt)-1) _prevented.removeAt(i);
}

void Function::clearPrevented()
{
  _prevented.clear();
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
            (!isPrevented(v)))
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
            (!isPrevented(v)))
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
            (!isPrevented(v)))
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
  UInt8 old = v->_registerCode;

  v->_state = VARIABLE_STATE_REGISTER;
  v->_registerCode = code;

  _allocReg(code, v);

  if (v->isCustom())
  {
    if (v->_allocFn && mode != VARIABLE_ALLOC_WRITE) v->_allocFn(v);
  }
  else if (copy && mode != VARIABLE_ALLOC_WRITE)
  {
    switch (v->type())
    {
      case VARIABLE_TYPE_INT32:
      {
        Register dst = mk_gpd(v->_registerCode);
        if (old != NO_REG)
          compiler()->mov(dst, mk_gpd(old));
        else
          compiler()->mov(dst, *v->_memoryOperand);        
        break;
      }

#if defined(ASMJIT_X64)
      case VARIABLE_TYPE_INT64:
      {
        Register dst = mk_gpq(v->_registerCode);
        if (old != NO_REG)
          compiler()->mov(dst, mk_gpq(old));
        else
          compiler()->mov(dst, *v->_memoryOperand);
        break;
      }
#endif // ASMJIT_X64

      case VARIABLE_TYPE_FLOAT:
        // TODO: NOT IMPLEMENTED
        break;

      case VARIABLE_TYPE_DOUBLE:
        // TODO: NOT IMPLEMENTED
        break;

      case VARIABLE_TYPE_MM:
      {
        MMRegister dst = mk_mm(v->_registerCode);
        if (old != NO_REG)
          compiler()->movq(dst, mk_mm(old));
        else
          compiler()->movq(dst, *v->_memoryOperand);
        break;
      }

      case VARIABLE_TYPE_XMM:
      {
        XMMRegister dst = mk_xmm(v->_registerCode);
        if (old != NO_REG)
          compiler()->movdqa(dst, mk_xmm(old));
        // Alignment is not guaranted for naked functions in 32 bit mode
        // FIXME: And what about 64 bit mode ?
        else if (naked())
          compiler()->movdqu(dst, *v->_memoryOperand);
        else
          compiler()->movdqa(dst, *v->_memoryOperand);
        break;
      }
    }

    if (old != NO_REG)
    {
      v->_registerAccessCount++;
      v->_globalRegisterAccessCount++;
    }
    else
    {
      v->_memoryAccessCount++;
      v->_globalMemoryAccessCount++;
    }
  }
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
      _state.gp[code & 0x0F] = v;
      break;
    case REG_MM:
      useMmRegisters(mask);
      modifyMmRegisters(mask);
      _state.mm[code & 0x0F] = v;
      break;
    case REG_XMM:
      useXmmRegisters(mask);
      modifyXmmRegisters(mask);
      _state.xmm[code & 0x0F] = v;
      break;
  }

  // Set home code, Compiler is able to reuse it again.
  v->_homeRegisterCode = code;
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
      _state.gp[code & 0x0F] = NULL;
      break;
    case REG_MM:
      unuseMmRegisters(mask);
      _state.mm[code & 0x0F] = NULL;
      break;
    case REG_XMM:
      unuseXmmRegisters(mask);
      _state.xmm[code & 0x0F] = NULL;
      break;
  }
}

void Function::_exchangeGp(Variable* v, UInt8 mode, Variable* other)
{
  ASMJIT_ASSERT(v->state() == VARIABLE_STATE_REGISTER);
  ASMJIT_ASSERT(other->state() == VARIABLE_STATE_REGISTER);

  UInt8 code1 = v->registerCode();
  UInt8 code2 = other->registerCode();

  UInt8 type1 = code1 & REGTYPE_MASK;
  UInt8 type2 = code2 & REGTYPE_MASK;

  UInt8 index1 = code1 & REGCODE_MASK;
  UInt8 index2 = code2 & REGCODE_MASK;

  // Make sure that register classes match, we can't exchange for example
  // general purpose register with sse one
  ASMJIT_ASSERT(type1 <= REG_GPQ && type2 <= REG_GPQ);

  Register reg1 = mk_gpn(index1);
  Register reg2 = mk_gpn(index2);

  if (mode == VARIABLE_ALLOC_WRITE)
  {
    // If we are completely rewriting variable, we can use only mov to save
    // 'other'. This can be faster or not than using xchg.
    compiler()->mov(reg1, reg2);
  }
  else
  {
    // Standard exchange instruction supported by x86 architecture.
    compiler()->xchg(reg1, reg2);
  }

  // Swap registers
  v->_registerCode = index2 | type1;
  other->_registerCode = index1 | type2;

  // Update state
  _state.gp[index1] = other;
  _state.gp[index2] = v;

  // Statistics
  v->_registerAccessCount++;
  v->_globalRegisterAccessCount++;

  other->_registerAccessCount++;
  other->_globalRegisterAccessCount++;
}

void Function::_postAlloc(Variable* v, UInt8 mode)
{
  // Mark variable as changed if needed
  if ((mode & VARIABLE_ALLOC_WRITE) != 0) v->_changed = true;

  // Add variable to prevented ones. This will be cleared when instruction
  // is emitted.
  addPrevented(v);
}

State *Function::saveState()
{
  State* s = compiler()->newObject<State>(this);
  State::saveFunctionState(&s->_data, this);
  return s;
}

void Function::restoreState(State* s)
{
  ASMJIT_ASSERT(s->_function == this);

  // Stop prevention
  _usePrevention = false;

  // make local copy of function state
  State::Data f_d;
  State::Data& s_d = s->_data;

  State::saveFunctionState(&f_d, this);

  SysInt base;
  SysInt i;

  // Spill registers
  for (base = 0, i = 0; i < 16+8+16; i++)
  {
    if (i == 16 || i == 24) base = i;

    State::Entry* from = &f_d.regs[i];
    State::Entry* to   = &s_d.regs[i];

    Variable* from_v = from->v;
    Variable* to_v = to->v;

    if (from_v != to_v)
    {
      UInt8 regIndex = (UInt8)(i - base);

      // Spill register
      if (from_v != NULL) 
      {
        // Here is important step. It can happen that variable that was saved
        // in state currently not exists. We can check for it by comparing
        // saved lifeId with current variable lifeIf. If IDs are different,
        // variables not match. Another optimization is that we will spill 
        // variable only if it's used in context we need. If it's unused, there
        // is no reason to save it on the stack.
        if (from->lifeId != from_v->lifeId() || from_v->state() == VARIABLE_STATE_UNUSED)
        {
          // Optimization, do not spill it, we can simply abandon it
          _freeReg(getVariableRegisterCode(from_v->type(), regIndex));

          // TODO: Is this right way? We spilled variable manually, but I'm
          // not sure if I can set its state to MEMORY.

          // This will prevent to reset unused variable to be memory variable.
          if (from_v->state() == VARIABLE_STATE_REGISTER)
          {
            from_v->_state = VARIABLE_STATE_MEMORY;
          }
        }
        else
        {
          // Variables match, do normal spill
          spill(from_v);
        }
      }
    }
  }

  // Alloc registers
  for (base = 0, i = 0; i < 16+8+16; i++)
  {
    if (i == 16 || i == 24) base = i;

    State::Entry* from = &f_d.regs[i];
    State::Entry* to   = &s_d.regs[i];

    Variable* from_v = from->v;
    Variable* to_v = to->v;

    if (from_v != to_v)
    {
      UInt8 regIndex = (UInt8)(i - base);

      // Alloc register
      if (to_v != NULL) 
      {
        UInt8 code = getVariableRegisterCode(to_v->type(), regIndex);
        _allocAs(to_v, VARIABLE_ALLOC_READ, code);
      }
    }

    if (to_v)
    {
      to_v->_changed = to->changed;
    }
  }

  // Update masks
  _usedGpRegisters  = s->_data.usedGpRegisters;
  _usedMmRegisters  = s->_data.usedMmRegisters;
  _usedXmmRegisters = s->_data.usedXmmRegisters;

  // Restore and clear prevention
  _usePrevention = false;
  clearPrevented();
}

void Function::setState(State* s)
{
  ASMJIT_ASSERT(s->_function == this);

  for (SysUInt i = 0; i < 16+8+16; i++)
  {
    Variable* old = _state.regs[i];
    Variable* v = s->_data.regs[i].v;

    // This method is dirrerent to restoreState(), because we are not spilling
    // and allocating registers. We need to actualize all variables that was
    // changed by setState(). This means allocated and spilled variables. This
    // is reason why we are modifiyng 'old'
    if (v != old && old)
    {
      if (old->state() == VARIABLE_STATE_REGISTER)
      {
        old->_state = VARIABLE_STATE_MEMORY;
        old->_registerCode = NO_REG;
        old->_changed = false;
      }
    }

    if (v)
    {
      v->_state = s->_data.regs[i].state;
      v->_changed = s->_data.regs[i].changed;
    }

    _state.regs[i] = v;
  }

  // Update masks
  _usedGpRegisters  = s->_data.usedGpRegisters;
  _usedMmRegisters  = s->_data.usedMmRegisters;
  _usedXmmRegisters = s->_data.usedXmmRegisters;

  // Clear prevention
  s->_function->clearPrevented();
}

void Function::_jmpAndRestore(Compiler* c, Label* label)
{
  JumpAndRestore* jr = (JumpAndRestore*)label->_compilerData;
  Function* f = jr->from->_function;

  // Save internal state (we don't want to modify it)
  State backup(c, f);
  State::saveFunctionState(&backup._data, f);

  do {
    // Working variables we need
    State* from = jr->from;
    State* to = jr->to;

    bool isJmp = jr->instruction->code() == INST_JMP;
    bool modifiedState;

    // Emit code to the end (need to save old position) or if instructions is
    // simple jmp()( we can inline state restore before it.
    Emittable* old = c->setCurrent(isJmp ? jr->instruction->prev() : c->lastEmittable());
    Emittable* first = c->current();
    f->setState(from);
    f->restoreState(to);
    Emittable* last = c->current();
    modifiedState = old != last;

    // If state was modified and it isn't a jmp(), redirect jump
    if (modifiedState && !isJmp)
    {
      Label* L_block = c->newLabel();

      // Bind label to start of restore block
      c->setCurrent(first);
      c->align(sizeof(SysInt));
      c->bind(L_block);

      // Jump back from end of the block
      c->setCurrent(last);
      c->jmp(label);

      // Patch instruction jump target to our new label
      jr->instruction->_o[0] = L_block;
    }

    // Set pointer back
    c->setCurrent(old);

    // Next JumpAndRestore record
    jr = jr->next;
  } while (jr);

  // Clear data, this is not longer needed
  label->_compilerData = NULL;

  // Restore internal state
  f->setState(&backup);
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

  const CpuInfo* ci = cpuInfo();

  // First bind label (Function::_exitLabel) before the epilog
  if (_label) a.bind(_label);

  // Emms
  if (f->emms()) a.emms();

  // Sfence / Lfence / Mfence
  if ( f->sfence() && !f->lfence()) a.sfence(); // Only sfence
  if (!f->sfence() &&  f->lfence()) a.lfence(); // Only lfence
  if ( f->sfence() &&  f->lfence()) a.mfence(); // MFence == SFence & LFence

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
    bool emitLeave = (f->optimizedPrologEpilog() &&  ci->vendorId == CpuInfo::Vendor_AMD);

    if (emitLeave)
    {
      a.leave();
    }
    else
    {
      a.mov(nsp, nbp);
      a.pop(nbp);
    }
  }

  // Return using correct instruction
  if (f->calleePopsStack())
    a.ret((Int16)f->argumentsStackSize());
  else
    a.ret();
}

// ============================================================================
// [AsmJit::Target]
// ============================================================================

Target::Target(Compiler* c, Label* target) : 
  Emittable(c, EMITTABLE_TARGET), 
  _target(target)
{
}

Target::~Target()
{
}

void Target::emit(Assembler& a)
{
  a.bind(_target);
}

// ============================================================================
// [AsmJit::JumpTable]
// ============================================================================

JumpTable::JumpTable(Compiler* c) :
  Emittable(c, EMITTABLE_TARGET),
  _target(c->newLabel())
{
}

JumpTable::~JumpTable()
{
}

void JumpTable::emit(Assembler& a)
{
}

void JumpTable::postEmit(Assembler& a)
{
  a.align(sizeof(SysInt));

#if defined(ASMJIT_X64)
  // help with RIP addressing
  a._embedLabel(_target);
#endif

  a.bind(_target);

  SysUInt i, len = _labels.length();
  for (i = 0; i < len; i++)
  {
    Label* label = _labels[i];
    if (label)
      a._embedLabel(label);
    else
      a.dsysint(0);
  }
}

Label* JumpTable::addLabel(Label* target, SysInt pos)
{
  if (!target) target = compiler()->newLabel();

  if (pos != -1)
  {
    while (_labels.length() <= (SysUInt)pos) _labels.append(NULL);
    _labels[(SysUInt)pos] = target;
  }
  else
  {
    _labels.append(target);
  }
    
  return target;
}

// ============================================================================
// [AsmJit::Compiler - Construction / Destruction]
// ============================================================================

Compiler::Compiler() ASMJIT_NOTHROW :
  _first(NULL),
  _last(NULL),
  _current(NULL),
  _currentFunction(NULL),
  _labelIdCounter(1)
{
  _jumpTableLabel = newLabel();
}

Compiler::~Compiler() ASMJIT_NOTHROW
{
  delAll(_first);
}

// ============================================================================
// [AsmJit::Compiler - Buffer]
// ============================================================================

void Compiler::clear()
{
  delAll(_first);
  _first = NULL;
  _last = NULL;

  _zone.freeAll();

  _operands.clear();
  _jumpTableLabel = newLabel();
  _jumpTableData.clear();
}

void Compiler::free()
{
  clear();
  _operands.free();
  _jumpTableData.free();
}

// ============================================================================
// [AsmJit::Compiler - Emittables]
// ============================================================================

void Compiler::addEmittable(Emittable* emittable)
{
  ASMJIT_ASSERT(emittable != NULL);
  ASMJIT_ASSERT(emittable->_prev == NULL);
  ASMJIT_ASSERT(emittable->_next == NULL);

  if (_current == NULL)
  {
    if (!_first)
    {
      _first = emittable;
      _last = emittable;
    }
    else
    {
      emittable->_next = _first;
      _first->_prev = emittable;
      _first = emittable;
    }
  }
  else
  {
    Emittable* prev = _current;
    Emittable* next = _current->_next;

    emittable->_prev = prev;
    emittable->_next = next;

    prev->_next = emittable;
    if (next)
      next->_prev = emittable;
    else
      _last = emittable;
  }

  _current = emittable;
}

void Compiler::removeEmittable(Emittable* emittable)
{
  Emittable* prev = emittable->_prev;
  Emittable* next = emittable->_next;

  if (_first == emittable) { _first = next; } else { prev->_next = next; }
  if (_last  == emittable) { _last  = prev; } else { next->_prev = prev; }

  emittable->_prev = NULL;
  emittable->_next = NULL;

  if (emittable == _current) _current = prev;
}

Emittable* Compiler::setCurrent(Emittable* current)
{
  Emittable* old = _current;
  _current = current;
  return old;
}

// ============================================================================
// [AsmJit::Compiler - Logging]
// ============================================================================

void Compiler::comment(const char* fmt, ...)
{
  char buf[1024];
  char* p = buf;

  if (fmt)
  {
    *p++ = ';';
    *p++ = ' ';

    va_list ap;
    va_start(ap, fmt);
    p += vsnprintf(p, 1020, fmt, ap);
    va_end(ap);
  }

  *p++ = '\n';
  *p   = '\0';

  addEmittable(newObject<Comment>(buf));
}

// ============================================================================
// [AsmJit::Compiler - Function Builder]
// ============================================================================

Function* Compiler::newFunction_(UInt32 cconv, const UInt32* args, SysUInt count)
{
  ASMJIT_ASSERT(_currentFunction == NULL);

  Function* f = _currentFunction = newObject<Function>();
  f->setPrototype(cconv, args, count);

  addEmittable(f);

  Prolog* e = newProlog(f);
  e->_label = f->_prologLabel;

  return f;
}

Function* Compiler::endFunction()
{
  ASMJIT_ASSERT(_currentFunction != NULL);
  Function* f = _currentFunction;

  // Clear prevention (this is probably not needed anymore)
  f->clearPrevented();

  Epilog* e = newEpilog(f);
  e->_label = f->_exitLabel;

  _currentFunction = NULL;
  return f;
}

Prolog* Compiler::newProlog(Function* f)
{
  Prolog* e = newObject<Prolog>(f);
  addEmittable(e);
  return e;
}

Epilog* Compiler::newEpilog(Function* f)
{
  Epilog* e = newObject<Epilog>(f);
  addEmittable(e);
  return e;
}

// ==========================================================================
// [AsmJit::Compiler - Registers allocator / Variables]
// ==========================================================================

Variable* Compiler::argument(SysInt i)
{
  return currentFunction()->argument(i);
}

Variable* Compiler::newVariable(UInt8 type, UInt8 priority, UInt8 preferredRegister)
{
  return currentFunction()->newVariable(type, priority, preferredRegister);
}

bool Compiler::alloc(Variable* v, UInt8 mode, UInt8 preferredRegister)
{
  return currentFunction()->alloc(v, mode, preferredRegister);
}

bool Compiler::spill(Variable* v)
{
  return currentFunction()->spill(v);
}

void Compiler::unuse(Variable* v)
{
  return currentFunction()->unuse(v);
}

void Compiler::spillAll()
{
  return currentFunction()->spillAll();
}

void Compiler::spillAllGp()
{
  return currentFunction()->spillAllGp();
}

void Compiler::spillAllMm()
{
  return currentFunction()->spillAllMm();
}

void Compiler::spillAllXmm()
{
  return currentFunction()->spillAllXmm();
}

void Compiler::spillRegister(const BaseReg& reg)
{
  return currentFunction()->spillRegister(reg);
}

SysInt Compiler::numFreeGp() const
{
  return _currentFunction->numFreeGp();
}

SysInt Compiler::numFreeMm() const
{
  return _currentFunction->numFreeMm();
}

SysInt Compiler::numFreeXmm() const
{
  return _currentFunction->numFreeXmm();
}

bool Compiler::isPrevented(Variable* v)
{
  return currentFunction()->isPrevented(v);
}

void Compiler::addPrevented(Variable* v)
{
  return currentFunction()->addPrevented(v);
}

void Compiler::removePrevented(Variable* v)
{
  return currentFunction()->removePrevented(v);
}

void Compiler::clearPrevented()
{
  currentFunction()->clearPrevented();
}

// ==========================================================================
// [AsmJit::Compiler - State]
// ==========================================================================

State* Compiler::saveState()
{
  return currentFunction()->saveState();
}

void Compiler::restoreState(State* state)
{
  currentFunction()->restoreState(state);
}

void Compiler::setState(State* state)
{
  currentFunction()->setState(state);
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
// [AsmJit::Compiler - Jump Table]
// ============================================================================

JumpTable* Compiler::newJumpTable()
{
  JumpTable* e = newObject<JumpTable>();
  addEmittable(e);
  return e;
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
// [AsmJit::Compiler - Jumps / Calls]
// ============================================================================

void Compiler::jmp(void* target)
{
  jmp(ptr(_jumpTableLabel, _addTarget(target)));
}

void Compiler::call(void* target)
{
  call(ptr(_jumpTableLabel, _addTarget(target)));
}

void Compiler::jumpToTable(JumpTable* jt, const Register& index)
{
#if defined(ASMJIT_X64)
  // 64 bit mode: Complex address can't be used, because SIB byte not allows
  // to use RIP (relative addressing). SIB byte is always generated for 
  // complex addresses.
  // address form: [jumpTable + index * 8]
  shl(index, imm(3));                // index *= 8
  add(index, ptr(jt->target(), -8)); // index += jumpTable base address
  jmp(ptr(index));                   // jmp [index]
#else
  // 32 bit mode: Straighforward implementation, we are using complex address
  // form: [jumpTable + index * 4]
  jmp(ptr(jt->target(), index, TIMES_4));
#endif
}

SysInt Compiler::_addTarget(void* target)
{
  SysInt id = _jumpTableData.length() * sizeof(SysInt);
  _jumpTableData.append(target);
  return id;
}

// jmpAndRestore

void Compiler::_jmpAndRestore(UInt32 code, Label* label, State* state)
{
  JumpAndRestore* jr = (JumpAndRestore*)_zoneAlloc(sizeof(JumpAndRestore));
  jr->next = (JumpAndRestore*)label->_compilerData;
  jr->from = currentFunction()->saveState();
  jr->to = state;
  label->_compilerData = (void*)jr;

  __emitX86(code, label);
  jr->instruction = reinterpret_cast<Instruction*>(_current);
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
  addEmittable(newObject<Instruction>(code, o1, o2, o3));

  // We can clear last used register, because instruction was emitted.
  if (currentFunction()) currentFunction()->clearPrevented();
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
  addEmittable(e);
}

// ============================================================================
// [AsmJit::Compiler - Align]
// ============================================================================

void Compiler::align(SysInt m)
{
  addEmittable(newObject<Align>(m));
}

// ============================================================================
// [AsmJit::Compiler - Bind]
// ============================================================================

void Compiler::bind(Label* label)
{
  // JumpAndRestore is delayed to bind()
  if (label->_compilerData) Function::_jmpAndRestore(this, label);

  addEmittable(newObject<Target>(label));
}

// ============================================================================
// [AsmJit::Compiler - Make]
// ============================================================================

void* Compiler::make(UInt32 allocType)
{
  Assembler a;
  a._properties = _properties;
  serialize(a);

  if (a.error())
  {
    if (_logger)
    {
      _logger->logFormat("; Compiler failed (error %u).\n", (unsigned int)a.error());
    }

    setError(a.error());
    return NULL;
  }
  else
  {
    if (_logger)
    {
      _logger->logFormat("; Compiler successful (wrote %u bytes).\n", (unsigned int)a.codeSize());
      _logger->log("\n");
    }

    return a.make(allocType);
  }
}

// logger switcher used in Compiler::serialize().
struct ASMJIT_HIDDEN LoggerSwitcher
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

void Compiler::serialize(Assembler& a)
{
  LoggerSwitcher loggerSwitcher(&a, this);
  Emittable* cur;

  // Prepare (prepare action can append emittable)
  for (cur = _first; cur; cur = cur->next()) cur->prepare();

  // Emit and postEmit
  for (cur = _first; cur; cur = cur->next()) cur->emit(a);
  for (cur = _first; cur; cur = cur->next()) cur->postEmit(a);

  // Jump table
  SysUInt i, len;
  a.bind(_jumpTableLabel);

  len = _jumpTableData.length();
  for (i = 0; i < len; i++)
  {
    a.dptr(_jumpTableData[i]);
  }
}

} // AsmJit namespace

// [Warnings-Pop]
#include "WarningsPop.h"
