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

// The AsmJit compiler is also based on V8 JIT compiler:
//
// Copyright (c) 1994-2006 Sun Microsystems Inc.
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// - Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// - Redistribution in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.
//
// - Neither the name of Sun Microsystems or the names of contributors may
// be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

// The original source code covered by the above license above has been modified
// significantly by Google Inc.
// Copyright 2006-2008 the V8 project authors. All rights reserved.

#include "AsmJitAssemblerX86X64.h"
#include "AsmJitVM.h"

namespace AsmJit {

// [Registers]

// 8 bit general purpose registers
const Register al = { REG_AL };
const Register cl = { REG_CL };
const Register dl = { REG_DL };
const Register bl = { REG_BL };
const Register ah = { REG_AH };
const Register ch = { REG_CH };
const Register dh = { REG_DH };
const Register bh = { REG_BH };

#if defined(ASMJIT_X64)
const Register r8b = { REG_R8B };
const Register r9b = { REG_R9B };
const Register r10b = { REG_R10B };
const Register r11b = { REG_R11B };
const Register r12b = { REG_R12B };
const Register r13b = { REG_R13B };
const Register r14b = { REG_R14B };
const Register r15b = { REG_R15B };
#endif // ASMJIT_X64

// 16 bit general purpose registers
const Register ax = { REG_AX };
const Register cx = { REG_CX };
const Register dx = { REG_DX };
const Register bx = { REG_BX };
const Register sp = { REG_SP };
const Register bp = { REG_BP };
const Register si = { REG_SI };
const Register di = { REG_DI };

// 32 bit general purpose registers
const Register eax = { REG_EAX };
const Register ecx = { REG_ECX };
const Register edx = { REG_EDX };
const Register ebx = { REG_EBX };
const Register esp = { REG_ESP };
const Register ebp = { REG_EBP };
const Register esi = { REG_ESI };
const Register edi = { REG_EDI };

#if defined(ASMJIT_X64)
const Register r8d = { REG_R8D };
const Register r9d = { REG_R9D };
const Register r10d = { REG_R10D };
const Register r11d = { REG_R11D };
const Register r12d = { REG_R12D };
const Register r13d = { REG_R13D };
const Register r14d = { REG_R14D };
const Register r15d = { REG_R15D };
#endif // ASMJIT_X64

// mmx registers
const MMRegister mm0 = { REG_MM0 };
const MMRegister mm1 = { REG_MM1 };
const MMRegister mm2 = { REG_MM2 };
const MMRegister mm3 = { REG_MM3 };
const MMRegister mm4 = { REG_MM4 };
const MMRegister mm5 = { REG_MM5 };
const MMRegister mm6 = { REG_MM6 };
const MMRegister mm7 = { REG_MM7 };

// sse registers
const XMMRegister xmm0 = { REG_XMM0 };
const XMMRegister xmm1 = { REG_XMM1 };
const XMMRegister xmm2 = { REG_XMM2 };
const XMMRegister xmm3 = { REG_XMM3 };
const XMMRegister xmm4 = { REG_XMM4 };
const XMMRegister xmm5 = { REG_XMM5 };
const XMMRegister xmm6 = { REG_XMM6 };
const XMMRegister xmm7 = { REG_XMM7 };

// 64 bit mode
#if defined(ASMJIT_X64)
const XMMRegister xmm8 = { REG_XMM8 };
const XMMRegister xmm9 = { REG_XMM9 };
const XMMRegister xmm10 = { REG_XMM10 };
const XMMRegister xmm11 = { REG_XMM11 };
const XMMRegister xmm12 = { REG_XMM12 };
const XMMRegister xmm13 = { REG_XMM13 };
const XMMRegister xmm14 = { REG_XMM14 };
const XMMRegister xmm15 = { REG_XMM15 };
#endif // ASMJIT_X64

// 64 bit mode
#if defined(ASMJIT_X64)
// 64 bit mode general purpose registers
const Register rax = { REG_RAX };
const Register rcx = { REG_RCX };
const Register rdx = { REG_RDX };
const Register rbx = { REG_RBX };
const Register rsp = { REG_RSP };
const Register rbp = { REG_RBP };
const Register rsi = { REG_RSI };
const Register rdi = { REG_RDI };
const Register r8  = { REG_R8  };
const Register r9  = { REG_R9  };
const Register r10 = { REG_R10 };
const Register r11 = { REG_R11 };
const Register r12 = { REG_R12 };
const Register r13 = { REG_R13 };
const Register r14 = { REG_R14 };
const Register r15 = { REG_R15 };
#endif // ASMJIT_X64

// native general purpose registers
// (shared between 32 bit mode and 64 bit mode)

const Register nax = { REG_NAX };
const Register ncx = { REG_NCX };
const Register ndx = { REG_NDX };
const Register nbx = { REG_NBX };
const Register nsp = { REG_NSP };
const Register nbp = { REG_NBP };
const Register nsi = { REG_NSI };
const Register ndi = { REG_NDI };

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Construction / Destruction]
// ----------------------------------------------------------------------------

