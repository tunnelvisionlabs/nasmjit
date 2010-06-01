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

// We are using sprintf() here.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "Assembler.h"
#include "Compiler.h"
#include "CpuInfo.h"
#include "Logger.h"
#include "Util_p.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [Helpers - Essential]
// ============================================================================

// From http://graphics.stanford.edu/~seander/bithacks.html .
static inline uint32_t bitCount(uint32_t x)
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  return ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

template<typename T>
static inline T alignTo16Bytes(const T& x)
{
  return (x + (T)15) & (T)~15;
}

// ============================================================================
// [Helpers - Logging]
// ============================================================================

// Defined in AssemblerX86X64.cpp.
ASMJIT_HIDDEN char* dumpRegister(char* buf, uint32_t type, uint32_t index) ASMJIT_NOTHROW;
ASMJIT_HIDDEN char* dumpOperand(char* buf, const Operand* op) ASMJIT_NOTHROW;

// ============================================================================
// [Helpers - Variables]
// ============================================================================

static inline uint32_t getVarIndex(const VarAllocRecord* list, uint32_t count, VarData* vdata)
{
  for (uint32_t i = 0; i < count; i++) { if (list->vdata == vdata) return i; }
  return INVALID_VALUE;
}

struct VariableInfo
{
  enum CLASS_INFO
  {
    CLASS_NONE   = 0x00,
    CLASS_GP     = 0x01,
    CLASS_X87    = 0x02,
    CLASS_MM     = 0x04,
    CLASS_XMM    = 0x08,
    CLASS_SP_FP  = 0x10,
    CLASS_DP_FP  = 0x20,
    CLASS_VECTOR = 0x40
  };

  uint32_t code;
  uint8_t size;
  uint8_t clazz;
  uint8_t reserved;
  char name[9];
};

#define C(c) VariableInfo::CLASS_##c
static const VariableInfo variableInfo[] =
{
  /*  0 */ { REG_TYPE_GPD   , 4 , C(GP)                        , 0, "GP.D"        },
  /*  1 */ { REG_TYPE_GPQ   , 8 , C(GP)                        , 0, "GP.Q"        },
  /*  2 */ { REG_TYPE_X87   , 4 , C(X87) | C(SP_FP)            , 0, "X87"         },
  /*  3 */ { REG_TYPE_X87   , 4 , C(X87) | C(SP_FP)            , 0, "X87.F"       },
  /*  4 */ { REG_TYPE_X87   , 8 , C(X87) | C(DP_FP)            , 0, "X87.D"       },
  /*  5 */ { REG_TYPE_MM    , 8 , C(MM)                        , 0, "MM"          },
  /*  6 */ { REG_TYPE_XMM   , 16, C(XMM)                       , 0, "XMM"         },
  /*  7 */ { REG_TYPE_XMM   , 4 , C(XMM) | C(SP_FP)            , 0, "XMM.1F"      },
  /*  8 */ { REG_TYPE_XMM   , 8 , C(XMM) | C(DP_FP)            , 0, "XMM.1D"      },
  /*  9 */ { REG_TYPE_XMM   , 16, C(XMM) | C(SP_FP) | C(VECTOR), 0, "XMM.4F"      },
  /* 10 */ { REG_TYPE_XMM   , 16, C(XMM) | C(DP_FP) | C(VECTOR), 0, "XMM.2D"      }
};
#undef C

static uint32_t getVariableSize(uint32_t type)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableInfo));
  return variableInfo[type].size;
}

static uint32_t getVariableRegisterCode(uint32_t type, uint32_t index)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableInfo));
  return variableInfo[type].code | index;
}

static bool isVariableInteger(uint32_t type)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableInfo));
  return (variableInfo[type].clazz & VariableInfo::CLASS_GP) != 0;
}

static bool isVariableFloat(uint32_t type)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableInfo));
  return (variableInfo[type].clazz & (VariableInfo::CLASS_SP_FP | VariableInfo::CLASS_DP_FP)) != 0;
}

// ============================================================================
// [Helpers - Emittables]
// ============================================================================

static void delAll(Emittable* first) ASMJIT_NOTHROW
{
  Emittable* cur = first;

  while (cur)
  {
    Emittable* next = cur->getNext();
    cur->~Emittable();
    cur = next;
  }
}

// ============================================================================
// [Helpers - Compiler]
// ============================================================================

template<typename T>
inline T* Compiler_newObject(CompilerCore* self) ASMJIT_NOTHROW
{
  void* addr = self->getZone().zalloc(sizeof(T));
  return new(addr) T(reinterpret_cast<Compiler*>(self));
}

template<typename T, typename P1>
inline T* Compiler_newObject(CompilerCore* self, P1 p1) ASMJIT_NOTHROW
{
  void* addr = self->getZone().zalloc(sizeof(T));
  return new(addr) T(reinterpret_cast<Compiler*>(self), p1);
}

template<typename T, typename P1, typename P2>
inline T* Compiler_newObject(CompilerCore* self, P1 p1, P2 p2) ASMJIT_NOTHROW
{
  void* addr = self->getZone().zalloc(sizeof(T));
  return new(addr) T(reinterpret_cast<Compiler*>(self), p1, p2);
}

template<typename T, typename P1, typename P2, typename P3>
inline T* Compiler_newObject(CompilerCore* self, P1 p1, P2 p2, P3 p3) ASMJIT_NOTHROW
{
  void* addr = self->getZone().zalloc(sizeof(T));
  return new(addr) T(reinterpret_cast<Compiler*>(self), p1, p2, p3);
}

template<typename T, typename P1, typename P2, typename P3, typename P4>
inline T* Compiler_newObject(CompilerCore* self, P1 p1, P2 p2, P3 p3, P4 p4) ASMJIT_NOTHROW
{
  void* addr = self->getZone().zalloc(sizeof(T));
  return new(addr) T(reinterpret_cast<Compiler*>(self), p1, p2, p3, p4);
}

template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
inline T* Compiler_newObject(CompilerCore* self, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) ASMJIT_NOTHROW
{
  void* addr = self->getZone().zalloc(sizeof(T));
  return new(addr) T(reinterpret_cast<Compiler*>(self), p1, p2, p3, p4, p5);
}

// ============================================================================
// [AsmJit::FunctionPrototype]
// ============================================================================

FunctionPrototype::FunctionPrototype() ASMJIT_NOTHROW
{
  // Safe defaults.
  _clear();
}

FunctionPrototype::~FunctionPrototype() ASMJIT_NOTHROW
{
}

void FunctionPrototype::setPrototype(uint32_t callingConvention, const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW
{
  _setCallingConvention(callingConvention);

  if (count > 32) count = 32;
  _setPrototype(args, count);
}

void FunctionPrototype::_clear() ASMJIT_NOTHROW
{
  _callingConvention = CALL_CONV_NONE;
  _calleePopsStack = false;

  _argumentsCount = 0;
  _argumentsDirection = ARGUMENT_DIR_RIGHT_TO_LEFT;
  _argumentsStackSize = 0;

  Util::memset32(_argumentsGP , INVALID_VALUE, ASMJIT_ARRAY_SIZE(_argumentsGP ));
  Util::memset32(_argumentsXMM, INVALID_VALUE, ASMJIT_ARRAY_SIZE(_argumentsXMM));

  _preservedGP = 0;
  _preservedXMM = 0;
}

void FunctionPrototype::_setCallingConvention(uint32_t callingConvention) ASMJIT_NOTHROW
{
  // Safe defaults.
  _clear();

  _callingConvention = callingConvention;

#if defined(ASMJIT_X86)
  // [X86 calling conventions]
  _preservedGP =
    (1 << (REG_EBX & REG_INDEX_MASK)) |
    (1 << (REG_ESP & REG_INDEX_MASK)) |
    (1 << (REG_EBP & REG_INDEX_MASK)) |
    (1 << (REG_ESI & REG_INDEX_MASK)) |
    (1 << (REG_EDI & REG_INDEX_MASK)) ;
  _preservedXMM = 0;

  switch (_callingConvention)
  {
    case CALL_CONV_CDECL:
      break;

    case CALL_CONV_STDCALL:
      _calleePopsStack = true;
      break;

    case CALL_CONV_MSTHISCALL:
      _calleePopsStack = true;
      _argumentsGP[0] = (REG_ECX & REG_INDEX_MASK);
      break;

    case CALL_CONV_MSFASTCALL:
      _calleePopsStack = true;
      _argumentsGP[0] = (REG_ECX & REG_INDEX_MASK);
      _argumentsGP[1] = (REG_EDX & REG_INDEX_MASK);
      break;

    case CALL_CONV_BORLANDFASTCALL:
      _calleePopsStack = true;
      _argumentsDirection = ARGUMENT_DIR_LEFT_TO_RIGHT;
      _argumentsGP[0] = (REG_EAX & REG_INDEX_MASK);
      _argumentsGP[1] = (REG_EDX & REG_INDEX_MASK);
      _argumentsGP[2] = (REG_ECX & REG_INDEX_MASK);
      break;

    case CALL_CONV_GCCFASTCALL_2:
      _calleePopsStack = false;
      _argumentsGP[0] = (REG_ECX & REG_INDEX_MASK);
      _argumentsGP[1] = (REG_EDX & REG_INDEX_MASK);
      break;

    case CALL_CONV_GCCFASTCALL_3:
      _calleePopsStack = false;
      _argumentsGP[0] = (REG_EDX & REG_INDEX_MASK);
      _argumentsGP[1] = (REG_ECX & REG_INDEX_MASK);
      _argumentsGP[2] = (REG_EAX & REG_INDEX_MASK);
      break;

    default:
      // Illegal calling convention.
      ASMJIT_ASSERT(0);
  }
#endif // ASMJIT_X86

#if defined(ASMJIT_X64)
  // [X64 calling conventions]
  switch(_callingConvention)
  {
    case CALL_CONV_X64W:
      _argumentsGP[0] = (REG_RCX  & REG_INDEX_MASK);
      _argumentsGP[1] = (REG_RDX  & REG_INDEX_MASK);
      _argumentsGP[2] = (REG_R8   & REG_INDEX_MASK);
      _argumentsGP[3] = (REG_R9   & REG_INDEX_MASK);

      _argumentsXMM[0] = (REG_XMM0 & REG_INDEX_MASK);
      _argumentsXMM[1] = (REG_XMM1 & REG_INDEX_MASK);
      _argumentsXMM[2] = (REG_XMM2 & REG_INDEX_MASK);
      _argumentsXMM[3] = (REG_XMM3 & REG_INDEX_MASK);

      _preservedGP =
        (1 << (REG_RBX   & REG_INDEX_MASK)) |
        (1 << (REG_RSP   & REG_INDEX_MASK)) |
        (1 << (REG_RBP   & REG_INDEX_MASK)) |
        (1 << (REG_RSI   & REG_INDEX_MASK)) |
        (1 << (REG_RDI   & REG_INDEX_MASK)) |
        (1 << (REG_R12   & REG_INDEX_MASK)) |
        (1 << (REG_R13   & REG_INDEX_MASK)) |
        (1 << (REG_R14   & REG_INDEX_MASK)) |
        (1 << (REG_R15   & REG_INDEX_MASK)) ;
      _preservedXMM =
        (1 << (REG_XMM6  & REG_INDEX_MASK)) |
        (1 << (REG_XMM7  & REG_INDEX_MASK)) |
        (1 << (REG_XMM8  & REG_INDEX_MASK)) |
        (1 << (REG_XMM9  & REG_INDEX_MASK)) |
        (1 << (REG_XMM10 & REG_INDEX_MASK)) |
        (1 << (REG_XMM11 & REG_INDEX_MASK)) |
        (1 << (REG_XMM12 & REG_INDEX_MASK)) |
        (1 << (REG_XMM13 & REG_INDEX_MASK)) |
        (1 << (REG_XMM14 & REG_INDEX_MASK)) |
        (1 << (REG_XMM15 & REG_INDEX_MASK)) ;
      break;

    case CALL_CONV_X64U:
      _argumentsGP[0] = (REG_RDI  & REG_INDEX_MASK);
      _argumentsGP[1] = (REG_RSI  & REG_INDEX_MASK);
      _argumentsGP[2] = (REG_RDX  & REG_INDEX_MASK);
      _argumentsGP[3] = (REG_RCX  & REG_INDEX_MASK);
      _argumentsGP[4] = (REG_R8   & REG_INDEX_MASK);
      _argumentsGP[5] = (REG_R9   & REG_INDEX_MASK);

      _argumentsXMM[0] = (REG_XMM0 & REG_INDEX_MASK);
      _argumentsXMM[1] = (REG_XMM1 & REG_INDEX_MASK);
      _argumentsXMM[2] = (REG_XMM2 & REG_INDEX_MASK);
      _argumentsXMM[3] = (REG_XMM3 & REG_INDEX_MASK);
      _argumentsXMM[4] = (REG_XMM4 & REG_INDEX_MASK);
      _argumentsXMM[5] = (REG_XMM5 & REG_INDEX_MASK);
      _argumentsXMM[6] = (REG_XMM6 & REG_INDEX_MASK);
      _argumentsXMM[7] = (REG_XMM7 & REG_INDEX_MASK);

      _preservedGP =
        (1 << (REG_RBX   & REG_INDEX_MASK)) |
        (1 << (REG_RSP   & REG_INDEX_MASK)) |
        (1 << (REG_RBP   & REG_INDEX_MASK)) |
        (1 << (REG_R12   & REG_INDEX_MASK)) |
        (1 << (REG_R13   & REG_INDEX_MASK)) |
        (1 << (REG_R14   & REG_INDEX_MASK)) |
        (1 << (REG_R15   & REG_INDEX_MASK)) ;
      _preservedXMM = 0;
      break;

    default:
      // Illegal calling convention.
      ASMJIT_ASSERT(0);
  }
#endif // ASMJIT_X64
}

void FunctionPrototype::_setPrototype(const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(count <= 32);

  sysint_t i;

  int32_t posGP = 0;
  int32_t posXMM = 0;
  int32_t stackOffset = 0;

  for (i = 0; i < (sysint_t)count; i++)
  {
    Argument& a = _arguments[i];
    a.variableType = args[i];
    a.registerIndex = INVALID_VALUE;
    a.stackOffset = INVALID_VALUE;
  }

  _argumentsCount = count;
  if (_argumentsCount == 0) return;

  // --------------------------------------------------------------------------
  // [X86 Calling Conventions]
  // --------------------------------------------------------------------------

#if defined(ASMJIT_X86)
  // Register arguments (Integer), always left-to-right.
  for (i = 0; i != count; i++)
  {
    Argument& a = _arguments[i];
    if (isVariableInteger(a.variableType) && posGP < 32 && _argumentsGP[posGP] != INVALID_VALUE)
    {
      a.registerIndex = _argumentsGP[posGP++];
    }
  }

  // Stack arguments.
  bool ltr = _argumentsDirection == ARGUMENT_DIR_LEFT_TO_RIGHT;
  sysint_t istart = ltr ? 0 : (sysint_t)count - 1;
  sysint_t iend   = ltr ? (sysint_t)count : -1;
  sysint_t istep  = ltr ? 1 : -1;

  for (i = istart; i != iend; i += istep)
  {
    Argument& a = _arguments[i];

    if (isVariableInteger(a.variableType))
    {
      stackOffset -= 4;
      a.stackOffset = stackOffset;
    }
    else if (isVariableFloat(a.variableType))
    {
      int32_t size = (int32_t)variableInfo[a.variableType].size;
      stackOffset -= size;
      a.stackOffset = stackOffset;
    }
  }
#endif // ASMJIT_X86

  // --------------------------------------------------------------------------
  // [X64 Calling Conventions]
  // --------------------------------------------------------------------------

#if defined(ASMJIT_X64)
  // Windows 64-bit specific.
  if (_callingConvention == CALL_CONV_X64W)
  {
    sysint_t max = count < 4 ? count : 4;

    // Register arguments (Integer / FP), always left to right.
    for (i = 0; i != max; i++)
    {
      Argument& a = _arguments[i];

      if (isVariableInteger(a.variableType))
        a.registerIndex = _argumentsGP[i];
      else if (isVariableFloat(a.variableType))
        a.registerIndex = _argumentsXMM[i];
    }

    // Stack arguments (always right-to-left).
    for (i = count - 1; i != -1; i--)
    {
      Argument& a = _arguments[i];
      if (a.isAssigned()) continue;

      if (isVariableInteger(a.variableType))
      {
        stackOffset -= 8; // Always 8 bytes.
        a.stackOffset = stackOffset;
      }
      else if (isVariableFloat(a.variableType))
      {
        int32_t size = (int32_t)variableInfo[a.variableType].size;
        stackOffset -= size;
        a.stackOffset = stackOffset;
      }
    }

    // 32 bytes shadow space (X64W calling convention specific).
    stackOffset -= 4 * 8;
  }
  // Linux/Unix 64-bit (AMD64 calling convention).
  else
  {
    // Register arguments (Integer), always left to right.
    for (i = 0; i != count; i++)
    {
      Argument& a = _arguments[i];
      if (isVariableInteger(a.variableType) && posGP < 32 && _argumentsGP[posGP] != INVALID_VALUE)
      {
        a.registerIndex = _argumentsGP[posGP++];
      }
    }

    // Register arguments (FP), always left to right.
    for (i = 0; i != count; i++)
    {
      Argument& a = _arguments[i];
      if (isVariableFloat(a.variableType))
      {
        a.registerIndex = _argumentsXMM[posXMM++];
      }
    }

    // Stack arguments.
    for (i = count - 1; i != -1; i--)
    {
      Argument& a = _arguments[i];
      if (a.isAssigned()) continue;

      if (isVariableInteger(a.variableType))
      {
        stackOffset -= 8;
        a.stackOffset = stackOffset;
      }
      else if (isVariableFloat(a.variableType))
      {
        int32_t size = (int32_t)variableInfo[a.variableType].size;

        stackOffset -= size;
        a.stackOffset = stackOffset;
      }
    }
  }
#endif // ASMJIT_X64

  // Modify stack offset (all function parameters will be in positive stack
  // offset that is never zero).
  for (i = 0; i < (sysint_t)count; i++)
  {
    if (_arguments[i].registerIndex == INVALID_VALUE)
      _arguments[i].stackOffset += sizeof(sysint_t) - stackOffset;
  }

  _argumentsStackSize = (uint32_t)(-stackOffset);
}

void FunctionPrototype::_setReturnValue(uint32_t valueId) ASMJIT_NOTHROW
{
  // TODO.
}

// ============================================================================
// [AsmJit::EVariableHint]
// ============================================================================

EVariableHint::EVariableHint(Compiler* c, VarData* vdata, uint32_t hintId, uint32_t hintValue) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_VARIABLE_HINT),
  _vdata(vdata),
  _hintId(hintId),
  _hintValue(hintValue)
{
}

