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

// [Dependencies]
#include "Defs.h"
#include "Operand.h"

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Immediate]
// ============================================================================

//! @brief Create signed immediate value operand.
Imm imm(sysint_t i) ASMJIT_NOTHROW
{ 
  return Imm(i, false);
}

//! @brief Create unsigned immediate value operand.
Imm uimm(sysuint_t i) ASMJIT_NOTHROW
{
  return Imm((sysint_t)i, true);
}

// ============================================================================
// [AsmJit::BaseVar]
// ============================================================================

Mem _baseVarMem(const BaseVar& var, uint32_t ptrSize) ASMJIT_NOTHROW
{
  Mem m; //(_DontInitialize());

  m._mem.op = OPERAND_MEM;
  m._mem.size = ptrSize == INVALID_VALUE ? var.getSize() : (uint8_t)ptrSize;
  m._mem.type = OPERAND_MEM_NATIVE;
  m._mem.segmentPrefix = SEGMENT_NONE;

  m._mem.id = var.getId();

  m._mem.base = INVALID_VALUE;
  m._mem.index = INVALID_VALUE;
  m._mem.shift = 0;

  m._mem.target = NULL;
  m._mem.displacement = 0;

  return m;
}


Mem _baseVarMem(const BaseVar& var, uint32_t ptrSize, sysint_t disp) ASMJIT_NOTHROW
{
  Mem m; //(_DontInitialize());

  m._mem.op = OPERAND_MEM;
  m._mem.size = ptrSize == INVALID_VALUE ? var.getSize() : (uint8_t)ptrSize;
  m._mem.type = OPERAND_MEM_NATIVE;
  m._mem.segmentPrefix = SEGMENT_NONE;

  m._mem.id = var.getId();

  m._mem.base = INVALID_VALUE;
  m._mem.index = INVALID_VALUE;
  m._mem.shift = 0;

  m._mem.target = NULL;
  m._mem.displacement = disp;

  return m;
}

Mem _baseVarMem(const BaseVar& var, uint32_t ptrSize, const GPVar& index, uint32_t shift, sysint_t disp) ASMJIT_NOTHROW
{
  Mem m; //(_DontInitialize());

  m._mem.op = OPERAND_MEM;
  m._mem.size = ptrSize == INVALID_VALUE ? var.getSize() : (uint8_t)ptrSize;
  m._mem.type = OPERAND_MEM_NATIVE;
  m._mem.segmentPrefix = SEGMENT_NONE;

  m._mem.id = var.getId();

  m._mem.base = INVALID_VALUE;
  m._mem.index = index.getId();
  m._mem.shift = shift;

  m._mem.target = NULL;
  m._mem.displacement = disp;

  return m;
}

// ============================================================================
// [AsmJit::Mem - ptr[]]
// ============================================================================

Mem _MemPtrBuild(
  const Label& label, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(label, disp, ptrSize);
}

Mem _MemPtrBuild(
  const Label& label,
  const GPReg& index, uint32_t shift, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  Mem m(label, disp, ptrSize);

  m._mem.index = index.getRegIndex();
  m._mem.shift = shift;

  return m;
}

Mem _MemPtrBuild(
  const Label& label,
  const GPVar& index, uint32_t shift, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  Mem m(label, disp, ptrSize);

  m._mem.index = index.getId();
  m._mem.shift = shift;

  return m;
}

// ============================================================================
// [AsmJit::Mem - ptr[] - Absolute Addressing]
// ============================================================================

ASMJIT_API Mem _MemPtrAbs(
  void* target, sysint_t disp,
  uint32_t segmentPrefix, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  Mem m;

  m._mem.size = ptrSize;
  m._mem.type = OPERAND_MEM_ABSOLUTE;
  m._mem.segmentPrefix = segmentPrefix;

  m._mem.target = target;
  m._mem.displacement = disp;

  return m;
}

ASMJIT_API Mem _MemPtrAbs(
  void* target,
  const GPReg& index, uint32_t shift, sysint_t disp,
  uint32_t segmentPrefix, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  Mem m;// (_DontInitialize());

  m._mem.op = OPERAND_MEM;
  m._mem.size = ptrSize;
  m._mem.type = OPERAND_MEM_ABSOLUTE;
  m._mem.segmentPrefix = (uint8_t)segmentPrefix;

  m._mem.id = INVALID_VALUE;

  m._mem.base = INVALID_VALUE;
  m._mem.index = index.getRegIndex();
  m._mem.shift = shift;

  m._mem.target = target;
  m._mem.displacement = disp;

  return m;
}

// ============================================================================
// [AsmJit::Mem - ptr[base + displacement]]
// ============================================================================

Mem _MemPtrBuild(
  const GPReg& base, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(base, disp, ptrSize);
}

Mem _MemPtrBuild(
  const GPReg& base,
  const GPReg& index, uint32_t shift, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(base, index, shift, disp, ptrSize);
}

Mem _MemPtrBuild(
  const GPVar& base, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(base, disp, ptrSize);
}

Mem _MemPtrBuild(
  const GPVar& base,
  const GPVar& index, uint32_t shift, sysint_t disp, uint32_t ptrSize)
  ASMJIT_NOTHROW
{
  return Mem(base, index, shift, disp, ptrSize);
}

} // AsmJit namespace