Assembler::Assembler() : 
  _buffer(16)
{
}

Assembler::~Assembler()
{
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Internal Buffer]
// ----------------------------------------------------------------------------

void Assembler::free()
{
  _buffer.free();
  _relocations.clear();
}

UInt8* Assembler::takeCode()
{
  UInt8* code = _buffer.take();
  _relocations.clear();
  return code;
}

void Assembler::clear()
{
  _buffer.clear();
  _relocations.clear();
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Setters / Getters]
// ----------------------------------------------------------------------------

void Assembler::setVarAt(SysInt pos, SysInt i, bool isUnsigned, UInt32 size)
{
  if (size == 1 &&  isUnsigned) setByteAt (pos, (UInt8 )       i);
  if (size == 1 && !isUnsigned) setByteAt (pos, (UInt8 )(Int8 )i);
  if (size == 2 &&  isUnsigned) setWordAt (pos, (UInt16)       i);
  if (size == 2 && !isUnsigned) setWordAt (pos, (UInt16)(Int16)i);
  if (size == 4 &&  isUnsigned) setDWordAt(pos, (UInt32)       i);
  if (size == 4 && !isUnsigned) setDWordAt(pos, (UInt32)(Int32)i);
  if (size == 8 &&  isUnsigned) setQWordAt(pos, (UInt64)       i);
  if (size == 8 && !isUnsigned) setQWordAt(pos, (UInt64)(Int64)i);
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Bind and Labels Declaration]
// ----------------------------------------------------------------------------

void Assembler::bind(Label* L)
{
  // label can only be bound once
  ASMJIT_ASSERT(!L->isBound());
  bindTo(L, offset());
}

void Assembler::bindTo(Label* L, SysInt pos)
{
  // must have a valid binding position
  ASMJIT_ASSERT((SysInt)pos <= (SysInt)offset());

  while (L->isLinked())
  {
    Displacement disp = getDispAt(L);
    SysInt fixup_pos = L->pos();
    if (disp.type() == Displacement::UNCONDITIONAL_JUMP)
    {
      // jmp expected
      ASMJIT_ASSERT(getByteAt(fixup_pos - 1) == 0xE9);
    }

    // relative address, relative to point after address
    SysInt immx = pos - (fixup_pos + 4);
    setDWordAt(fixup_pos, (UInt32)(SysUInt)immx);
    disp.next(L);
  }
  L->bindTo(pos);
}

void Assembler::linkTo(Label* L, Label* appendix)
{
  if (appendix->isLinked())
  {
    if (L->isLinked())
    {
      // append appendix to L's list
      Label p;
      Label q = *L;
      do {
        p = q;
        Displacement disp = getDispAt(&q);
        disp.next(&q);
      } while (q.isLinked());
      Displacement disp = getDispAt(&p);
      disp.linkTo(appendix);
      setDispAt(&p, disp);
      // to avoid assertion failure in ~Label
      p.unuse();
    }
    else
    {
      // L is empty, simply use appendix
      *L = *appendix;
    }
  }
  // appendix should not be used anymore
  appendix->unuse();
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Relocation helpers]
// ----------------------------------------------------------------------------