EVariableHint::~EVariableHint() ASMJIT_NOTHROW
{
}

void EVariableHint::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset;

  switch (_hintId)
  {
    case VARIABLE_HINT_ALLOC:
    case VARIABLE_HINT_SPILL:
      if (!c._isActive(_vdata)) c._addActive(_vdata);
      break;
    case VARIABLE_HINT_UNUSE:
      if (c._isActive(_vdata)) c._freeActive(_vdata);
      break;
  }
}

void EVariableHint::translate(CompilerContext& c) ASMJIT_NOTHROW
{
  switch (_hintId)
  {
    case VARIABLE_HINT_ALLOC:
      c.allocVar(_vdata, _hintValue, VARIABLE_ALLOC_READWRITE);
      break;
    case VARIABLE_HINT_SPILL:
      c.spillVar(_vdata);
      break;
    case VARIABLE_HINT_UNUSE:
      c.unuseVar(_vdata, VARIABLE_STATE_UNUSED);
      break;
  }
}

// ============================================================================
// [AsmJit::EInstruction]
// ============================================================================

EInstruction::EInstruction(Compiler* c, uint32_t code, Operand* operandsData, uint32_t operandsCount) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
  _code = code;

  _operands = operandsData;
  _operandsCount = operandsCount;
  _memOp = NULL;

  _variables = NULL;
  _variablesCount = 0;

  uint32_t i;
  for (i = 0; i < operandsCount; i++)
  {
    if (_operands[i].isMem()) 
    {
      _memOp = reinterpret_cast<Mem*>(&_operands[i]);
      break;
    }
  }
}

EInstruction::~EInstruction() ASMJIT_NOTHROW
{
}

void EInstruction::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset;

