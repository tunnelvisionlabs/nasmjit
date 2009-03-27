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

// [Dependencies]
#include "AsmJitSerializer.h"

// [Warnings-Push]
#include "AsmJitWarningsPush.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Mem - ptr[]]
// ============================================================================

Mem _ptr_build(
  Label* label, SysInt disp, UInt8 ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(label, disp, ptrSize);
}

Mem _ptr_build(
  Label* label, 
  const Register& index, UInt32 shift, SysInt disp, UInt8 ptrSize)
  ASMJIT_NOTHROW
{
  Mem m(label, disp, ptrSize);
  m._mem.index = index.code() & REGCODE_MASK;
  m._mem.shift = shift;
  return m;
}

// --- absolute addressing ---
ASMJIT_API Mem _ptr_build_abs(
  void* target, SysInt disp,
  UInt32 segmentPrefix, UInt8 ptrSize)
  ASMJIT_NOTHROW
{
  Mem m;
  m._mem.size = ptrSize;
  m._mem.base = NO_REG;
  m._mem.index = NO_REG;
  m._mem.segmentPrefix = segmentPrefix;
  m._mem.target = target;
  m._mem.displacement = disp;
  return m;
}

ASMJIT_API Mem _ptr_build_abs(
  void* target, 
  const Register& index, UInt32 shift, SysInt disp,
  UInt32 segmentPrefix, UInt8 ptrSize)
  ASMJIT_NOTHROW
{
  Mem m;
  m._mem.size = ptrSize;
  m._mem.base = NO_REG;
  m._mem.index = index.index();
  m._mem.shift = shift;
  m._mem.segmentPrefix = (SysInt)segmentPrefix;
  m._mem.target = target;
  m._mem.displacement = disp;
  return m;
}
// --- absolute addressing ---

Mem _ptr_build(
  const Register& base, SysInt disp, UInt8 ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(base, disp, ptrSize);
}

Mem _ptr_build(
  const Register& base, 
  const Register& index, UInt32 shift, SysInt disp, UInt8 ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(base, index, shift, disp, ptrSize);
}

// ============================================================================
// [AsmJit::Immediate]
// ============================================================================

//! @brief Create signed immediate value operand.
Immediate imm(SysInt i) ASMJIT_NOTHROW
{ 
  return Immediate(i, false);
}

//! @brief Create unsigned immediate value operand.
Immediate uimm(SysUInt i) ASMJIT_NOTHROW
{
  return Immediate((SysInt)i, true);
}

// ============================================================================
// [AsmJit::_Serializer - Construction / Destruction]
// ============================================================================

_Serializer::_Serializer() ASMJIT_NOTHROW :
  _logger(NULL),
  _zone(65536 - sizeof(Zone::Chunk) - 32),
  _properties(0),
  _error(0)
{
  // Set default properties for Assembler and Compiler
  // (compiler can set other ones)
  _properties |= (1 << PROPERTY_OPTIMIZE_ALIGN) |
                 (1 << PROPERTY_JCC_HINTS     ) ;
}

_Serializer::~_Serializer() ASMJIT_NOTHROW
{
}

// ============================================================================
// [AsmJit::_Serializer - Properties]
// ============================================================================

//! @brief Get property @a key from serializer.
UInt32 _Serializer::getProperty(UInt32 key) ASMJIT_NOTHROW
{
  if (key < 32)
    return (_properties >> key) & 1;
  else
    return 0xFFFFFFFF;
}

//! @brief Set property @a key to @a value.
UInt32 _Serializer::setProperty(UInt32 key, UInt32 value) ASMJIT_NOTHROW
{
  if (key < 32)
  {
    UInt32 mask = (1 << key);
    UInt32 result = (_properties & mask) >> key;

    if (value)
      _properties |= mask;
    else
      _properties &= ~mask;
    return result;
  }
  else
    return 0xFFFFFFFF;
}

// ============================================================================
// [AsmJit::_Serializer - Memory Management]
// ============================================================================

//! @brief Allocate memory using compiler internal memory manager.
void* _Serializer::_zoneAlloc(SysUInt size)
{
  return _zone.alloc(size);
}

// ============================================================================
// [AsmJit::_Serializer - Helpers]
// ============================================================================

// Used for NULL operands in _emitX86() function
static const UInt8 none[sizeof(Operand)] = { 0 };

void _Serializer::__emitX86(UInt32 code)
{
  _emitX86(code, 
    reinterpret_cast<const Operand*>(none), 
    reinterpret_cast<const Operand*>(none), 
    reinterpret_cast<const Operand*>(none));
}

void _Serializer::__emitX86(UInt32 code, const Operand* o1)
{
  _emitX86(code, 
    o1, 
    reinterpret_cast<const Operand*>(&none), 
    reinterpret_cast<const Operand*>(&none));
}

void _Serializer::__emitX86(UInt32 code, const Operand* o1, const Operand* o2)
{
  _emitX86(code, 
    o1, 
    o2, 
    reinterpret_cast<const Operand*>(&none));
}

void _Serializer::__emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  _emitX86(code, 
    o1, 
    o2, 
    o3);
}

//! @brief Private method for emitting jcc.
void _Serializer::_emitJcc(UInt32 code, Label* label, UInt32 hint)
{
  if (!hint)
  {
    __emitX86(code, label);
  }
  else
  {
    Immediate imm(hint);
    __emitX86(code, label, &imm);
  }
}

const UInt32 _Serializer::_jcctable[16] = 
{
  INST_JO,
  INST_JNO,
  INST_JB,
  INST_JAE,
  INST_JE,
  INST_JNE,
  INST_JBE,
  INST_JA,
  INST_JS,
  INST_JNS,
  INST_JPE,
  INST_JPO,
  INST_JL,
  INST_JGE,
  INST_JLE,
  INST_JG
};

const UInt32 _Serializer::_cmovcctable[16] = 
{
  INST_CMOVO,
  INST_CMOVNO,
  INST_CMOVB,
  INST_CMOVAE,
  INST_CMOVE,
  INST_CMOVNE,
  INST_CMOVBE,
  INST_CMOVA,
  INST_CMOVS,
  INST_CMOVNS,
  INST_CMOVPE,
  INST_CMOVPO,
  INST_CMOVL,
  INST_CMOVGE,
  INST_CMOVLE,
  INST_CMOVG
};

} // AsmJit namespace

// [Warnings-Pop]
#include "AsmJitWarningsPop.h"
