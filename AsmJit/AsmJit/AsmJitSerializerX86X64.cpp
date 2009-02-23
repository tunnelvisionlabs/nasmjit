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

namespace AsmJit {

// ============================================================================
// [AsmJit::_Serializer - Construction / Destruction]
// ============================================================================

_Serializer::_Serializer()
{
}

_Serializer::~_Serializer()
{
}

// ============================================================================
// [AsmJit::_Serializer - Helpers]
// ============================================================================

// Used for NULL operands in _emitX86() function
static const Operand::GenData none = { OP_NONE, 0, 0, 0, 0 };

void _Serializer::__emitX86(UInt32 code)
{
  _emitX86(code, 
    reinterpret_cast<const Operand*>(&none), 
    reinterpret_cast<const Operand*>(&none), 
    reinterpret_cast<const Operand*>(&none));
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

void _Serializer::_emitJcc(CONDITION cc, Label* label, UInt32 hint)
{
  ASMJIT_ASSERT((SysUInt)cc <= 0xF);

  static const UInt32 jinst[] = 
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
  _emitJ(jinst[cc], label, hint);
}

//! @brief Private method for emitting jcc.
void _Serializer::_emitJ(UInt32 code, Label* label, UInt32 hint)
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

} // AsmJit namespace