#define __GET_VARIABLE(__id__) \
  { \
    VarData* candidate = _compiler->_getVarData(__id__); \
    \
    for (var = cur; ;) \
    { \
      if (var == _variables) \
      { \
        var = cur++; \
        var->vdata = candidate; \
        var->vflags = 0; \
        break; \
      } \
      \
      var--; \
      \
      if (var->vdata == candidate) \
      { \
        break; \
      } \
      \
    } \
    \
    ASMJIT_ASSERT(var != NULL); \
  }

  const InstructionDescription* id = &instructionDescription[_code];

  uint32_t i, len = _operandsCount;
  uint32_t variablesCount = 0;

  for (i = 0; i < len; i++)
  {
    Operand& o = _operands[i];

    if (o.isVar())
    {
      VarData* vdata = _compiler->_getVarData(o.getId());
      ASMJIT_ASSERT(vdata != NULL);

      if (vdata->workOffset == _offset) continue;
      if (!c._isActive(vdata)) c._addActive(vdata);

      vdata->workOffset = _offset;
      variablesCount++;
    }
    else if (o.isMem())
    {
      if ((o.getId() & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        VarData* vdata = _compiler->_getVarData(o.getId());
        ASMJIT_ASSERT(vdata != NULL);

        c._markMemoryUsed(vdata);
        if (!c._isActive(vdata)) c._addActive(vdata);
        continue;
      }
      else if ((o._mem.base & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        VarData* vdata = _compiler->_getVarData(o._mem.base);
        ASMJIT_ASSERT(vdata != NULL);

        if (vdata->workOffset == _offset) continue;
        if (!c._isActive(vdata)) c._addActive(vdata);

        vdata->workOffset = _offset;
        variablesCount++;
      }

      if ((o._mem.index & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        VarData* vdata = _compiler->_getVarData(o._mem.index);
        ASMJIT_ASSERT(vdata != NULL);

        if (vdata->workOffset == _offset) continue;
        if (!c._isActive(vdata)) c._addActive(vdata);

        vdata->workOffset = _offset;
        variablesCount++;
      }
    }
  }

  if (!variablesCount)
  {
    c._currentOffset++;
    return;
  }

  _variables = reinterpret_cast<VarAllocRecord*>(_compiler->getZone().zalloc(sizeof(VarAllocRecord) * variablesCount));
  if (!_variables)
  {
    _compiler->setError(ERROR_NO_HEAP_MEMORY);
    c._currentOffset++;
    return;
  }

  _variablesCount = variablesCount;

  VarAllocRecord* cur = _variables;
  VarAllocRecord* var = NULL;

  for (i = 0; i < len; i++)
  {
    Operand& o = _operands[i];

    if (o.isVar())
    {
      __GET_VARIABLE(o.getId())
      var->vflags |= VARIABLE_ALLOC_REGISTER;

      if (id->isSpecial())
      {
        // TODO.
      }
      else
      {
        if (i == 0)
        {
          // If variable is MOV instruction type (source replaces the destination)
          // or variable is MOVSS/MOVSD instruction and source operand is memory,
          // then register allocator should know that previous destination value
          // is lost (write only operation).
          if (id->isMov() || ((id->code == INST_MOVSS || id->code == INST_MOVSD) && _operands[1].isMem()))
          {
            // Write only case.
            var->vdata->registerWriteCount++;
            var->vflags |= VARIABLE_ALLOC_WRITE;
          }
          else
          {
            var->vdata->registerRWCount++;
            var->vflags |= VARIABLE_ALLOC_READWRITE;
          }
        }
        else
        {
          var->vdata->registerReadCount++;
          var->vflags |= VARIABLE_ALLOC_READ;
        }

        if (!_memOp && i < 2 && (id->oflags[i] & InstructionDescription::O_MEM) != 0)
        {
          var->vflags |= VARIABLE_ALLOC_MEMORY;
        }
      }
    }
    else if (o.isMem())
    {
      if ((o.getId() & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        VarData* vdata = _compiler->_getVarData(o.getId());
        ASMJIT_ASSERT(vdata != NULL);

        if (i == 0)
        {
          // If variable is MOV instruction type (source replaces the destination)
          // or variable is MOVSS/MOVSD instruction then register allocator should
          // know that previous destination value is lost (write only operation).
          if (id->isMov() || ((id->code == INST_MOVSS || id->code == INST_MOVSD)))
          {
            // Write only case.
            vdata->memoryWriteCount++;
          }
          else
          {
            vdata->memoryRWCount++;
          }
        }
        else
        {
          vdata->memoryReadCount++;
        }
      }
      else if ((o._mem.base & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        __GET_VARIABLE(reinterpret_cast<Mem&>(o).getBase())
        var->vdata->registerReadCount++;
        var->vflags |= VARIABLE_ALLOC_REGISTER | VARIABLE_ALLOC_READ;
      }

      if ((o._mem.index & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        __GET_VARIABLE(reinterpret_cast<Mem&>(o).getIndex())
        var->vdata->registerReadCount++;
        var->vflags |= VARIABLE_ALLOC_REGISTER | VARIABLE_ALLOC_READ;
      }
    }
  }

  // Traverse all variables and update firstEmittable / lastEmittable. This
  // function is called from iterator that scans emittables using forward
  // direction so we can use this knowledge to optimize process.
  for (i = 0; i < _variablesCount; i++)
  {
    VarData* v = _variables[i].vdata;

    // First emittable (begin of variable scope).
    if (v->firstEmittable == NULL) v->firstEmittable = this;

    // Last emittable (end of variable scope).
    v->lastEmittable = this;
  }

  // There are some instructions that can be used to clear register or to set
  // register to some value (ideal case is all zeros or all ones).
  //
  // xor/pxor reg, reg    ; Set all bits in reg to 0.
  // sub/psub reg, reg    ; Set all bits in reg to 0.
  // andn reg, reg        ; Set all bits in reg to 0.
  // pcmpgt reg, reg      ; Set all bits in reg to 0.
  // pcmpeq reg, reg      ; Set all bits in reg to 1.

  if (_variablesCount == 1 && 
      _operandsCount > 1 &&
      _operands[0].isVar() &&
      _operands[1].isVar() &&
      !_memOp)
  {
    switch (_code)
    {
      // XOR Instructions.
      case INST_XOR:
      case INST_XORPD:
      case INST_XORPS:
      case INST_PXOR:

      // ANDN Instructions.
      case INST_PANDN:

      // SUB Instructions.
      case INST_SUB:
      case INST_PSUBB:
      case INST_PSUBW:
      case INST_PSUBD:
      case INST_PSUBQ:
      case INST_PSUBSB:
      case INST_PSUBSW:
      case INST_PSUBUSB:
      case INST_PSUBUSW:

      // PCMPEQ Instructions.
      case INST_PCMPEQB:
      case INST_PCMPEQW:
      case INST_PCMPEQD:
      case INST_PCMPEQQ:

      // PCMPGT Instructions.
      case INST_PCMPGTB:
      case INST_PCMPGTW:
      case INST_PCMPGTD:
      case INST_PCMPGTQ:
        // Clear read flag. We are not interested about reading spilled variable
        // into register, because result of instruction not depends to it.
        _variables[0].vflags = VARIABLE_ALLOC_WRITE;
        _variables[0].vdata->registerReadCount--;
        break;
    }
  }
#undef __GET_VARIABLE

  c._currentOffset++;
}

void EInstruction::translate(CompilerContext& c) ASMJIT_NOTHROW
{
  sysuint_t i;
  sysuint_t variablesCount = _variablesCount;

  if (variablesCount > 0)
  {
    // These variables are used by the instruction and we set current offset
    // to their work offsets -> getSpillCandidate never return this variable
    // for this instruction.
    for (i = 0; i < variablesCount; i++)
    {
      _variables->vdata->workOffset = c._currentOffset;
    }

    // Alloc variables used by the instruction.
    for (i = 0; i < variablesCount; i++)
    {
      VarAllocRecord& r = _variables[i];
      c.allocVar(r.vdata, INVALID_VALUE, r.vflags);
    }

    // Translate variables to registers.
    uint32_t operandsCount = _operandsCount;
    for (i = 0; i < operandsCount; i++)
    {
      Operand& o = _operands[i];

      if (o.isVar())
      {
        VarData* vdata = c._compiler->_getVarData(o.getId());
        ASMJIT_ASSERT(vdata != NULL);

        o._reg.op = OPERAND_REG;
        o._reg.code |= vdata->registerIndex;
      }
      else if (o.isMem())
      {
        if ((o.getId() & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
        {
          VarData* vdata = c._compiler->_getVarData(o.getId());
          ASMJIT_ASSERT(vdata != NULL);

          // Memory access. We just increment here actual displacement.
          o._mem.displacement += vdata->isMemArgument 
            ? c._argumentsActualDisp
            : c._variablesActualDisp;

          // NOTE: This is not enough, variable position will be patched later
          // by CompilerContext::_patchMemoryOperands().
        }
        else if ((o._mem.base & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
        {
          VarData* vdata = c._compiler->_getVarData(o._mem.base);
          ASMJIT_ASSERT(vdata != NULL);

          o._mem.base = vdata->registerIndex;
        }

        if ((o._mem.index & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
        {
          VarData* vdata = c._compiler->_getVarData(o._mem.index);
          ASMJIT_ASSERT(vdata != NULL);

          o._mem.index = vdata->registerIndex;
        }
      }
    }
  }

  if (_memOp && (_memOp->getId() & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
  {
    VarData* vdata = _compiler->_getVarData(_memOp->getId());
    ASMJIT_ASSERT(vdata != NULL);

    switch (vdata->state)
    {
      case VARIABLE_STATE_UNUSED:
        vdata->state = VARIABLE_STATE_MEMORY;
        break;
      case VARIABLE_STATE_REGISTER:
        vdata->changed = false;
        c.unuseVar(vdata, VARIABLE_STATE_MEMORY);
        break;
    }
  }
}

void EInstruction::emit(Assembler& a) ASMJIT_NOTHROW
{
  if (_comment) a._comment = _comment;

  switch (_operandsCount)
  {
    case 0:
      a._emitInstruction(_code);
      break;
    case 1:
      a._emitInstruction(_code, &_operands[0]);
      break;
    case 2:
      a._emitInstruction(_code, &_operands[0], &_operands[1]);
      break;
    case 3:
      a._emitInstruction(_code, &_operands[0], &_operands[1], &_operands[2]);
      break;
    default:
      ASMJIT_ASSERT(0);
      break;
  }
}

ETarget* EInstruction::getJumpTarget() const ASMJIT_NOTHROW
{
  return NULL;
}

// ============================================================================
// [AsmJit::EJmpInstruction]
// ============================================================================

EJmpInstruction::EJmpInstruction(Compiler* c, uint32_t code, Operand* operandsData, uint32_t operandsCount) ASMJIT_NOTHROW :
  EInstruction(c, code, operandsData, operandsCount)
{
  _jumpTarget = _compiler->_getTarget(_operands[0].getId());
  _jumpTarget->_jumpsCount++;

  _jumpNext = _jumpTarget->_from;
  _jumpTarget->_from = this;

  // The 'jmp' is always taken, conditional jump can contain hint, we detect it.
  _isTaken = (getCode() == INST_JMP) ||
             (operandsCount > 1 && 
              operandsData[1].isImm() && 
              reinterpret_cast<Imm*>(&operandsData[1])->getValue() == HINT_TAKEN);
}

EJmpInstruction::~EJmpInstruction() ASMJIT_NOTHROW
{
}

void EJmpInstruction::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset;

  // Now patch all variables where jump location is in the active range.
  if (_jumpTarget->getOffset() != INVALID_VALUE && c._active)
  {
    VarData* first = c._active;
    VarData* var = first;
    uint32_t jumpOffset = _jumpTarget->getOffset();

    do {
      if (var->firstEmittable)
      {
        ASMJIT_ASSERT(var->lastEmittable != NULL);
        uint32_t start = var->firstEmittable->getOffset();
        uint32_t end = var->lastEmittable->getOffset();

        if (jumpOffset >= start && jumpOffset <= end) var->lastEmittable = this;
      }
      var = var->nextActive;
    } while (var != first);
  }

  c._currentOffset++;
}

void EJmpInstruction::translate(CompilerContext& c) ASMJIT_NOTHROW
{
  EInstruction::translate(c);
  _state = c._saveState();

  if (_jumpTarget->getOffset() > getOffset())
  {
    // State is not known, so we need to call _doJump() later. Compiler will
    // do it for us.
    c.addForwardJump(this);
    _jumpTarget->_state = _state;
  }
  else
  {
    _doJump(c);
  }

  // Mark next code as unrecheable, cleared by a next label (ETarget).
  if (_code == INST_JMP)
  {
    c._unrecheable = true;
  }
}

void EJmpInstruction::_doJump(CompilerContext& c) ASMJIT_NOTHROW
{
  // The state have to be already known. The _doJump() method is called by
  // translate() or by Compiler in case that it's forward jump.
  ASMJIT_ASSERT(_jumpTarget->getState());

  if (getCode() == INST_JMP || (isTaken() && _jumpTarget->getOffset() < getOffset()))
  {
    // Instruction type is JMP or conditional jump that should be taken (likely).
    // We can set state here instead of jumping out, setting state and jumping 
    // to _jumpTarget.
    //
    // NOTE: We can't use this technique if instruction is forward conditional 
    // jump. The reason is that when generating code we can't change state here,
    // because next instruction depends to it.
    c._restoreState(_jumpTarget->getState());
  }
  else
  {
    // Instruction type is JMP or conditional jump that should be normally not 
    // taken. If we need add code that will switch between different states we 
    // add it after the end of function body (after epilog, using 'ExtraBlock').
    Compiler* compiler = c.getCompiler();

    Emittable* ext = c.getExtraBlock();
    Emittable* old = compiler->setCurrentEmittable(ext);
    
    c._restoreState(_jumpTarget->getState());

    if (compiler->getCurrentEmittable() != old)
    {
      // Add the jump to the target.
      compiler->jmp(_jumpTarget->_label);
      ext = compiler->getCurrentEmittable();

      // The c._restoreState() method emitted some instructions so we need to
      // patch the jump.
      Label L = compiler->newLabel();
      compiler->setCurrentEmittable(c.getExtraBlock());
      compiler->bind(L);

      // Finally, patch the jump target.
      ASMJIT_ASSERT(_operandsCount > 0);
      _operands[0] = L;                              // Operand part (Label).
      _jumpTarget = compiler->_getTarget(L.getId()); // Emittable part (ETarget).
    }

    c.setExtraBlock(ext);
    compiler->setCurrentEmittable(old);
  }
}

ETarget* EJmpInstruction::getJumpTarget() const ASMJIT_NOTHROW
{
  return _jumpTarget;
}

// ============================================================================
// [AsmJit::EFunction]
// ============================================================================

EFunction::EFunction(Compiler* c) ASMJIT_NOTHROW : Emittable(c, EMITTABLE_FUNCTION)
{
  _argumentVariables = NULL;
  Util::memset32(_hints, INVALID_VALUE, ASMJIT_ARRAY_SIZE(_hints));

  // Stack is always aligned to 16-bytes when using 64-bit mode.
  _isStackAlignedTo16Bytes = (sizeof(sysuint_t) == 8);

  // Linux guarantees stack alignment to 16 bytes by default, I'm not sure about
  // other OSes. Windows not guarantees that and I'm not sure about BSD/MAC.
#if defined(__linux__) || defined(__linux) || defined(linux)
  _isStackAlignedTo16Bytes = true;
#endif // __linux__

  // Just clear to safe defaults.
  _isNaked = false;
  _isEspAdjusted = false;
  _isCallee = false;

  _prologEpilogPushPop = true;
  _emitEMMS = false;
  _emitSFence = false;
  _emitLFence = false;

  _modifiedAndPreservedGP = 0;
  _modifiedAndPreservedXMM = 0;

  _prologEpilogStackSize = 0;
  _prologEpilogStackSizeAligned16 = 0;
  _memStackSize = 0;
  _memStackSizeAligned16 = 0;
  _stackAdjust = 0;

  _entryLabel = c->newLabel();
  _exitLabel = c->newLabel();

  _prolog = Compiler_newObject<EProlog>(c, this);
  _epilog = Compiler_newObject<EEpilog>(c, this);
  _end = Compiler_newObject<EDummy>(c);
}

EFunction::~EFunction() ASMJIT_NOTHROW
{
}

void EFunction::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset++;
}

void EFunction::setPrototype(uint32_t callingConvention, const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW
{
  _functionPrototype.setPrototype(callingConvention, args, count);
}

void EFunction::setHint(uint32_t hint, uint32_t value) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(hint < ASMJIT_ARRAY_SIZE(_hints));
  _hints[hint] = value;
}

void EFunction::_createVariables() ASMJIT_NOTHROW
{
  uint32_t i, count = _functionPrototype.getArgumentsCount();
  if (count == 0) return;

  _argumentVariables = reinterpret_cast<VarData**>(_compiler->getZone().zalloc(count * sizeof(VarData*)));
  if (_argumentVariables == NULL)
  {
    _compiler->setError(ERROR_NO_HEAP_MEMORY);
    return;
  }

  char argNameStorage[64];
  char* argName = NULL;

  bool debug = _compiler->getLogger() != NULL;
  if (debug) argName = argNameStorage;

  for (i = 0; i < count; i++)
  {
    FunctionPrototype::Argument& a = _functionPrototype.getArguments()[i];
    if (debug) snprintf(argName, ASMJIT_ARRAY_SIZE(argNameStorage), "arg_%u", i);

    uint32_t size = getVariableSize(a.variableType);
    VarData* vdata = _compiler->_newVarData(argName, a.variableType, size);

    if (a.registerIndex != (uint32_t)INVALID_VALUE)
    {
      vdata->isRegArgument = true;
      vdata->registerIndex = a.registerIndex;
      vdata->homeRegisterIndex = a.registerIndex;
    }

    if (a.stackOffset != (int32_t)INVALID_VALUE)
    {
      vdata->isMemArgument = true;
      vdata->homeMemoryOffset = a.stackOffset;
    }

    _argumentVariables[i] = vdata;
  }
}

void EFunction::_prepareVariables(Emittable* first) ASMJIT_NOTHROW
{
  uint32_t i, count = _functionPrototype.getArgumentsCount();
  if (count == 0) return;

  for (i = 0; i < count; i++)
  {
    VarData* vdata = _argumentVariables[i];

    // This is where variable scope starts.
    vdata->firstEmittable = first;
    // If this will not be changed then it will be deallocated immediately.
    vdata->lastEmittable = first;
  }
}

void EFunction::_allocVariables(CompilerContext& c) ASMJIT_NOTHROW
{
  uint32_t i, count = _functionPrototype.getArgumentsCount();
  if (count == 0) return;

  for (i = 0; i < count; i++)
  {
    VarData* vdata = _argumentVariables[i];

    if (vdata->firstEmittable != vdata->lastEmittable)
    {
      // Variable is used.
      if (vdata->registerIndex != INVALID_VALUE)
      {
        // If variable is in register -> mark it as changed so it will not be
        // lost by first spill.
        vdata->changed = true;
        c._allocatedVariable(vdata);
      }
      else if (vdata->isMemArgument)
      {
        vdata->state = VARIABLE_STATE_MEMORY;
      }
    }
    else
    {
      // Variable is not used.
      vdata->registerIndex = INVALID_VALUE;
    }
  }
}




















#if 0
// TODO:
void Function::prepare(CompilerContext& c)
{
  // Prepare variables
  static const UInt32 sizes[] = { 16, 8, 4, 2, 1 };

  SysUInt i, v;

  SysInt sp = 0;       // Stack offset
  SysInt pe;           // Prolog / epilog size
  SysInt peGp;         // Prolog / epilog for GP registers size
  SysInt peXmm;        // Prolog / epilog for XMM registers size

  UInt8 argMemBase;    // Address base register for function arguments
  UInt8 varMemBase;    // Address base register for function variables
  SysInt argDisp = 0;  // Displacement for arguments
  SysInt varDisp = 0;  // Displacement for variables
  UInt32 alignSize = 0;// Maximum alignment stack size

  // This is simple optimization to do 16 byte aligned variables first and
  // all others next.
  for (i = 0; i < ASMJIT_ARRAY_SIZE(sizes); i++)
  {
    // See sizes declaration. We allocate variables on the stack in ordered way:
    // 16 byte variables (xmm) first,
    // 8 byte variables (mmx, gpq) second,
    // 4,2,1 (these not needs to be aligned)
    UInt32 size = sizes[i];

    for (v = 0; v < _variables.length(); v++)
    {
      Variable* var = _variables[v];

      // Use only variable with size 'size' and function arguments that was
      // passed through registers.
      if (var->size() == size && !var->stackArgument() && var->_globalMemoryAccessCount > 0)
      {
        // X86 stack is aligned to 32 bits (4 bytes) in 32-bit mode (Is this
        // correct?) and for 128-bits (16 bytes) in 64-bit mode.
        //
        // For MMX and SSE  programming we need 8 or 16 bytes alignment. For
        // MMX/SSE memory operands we can be adjusted by 4 or 12 bytes.
#if defined(ASMJIT_X86)
        if (size ==  8 && alignSize <  8) alignSize =  8;
        if (size == 16 && alignSize < 16) alignSize = 16;
#endif // ASMJIT_X86

        _variables[v]->_stackOffset = sp;
        sp += size;
      }
    }
  }

  // Align to 16 bytes.
  sp = (sp + 15) & ~15;

  // Get prolog/epilog push/pop size on the stack.
  peGp = countOfGpRegistersToBeSaved() * sizeof(SysInt);
  peXmm = countOfXmmRegistersToBeSaved() * 16;
  pe = peGp + peXmm;

  _prologEpilogStackSize = pe;
  _variablesStackSize = sp;
  _stackAlignmentSize = alignSize;

  // Calculate displacements.
  if (naked())
  {
    // Naked functions are using always esp/rsp.
    argMemBase = RID_ESP;
    argDisp = prologEpilogPushPop() ? peGp : 0;

    varMemBase = RID_ESP;
    varDisp = -sp - sizeof(SysInt);
  }
  else
  {
    // Functions with prolog/epilog are using ebp/rbp for variables.
    argMemBase = RID_EBP;
    // Push ebp/rpb size (return address is already in arguments stack offset).
    argDisp = sizeof(SysInt);

    varMemBase = RID_ESP;
    varDisp = 0;
  }

  // Patch all variables to point to correct address in memory
  for (v = 0; v < _variables.length(); v++)
  {
    Variable* var = _variables[v];
    Mem* memop = var->_memoryOperand;

    // Different stack home for function arguments and variables.
    //
    // NOTE: Function arguments given in registers can be relocated onto the
    // stack without problems. This code doest something different, it will
    // not change stack arguments location. So only stack based arguments needs
    // this special handling
    if (var->stackArgument())
    {
      memop->_mem.base = argMemBase;
      memop->_mem.displacement = var->stackOffset() + argDisp;
    }
    else
    {
      memop->_mem.base = varMemBase;
      memop->_mem.displacement = var->stackOffset() + varDisp;
    }
  }
}
#endif

/*
TODO:
static uint32_t getStackSize(EFunction* f, uint32_t stackAdjust)
{
  // Get stack size needed for store all variables and to save all used
  // registers. AlignedStackSize is stack size adjusted for aligning.
  uint32_t variables = alignTo16Bytes(f->variablesStackSize());
  uint32_t prologEpilog = alignTo16Bytes(f->prologEpilogStackSize());

  return variables + prologEpilog;
}
*/

void EFunction::_preparePrologEpilog(CompilerContext& c) ASMJIT_NOTHROW
{
  const CpuInfo* cpuInfo = getCpuInfo();

  _prologEpilogPushPop = true;
  _emitEMMS = false;
  _emitSFence = false;
  _emitLFence = false;

  // If another function is called by the function it's needed to adjust
  // ESP.
  if (_isCallee)
    _isEspAdjusted = true;

  if (_hints[FUNCTION_HINT_NAKED] != INVALID_VALUE)
    _isNaked = (bool)_hints[FUNCTION_HINT_NAKED];

  if (_hints[FUNCTION_HINT_PUSH_POP_SEQUENCE] != INVALID_VALUE)
    _prologEpilogPushPop = (bool)_hints[FUNCTION_HINT_PUSH_POP_SEQUENCE];

  if (_hints[FUNCTION_HINT_EMMS] != INVALID_VALUE)
    _emitEMMS = (bool)_hints[FUNCTION_HINT_EMMS];

  if (_hints[FUNCTION_HINT_SFENCE] != INVALID_VALUE)
    _emitSFence = (bool)_hints[FUNCTION_HINT_SFENCE];

  if (_hints[FUNCTION_HINT_LFENCE] != INVALID_VALUE)
    _emitLFence = (bool)_hints[FUNCTION_HINT_LFENCE];

  _modifiedAndPreservedGP = c._modifiedGPRegisters  & _functionPrototype.getPreservedGP() & ~(1 << REG_INDEX_ESP);
  _modifiedAndPreservedXMM = c._modifiedXMMRegisters & _functionPrototype.getPreservedXMM();

  _movDqaInstruction = _isStackAlignedTo16Bytes ? INST_MOVDQA : INST_MOVDQU;

  _prologEpilogStackSize = 
    bitCount(_modifiedAndPreservedGP ) * sizeof(sysuint_t) +
    bitCount(_modifiedAndPreservedXMM) * 16;
  _prologEpilogStackSizeAligned16 = alignTo16Bytes(_prologEpilogStackSize);

  _memStackSize = c._memBytesTotal;
  _memStackSizeAligned16 = alignTo16Bytes(_memStackSize);

  // How many bytes to add to stack to make it aligned to 16 bytes.
  if (_isNaked)
  {
    // TODO:
    //_stackAdjust = ((sizeof(sysint_t) == 8) ? 8 : 12);
    // stackAdjust = sizeof(sysint_t); //((sizeof(sysint_t) == 8) ? 8 : 4);
    _stackAdjust = 0; 

    c._argumentsBaseReg = REG_INDEX_ESP;
    c._variablesBaseReg = REG_INDEX_ESP;

    c._argumentsBaseOffset = _prologEpilogStackSize + _stackAdjust;
    c._variablesBaseOffset = -(_prologEpilogStackSizeAligned16 + _memStackSizeAligned16);
  }
  else
  {
    // TODO:
    //((sizeof(sysint_t) == 8) ? 0 : 8);
    _stackAdjust = sizeof(sysint_t);

    c._argumentsBaseReg = REG_INDEX_EBP;
    c._variablesBaseReg = REG_INDEX_ESP;

    c._argumentsBaseOffset = (int32_t)(_stackAdjust);
    c._variablesBaseOffset = -(int32_t)(_prologEpilogStackSizeAligned16 + _memStackSizeAligned16) + (int32_t)_stackAdjust;
  }
}

void EFunction::_dumpFunction(CompilerContext& c) ASMJIT_NOTHROW
{
  Logger* logger = _compiler->getLogger();
  ASMJIT_ASSERT(logger != NULL);

  uint32_t i;
  char _buf[1024];
  char* p;

  // Log function prototype.
  {
    uint32_t argumentsCount = _functionPrototype.getArgumentsCount();
    bool first = true;

    logger->logString("; Function Prototype:\n");
    logger->logString(";\n");

    for (i = 0; i < argumentsCount; i++)
    {
      const FunctionPrototype::Argument& a = _functionPrototype.getArguments()[i];
      VarData* vdata = _argumentVariables[i];

      if (first)
      {
        logger->logString("; IDX| Type     | Sz | Home            |\n");
        logger->logString("; ---+----------+----+-----------------+\n");
      }

      char* memHome = memHome = _buf;

      if (a.registerIndex != INVALID_VALUE)
      {
        BaseReg regOp(a.registerIndex | REG_TYPE_GPN, 0);
        dumpOperand(memHome, &regOp)[0] = '\0';
      }
      else
      {
        Mem memOp;
        memOp._mem.base = REG_INDEX_ESP;
        memOp._mem.displacement = a.stackOffset;
        dumpOperand(memHome, &memOp)[0] = '\0';
      }

      logger->logFormat("; %-3u| %-9s| %-3u| %-16s|\n",
        // Argument index.
        i,
        // Argument type.
        vdata->type < _VARIABLE_TYPE_COUNT ? variableInfo[vdata->type].name : "invalid",
        // Argument size.
        vdata->size,
        // Argument memory home.
        memHome
      );

      first = false;
    }
    logger->logString(";\n");
  }

  // Log variables.
  {
    uint32_t variablesCount = (uint32_t)_compiler->_varData.getLength();
    bool first = true;

    logger->logString("; Variables:\n");
    logger->logString(";\n");

    for (i = 0; i < variablesCount; i++)
    {
      VarData* vdata = _compiler->_varData[i];

      // If this variable is not related to this function then skip it.
      if (vdata->scope != this) continue;

      // Get some information about variable type.
      const VariableInfo& vinfo = variableInfo[vdata->type];

      if (first)
      {
        logger->logString("; ID | Type     | Sz | Home            | Register Access    | Memory Access      |\n");
        logger->logString("; ---+----------+----+-----------------+--------------------+--------------------+\n");
      }

      char* memHome = (char*)"[None]";
      if (vdata->homeMemoryData != NULL)
      {
        memHome = _buf;

        Mem memOp;
        memOp._mem.displacement = vdata->homeMemoryOffset;

        if (vdata->homeMemoryRegister == INVALID_VALUE)
        {
          if (vdata->isMemArgument)
          {
            memOp._mem.base = c._argumentsBaseReg;
            memOp._mem.displacement += c._argumentsBaseOffset;
          }
          else
          {
            memOp._mem.base = c._variablesBaseReg;
            memOp._mem.displacement += c._variablesBaseOffset;
          }
        }
        else
        {
          memOp._mem.base = vdata->homeMemoryRegister & REG_INDEX_MASK;
        }
        dumpOperand(memHome, &memOp)[0] = '\0';
      }

      logger->logFormat("; %-3u| %-9s| %-3u| %-16s| r=%-4uw=%-4urw=%-4u| r=%-4uw=%-4urw=%-4u|\n",
        // Variable id.
        (uint)(i & OPERAND_ID_VALUE_MASK),
        // Variable type.
        vdata->type < _VARIABLE_TYPE_COUNT ? vinfo.name : "invalid",
        // Variable size.
        vdata->size,
        // Variable memory home.
        memHome,
        // Register access count.
        (unsigned int)vdata->registerReadCount,
        (unsigned int)vdata->registerWriteCount,
        (unsigned int)vdata->registerRWCount,
        // Memory access count.
        (unsigned int)vdata->memoryReadCount,
        (unsigned int)vdata->memoryWriteCount,
        (unsigned int)vdata->memoryRWCount
      );
      first = false;
    }
    logger->logString(";\n");
  }

  // Log modified registers.
  {
    p = _buf;

    uint32_t r;
    uint32_t modifiedRegisters = 0;

    for (r = 0; r < 3; r++)
    {
      bool first = true;
      uint32_t regs;
      uint32_t type;

      switch (r)
      {
        case 0:
          regs = c._modifiedGPRegisters;
          type = REG_TYPE_GPN;
          p = Util::mycpy(p, "; GP : ");
          break;
        case 1:
          regs = c._modifiedMMRegisters;
          type = REG_TYPE_MM;
          p = Util::mycpy(p, "; MM : ");
          break;
        case 2:
          regs = c._modifiedXMMRegisters;
          type = REG_TYPE_XMM;
          p = Util::mycpy(p, "; XMM: ");
          break;
        default:
          ASMJIT_ASSERT(0);
      }

      for (i = 0; i < REG_NUM; i++)
      {
        if ((regs & (1 << i)) != 0)
        {
          if (!first) { *p++ = ','; *p++ = ' '; }
          p = dumpRegister(p, type, i);
          first = false;
          modifiedRegisters++;
        }
      }
      *p++ = '\n';
    }
    *p = '\0';

    logger->logFormat("; Modified registers (%u):\n", (unsigned int)modifiedRegisters);
    logger->logString(_buf);
  }

  logger->logString("\n");
}

void EFunction::_emitProlog(CompilerContext& c) ASMJIT_NOTHROW
{
  const CpuInfo* cpuInfo = getCpuInfo();

  uint32_t preservedGP  = _modifiedAndPreservedGP;
  uint32_t preservedXMM = _modifiedAndPreservedXMM;

  // Calculate stack size with stack adjustment. This will give us proper
  // count of bytes to subtract from esp/rsp.
  int32_t stackSize = _prologEpilogStackSize + _memStackSize; //_legacy_getStackSize(_stackAdjust);
  int32_t stackSubtract = stackSize;

  uint32_t i, mask;

  if (_compiler->getLogger() && _compiler->getLogger()->isUsed())
  {
    // Here function prolog starts.
    _compiler->comment("Function prolog");
  }

  // Emit standard prolog entry code (but don't do it if function is set to be
  // naked).
  //
  // Also see the _stackAdjust variable. If function is naked (so prolog and
  // epilog will not contain "push ebp" and "mov ebp, esp", we need to adjust
  // stack by 8 bytes in 64-bit mode (this will give us that stack will remain
  // aligned to 16 bytes).
  if (!_isNaked)
  {
    _compiler->emit(INST_PUSH, nbp);
    _compiler->emit(INST_MOV, nbp, nsp);
  }

  // Save GP registers using PUSH/POP.
  if (preservedGP && _prologEpilogPushPop)
  {
    for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
    {
      if (preservedGP & mask)
      {
        _compiler->emit(INST_PUSH, gpn(i));
      }
    }
    stackSubtract -= bitCount(preservedGP) * sizeof(sysint_t);
  }

  if (!_isNaked)
  {
    if (_isEspAdjusted)
    {
      if (stackSubtract)
        _compiler->emit(INST_SUB, nsp, imm(stackSubtract));

#if defined(ASMJIT_X86)
      // TODO:
      //
      // Manual alignment. This is a bit complicated if we don't want to pollute
      // ebp/rpb register and standard prolog.
      if (stackSize && 0)
      {
        // stackAlignmentSize can be 8 or 16
        _compiler->emit(INST_AND, nsp, imm(-16));
        // _isStackAlignedTo16Bytes = true;
      }
#endif // ASMJIT_X86
    }
  }

  sysint_t nspPos = _memStackSizeAligned16; // alignTo16Bytes( _legacy_variablesStackSize());
  if (_isNaked) nspPos -= stackSize + _stackAdjust;

  // Save XMM registers using MOVDQA/MOVDQU.
  if (preservedXMM)
  {
    for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
    {
      if (preservedXMM & mask)
      {
        _compiler->emit(_movDqaInstruction, dqword_ptr(nsp, nspPos), xmm(i));
        nspPos += 16;
      }
    }
  }

  // Save GP registers using MOV.
  if (preservedGP && !_prologEpilogPushPop)
  {
    for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
    {
      if (preservedGP & mask)
      {
        _compiler->emit(INST_MOV, sysint_ptr(nsp, nspPos), gpn(i));
        nspPos += sizeof(sysint_t);
      }
    }
  }

  if (_compiler->getLogger() && _compiler->getLogger()->isUsed())
  {
    _compiler->comment("Function body");
  }
}

void EFunction::_emitEpilog(CompilerContext& c) ASMJIT_NOTHROW
{
  const CpuInfo* cpuInfo = getCpuInfo();

  uint32_t preservedGP  = _modifiedAndPreservedGP;
  uint32_t preservedXMM = _modifiedAndPreservedXMM;

  // In 64-bit mode the stack is aligned to 16 bytes by default.
  // bool isStackAlignedTo16Bytes = sizeof(sysint_t) == 8;

  // Calculate stack size with stack adjustment. This will give us proper
  // count of bytes to subtract from esp/rsp.
  int32_t stackSize = _prologEpilogStackSize + _memStackSize; //_legacy_getStackSize(_stackAdjust);

  uint32_t i, mask;

  // TODO:
//#if defined(ASMJIT_X86)
//  if (!_isNaked && stackSize && f->stackAlignmentSize())
//  {
//    isStackAlignedTo16Bytes = true;
//  }
//#endif // ASMJIT_X86

  if (_compiler->getLogger() && _compiler->getLogger()->isUsed())
  {
    _compiler->comment("Function epilog");
  }

  sysint_t nspPos = _memStackSizeAligned16; //alignTo16Bytes(_legacy_variablesStackSize());
  if (_isNaked) nspPos -= stackSize + _stackAdjust;

  // Restore XMM registers using MOV.
  if (preservedXMM)
  {
    for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
    {
      if (preservedXMM & mask)
      {
        _compiler->emit(_movDqaInstruction, xmm(i), dqword_ptr(nsp, nspPos));
        nspPos += 16;
      }
    }
  }

  // Restore GP registers.
  if (preservedGP)
  {
    if (_prologEpilogPushPop)
    {
      // Restore GP registers using MOV.
      if (!_isNaked)
      {
        if (_isEspAdjusted)
        {
          int32_t stackAdd = stackSize - bitCount(preservedGP) * sizeof(sysint_t);
          if (stackAdd != 0) _compiler->emit(INST_ADD, nsp, imm(stackAdd));
        }
      }

      for (i = REG_NUM - 1, mask = 1 << i; (int32_t)i >= 0; i--, mask >>= 1)
      {
        if (preservedGP & mask)
        {
          _compiler->emit(INST_POP, gpn(i));
        }
      }
    }
    else
    {
      // Restore GP registers using PUSH/POP.
      for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
      {
        if (preservedGP & mask)
        {
          _compiler->emit(INST_MOV, gpn(i), sysint_ptr(nsp, nspPos));
          nspPos += sizeof(sysint_t);
        }
      }
    }
  }

  // Emit Emms.
  if (_emitEMMS) _compiler->emit(INST_EMMS);

  // Emit SFence / LFence / MFence.
  if ( _emitSFence &&  _emitLFence) _compiler->emit(INST_MFENCE); // MFence == SFence & LFence.
  if ( _emitSFence && !_emitLFence) _compiler->emit(INST_SFENCE); // Only SFence.
  if (!_emitSFence &&  _emitLFence) _compiler->emit(INST_LFENCE); // Only LFence.

  // Emit standard epilog leave code (if needed).
  if (!_isNaked)
  {
    if (cpuInfo->vendorId == CpuInfo::Vendor_AMD)
    {
      // AMD seems to prefer LEAVE instead of MOV/POP sequence.
      _compiler->emit(INST_LEAVE);
    }
    else
    {
      _compiler->emit(INST_MOV, nsp, nbp);
      _compiler->emit(INST_POP, nbp);
    }
  }

  // Emit return using correct instruction.
  if (_functionPrototype.getCalleePopsStack())
    _compiler->emit(INST_RET, imm((int16_t)_functionPrototype.getArgumentsStackSize()));
  else
    _compiler->emit(INST_RET);
}

// ============================================================================
// [AsmJit::EProlog]
// ============================================================================

EProlog::EProlog(Compiler* c, EFunction* f) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_PROLOG),
  _function(f)
{
}

EProlog::~EProlog() ASMJIT_NOTHROW
{
}

void EProlog::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset++;
  _function->_prepareVariables(this);
}

void EProlog::translate(CompilerContext& c) ASMJIT_NOTHROW
{
  _function->_allocVariables(c);
}

// ============================================================================
// [AsmJit::EEpilog]
// ============================================================================

EEpilog::EEpilog(Compiler* c, EFunction* f) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_EPILOG),
  _function(f)
{
}

EEpilog::~EEpilog() ASMJIT_NOTHROW
{
}

void EEpilog::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset++;
}

void EEpilog::translate(CompilerContext& c) ASMJIT_NOTHROW
{
}

// ============================================================================
// [AsmJit::CompilerContext - Construction / Destruction]
// ============================================================================

CompilerContext::CompilerContext(Compiler* compiler) ASMJIT_NOTHROW :
  _zone(8192 - sizeof(Zone::Chunk) - 32)
{
  _compiler = compiler;
  _clear();

  _emitComments = compiler->getLogger() != NULL && 
                  compiler->getLogger()->isUsed();
}

CompilerContext::~CompilerContext() ASMJIT_NOTHROW
{
}

// ============================================================================
// [AsmJit::CompilerContext - Clear]
// ============================================================================

void CompilerContext::_clear() ASMJIT_NOTHROW
{
  _zone.clear();
  _function = NULL;

  _start = NULL;
  _stop = NULL;

  _state.clear();
  _active = NULL;

  _forwardJumps = NULL;

  _currentOffset = 0;
  _unrecheable = false;

  _modifiedGPRegisters = 0;
  _modifiedMMRegisters = 0;
  _modifiedXMMRegisters = 0;

  _allocableEBP = false;
  _allocableESP = false;

  _argumentsBaseReg = INVALID_VALUE; // Used by patcher.
  _argumentsBaseOffset = 0;          // Used by patcher.
  _argumentsActualDisp = 0;          // Used by translate().

  _variablesBaseReg = INVALID_VALUE; // Used by patcher.
  _variablesBaseOffset = 0;          // Used by patcher.
  _variablesActualDisp = 0;          // Used by translate()

  _memUsed = NULL;
  _memFree = NULL;

  _mem4BlocksCount = 0;
  _mem8BlocksCount = 0;
  _mem16BlocksCount = 0;

  _memBytesTotal = 0;
}

// ============================================================================
// [AsmJit::CompilerContext - Construction / Destruction]
// ============================================================================

void CompilerContext::allocVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW
{
  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
#if defined(ASMJIT_X64)
    case VARIABLE_TYPE_GPQ:
#endif // ASMJIT_X64
      allocGPVar(vdata, regIndex, vflags);
      break;

    case VARIABLE_TYPE_X87:
    case VARIABLE_TYPE_X87_F:
    case VARIABLE_TYPE_X87_D:
      // TODO: X87 VARIABLES NOT IMPLEMENTED.
      break;

    case VARIABLE_TYPE_MM:
      allocMMVar(vdata, regIndex, vflags);
      break;

    case VARIABLE_TYPE_XMM:
    case VARIABLE_TYPE_XMM_1F:
    case VARIABLE_TYPE_XMM_4F:
    case VARIABLE_TYPE_XMM_1D:
    case VARIABLE_TYPE_XMM_2D:
      allocXMMVar(vdata, regIndex, vflags);
      break;
  }

  _postAlloc(vdata, vflags);
}

void CompilerContext::spillVar(VarData* vdata) ASMJIT_NOTHROW
{
  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
#if defined(ASMJIT_X64)
    case VARIABLE_TYPE_GPQ:
#endif // ASMJIT_X64
      spillGPVar(vdata);
      break;

    case VARIABLE_TYPE_X87:
    case VARIABLE_TYPE_X87_F:
    case VARIABLE_TYPE_X87_D:
      // TODO: X87 VARIABLES NOT IMPLEMENTED.
      break;

    case VARIABLE_TYPE_MM:
      spillMMVar(vdata);
      break;

    case VARIABLE_TYPE_XMM:
    case VARIABLE_TYPE_XMM_1F:
    case VARIABLE_TYPE_XMM_4F:
    case VARIABLE_TYPE_XMM_1D:
    case VARIABLE_TYPE_XMM_2D:
      spillXMMVar(vdata);
      break;
  }
}

void CompilerContext::unuseVar(VarData* vdata, uint32_t toState) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(toState != VARIABLE_STATE_REGISTER);

  if (vdata->state == VARIABLE_STATE_REGISTER)
  {
    uint32_t registerIndex = vdata->registerIndex;
    switch (vdata->type)
    {
      case VARIABLE_TYPE_GPD:
#if defined(ASMJIT_X64)
      case VARIABLE_TYPE_GPQ:
#endif // ASMJIT_X64
        _state.gp[registerIndex] = NULL;
        _freedGPRegister(registerIndex);
        break;

      case VARIABLE_TYPE_X87:
      case VARIABLE_TYPE_X87_F:
      case VARIABLE_TYPE_X87_D:
        // TODO: X87 VARIABLES NOT IMPLEMENTED.
        break;

      case VARIABLE_TYPE_MM:
        _state.mm[registerIndex] = NULL;
        _freedMMRegister(registerIndex);
        break;

      case VARIABLE_TYPE_XMM:
      case VARIABLE_TYPE_XMM_1F:
      case VARIABLE_TYPE_XMM_4F:
      case VARIABLE_TYPE_XMM_1D:
      case VARIABLE_TYPE_XMM_2D:
        _state.xmm[registerIndex] = NULL;
        _freedXMMRegister(registerIndex);
        break;
    }
  }

  vdata->state = toState;
  vdata->changed = false;
  vdata->registerIndex = INVALID_VALUE;
}

void CompilerContext::allocGPVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW
{
  uint32_t i;

  // Preferred register code.
  uint32_t pref = (regIndex != INVALID_VALUE) ? regIndex : vdata->prefRegisterIndex;
  // Last register code (aka home).
  uint32_t home = vdata->homeRegisterIndex;
  // New register code.
  uint32_t idx = INVALID_VALUE;

  // Preserved GP variables.
  uint32_t preservedGP = vdata->scope->getPrototype().getPreservedGP();

  // Spill candidate.
  VarData* spillCandidate = NULL;

  // --------------------------------------------------------------------------
  // [Already Allocated]
  // --------------------------------------------------------------------------

  // Go away if variable is already allocated.
  if (vdata->state == VARIABLE_STATE_REGISTER)
  {
    uint32_t oldIndex = vdata->registerIndex;
    uint32_t newIndex = pref;

    // Preferred register is none or same as currently allocated one, this is
    // best case.
    if (pref == INVALID_VALUE || oldIndex == newIndex) return;

    VarData* other = _state.gp[newIndex];

    emitExchangeVar(vdata, pref, vflags, other);
    _allocatedVariable(vdata);
    return;
  }

  // --------------------------------------------------------------------------
  // [Find Unused GP]
  // --------------------------------------------------------------------------

  // Preferred register.
  if (pref != INVALID_VALUE)
  {
    if ((_state.usedGP & (1U << pref)) == 0)
    {
      idx = pref;
    }
    else
    {
      // Spill register we need
      spillCandidate = _state.gp[pref];

      // Jump to spill part of allocation
      goto L_Spill;
    }
  }

  // Home register code.
  if (idx == INVALID_VALUE && home != INVALID_VALUE)
  {
    if ((_state.usedGP & (1U << home)) == 0) idx = home;
  }

  // We start from 1, because EAX/RAX register is sometimes explicitly
  // needed. So we trying to prevent register reallocation.
  if (idx == INVALID_VALUE)
  {
    uint32_t mask;
    for (i = 1, mask = (1 << i); i < REG_NUM; i++, mask <<= 1)
    {
      if ((_state.usedGP & mask) == 0 &&
          (i != REG_INDEX_EBP || _allocableEBP) &&
          (i != REG_INDEX_ESP || _allocableESP))
      {
        // Convenience to alloc non-preserved first.
        if (idx != INVALID_VALUE && (preservedGP & mask) != 0) continue;
        idx = i;

        // If current register is preserved, we should try to find different
        // one that is not. This can save one push / pop in prolog / epilog.
        if ((preservedGP & mask) == 0) break;
      }
    }
  }

  // If not found, try EAX/RAX.
  if (idx == INVALID_VALUE && (_state.usedGP & 1) == 0)
  {
    idx = REG_INDEX_EAX;
  }

  // --------------------------------------------------------------------------
  // [Spill]
  // --------------------------------------------------------------------------

  // If register is still not found, spill other variable.
  if (idx == INVALID_VALUE)
  {
    if (spillCandidate == NULL)
    {
      spillCandidate = _getSpillCandidateGP();
    }

    // Spill candidate not found?
    if (spillCandidate == NULL)
    {
      _compiler->setError(ERROR_NOT_ENOUGH_REGISTERS);
      return;
    }

L_Spill:

    // Prevented variables can't be spilled. _getSpillCandidate() never returns
    // prevented variables, but when jumping to L_spill it can happen.
    if (spillCandidate->workOffset == _currentOffset)
    {
      _compiler->setError(ERROR_REGISTERS_OVERLAP);
      return;
    }

    idx = spillCandidate->registerIndex;
    spillGPVar(spillCandidate);
  }

  // --------------------------------------------------------------------------
  // [Alloc]
  // --------------------------------------------------------------------------

  if (vdata->state == VARIABLE_STATE_MEMORY && (vflags & VARIABLE_ALLOC_READ) != 0)
  {
    emitLoadVar(vdata, idx);
  }

  // Update VarData.
  vdata->state = VARIABLE_STATE_REGISTER;
  vdata->registerIndex = idx;
  vdata->homeRegisterIndex = idx;

  // Update StateData.
  _allocatedVariable(vdata);
}

void CompilerContext::spillGPVar(VarData* vdata) ASMJIT_NOTHROW
{
  // Can't spill variable that isn't allocated.
  ASMJIT_ASSERT(vdata->state == VARIABLE_STATE_REGISTER);
  ASMJIT_ASSERT(vdata->registerIndex != INVALID_VALUE);

  uint32_t idx = vdata->registerIndex;

  if (vdata->changed) emitSaveVar(vdata, idx);

  // Update VarData.
  vdata->registerIndex = INVALID_VALUE;
  vdata->state = VARIABLE_STATE_MEMORY;
  vdata->changed = false;

  // Update StateData.
  _state.gp[idx] = NULL;
  _freedGPRegister(idx);
}

void CompilerContext::allocMMVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW
{
  uint32_t i;

  // Preferred register code.
  uint32_t pref = (regIndex != INVALID_VALUE) ? regIndex : vdata->prefRegisterIndex;
  // Last register code (aka home).
  uint32_t home = vdata->homeRegisterIndex;
  // New register code.
  uint32_t idx = INVALID_VALUE;

  // Preserved MM variables.
  //
  // NOTE: Currently MM variables are not preserved and there is no calling
  // convention known to me that does that. But on the other side it's possible
  // to write such calling convention.
  uint32_t preservedMM = 0; // vdata->scope->getPrototype().getPreservedMM();

  // Spill candidate.
  VarData* spillCandidate = NULL;

  // --------------------------------------------------------------------------
  // [Already Allocated]
  // --------------------------------------------------------------------------

  // Go away if variable is already allocated.
  if (vdata->state == VARIABLE_STATE_REGISTER)
  {
    uint32_t oldIndex = vdata->registerIndex;
    uint32_t newIndex = pref;

    // Preferred register is none or same as currently allocated one, this is
    // best case.
    if (pref == INVALID_VALUE || oldIndex == newIndex) return;

    VarData* other = _state.mm[newIndex];
    if (other) spillMMVar(other);

    _freedMMRegister(oldIndex);
    _allocatedVariable(vdata);

    emitMoveVar(vdata, pref, vflags);
    return;
  }

  // --------------------------------------------------------------------------
  // [Find Unused MM]
  // --------------------------------------------------------------------------

  // Preferred register.
  if (pref != INVALID_VALUE)
  {
    if ((_state.usedMM & (1U << pref)) == 0)
    {
      idx = pref;
    }
    else
    {
      // Spill register we need
      spillCandidate = _state.mm[pref];

      // Jump to spill part of allocation
      goto L_Spill;
    }
  }

  // Home register code.
  if (idx == INVALID_VALUE && home != INVALID_VALUE)
  {
    if ((_state.usedMM & (1U << home)) == 0) idx = home;
  }

  if (idx == INVALID_VALUE)
  {
    uint32_t mask;
    for (i = 0, mask = (1 << i); i < 8; i++, mask <<= 1)
    {
      if ((_state.usedMM & mask) == 0)
      {
        // Convenience to alloc non-preserved first.
        if (idx != INVALID_VALUE && (preservedMM & mask) != 0) continue;
        idx = i;

        // If current register is preserved, we should try to find different
        // one that is not. This can save one push / pop in prolog / epilog.
        if ((preservedMM & mask) == 0) break;
      }
    }
  }

  // --------------------------------------------------------------------------
  // [Spill]
  // --------------------------------------------------------------------------

  // If register is still not found, spill other variable.
  if (idx == INVALID_VALUE)
  {
    if (spillCandidate == NULL) spillCandidate = _getSpillCandidateMM();

    // Spill candidate not found?
    if (spillCandidate == NULL)
    {
      _compiler->setError(ERROR_NOT_ENOUGH_REGISTERS);
      return;
    }

L_Spill:

    // Prevented variables can't be spilled. _getSpillCandidate() never returns
    // prevented variables, but when jumping to L_spill it can happen.
    if (spillCandidate->workOffset == _currentOffset)
    {
      _compiler->setError(ERROR_REGISTERS_OVERLAP);
      return;
    }

    idx = spillCandidate->registerIndex;
    spillMMVar(spillCandidate);
  }

  // --------------------------------------------------------------------------
  // [Alloc]
  // --------------------------------------------------------------------------

  if (vdata->state == VARIABLE_STATE_MEMORY && (vflags & VARIABLE_ALLOC_READ) != 0)
  {
    emitLoadVar(vdata, idx);
  }

  // Update VarData.
  vdata->state = VARIABLE_STATE_REGISTER;
  vdata->registerIndex = idx;
  vdata->homeRegisterIndex = idx;

  // Update StateData.
  _allocatedVariable(vdata);
}

void CompilerContext::spillMMVar(VarData* vdata) ASMJIT_NOTHROW
{
  // Can't spill variable that isn't allocated.
  ASMJIT_ASSERT(vdata->state == VARIABLE_STATE_REGISTER);
  ASMJIT_ASSERT(vdata->registerIndex != INVALID_VALUE);

  uint32_t idx = vdata->registerIndex;

  if (vdata->changed) emitSaveVar(vdata, idx);

  // Update VarData.
  vdata->registerIndex = INVALID_VALUE;
  vdata->state = VARIABLE_STATE_MEMORY;
  vdata->changed = false;

  // Update StateData.
  _state.mm[idx] = NULL;
  _freedMMRegister(idx);
}

void CompilerContext::allocXMMVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW
{
  uint32_t i;

  // Preferred register code.
  uint32_t pref = (regIndex != INVALID_VALUE) ? regIndex : vdata->prefRegisterIndex;
  // Last register code (aka home).
  uint32_t home = vdata->homeRegisterIndex;
  // New register code.
  uint32_t idx = INVALID_VALUE;

  // Preserved XMM variables.
  uint32_t preservedXMM = vdata->scope->getPrototype().getPreservedXMM();

  // Spill candidate.
  VarData* spillCandidate = NULL;

  // --------------------------------------------------------------------------
  // [Already Allocated]
  // --------------------------------------------------------------------------

  // Go away if variable is already allocated.
  if (vdata->state == VARIABLE_STATE_REGISTER)
  {
    uint32_t oldIndex = vdata->registerIndex;
    uint32_t newIndex = pref;

    // Preferred register is none or same as currently allocated one, this is
    // best case.
    if (pref == INVALID_VALUE || oldIndex == newIndex) return;

    VarData* other = _state.xmm[newIndex];
    if (other) spillXMMVar(other);

    _freedXMMRegister(oldIndex);
    _allocatedVariable(vdata);

    emitMoveVar(vdata, pref, vflags);
    return;
  }

  // --------------------------------------------------------------------------
  // [Find Unused XMM]
  // --------------------------------------------------------------------------

  // Preferred register.
  if (pref != INVALID_VALUE)
  {
    if ((_state.usedXMM & (1U << pref)) == 0)
    {
      idx = pref;
    }
    else
    {
      // Spill register we need
      spillCandidate = _state.xmm[pref];

      // Jump to spill part of allocation
      goto L_Spill;
    }
  }

  // Home register code.
  if (idx == INVALID_VALUE && home != INVALID_VALUE)
  {
    if ((_state.usedXMM & (1U << home)) == 0) idx = home;
  }

  if (idx == INVALID_VALUE)
  {
    uint32_t mask;
    for (i = 0, mask = (1 << i); i < REG_NUM; i++, mask <<= 1)
    {
      if ((_state.usedXMM & mask) == 0)
      {
        // Convenience to alloc non-preserved first.
        if (idx != INVALID_VALUE && (preservedXMM & mask) != 0) continue;
        idx = i;

        // If current register is preserved, we should try to find different
        // one that is not. This can save one push / pop in prolog / epilog.
        if ((preservedXMM & mask) == 0) break;
      }
    }
  }

  // --------------------------------------------------------------------------
  // [Spill]
  // --------------------------------------------------------------------------

  // If register is still not found, spill other variable.
  if (idx == INVALID_VALUE)
  {
    if (spillCandidate == NULL) spillCandidate = _getSpillCandidateXMM();

    // Spill candidate not found?
    if (spillCandidate == NULL)
    {
      _compiler->setError(ERROR_NOT_ENOUGH_REGISTERS);
      return;
    }

L_Spill:

    // Prevented variables can't be spilled. _getSpillCandidate() never returns
    // prevented variables, but when jumping to L_spill it can happen.
    if (spillCandidate->workOffset == _currentOffset)
    {
      _compiler->setError(ERROR_REGISTERS_OVERLAP);
      return;
    }

    idx = spillCandidate->registerIndex;
    spillXMMVar(spillCandidate);
  }

  // --------------------------------------------------------------------------
  // [Alloc]
  // --------------------------------------------------------------------------

  if (vdata->state == VARIABLE_STATE_MEMORY && (vflags & VARIABLE_ALLOC_READ) != 0)
  {
    emitLoadVar(vdata, idx);
  }

  // Update VarData.
  vdata->state = VARIABLE_STATE_REGISTER;
  vdata->registerIndex = idx;
  vdata->homeRegisterIndex = idx;

  // Update StateData.
  _allocatedVariable(vdata);
}

void CompilerContext::spillXMMVar(VarData* vdata) ASMJIT_NOTHROW
{
  // Can't spill variable that isn't allocated.
  ASMJIT_ASSERT(vdata->state == VARIABLE_STATE_REGISTER);
  ASMJIT_ASSERT(vdata->registerIndex != INVALID_VALUE);

  uint32_t idx = vdata->registerIndex;

  if (vdata->changed) emitSaveVar(vdata, idx);

  // Update VarData.
  vdata->registerIndex = INVALID_VALUE;
  vdata->state = VARIABLE_STATE_MEMORY;
  vdata->changed = false;

  // Update StateData.
  _state.xmm[idx] = NULL;
  _freedXMMRegister(idx);
}

void CompilerContext::emitLoadVar(VarData* vdata, uint32_t regIndex) ASMJIT_NOTHROW
{
  Mem m;
  m._mem.id = vdata->id;

  _markMemoryUsed(vdata);

  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
      _compiler->emit(INST_MOV, gpd(regIndex), m);
      if (_emitComments) goto addComment;
      break;
#if defined(ASMJIT_X64)
    case VARIABLE_TYPE_GPQ:
      _compiler->emit(INST_MOV, gpq(regIndex), m);
      if (_emitComments) goto addComment;
      break;
#endif // ASMJIT_X64

    case VARIABLE_TYPE_X87:
    case VARIABLE_TYPE_X87_F:
    case VARIABLE_TYPE_X87_D:
      // TODO: X87 VARIABLES NOT IMPLEMENTED.
      break;

    case VARIABLE_TYPE_MM:
      _compiler->emit(INST_MOVQ, mm(regIndex), m);
      if (_emitComments) goto addComment;
      break;

    case VARIABLE_TYPE_XMM:
      _compiler->emit(INST_MOVDQA, xmm(regIndex), m);
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_1F:
      _compiler->emit(INST_MOVSS, xmm(regIndex), m);
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_1D:
      _compiler->emit(INST_MOVSD, xmm(regIndex), m);
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_4F:
      _compiler->emit(INST_MOVAPS, xmm(regIndex), m);
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_2D:
      _compiler->emit(INST_MOVAPD, xmm(regIndex), m);
      if (_emitComments) goto addComment;
      break;
  }
  return;

addComment:
  _compiler->getCurrentEmittable()->setCommentF("Alloc %s", vdata->name);
}

void CompilerContext::emitSaveVar(VarData* vdata, uint32_t regIndex) ASMJIT_NOTHROW
{
  // Caller must ensure that variable is allocated.
  ASMJIT_ASSERT(regIndex != INVALID_VALUE);

  Mem m;
  m._mem.id = vdata->id;

  _markMemoryUsed(vdata);

  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
      _compiler->emit(INST_MOV, m, gpd(regIndex));
      if (_emitComments) goto addComment;
      break;
#if defined(ASMJIT_X64)
    case VARIABLE_TYPE_GPQ:
      _compiler->emit(INST_MOV, m, gpq(regIndex));
      if (_emitComments) goto addComment;
      break;
#endif // ASMJIT_X64

    case VARIABLE_TYPE_X87:
    case VARIABLE_TYPE_X87_F:
    case VARIABLE_TYPE_X87_D:
      // TODO: X87 VARIABLES NOT IMPLEMENTED.
      break;

    case VARIABLE_TYPE_MM:
      _compiler->emit(INST_MOVQ, m, mm(regIndex));
      if (_emitComments) goto addComment;
      break;

    case VARIABLE_TYPE_XMM:
      _compiler->emit(INST_MOVDQA, m, xmm(regIndex));
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_1F:
      _compiler->emit(INST_MOVSS, m, xmm(regIndex));
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_1D:
      _compiler->emit(INST_MOVSD, m, xmm(regIndex));
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_4F:
      _compiler->emit(INST_MOVAPS, m, xmm(regIndex));
      if (_emitComments) goto addComment;
      break;
    case VARIABLE_TYPE_XMM_2D:
      _compiler->emit(INST_MOVAPD, m, xmm(regIndex));
      if (_emitComments) goto addComment;
      break;
  }
  return;

addComment:
  _compiler->getCurrentEmittable()->setCommentF("Spill %s", vdata->name);
}

void CompilerContext::emitMoveVar(VarData* vdata, uint32_t regIndex, uint32_t vflags) ASMJIT_NOTHROW
{
  // Caller must ensure that variable is allocated.
  ASMJIT_ASSERT(vdata->registerIndex != INVALID_VALUE);

  if ((vflags & VARIABLE_ALLOC_READ) == 0) return;

  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
      _compiler->emit(INST_MOV, gpd(regIndex), gpd(vdata->registerIndex));
      break;
#if defined(ASMJIT_X64)
    case VARIABLE_TYPE_GPQ:
      _compiler->emit(INST_MOV, gpq(regIndex), gpq(vdata->registerIndex));
      break;
#endif // ASMJIT_X64

    case VARIABLE_TYPE_X87:
    case VARIABLE_TYPE_X87_F:
    case VARIABLE_TYPE_X87_D:
      // TODO: X87 VARIABLES NOT IMPLEMENTED.
      break;

    case VARIABLE_TYPE_MM:
      _compiler->emit(INST_MOVQ, mm(regIndex), mm(vdata->registerIndex));
      break;

    case VARIABLE_TYPE_XMM:
      _compiler->emit(INST_MOVDQA, xmm(regIndex), xmm(vdata->registerIndex));
      break;
    case VARIABLE_TYPE_XMM_1F:
      _compiler->emit(INST_MOVSS, xmm(regIndex), xmm(vdata->registerIndex));
      break;
    case VARIABLE_TYPE_XMM_1D:
      _compiler->emit(INST_MOVSD, xmm(regIndex), xmm(vdata->registerIndex));
      break;
    case VARIABLE_TYPE_XMM_4F:
      _compiler->emit(INST_MOVAPS, xmm(regIndex), xmm(vdata->registerIndex));
      break;
    case VARIABLE_TYPE_XMM_2D:
      _compiler->emit(INST_MOVAPD, xmm(regIndex), xmm(vdata->registerIndex));
      break;
  }
}

void CompilerContext::emitExchangeVar(VarData* vdata, uint32_t regIndex, uint32_t vflags, VarData* other) ASMJIT_NOTHROW
{
  // Caller must ensure that variable is allocated.
  ASMJIT_ASSERT(vdata->registerIndex != INVALID_VALUE);

  // If other is not valid then we can just emit MOV (or other similar instruction).
  if (other == NULL)
  {
    emitMoveVar(vdata, regIndex, vflags);
    return;
  }

  // If we need to alloc for write-only operation then we can move other
  // variable away instead of exchanging them.
  if ((vflags & VARIABLE_ALLOC_READ) == 0)
  {
    emitMoveVar(other, vdata->registerIndex, VARIABLE_ALLOC_READ);
    return;
  }

  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
      _compiler->emit(INST_XCHG, gpd(regIndex), gpd(vdata->registerIndex));
      break;
#if defined(ASMJIT_X64)
    case VARIABLE_TYPE_GPQ:
      _compiler->emit(INST_XCHG, gpq(regIndex), gpq(vdata->registerIndex));
      break;
#endif // ASMJIT_X64

    case VARIABLE_TYPE_X87:
    case VARIABLE_TYPE_X87_F:
    case VARIABLE_TYPE_X87_D:
      // TODO: X87 VARIABLES NOT IMPLEMENTED.
      break;

    // NOTE: MM and XMM registers shoudln't be exchanged using this way, it's
    // correct, but it sucks.

    case VARIABLE_TYPE_MM:
    {
      MMReg a = mm(regIndex);
      MMReg b = mm(vdata->registerIndex);

      _compiler->emit(INST_PXOR, a, b);
      _compiler->emit(INST_PXOR, b, a);
      _compiler->emit(INST_PXOR, a, b);
      break;
    }

    case VARIABLE_TYPE_XMM_1F:
    case VARIABLE_TYPE_XMM_4F:
    {
      XMMReg a = xmm(regIndex);
      XMMReg b = xmm(vdata->registerIndex);

      _compiler->emit(INST_XORPS, a, b);
      _compiler->emit(INST_XORPS, b, a);
      _compiler->emit(INST_XORPS, a, b);
      break;
    }

    case VARIABLE_TYPE_XMM_1D:
    case VARIABLE_TYPE_XMM_2D:
    {
      XMMReg a = xmm(regIndex);
      XMMReg b = xmm(vdata->registerIndex);

      _compiler->emit(INST_XORPD, a, b);
      _compiler->emit(INST_XORPD, b, a);
      _compiler->emit(INST_XORPD, a, b);
      break;
    }

    case VARIABLE_TYPE_XMM:
    {
      XMMReg a = xmm(regIndex);
      XMMReg b = xmm(vdata->registerIndex);

      _compiler->emit(INST_PXOR, a, b);
      _compiler->emit(INST_PXOR, b, a);
      _compiler->emit(INST_PXOR, a, b);
      break;
    }
  }
}