void Assembler::relocCode(void* _dst) const
{
  // Copy code
  UInt8* dst = reinterpret_cast<UInt8*>(_dst);
  memcpy(dst, _buffer.data(), codeSize());

  // Relocate
  const RelocInfo* rel = _relocations.data();
  SysUInt i, len = _relocations.length();

  for (i = 0; i < len; i++)
  {
    const RelocInfo& ri = rel[i];
    switch (ri.mode)
    {
      // jmp_rel relative jump patch
      case RELOC_JMP_RELATIVE:
      {
        // Reloc info variables generated by jmp_rel()
        SysInt jmpStart = ri.offset;
        SysInt jmpAddress = ri.offset + (ri.data >> 8);
        SysInt jmpSize = ri.data & 0xFF;

        // Source and target addresses
        SysUInt jmpFrom = ((SysUInt)dst) + jmpStart;
        SysUInt jmpTo = *reinterpret_cast<SysUInt *>(dst + jmpAddress);

        const int short_size = 2;
        const int long_size  = 5;

        Int32 displacement = 0;

        // Calculate displacement, but very safe!
        if (jmpTo > jmpFrom)
        {
          SysUInt diff = jmpTo - jmpFrom;
          if (diff < 2147483647 && diff >= long_size) displacement = (Int32)(diff);
        }
        else
        {
          SysUInt diff = jmpFrom - jmpTo;
          if (diff < 2147483647 - long_size) displacement = -(Int32)(diff);
        }

        // Ready to patch to relative displacement?
        if (displacement != 0)
        {
          SysUInt z;

          if (isInt8(displacement - short_size))
          {
            // jmp rel8 (EB cb)
            dst[jmpStart] = 0xEB;
            dst[jmpStart+1] = (displacement - short_size) & 0xFF;
            z = short_size;
          }
          else
          {
            // jmp rel32 (E9 cd)
            dst[jmpStart] = 0xE9;
            *reinterpret_cast<Int32*>(dst + jmpStart + 1) = (displacement - long_size);
            z = long_size;
          }

          // set int3 to remaining bytes
          for (; z < (SysUInt)jmpSize; z++) dst[jmpStart+z] = 0xCC;
        }
        break;
      }

      // Illegal mode ?
      default:
      {
        ASMJIT_CRASH();
      }
    }
  }
}

bool Assembler::writeRelocInfo(const Op& immediate, SysUInt relocOffset, UInt8 relocSize)
{
  ASMJIT_ASSERT(immediate.op() == OP_IMM);

  RelocInfo ri;
  ri.offset = relocOffset;
  ri.size = relocSize;
  ri.mode = immediate.relocMode();
  ri.data = 0;
  return immediate._relocations.append(ri);
}

void Assembler::overwrite(const Op& immediate) 
{
  const RelocInfo* rel = immediate._relocations.data();
  SysUInt i, len = immediate._relocations.length();
  for (i = 0; i < len; i++)
  {
    const RelocInfo& ri = rel[i];
    ASMJIT_ASSERT((SysInt)ri.offset + ri.size <= codeSize());
    setVarAt((SysInt)ri.offset, immediate.imm(), immediate.isUnsigned(), ri.size);
  }
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Emitters]
// ----------------------------------------------------------------------------

void Assembler::_emitImmediate(const Op& imm, UInt32 size)
{
  bool isUnsigned = imm.isUnsigned();
  SysInt i = imm.imm();

  if (imm.relocMode() != RELOC_NONE) writeRelocInfo(imm, (SysUInt)offset(), size);

  if (size == 1 &&  isUnsigned) _emitByte ((UInt8 )       i);
  if (size == 1 && !isUnsigned) _emitByte ((UInt8 )(Int8 )i);
  if (size == 2 &&  isUnsigned) _emitWord ((UInt16)       i);
  if (size == 2 && !isUnsigned) _emitWord ((UInt16)(Int16)i);
  if (size == 4 &&  isUnsigned) _emitDWord((UInt32)       i);
  if (size == 4 && !isUnsigned) _emitDWord((UInt32)(Int32)i);
  if (size == 8 &&  isUnsigned) _emitQWord((UInt64)       i);
  if (size == 8 && !isUnsigned) _emitQWord((UInt64)(Int64)i);
}