void CompilerContext::_postAlloc(VarData* vdata, uint32_t vflags) ASMJIT_NOTHROW
{
  if (vflags & VARIABLE_ALLOC_WRITE) vdata->changed = true;
}

void CompilerContext::_markMemoryUsed(VarData* vdata) ASMJIT_NOTHROW
{
  if (vdata->homeMemoryData != NULL) return;

  VarMemBlock* mem = _allocMemBlock(vdata->size);
  if (!mem) return;

  vdata->homeMemoryData = mem;
}

static int32_t getSpillScore(VarData* v, uint32_t currentOffset)
{
  int32_t score = 0;

  ASMJIT_ASSERT(v->lastEmittable != NULL);
  uint32_t lastOffset = v->lastEmittable->getOffset();

  if (lastOffset >= currentOffset)
    score += (int32_t)(lastOffset - currentOffset);

  // Each write access decreases probability of spill.
  score -= (int32_t)v->registerWriteCount + (int32_t)v->registerRWCount;
  // Each read-only access increases probability of spill.
  score += (int32_t)v->registerReadCount;

  // Each memory access increases probability of spill.
  score += (int32_t)v->memoryWriteCount + (int32_t)v->memoryRWCount;
  score += (int32_t)v->memoryReadCount;

  return score;
}

VarData* CompilerContext::_getSpillCandidateGP() ASMJIT_NOTHROW
{
  return _getSpillCandidateGeneric(_state.gp, REG_NUM);
}

VarData* CompilerContext::_getSpillCandidateMM() ASMJIT_NOTHROW
{
  return _getSpillCandidateGeneric(_state.mm, 8);
}

VarData* CompilerContext::_getSpillCandidateXMM() ASMJIT_NOTHROW
{
  return _getSpillCandidateGeneric(_state.xmm, REG_NUM);
}

VarData* CompilerContext::_getSpillCandidateGeneric(VarData** varArray, uint32_t count) ASMJIT_NOTHROW
{
  uint32_t i;

  VarData* candidate = NULL;
  uint32_t candidatePriority = 0;
  int32_t candidateScore = 0;

  uint32_t currentOffset = _compiler->getCurrentEmittable()->getOffset();

  for (i = 0; i < count; i++)
  {
    // Get variable.
    VarData* vdata = varArray[i];

    // Never spill variables needed for next instruction.
    if (vdata == NULL || vdata->workOffset == _currentOffset) continue;

    uint32_t variablePriority = vdata->priority;
    int32_t variableScore = getSpillScore(vdata, currentOffset);

    if ((candidate == NULL) ||
        (variablePriority > candidatePriority) ||
        (variablePriority == candidatePriority && variableScore > candidateScore))
    {
      candidate = vdata;
      candidatePriority = variablePriority;
      candidateScore = variableScore;
    }
  }

  return candidate;
}