void Assembler::_emitMem(UInt8 o, Int32 disp)
{
  _emitMod(0, o, 5);
  _emitDWord(disp);
}

void Assembler::_emitMem(UInt8 o, UInt8 baseReg, Int32 disp)
{
  if (baseReg == R_ESP)
  {
    if (disp == 0)
    {
      _emitMod(0, o, R_ESP);
      _emitMod(0, R_ESP, R_ESP);
    }
    else if (isInt8(disp))
    {
      _emitMod(1, o, R_ESP);
      _emitMod(0, R_ESP, R_ESP);
      _emitByte(disp);
    }
    else
    {
      _emitMod(2, o, R_ESP);
      _emitMod(0, R_ESP, R_ESP);
      _emitDWord(disp);
    }
  }
  else if (baseReg != R_EBP && disp == 0)
  {
    _emitMod(0, o, baseReg);
  }
  else if (isInt8(disp))
  {
    _emitMod(1, o, baseReg);
    _emitByte(disp);
  }
  else
  {
    _emitMod(2, o, baseReg);
    _emitDWord(disp);
  }
}

void Assembler::_emitMem(UInt8 o, UInt8 baseReg, Int32 disp, UInt8 indexReg, int shift)
{
  if (baseReg >= R_INVALID)
  {
    _emitMod(0, o, 4);
    _emitMod(shift, indexReg, 5);
    _emitDWord(disp);
  }
  else if (disp == 0 && baseReg != R_EBP)
  {
    _emitMod(0, o, 4);
    _emitMod(shift, indexReg, baseReg);
  }
  else if (isInt8(disp))
  {
    _emitMod(1, o, 4);
    _emitMod(shift, indexReg, baseReg);
    _emitByte(disp);
  }
  else
  {
    _emitMod(2, o, 4);
    _emitMod(shift, indexReg, 5);
    _emitDWord(disp);
  }
}

void Assembler::_emitMem(UInt8 o, const Op& mem)
{
  ASMJIT_ASSERT(mem.op() == OP_MEM);

  if (mem.hasIndex())
    _emitMem(o, mem.baseRegCode(), mem.disp(), mem.indexRegCode(), mem.indexShift());
  else
    _emitMem(o, mem.baseRegCode(), mem.disp());
}

void Assembler::_emitOp(UInt8 o, const Op& op)
{
  if (op.op() == OP_REG)
    _emitReg(o, op.regCode());
  else if (op.op() == OP_MEM)
    _emitMem(o, op);
  else
    ASMJIT_CRASH();
}

void Assembler::_emitBitArith(const Op& dst, const Op& src, UInt8 ModR)
{
  ASMJIT_ASSERT((dst.op() == OP_REG) ||
                (dst.op() == OP_MEM));
  ASMJIT_ASSERT((src.op() == OP_REG && src.reg() == REG_CL) ||
                (src.op() == OP_IMM));

  if (!ensureSpace()) return;

  // generate opcode. For these operations is base 0xC0 or 0xD0.
  bool useImm8 = (src.op() == OP_IMM && (src.imm() != 1 || src.relocMode() != RELOC_NONE));
  UInt8 opCode = useImm8 ? 0xC0 : 0xD0;

  // size and operand type modifies the opcode
  if (dst.size() != 1) opCode |= 0x01;
  if (src.op() == OP_REG) opCode |= 0x02;

  if (dst.size() == 2) _emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
  _emitRex(dst.size() == 8, ModR, dst);
#endif // ASMJIT_X64
  _emitByte(opCode);
  _emitOp(ModR, dst);
  if (useImm8) _emitImmediate(src, 1);
}