void CompilerContext::_addActive(VarData* vdata) ASMJIT_NOTHROW
{
  // Never call with variable that is already in active list.
  ASMJIT_ASSERT(vdata->nextActive == NULL);
  ASMJIT_ASSERT(vdata->prevActive == NULL);

  if (_active == NULL)
  {
    vdata->nextActive = vdata;
    vdata->prevActive = vdata;

    _active = vdata;
  }
  else
  {
    VarData* vlast = _active->prevActive;

    vlast->nextActive = vdata;
    _active->prevActive = vdata;

    vdata->nextActive = _active;
    vdata->prevActive = vlast;
  }
}

void CompilerContext::_freeActive(VarData* vdata) ASMJIT_NOTHROW
{
  VarData* next = vdata->nextActive;
  VarData* prev = vdata->prevActive;

  if (prev == next)
  {
    _active = NULL;
  }
  else
  {
    if (_active == vdata) _active = next;
    prev->nextActive = next;
    next->prevActive = prev;
  }

  vdata->nextActive = NULL;
  vdata->prevActive = NULL;
}

void CompilerContext::_freeAllActive() ASMJIT_NOTHROW
{
  if (_active == NULL) return;

  VarData* cur = _active;
  for (;;)
  {
    VarData* next = cur->nextActive;
    cur->nextActive = NULL;
    cur->prevActive = NULL;
    if (next == _active) break;
  }

  _active = NULL;
}

void CompilerContext::_allocatedVariable(VarData* vdata) ASMJIT_NOTHROW
{
  uint32_t idx = vdata->registerIndex;

  switch (vdata->type)
  {
    case VARIABLE_TYPE_GPD:
    case VARIABLE_TYPE_GPQ:
      _state.gp[idx] = vdata;
      _allocatedGPRegister(idx);
      break;

    case VARIABLE_TYPE_MM:
      _state.mm[idx] = vdata;
      _allocatedMMRegister(idx);
      break;

    case VARIABLE_TYPE_XMM:
    case VARIABLE_TYPE_XMM_1F:
    case VARIABLE_TYPE_XMM_4F:
    case VARIABLE_TYPE_XMM_1D:
    case VARIABLE_TYPE_XMM_2D:
      _state.xmm[idx] = vdata;
      _allocatedXMMRegister(idx);
      break;

    default:
      ASMJIT_ASSERT(0);
      break;
  }
}

void CompilerContext::addForwardJump(EJmpInstruction* inst) ASMJIT_NOTHROW
{
  ForwardJumpData* j = 
    reinterpret_cast<ForwardJumpData*>(_zone.zalloc(sizeof(ForwardJumpData)));
  if (j == NULL) { _compiler->setError(ERROR_NO_HEAP_MEMORY); return; }

  j->inst = inst;
  j->state = _saveState();
  j->next = _forwardJumps;
  _forwardJumps = j;
}

StateData* CompilerContext::_saveState() ASMJIT_NOTHROW
{
  StateData* state = _compiler->_newStateData();
  memcpy(state, &_state, sizeof(StateData));

  state->changedGP = 0;
  state->changedMM = 0;
  state->changedXMM = 0;

  uint i;
  uint mask;

  for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
  {
    if (state->gp [i] && state->gp [i]->changed) state->changedGP  |= mask;
    if (state->xmm[i] && state->xmm[i]->changed) state->changedXMM |= mask;
  }
  for (i = 0, mask = 1; i < 8; i++, mask <<= 1)
  {
    if (state->mm [i] && state->mm [i]->changed) state->changedMM  |= mask;
  }

  return state;
}

void CompilerContext::_assignState(StateData* state) ASMJIT_NOTHROW
{
  memcpy(&_state, state, sizeof(StateData));

  _state.changedGP = state->changedGP;
  _state.changedMM = state->changedMM;
  _state.changedXMM = state->changedXMM;

  uint i;
  uint mask;

  for (i = 0, mask = 1; i < REG_NUM; i++, mask <<= 1)
  {
    if (state->changedGP  & mask) state->gp[i]->changed = 1;
    if (state->changedXMM & mask) state->xmm[i]->changed = 1;
  }
  for (i = 0, mask = 1; i < 8; i++, mask <<= 1)
  {
    if (state->changedMM  & mask) state->mm[i]->changed = 1;
  }
}

void CompilerContext::_restoreState(StateData* state) ASMJIT_NOTHROW
{
  StateData* fromState = &_state;
  StateData* toState = state;

  if (fromState == toState) return;

  uint32_t base;
  uint32_t i;

  // Spill.
  for (base = 0, i = 0; i < 16 + 8 + 16; i++)
  {
    if (i == 16 || i == 16 + 8) base = i;

    VarData* fromVar = fromState->regs[i];
    VarData* toVar = toState->regs[i];

    if (fromVar != toVar)
    {
      uint32_t regIndex = i - base;

      // Spill register
      if (fromVar != NULL)
      {
        // It is possible that variable that was saved in state currently not 
        // exists.
        if (fromVar->state == VARIABLE_STATE_UNUSED)
        {
          // TODO:
          // Optimization, do not spill it, we can simply abandon it
          // _freeReg(getVariableRegisterCode(from_v->type(), regIndex));
        }
        else
        {
          // Variables match, do normal spill
          spillVar(fromVar);
        }
      }
    }
  }

  // Alloc.
  for (base = 0, i = 0; i < 16 + 8 + 16; i++)
  {
    if (i == 16 || i == 24) base = i;

    VarData* fromVar = fromState->regs[i];
    VarData* toVar = toState->regs[i];

    if (fromVar != toVar)
    {
      uint32_t regIndex = i - base;

      // Alloc register
      if (toVar != NULL)
      {
        allocVar(toVar, regIndex, VARIABLE_ALLOC_READ);
      }
    }

    // TODO:
    //if (toVar)
    //{
      // toVar->changed = to->changed;
    //}
  }

  // Update masks
  _state.usedGP = state->usedGP;
  _state.usedMM = state->usedMM;
  _state.usedXMM = state->usedXMM;
}

VarMemBlock* CompilerContext::_allocMemBlock(uint32_t size) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(size != 0);

  // First try to find mem blocks.
  VarMemBlock* mem = _memFree;
  VarMemBlock* prev = NULL;

  while (mem)
  {
    VarMemBlock* next = mem->nextFree;

    if (mem->size == size)
    {
      if (prev)
        prev->nextFree = next;
      else
        _memFree = next;

      mem->nextFree = NULL;
      return mem;
    }

    prev = mem;
    mem = next;
  }

  // Never mind, create new.
  mem = reinterpret_cast<VarMemBlock*>(_zone.zalloc(sizeof(VarMemBlock)));
  if (!mem)
  {
    _compiler->setError(ERROR_NO_HEAP_MEMORY);
    return NULL;
  }

  mem->offset = 0;
  mem->size = size;

  mem->nextUsed = _memUsed;
  mem->nextFree = NULL;

  _memUsed = mem;

  switch (size)
  {
    case 16: _mem16BlocksCount++; break;
    case 8: _mem8BlocksCount++; break;
    case 4: _mem4BlocksCount++; break;
  }

  return mem;
}

void CompilerContext::_freeMemBlock(VarMemBlock* mem) ASMJIT_NOTHROW
{
  // Add mem to free blocks.
  mem->nextFree = _memFree;
  _memFree = mem;
}

void CompilerContext::_allocMemoryOperands() ASMJIT_NOTHROW
{
  VarMemBlock* mem;

  // Variables are allocated in this order:
  // 1. 16-bytes variables.
  // 2. 8-bytes variables.
  // 3. 4-bytes variables.
  // 4. All others.

  uint32_t start16 = 0;
  uint32_t start8 = start16 + _mem16BlocksCount * 16;
  uint32_t start4 = start8 + _mem8BlocksCount * 8;
  uint32_t startX = (start4 + _mem4BlocksCount * 4 + 15) & ~15;

  for (mem = _memUsed; mem; mem = mem->nextUsed)
  {
    uint32_t size = mem->size;
    uint32_t offset;

    switch (size)
    {
      case 16:
        offset = start16;
        start16 += 16;
        break;

      case 8:
        offset = start8;
        start8 += 8;
        break;

      case 4:
        offset = start4;
        start4 += 4;
        break;

      default:
        // Align to 16 bytes if size is 16 or more.
        if (size >= 16)
        {
          size = (size + 15) & ~15;
          startX = (startX + 15) & ~15;
        }
        offset = startX;
        startX += size;
        break;
    }

    mem->offset = (int32_t)offset;
    _memBytesTotal += size;
  }
}

void CompilerContext::_patchMemoryOperands() ASMJIT_NOTHROW
{
  Emittable* cur;

  for (cur = _compiler->getFirstEmittable(); cur; cur = cur->getNext())
  {
    if (cur->getType() == EMITTABLE_INSTRUCTION)
    {
      Mem* mem = reinterpret_cast<EInstruction*>(cur)->_memOp;

      if (mem && (mem->_mem.id & OPERAND_ID_TYPE_MASK) == OPERAND_ID_TYPE_VAR)
      {
        VarData* vdata = _compiler->_getVarData(mem->_mem.id);
        ASMJIT_ASSERT(vdata != NULL);

        if (vdata->isMemArgument)
        {
          mem->_mem.base = _argumentsBaseReg;
          mem->_mem.displacement += vdata->homeMemoryOffset;
          mem->_mem.displacement += _argumentsBaseOffset;
        }
        else
        {
          VarMemBlock* mb = reinterpret_cast<VarMemBlock*>(vdata->homeMemoryData);
          ASMJIT_ASSERT(mb != NULL);

          mem->_mem.base = _variablesBaseReg;
          mem->_mem.displacement += mb->offset;
          mem->_mem.displacement += _variablesBaseOffset;
        }
      }
    }
  }
}

// ============================================================================
// [AsmJit::CompilerCore - Construction / Destruction]
// ============================================================================

CompilerCore::CompilerCore() ASMJIT_NOTHROW :
  _zone(16384 - sizeof(Zone::Chunk) - 32),
  _logger(NULL),
  _error(0),
  _properties((1 << PROPERTY_OPTIMIZE_ALIGN)),
  _finished(false),
  _first(NULL),
  _last(NULL),
  _current(NULL),
  _function(NULL),
  _varNameId(0)
{
}

CompilerCore::~CompilerCore() ASMJIT_NOTHROW
{
  free();
}

// ============================================================================
// [AsmJit::CompilerCore - Logging]
// ============================================================================

void CompilerCore::setLogger(Logger* logger) ASMJIT_NOTHROW
{
  _logger = logger;
}

// ============================================================================
// [AsmJit::CompilerCore - Error Handling]
// ============================================================================

void CompilerCore::setError(uint32_t error) ASMJIT_NOTHROW
{
  _error = error;

  // Log.
  if (_error != ERROR_NONE && _logger && _logger->isUsed())
  {
    _logger->logFormat("*** COMPILER ERROR: %s (%u).\n",
      getErrorCodeAsString(error),
      (unsigned int)error);
  }
}

// ============================================================================
// [AsmJit::CompilerCore - Properties]
// ============================================================================

uint32_t CompilerCore::getProperty(uint32_t propertyId)
{
  return (_properties & (1 << propertyId)) != 0;
}

void CompilerCore::setProperty(uint32_t propertyId, uint32_t value)
{
  if (value)
    _properties |= (1 << propertyId);
  else
    _properties &= ~(1 << propertyId);
}

// ============================================================================
// [AsmJit::CompilerCore - Buffer]
// ============================================================================

void CompilerCore::clear() ASMJIT_NOTHROW
{
  _finished = false;

  delAll(_first);
  _first = NULL;
  _last = NULL;

  _zone.freeAll();
  _targetData.clear();
  _varData.clear();

  if (_error) setError(ERROR_NONE);
}

void CompilerCore::free() ASMJIT_NOTHROW
{
  clear();

  _targetData.free();
  _varData.free();
}

// ============================================================================
// [AsmJit::CompilerCore - Emittables]
// ============================================================================

void CompilerCore::addEmittable(Emittable* emittable) ASMJIT_NOTHROW
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

void CompilerCore::addEmittableAfter(Emittable* emittable, Emittable* ref) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(emittable != NULL);
  ASMJIT_ASSERT(emittable->_prev == NULL);
  ASMJIT_ASSERT(emittable->_next == NULL);
  ASMJIT_ASSERT(ref != NULL);

  Emittable* prev = ref;
  Emittable* next = ref->_next;

  emittable->_prev = prev;
  emittable->_next = next;

  prev->_next = emittable;
  if (next)
    next->_prev = emittable;
  else
    _last = emittable;
}

void CompilerCore::removeEmittable(Emittable* emittable) ASMJIT_NOTHROW
{
  Emittable* prev = emittable->_prev;
  Emittable* next = emittable->_next;

  if (_first == emittable) { _first = next; } else { prev->_next = next; }
  if (_last  == emittable) { _last  = prev; } else { next->_prev = prev; }

  emittable->_prev = NULL;
  emittable->_next = NULL;

  if (emittable == _current) _current = prev;
}

Emittable* CompilerCore::setCurrentEmittable(Emittable* current) ASMJIT_NOTHROW
{
  Emittable* old = _current;
  _current = current;
  return old;
}

// ============================================================================
// [AsmJit::CompilerCore - Logging]
// ============================================================================

void CompilerCore::comment(const char* fmt, ...) ASMJIT_NOTHROW
{
  char buf[128];
  char* p = buf;

  if (fmt)
  {
    *p++ = ';';
    *p++ = ' ';

    va_list ap;
    va_start(ap, fmt);
    p += vsnprintf(p, ASMJIT_ARRAY_SIZE(buf) - 1, fmt, ap);
    va_end(ap);
  }

  *p++ = '\n';
  *p   = '\0';

  addEmittable(Compiler_newObject<EComment>(this, buf));
}

// ============================================================================
// [AsmJit::CompilerCore - Function Builder]
// ============================================================================

EFunction* CompilerCore::newFunction_(uint32_t callingConvention, const uint32_t* args, sysuint_t count) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(_function == NULL);
  EFunction* f = _function = Compiler_newObject<EFunction>(this);

  f->setPrototype(callingConvention, args, count);
  addEmittable(f);

  bind(f->_entryLabel);
  addEmittable(f->_prolog);

  _varNameId = 0;

  f->_createVariables();
  return f;
}

EFunction* CompilerCore::endFunction() ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(_function != NULL);
  EFunction* f = _function;

  bind(f->_exitLabel);
  addEmittable(f->_epilog);
  addEmittable(f->_end);

  _function = NULL;
  return f;
}

// ============================================================================
// [AsmJit::CompilerCore - EmitInstruction]
// ============================================================================

void CompilerCore::_emitInstruction(uint32_t code) ASMJIT_NOTHROW
{
  EInstruction* e = newInstruction(code, NULL, 0);
  if (!e) return;

  addEmittable(e);
}

void CompilerCore::_emitInstruction(uint32_t code, const Operand* o0) ASMJIT_NOTHROW
{
  Operand* operands = reinterpret_cast<Operand*>(_zone.zalloc(1 * sizeof(Operand)));
  if (!operands) return;

  operands[0] = *o0;

  EInstruction* e = newInstruction(code, operands, 1);
  if (!e) return;

  addEmittable(e);
}

void CompilerCore::_emitInstruction(uint32_t code, const Operand* o0, const Operand* o1) ASMJIT_NOTHROW
{
  Operand* operands = reinterpret_cast<Operand*>(_zone.zalloc(2 * sizeof(Operand)));
  if (!operands) return;

  operands[0] = *o0;
  operands[1] = *o1;

  EInstruction* e = newInstruction(code, operands, 2);
  if (!e) return;

  addEmittable(e);
}

void CompilerCore::_emitInstruction(uint32_t code, const Operand* o0, const Operand* o1, const Operand* o2) ASMJIT_NOTHROW
{
  Operand* operands = reinterpret_cast<Operand*>(_zone.zalloc(3 * sizeof(Operand)));
  if (!operands) return;

  operands[0] = *o0;
  operands[1] = *o1;
  operands[2] = *o2;

  EInstruction* e = newInstruction(code, operands, 3);
  if (!e) return;

  addEmittable(e);
}