void Assembler::_emitShldShrd(const Op& dst, const Register& src1, const Op& src2, UInt8 opCode)
{
  ASMJIT_ASSERT(dst .op() == OP_MEM || (dst .op() == OP_REG));
  ASMJIT_ASSERT(src2.op() == OP_IMM || (src2.op() == OP_REG && src2.reg() == REG_CL));
  ASMJIT_ASSERT(dst.size() == src1.regSize());

  if (!ensureSpace()) return;

  if (src1.regCode() == REG_GPW) _emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
  _emitRex(src1.regType() == REG_GPQ, src1.regCode(), dst);
#endif // ASMJIT_X64
  _emitByte(0x0F);
  _emitByte(opCode + (src2.op() == OP_REG));
  _emitOp(src1.regCode(), dst);
  if (src2.op() == OP_IMM) _emitImmediate(src2, 1);
}

void Assembler::_emitArithOp(const Op& _dst, const Op& _src, UInt8 opcode, UInt8 o)
{
  if (!ensureSpace()) return;

  const Op* dst = &_dst;
  const Op* src = &_src;

  switch ((dst->op() << 4) | src->op())
  {
    // Mem <- Reg
    case (OP_MEM << 4) | OP_REG:
      // easiest way, how to do it (reverse operands)
      opcode -= 2;
      src = &_dst;
      dst = &_src;
      // ... fall through ...

    // Reg <- Reg/Mem
    case (OP_REG << 4) | OP_REG:
    case (OP_REG << 4) | OP_MEM:
      switch (dst->regType())
      {
        case REG_GPB:
#if defined(ASMJIT_X64)
          _emitRex(0, dst->regCode(), *src);
#endif // ASMJIT_X64
          _emitByte(opcode + 2);
          _emitOp(dst->regCode(), *src);
          return;
        case REG_GPW:
          _emitByte(0x66); // 16 bit prefix
          // ... fall through ...
        case REG_GPD:
#if defined(ASMJIT_X64)
          _emitRex(0, dst->regCode(), *src);
#endif // ASMJIT_X64
          _emitByte(opcode + 3);
          _emitOp(dst->regCode(), *src);
          return;
#if defined(ASMJIT_X64)
        case REG_GPQ:
          _emitRex(1, dst->regCode(), *src);
          _emitByte(opcode + 3);
          _emitOp(dst->regCode(), *src);
          return;
#endif // ASMJIT_X86
      }
      break;

    // Reg <- Imm
    case (OP_REG << 4) | OP_IMM:
    {
      // AL, AX, EAX, RAX register shortcuts
      switch (dst->reg())
      {
        // TODO: Check for REX prefixes here!
        case REG_AL:
          // short form if the destination is 'al'.
          _emitByte((o << 3) | 0x04);
          _emitImmediate(*src, 1);
          return;
        case REG_AX:
          _emitByte(0x66); // 16 bit
          _emitByte((o << 3) | 0x05);
          _emitImmediate(*src, 2);
          return;
        case REG_EAX:
          _emitByte((o << 3) | 0x05);
          _emitImmediate(*src, 4);
          return;
#if defined(ASMJIT_X64)
        case REG_RAX:
          _emitByte(0x48); // REX.W
          _emitByte((o << 3) | 0x05);
          _emitImmediate(*src, 4);
          return;
#endif // ASMJIT_X64
      }
    }

    // ... fall through ...

    // Reg/Mem <- Imm
    case (OP_MEM << 4) | OP_IMM:
    {
      bool imm8Bit = isInt8(src->imm()) && src->relocMode() == RELOC_NONE;

      // all others
      switch (dst->size())
      {
        case 1:
#if defined(ASMJIT_X64)
          _emitRex(0, o, *dst); // REX
#endif // ASMJIT_X64
          _emitByte(0x80);
          _emitOp(o, *dst);
          _emitImmediate(*src, 1);
          return;

        case 2:
          _emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
          _emitRex(0, o, *dst); // REX
#endif // ASMJIT_X64
          _emitByte(imm8Bit ? 0x03 : 0x81);
          _emitOp(o, *dst);
          if (imm8Bit)
            _emitImmediate(*src, 1);
          else
            _emitImmediate(*src, 2);
          return;

        case 4:
#if defined(ASMJIT_X64)
          _emitRex(0, o, *dst); // REX
#endif // ASMJIT_X64
          _emitByte(imm8Bit ? 0x83 : 0x81);
          _emitOp(o, *dst);
          if (imm8Bit)
            _emitImmediate(*src, 1);
          else
            _emitImmediate(*src, 4);
          return;
#if defined(ASMJIT_X64)
        case 8:
          _emitRex(1, o, *dst); // REX.W
          _emitByte(imm8Bit ? 0x83 : 0x81);
          _emitOp(o, *dst);
          if (imm8Bit)
            _emitImmediate(*src, 1);
          else
            _emitImmediate(*src, 4);
          return;
#endif // ASMJIT_X64
      }

      break;
    }
  }

  ASMJIT_CRASH();
}