void CompilerCore::_emitInstruction(uint32_t code, const Operand* o0, const Operand* o1, const Operand* o2, const Operand* o3) ASMJIT_NOTHROW
{
  Operand* operands = reinterpret_cast<Operand*>(_zone.zalloc(4 * sizeof(Operand)));
  if (!operands) return;

  operands[0] = *o0;
  operands[1] = *o1;
  operands[2] = *o2;
  operands[3] = *o3;

  EInstruction* e = newInstruction(code, operands, 4);
  if (!e) return;

  addEmittable(e);
}

void CompilerCore::_emitJcc(uint32_t code, const Label* label, uint32_t hint) ASMJIT_NOTHROW
{
  if (!hint)
  {
    _emitInstruction(code, label);
  }
  else
  {
    Imm imm(hint);
    _emitInstruction(code, label, &imm);
  }
}

void CompilerCore::_emitReturn(const Operand* val)
{
  // TODO.
}

// ============================================================================
// [AsmJit::CompilerCore - Embed]
// ============================================================================

void CompilerCore::embed(const void* data, sysuint_t size) ASMJIT_NOTHROW
{
  // Align length to 16 bytes.
  sysuint_t alignedSize = (size + 15) & ~15;

  EData* e =
    new(_zone.zalloc(sizeof(EData) - sizeof(void*) + alignedSize))
      EData(reinterpret_cast<Compiler*>(this), data, size);
  addEmittable(e);
}

// ============================================================================
// [AsmJit::CompilerCore - Align]
// ============================================================================

void CompilerCore::align(uint32_t m) ASMJIT_NOTHROW
{
  addEmittable(Compiler_newObject<EAlign>(this, m));
}

// ============================================================================
// [AsmJit::CompilerCore - Label]
// ============================================================================

Label CompilerCore::newLabel() ASMJIT_NOTHROW
{
  Label label;
  label._base.id = _targetData.getLength() | OPERAND_ID_TYPE_LABEL;

  ETarget* target = Compiler_newObject<ETarget>(this, label);
  _targetData.append(target);

  return label;
}

void CompilerCore::bind(const Label& label) ASMJIT_NOTHROW
{
  uint32_t id = label.getId() & OPERAND_ID_VALUE_MASK;
  ASMJIT_ASSERT(id != INVALID_VALUE);
  ASMJIT_ASSERT(id < _targetData.getLength());

  addEmittable(_targetData[id]);
}

// ============================================================================
// [AsmJit::CompilerCore - Variables]
// ============================================================================

VarData* CompilerCore::_newVarData(const char* name, uint32_t type, uint32_t size) ASMJIT_NOTHROW
{
  VarData* vdata = reinterpret_cast<VarData*>(_zone.zalloc(sizeof(VarData)));
  if (vdata == NULL) return NULL;

  char nameBuffer[32];
  if (name == NULL)
  {
    sprintf(nameBuffer, "var_%d", _varNameId);
    name = nameBuffer;
    _varNameId++;
  }

  vdata->scope = getFunction();
  vdata->firstEmittable = NULL;
  vdata->lastEmittable = NULL;

  vdata->name = _zone.zstrdup(name);
  vdata->id = _varData.getLength() | OPERAND_ID_TYPE_VAR;
  vdata->type = type;
  vdata->size = size;

  vdata->homeRegisterIndex = INVALID_VALUE;
  vdata->prefRegisterIndex = INVALID_VALUE;

  vdata->homeMemoryRegister = INVALID_VALUE;
  vdata->homeMemoryOffset = 0;
  vdata->homeMemoryData = NULL;

  vdata->registerIndex = INVALID_VALUE;
  vdata->workOffset = INVALID_VALUE;

  vdata->nextActive = NULL;
  vdata->prevActive = NULL;

  vdata->priority = 10;
  vdata->calculated = false;
  vdata->isRegArgument = false;
  vdata->isMemArgument = false;

  vdata->state = VARIABLE_STATE_UNUSED;
  vdata->changed = false;

  vdata->registerReadCount = 0;
  vdata->registerWriteCount = 0;
  vdata->registerRWCount = 0;

  vdata->registerGPBLoCount = 0;
  vdata->registerGPBHiCount = 0;

  vdata->memoryReadCount = 0;
  vdata->memoryWriteCount = 0;
  vdata->memoryRWCount = 0;

  _varData.append(vdata);
  return vdata;
}

GPVar CompilerCore::newGP(uint32_t variableType, const char* name) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT((variableType < _VARIABLE_TYPE_COUNT) &&
                (variableInfo[variableType].clazz & VariableInfo::CLASS_GP) != 0);

  GPVar var;
  VarData* vdata = _newVarData(name, variableType, sizeof(sysint_t));

  var._var.id = vdata->id;
  var._var.size = vdata->size;
  var._var.registerCode = variableInfo[vdata->type].code;
  var._var.variableType = vdata->type;

  return var;
}

GPVar CompilerCore::argGP(uint32_t index) ASMJIT_NOTHROW
{
  GPVar var;
  EFunction* f = getFunction();

  if (f)
  {
    const FunctionPrototype& prototype = f->getPrototype();
    if (index < prototype.getArgumentsCount())
    {
      VarData* vdata = getFunction()->_argumentVariables[index];

      var._var.id = vdata->id;
      var._var.size = vdata->size;
      var._var.registerCode = variableInfo[vdata->type].code;
      var._var.variableType = vdata->type;
    }
  }

  return var;
}

MMVar CompilerCore::newMM(uint32_t variableType, const char* name) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT((variableType < _VARIABLE_TYPE_COUNT) &&
                (variableInfo[variableType].clazz & VariableInfo::CLASS_MM) != 0);

  MMVar var;
  VarData* vdata = _newVarData(name, variableType, 8);

  var._var.id = vdata->id;
  var._var.size = vdata->size;
  var._var.registerCode = variableInfo[vdata->type].code;
  var._var.variableType = vdata->type;

  return var;
}

MMVar CompilerCore::argMM(uint32_t index) ASMJIT_NOTHROW
{
  MMVar var;
  EFunction* f = getFunction();

  if (f)
  {
    const FunctionPrototype& prototype = f->getPrototype();
    if (prototype.getArgumentsCount() < index)
    {
      VarData* vdata = getFunction()->_argumentVariables[index];

      var._var.id = vdata->id;
      var._var.size = vdata->size;
      var._var.registerCode = variableInfo[vdata->type].code;
      var._var.variableType = vdata->type;
    }
  }

  return var;
}

XMMVar CompilerCore::newXMM(uint32_t variableType, const char* name) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT((variableType < _VARIABLE_TYPE_COUNT) &&
                (variableInfo[variableType].clazz & VariableInfo::CLASS_XMM) != 0);

  XMMVar var;
  VarData* vdata = _newVarData(name, variableType, 16);

  var._var.id = vdata->id;
  var._var.size = vdata->size;
  var._var.registerCode = variableInfo[vdata->type].code;
  var._var.variableType = vdata->type;

  return var;
}

XMMVar CompilerCore::argXMM(uint32_t index) ASMJIT_NOTHROW
{
  XMMVar var;
  EFunction* f = getFunction();

  if (f)
  {
    const FunctionPrototype& prototype = f->getPrototype();
    if (prototype.getArgumentsCount() < index)
    {
      VarData* vdata = getFunction()->_argumentVariables[index];

      var._var.id = vdata->id;
      var._var.size = vdata->size;
      var._var.registerCode = variableInfo[vdata->type].code;
      var._var.variableType = vdata->type;
    }
  }

  return var;
}

void CompilerCore::_vhint(BaseVar& var, uint32_t hintId, uint32_t hintValue) ASMJIT_NOTHROW
{
  if (var.getId() == INVALID_VALUE) return;

  VarData* vdata = _getVarData(var.getId());
  ASMJIT_ASSERT(vdata != NULL);

  EVariableHint* e = Compiler_newObject<EVariableHint>(this, vdata, hintId, hintValue);
  addEmittable(e);
}

void CompilerCore::alloc(BaseVar& var) ASMJIT_NOTHROW
{
  _vhint(var, VARIABLE_HINT_ALLOC, INVALID_VALUE);
}

void CompilerCore::alloc(BaseVar& var, uint32_t regIndex) ASMJIT_NOTHROW
{
  _vhint(var, VARIABLE_HINT_ALLOC, regIndex);
}

void CompilerCore::alloc(BaseVar& var, const BaseReg& reg) ASMJIT_NOTHROW
{
  _vhint(var, VARIABLE_HINT_ALLOC, reg.getRegIndex());
}

void CompilerCore::spill(BaseVar& var) ASMJIT_NOTHROW
{
  _vhint(var, VARIABLE_HINT_SPILL, INVALID_VALUE);
}

void CompilerCore::unuse(BaseVar& var) ASMJIT_NOTHROW
{
  _vhint(var, VARIABLE_HINT_UNUSE, INVALID_VALUE);
}

uint32_t CompilerCore::getPriority(BaseVar& var) const ASMJIT_NOTHROW
{
  if (var.getId() == INVALID_VALUE) return INVALID_VALUE;

  VarData* vdata = _getVarData(var.getId());
  ASMJIT_ASSERT(vdata != NULL);

  return vdata->priority;
}

void CompilerCore::setPriority(BaseVar& var, uint priority) ASMJIT_NOTHROW
{
  if (var.getId() == INVALID_VALUE) return;

  VarData* vdata = _getVarData(var.getId());
  ASMJIT_ASSERT(vdata != NULL);

  if (priority > 100) priority = 100;
  vdata->priority = (uint8_t)priority;
}

void CompilerCore::rename(BaseVar& var, const char* name) ASMJIT_NOTHROW
{
  if (var.getId() == INVALID_VALUE) return;

  VarData* vdata = _getVarData(var.getId());
  ASMJIT_ASSERT(vdata != NULL);

  vdata->name = _zone.zstrdup(name);
}

// ============================================================================
// [AsmJit::CompilerCore - State]
// ============================================================================

StateData* CompilerCore::_newStateData() ASMJIT_NOTHROW
{
  StateData* state = reinterpret_cast<StateData*>(_zone.zalloc(sizeof(StateData)));
  state->clear();
  return state;
}

// ============================================================================
// [AsmJit::CompilerCore - Make]
// ============================================================================

void* CompilerCore::make(MemoryManager* memoryManager, uint32_t allocType) ASMJIT_NOTHROW
{
  Assembler a;
  a._properties = _properties;
  serialize(a);

  if (a.getError())
  {
    setError(a.getError());
    return NULL;
  }
  else
  {
    if (_logger && _logger->isUsed())
    {
      _logger->logFormat("*** COMPILER SUCCESS (wrote %u bytes).\n\n", (unsigned int)a.getCodeSize());
    }

    return a.make(memoryManager, allocType);
  }
}

// Logger switcher used in Compiler::serialize().
struct ASMJIT_HIDDEN LoggerSwitcher
{
  LoggerSwitcher(Assembler* a, Compiler* c) ASMJIT_NOTHROW :
    a(a),
    logger(a->getLogger())
  {
    // Set compiler logger.
    if (!logger && c->getLogger()) a->setLogger(c->getLogger());
  }

  ~LoggerSwitcher() ASMJIT_NOTHROW
  {
    // Restore logger.
    a->setLogger(logger);
  }

  Assembler* a;
  Logger* logger;
};

void CompilerCore::serialize(Assembler& a) ASMJIT_NOTHROW
{
  LoggerSwitcher loggerSwitcher(&a, reinterpret_cast<Compiler*>(this));

  // Context.
  CompilerContext c(reinterpret_cast<Compiler*>(this));

  Emittable* start = _first;
  Emittable* stop = NULL;

  // Register all labels.
  a.registerLabels(_targetData.getLength());

  // Make code.
  for (;;)
  {
    // ------------------------------------------------------------------------
    // Find function.
    for (;;)
    {
      if (start == NULL) return;
      if (start->getType() == EMITTABLE_FUNCTION)
      {
        break;
      }
      start = start->getNext();
    }
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Setup code generation context.
    Emittable* cur;

    c._function = reinterpret_cast<EFunction*>(start);
    c._start = start;
    c._stop = stop = c._function->getEnd();
    c._extraBlock = stop;
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Step 1.a:
    // - Assign offset to each emittable.
    // - Extract variables from instructions.
    // - Prepare variables for register allocator, doing:
    //   - Update read/write statistics.
    //   - Find scope (first/last emittable) where variable is used.
    for (cur = start; ; cur = cur->getNext())
    {
      cur->prepare(c);
      if (cur == stop) break;
    }

    // Step 1.b:
    // - Add "VARIABLE_HINT_UNUSE" hint to the end of each variable scope.
    if (c._active)
    {
      VarData* vdata = c._active;
      do {
        EVariableHint* e = Compiler_newObject<EVariableHint>(this, vdata, (uint32_t)VARIABLE_HINT_UNUSE, (uint32_t)INVALID_VALUE);
        addEmittableAfter(e, vdata->lastEmittable);

        vdata = vdata->nextActive;
      } while (vdata != c._active);
    }
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Step 2.a:
    // - Alloc registers.
    // - Translate special instructions (imul, cmpxchg8b, ...).
    Emittable* prev = NULL;
    for (cur = start; ; prev = cur, cur = cur->getNext())
    {
      _current = prev;
      c._currentOffset = cur->_offset;
      cur->translate(c);
      if (cur == stop) break;
    }

    // Step 2.b:
    // - Translate forward jumps.
    {
      ForwardJumpData* j = c._forwardJumps;
      while (j)
      {
        c._assignState(j->state);
        _current = j->inst->getPrev();
        j->inst->_doJump(c);
        j = j->next;
      }
    }

    // Step 2.c:
    // - Alloc memory operands (variables related).
    c._allocMemoryOperands();

    // Step 2.d:
    // - Emit function prolog.
    // - Emit function epilog.
    c._function->_preparePrologEpilog(c);

    _current = c._function->_prolog;
    c._function->_emitProlog(c);

    _current = c._function->_epilog;
    c._function->_emitEpilog(c);

    _current = _last;

    // Step 2.d:
    // - Patch memory operands (variables related).
    c._patchMemoryOperands();

    // Step 2.e:
    // - Dump function prototype and variable statistics if logging is enabled.
    if (_logger && _logger->isUsed())
    {
      c._function->_dumpFunction(c);
    }
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Hack, need to register labels that was created by the Step 2.
    if (a._labelData.getLength() < _targetData.getLength())
    {
      a.registerLabels(_targetData.getLength() - a._labelData.getLength());
    }

    Emittable* extraBlock = c._extraBlock;

    // Step 3:
    // - Emit instructions to Assembler stream.
    for (cur = start; ; cur = cur->getNext())
    {
      cur->emit(a);
      if (cur == extraBlock) break;
    }
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Step 4:
    // - Emit everything else (post action).
    for (cur = start; ; cur = cur->getNext())
    {
      cur->post(a);
      if (cur == extraBlock) break;
    }
    // ------------------------------------------------------------------------

    start = extraBlock->getNext();
    c._clear();
  }
}

// ============================================================================
// [AsmJit::Compiler - Construction / Destruction]
// ============================================================================

Compiler::Compiler() ASMJIT_NOTHROW {}
Compiler::~Compiler() ASMJIT_NOTHROW {}

} // AsmJit namespace

// [Api-End]
#include "ApiEnd.h"