void Assembler::_emitFP(int opcode1, int opcode2, int i)
{
  // wrong opcode
  ASMJIT_ASSERT(isUInt8(opcode1) && isUInt8(opcode2));
  // illegal stack offset
  ASMJIT_ASSERT(0 <= i && i < 8);

  if (!ensureSpace()) return;

  _emitByte(opcode1);
  _emitByte(opcode2 + i);
}

void Assembler::_emitFPMem(int opcode, UInt8 o, const Op& mem)
{
  ASMJIT_ASSERT(mem.op() == OP_MEM);
  if (!ensureSpace()) return;

  _emitByte(opcode);
  _emitMem(o, mem);
}

void Assembler::_emitINST(
  UInt8 prefix, UInt8 opcode1, UInt8 opcode2, UInt8 opcode3, 
  UInt8 i16bit, UInt8 rexw, UInt8 o, const Op& op)
{
  if (!ensureSpace()) return;

  if (i16bit) _emitByte(0x66);     // 16 bit prefix
  if (prefix) _emitByte(prefix);   // instruction prefix
#if defined(ASMJIT_X64)
  _emitRex(rexw, o, op);           // rex prefix
#endif // ASMJIT_X64
  if (opcode1) _emitByte(opcode1); // instruction opcodes
  if (opcode2) _emitByte(opcode2);
  _emitByte(opcode3);
  _emitOp(o, op);                  // ModR/M, ...
}

void Assembler::_emitMM(UInt8 prefix0, UInt8 opcode0, UInt8 opcode1, UInt8 opcode2, int dstCode, const Op& src, UInt8 rexw)
{
  if (!ensureSpace()) return;

  if (prefix0) _emitByte(prefix0);
#if defined(ASMJIT_X86)
  _emitRex(rexw, dstCode, src);
#endif // ASMJIT_X86
  if (opcode0) _emitByte(opcode0);
  _emitByte(opcode1);
  _emitByte(opcode2);
  _emitOp(dstCode, src);
}

void Assembler::_emitMMi(UInt8 prefix1, UInt8 prefix2, UInt8 opcode1, UInt8 opcode2, int dstCode, const Op& src, int imm8, UInt8 rexw)
{
  if (!ensureSpace()) return;

  _emitMM(prefix1, prefix2, opcode1, opcode2, dstCode, src, rexw);
  _emitByte(imm8 & 0xFF);
}

void Assembler::_emitDisp(Label* L, Displacement::Type type)
{
  Displacement disp(L, type);
  L->linkTo(offset());
  _emitDWord(static_cast<SysInt>(disp.data()));
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Helpers]
// ----------------------------------------------------------------------------

void Assembler::align(int m)
{
  if (!ensureSpace()) return;
  if (!m) return;

  ASMJIT_ASSERT(m == 1 || m == 2 || m == 4 || m == 8 || m == 16 || m == 32);
  while ((offset() & (m - 1)) != 0) nop();
}

} // AsmJit namespace
