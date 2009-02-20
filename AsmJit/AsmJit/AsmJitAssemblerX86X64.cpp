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

#include "AsmJitAssemblerX86X64.h"
#include "AsmJitVM.h"

namespace AsmJit {

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
// [AsmJit::Assembler - Buffer]
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
// [AsmJit::Assembler - Stream Setters / Getters]
// ----------------------------------------------------------------------------

void Assembler::setVarAt(SysInt pos, SysInt i, UInt8 isUnsigned, UInt32 size)
{
  if (size == 1 && !isUnsigned) setByteAt (pos, (Int8  )i);
  if (size == 1 &&  isUnsigned) setByteAt (pos, (UInt8 )i);
  if (size == 2 && !isUnsigned) setWordAt (pos, (Int16 )i);
  if (size == 2 &&  isUnsigned) setWordAt (pos, (UInt16)i);
  if (size == 4 && !isUnsigned) setDWordAt(pos, (Int32 )i);
  if (size == 4 &&  isUnsigned) setDWordAt(pos, (UInt32)i);
#if defined(ASMJIT_X64)
  if (size == 8 && !isUnsigned) setQWordAt(pos, (Int64 )i);
  if (size == 8 &&  isUnsigned) setQWordAt(pos, (UInt64)i);
#endif // ASMJIT_X64
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Assembler Emitters]
// ----------------------------------------------------------------------------

void Assembler::_emitImmediate(const Immediate& imm, UInt32 size)
{
  UInt8 isUnsigned = imm.isUnsigned();
  SysInt i = imm.value();

  if (imm.relocMode() != RELOC_NONE) 
  {
    writeRelocInfo(reinterpret_cast<const Relocable&>(imm), (SysUInt)offset(), size);
  }

  if (size == 1 && !isUnsigned) _emitByte ((Int8  )i);
  if (size == 1 &&  isUnsigned) _emitByte ((UInt8 )i);
  if (size == 2 && !isUnsigned) _emitWord ((Int16 )i);
  if (size == 2 &&  isUnsigned) _emitWord ((UInt16)i);
  if (size == 4 && !isUnsigned) _emitDWord((Int32 )i);
  if (size == 4 &&  isUnsigned) _emitDWord((UInt32)i);
#if defined(ASMJIT_X64)
  if (size == 8 && !isUnsigned) _emitQWord((Int64 )i);
  if (size == 8 &&  isUnsigned) _emitQWord((UInt64)i);
#endif // ASMJIT_X64
}

void Assembler::_emitModM(UInt8 opReg, const Mem& mem)
{
  ASMJIT_ASSERT(mem.op() == OP_MEM);

  // [base + index*scale + displacement]
  if (mem.hasIndex())
  {
    UInt8 baseReg = mem.base() & 0x7;
    UInt8 indexReg = mem.index() & 0x7;
    Int32 disp = (Int32)mem.displacement();
    UInt32 shift = mem.shift();

    if (baseReg >= RID_INVALID)
    {
      _emitMod(0, opReg, 4);
      _emitSib(shift, indexReg, 5);
      _emitDWord(static_cast<UInt32>((Int32)disp));
    }
    else if (disp == 0 && baseReg != RID_EBP)
    {
      _emitMod(0, opReg, 4);
      _emitSib(shift, indexReg, baseReg);
    }
    else if (isInt8(disp))
    {
      _emitMod(1, opReg, 4);
      _emitSib(shift, indexReg, baseReg);
      _emitByte(static_cast<UInt8>((Int8)disp));
    }
    else
    {
      _emitMod(2, opReg, 4);
      _emitSib(shift, indexReg, 5);
      _emitDWord(static_cast<UInt32>((Int32)disp));
    }
  }
  // [base + displacement]
  else
  {
    UInt8 baseReg = mem.base() & 0x7;
    Int32 disp = (Int32)mem.displacement();

    if (baseReg == RID_ESP)
    {
      if (disp == 0)
      {
        _emitMod(0, opReg, RID_ESP);
        _emitSib(0, RID_ESP, RID_ESP);
      }
      else if (isInt8(disp))
      {
        _emitMod(1, opReg, RID_ESP);
        _emitSib(0, RID_ESP, RID_ESP);
        _emitByte(static_cast<UInt8>((Int8)disp));
      }
      else
      {
        _emitMod(2, opReg, RID_ESP);
        _emitSib(0, RID_ESP, RID_ESP);
        _emitDWord(static_cast<UInt32>((Int32)disp));
      }
    }
    else if (baseReg != RID_EBP && disp == 0)
    {
      _emitMod(0, opReg, baseReg);
    }
    else if (isInt8(disp))
    {
      _emitMod(1, opReg, baseReg);
      _emitByte(static_cast<UInt8>((Int8)disp));
    }
    else
    {
      _emitMod(2, opReg, baseReg);
      _emitDWord(static_cast<UInt32>((Int32)disp));
    }
  }
}

void Assembler::_emitModRM(UInt8 opReg, const BaseRegMem& op)
{
  ASMJIT_ASSERT(op.op() == OP_REG || op.op() == OP_MEM);

  if (op.op() == OP_REG)
    _emitModR(opReg, operand_cast<const BaseReg&>(op).code());
  else
    _emitModM(opReg, operand_cast<const Mem&>(op));
}

void Assembler::_emitX86Inl(UInt32 opCode, UInt8 i16bit, UInt8 rexw, UInt8 reg)
{
  // 16 bit prefix
  if (i16bit) _emitByte(0x66);

  // instruction prefix
  if (opCode & 0xFF000000) _emitByte((UInt8)((opCode & 0xFF000000) >> 24));

  // rex prefix
#if defined(ASMJIT_X64)
  _emitRexR(rexw, 0, reg);
#endif // ASMJIT_X64

  // instruction opcodes
  if (opCode & 0x00FF0000) _emitByte((UInt8)((opCode & 0x00FF0000) >> 16));
  if (opCode & 0x0000FF00) _emitByte((UInt8)((opCode & 0x0000FF00) >>  8));

  _emitByte((UInt8)(opCode & 0x000000FF) + (reg & 0x7));
}

void Assembler::_emitX86RM(UInt32 opCode, UInt8 i16bit, UInt8 rexw, UInt8 o, const BaseRegMem& op)
{
  // 16 bit prefix
  if (i16bit) _emitByte(0x66);

  // instruction prefix
  if (opCode & 0xFF000000) _emitByte((UInt8)((opCode & 0xFF000000) >> 24));

  // rex prefix
#if defined(ASMJIT_X64)
  _emitRexRM(rexw, o, op);
#endif // ASMJIT_X64

  // instruction opcodes
  if (opCode & 0x00FF0000) _emitByte((UInt8)((opCode & 0x00FF0000) >> 16));
  if (opCode & 0x0000FF00) _emitByte((UInt8)((opCode & 0x0000FF00) >>  8));

  _emitByte((UInt8)(opCode & 0x000000FF));

  // ModR/M
  _emitModRM(o, op);
}

void Assembler::_emitFpu(UInt32 opCode)
{
  _emitOpCode(opCode);
}

void Assembler::_emitFpuSTI(UInt32 opCode, UInt32 sti)
{
  // illegal stack offset
  ASMJIT_ASSERT(0 <= sti && sti < 8);
  _emitOpCode(opCode + sti);
}

void Assembler::_emitFpuMEM(UInt32 opCode, UInt8 opReg, const Mem& mem)
{
  // instruction prefix
  if (opCode & 0xFF000000) _emitByte((UInt8)((opCode & 0xFF000000) >> 24));

  // rex prefix
#if defined(ASMJIT_X64)
  _emitRexRM(0, opReg, mem);
#endif // ASMJIT_X64

  // instruction opcodes
  if (opCode & 0x00FF0000) _emitByte((UInt8)((opCode & 0x00FF0000) >> 16));
  if (opCode & 0x0000FF00) _emitByte((UInt8)((opCode & 0x0000FF00) >>  8));

  _emitByte((UInt8)((opCode & 0x000000FF)));
  _emitModM(opReg, mem);
}

void Assembler::_emitMmu(UInt32 opCode, UInt8 rexw, UInt8 opReg, const BaseRegMem& src)
{
  // instruction prefix
  if (opCode & 0xFF000000) _emitByte((UInt8)((opCode & 0xFF000000) >> 24));

  // rex prefix
#if defined(ASMJIT_X64)
  _emitRexRM(rexw, opReg, src);
#endif // ASMJIT_X64

  // instruction opcodes
  if (opCode & 0x00FF0000) _emitByte((UInt8)((opCode & 0x00FF0000) >> 16));

  // no checking, MMX/SSE instructions have always two opcodes or more
  _emitByte((UInt8)((opCode & 0x0000FF00) >> 8));
  _emitByte((UInt8)((opCode & 0x000000FF)));

  if (src.isReg())
    _emitModR(opReg, operand_cast<const BaseReg&>(src).code());
  else
    _emitModM(opReg, operand_cast<const Mem&>(src));
}

void Assembler::_emitDisplacement(Label* label)
{
  ASMJIT_ASSERT(!label->isBound());

  SysInt o = offset();
  SysInt p = label->isLinked() ? label->position() - o : 0;

  label->set(LABEL_LINKED, o);
  _emitInt32(p);
}

/*
static void _emitModM(UInt8 o, Int32 disp)
{
  _emitMod(0, o, 5);
  _emitInt32(disp);
}
*/

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

        const SysInt rel8_size = 2;
        const SysInt rel32_size = 5;

        SysInt displacement = 0;

        // Calculate displacement, but very safe!
        if (jmpTo > jmpFrom)
        {
          SysUInt diff = jmpTo - jmpFrom;
          if (diff < 2147483647) displacement = (Int32)(diff);
        }
        else
        {
          SysUInt diff = jmpFrom - jmpTo;
          if (diff < 2147483647 - rel32_size) displacement = -(Int32)(diff);
        }

        // Ready to patch to relative displacement?
        if (displacement != 0)
        {
          SysUInt z;

          if (isInt8(displacement - rel8_size))
          {
            // jmp rel8 (EB cb)
            dst[jmpStart] = 0xEB;
            dst[jmpStart+1] = (UInt8)(Int8)(displacement - rel8_size);
            z = rel8_size;
          }
          else
          {
            // jmp rel32 (E9 cd)
            dst[jmpStart] = 0xE9;
            *reinterpret_cast<Int32*>(dst + jmpStart + 1) = (displacement - rel32_size);
            z = rel32_size;
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

bool Assembler::writeRelocInfo(const Relocable& relocable, SysUInt relocOffset, UInt8 relocSize)
{
  RelocInfo ri;
  ri.offset = relocOffset;
  ri.size = relocSize;
  ri.mode = relocable.relocMode();
  ri.data = 0;
  return relocable._relocations.append(ri);
}

void Assembler::overwrite(const Relocable& relocable)
{
  const RelocInfo* rel = relocable._relocations.data();
  SysUInt i, len = relocable._relocations.length();
  for (i = 0; i < len; i++)
  {
    const RelocInfo& ri = rel[i];
    ASMJIT_ASSERT((SysInt)ri.offset + ri.size <= codeSize());
    setVarAt((SysInt)ri.offset, relocable.value(), relocable.isUnsigned(), ri.size);
  }
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Abstract Emitters]
// ----------------------------------------------------------------------------

// #define ASMJIT_DEBUG_INSTRUCTION_MAP

// Instruction description
struct InstructionDescription
{
#if defined(ASMJIT_DEBUG_INSTRUCTION_MAP)
  UInt32 instruction;
  const char* name;
#endif // ASMJIT_DEBUG_INSTRUCTION_MAP

  // Instruction group
  UInt8 group;
  // First operand flags (some groups uses them, some not)
  UInt8 o1Flags;
  // Second operand flags (some groups uses them, some not)
  UInt8 o2Flags;
  // If instruction has only memory operand, this is register opcode
  UInt8 opCodeR;
  // Primary opcode
  UInt32 opCode1;
  // Secondary opcode (used only by few groups - mmx / sse)
  UInt32 opCode2;
};

// Instruction groups
enum I
{
  I_EMIT,

  I_ALU,
  I_BSWAP,
  I_BT,
  I_CALL,
  I_CRC32,
  I_IMUL,
  I_INC_DEC,
  I_J,
  I_JMP,
  I_JMP_PTR,
  I_LEA,
  I_M,
  I_MOV,
  I_MOV_PTR,
  I_MOVSX_MOVZX,
  I_MOVSXD,
  I_PUSH, // I_PUSH is implemented before I_POP
  I_POP,
  I_R_RM,
  I_RM,
  I_RM_R,
  I_RET,
  I_ROT,
  I_SHLD_SHRD,
  I_TEST,
  I_XCHG,

  // Group for x87 FP instructions in format mem or st(i), st(i) (fadd, fsub, fdiv, ...)
  I_X87_FPU,
  // Group for x87 FP instructions in format st(i), st(i)
  I_X87_STI,
  // Group for fld/fst/fstp instruction, internally uses I_X87_MEM group.
  I_X87_MEM_STI,
  // Group for x87 FP instructions that uses Word, DWord, QWord or TWord memory pointer.
  I_X87_MEM,
  // Group for x87 FSTSW/FNSTSW instructions
  I_X87_FSTSW,

  // Group for movbe instruction
  I_MOVBE,

  // Group for MMX/SSE instructions in format (X)MM|Reg|Mem <- (X)MM|Reg|Mem,
  // 0x66 prefix must be set manually in opcodes.
  // - Primary opcode is used for instructions in (X)MM <- (X)MM/Mem format,
  // - Secondary opcode is used for instructions in (X)MM/Mem <- (X)MM format.
  I_MMU_MOV,

  // Group for movd and movq instructions.
  I_MMU_MOVD,
  I_MMU_MOVQ,

  // Group for pextrd, pextrq and pextrw instructions (it's special instruction 
  // not similar to others)
  I_MMU_PEXTR,

  // Group for prefetch instruction
  I_MMU_PREFETCH,

  // Group for MMX/SSE instructions in format (X)MM|Reg <- (X)MM|Reg|Mem|Imm, 
  // 0x66 prefix is added for MMX instructions that used by SSE2 registers.
  // - Primary opcode is used for instructions in (X)MM|Reg <- (X)MM|Reg|Mem format,
  // - Secondary opcode is iused for instructions in (X)MM|Reg <- Imm format.
  I_MMU_RMI,
  I_MMU_RM_IMM8,

  // Group for 3dNow instructions
  I_MMU_RM_3DNOW
};

// Instruction operand flags
enum O
{
  // x86
  O_G8          = 0x01,
  O_G16         = 0x02,
  O_G32         = 0x04,
  O_G64         = 0x08,
  O_MEM         = 0x40,
  O_IMM         = 0x80,

  O_G8_16_32_64 = O_G64  | O_G32  | O_G16  | O_G8,
  O_G16_32_64   = O_G64  | O_G32  | O_G16,
  O_G32_64      = O_G64  | O_G32,

  // x87
  O_FM_1        = 0x01,
  O_FM_2        = 0x02,
  O_FM_4        = 0x04,
  O_FM_8        = 0x08,
  O_FM_10       = 0x10,

  O_FM_2_4      = O_FM_2 | O_FM_4,
  O_FM_2_4_8    = O_FM_2 | O_FM_4 | O_FM_8,
  O_FM_4_8      = O_FM_4 | O_FM_8,
  O_FM_4_8_10   = O_FM_4 | O_FM_8 | O_FM_10,

  // mm|xmm
  O_NOREX       = 0x01, // Used by MMX/SSE instructions, O_G8 is never used for them
  O_MM          = 0x10,
  O_XMM         = 0x20,

  O_MM_MEM      = O_MM   | O_MEM,
  O_XMM_MEM     = O_XMM  | O_MEM,
  O_MM_XMM      = O_MM   | O_XMM,
  O_MM_XMM_MEM  = O_MM   | O_XMM  | O_MEM
};

#if defined(ASMJIT_DEBUG_INSTRUCTION_MAP)
#define MAKE_INST(code, name, type, o1Flags, o2Flags, opReg, opCode1, opCode2) \
        { code, name, type, o1Flags, o2Flags, opReg, opCode1, opCode2 }
#else
#define MAKE_INST(code, name, type, o1Flags, o2Flags, opReg, opCode1, opCode2) \
        { type, o1Flags, o2Flags, opReg, opCode1, opCode2 }
#endif // ASMJIT_DEBUG_INSTRUCTION_MAP

#define ToDo 0

static const InstructionDescription x86instructions[] =
{
  // Instruction code (enum)      | instruction name   | group           | operator 1 flags| operator 2 flags| r| opCode1   | opcode2
  MAKE_INST(INST_ADC              , "adc"              , I_ALU           , 0               , 0               , 2, 0x00000010, 0x00000080),
  MAKE_INST(INST_ADD              , "add"              , I_ALU           , 0               , 0               , 0, 0x00000000, 0x00000080),
  MAKE_INST(INST_ADDPD            , "addpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F58, 0),
  MAKE_INST(INST_ADDPS            , "addps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F58, 0),
  MAKE_INST(INST_ADDSD            , "addsd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F58, 0),
  MAKE_INST(INST_ADDSS            , "addss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F58, 0),
  MAKE_INST(INST_ADDSUBPD         , "addsubpd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000FD0, 0),
  MAKE_INST(INST_ADDSUBPS         , "addsubps"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000FD0, 0),
  MAKE_INST(INST_AMD_PREFETCH     , "amd_prefetch"     , I_M             , O_MEM           , 0               , 0, 0x00000F0D, 0),
  MAKE_INST(INST_AMD_PREFETCHW    , "amd_prefetchw"    , I_M             , O_MEM           , 0               , 1, 0x00000F0D, 0),
  MAKE_INST(INST_AND              , "and"              , I_ALU           , 0               , 0               , 4, 0x00000020, 0x00000080),
  MAKE_INST(INST_ANDNPD           , "andnpd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F55, 0),
  MAKE_INST(INST_ANDNPS           , "andnps"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F55, 0),
  MAKE_INST(INST_ANDPD            , "andpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F54, 0),
  MAKE_INST(INST_ANDPS            , "andps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F54, 0),
  MAKE_INST(INST_BLENDPD          , "blendpd"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A0D, 0),
  MAKE_INST(INST_BLENDPS          , "blendps"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A0C, 0),
  MAKE_INST(INST_BLENDVPD         , "blendvpd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3815, 0),
  MAKE_INST(INST_BLENDVPS         , "blendvps"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3814, 0),
  MAKE_INST(INST_BSF              , "bsf"              , I_R_RM          , 0               , 0               , 0, 0x00000FBC, 0),
  MAKE_INST(INST_BSR              , "bsr"              , I_R_RM          , 0               , 0               , 0, 0x00000FBD, 0),
  MAKE_INST(INST_BSWAP            , "bswap"            , I_BSWAP         , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_BT               , "bt"               , I_BT            ,O_G16_32_64|O_MEM,O_G16_32_64|O_IMM, 4, 0x00000FA3, 0x00000FBA),
  MAKE_INST(INST_BTC              , "btc"              , I_BT            ,O_G16_32_64|O_MEM,O_G16_32_64|O_IMM, 7, 0x00000FBB, 0x00000FBA),
  MAKE_INST(INST_BTR              , "btr"              , I_BT            ,O_G16_32_64|O_MEM,O_G16_32_64|O_IMM, 6, 0x00000FB3, 0x00000FBA),
  MAKE_INST(INST_BTS              , "bts"              , I_BT            ,O_G16_32_64|O_MEM,O_G16_32_64|O_IMM, 5, 0x00000FAB, 0x00000FBA),
  MAKE_INST(INST_CALL             , "call"             , I_CALL          , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_CBW              , "cbw"              , I_EMIT          , 0               , 0               , 0, 0x66000099, 0),
  MAKE_INST(INST_CDQE             , "cdqe"             , I_EMIT          , 0               , 0               , 0, 0x48000099, 0),
  MAKE_INST(INST_CLC              , "clc"              , I_EMIT          , 0               , 0               , 0, 0x000000F8, 0),
  MAKE_INST(INST_CLD              , "cld"              , I_EMIT          , 0               , 0               , 0, 0x000000FC, 0),
  MAKE_INST(INST_CLFLUSH          , "clflush"          , I_M             , O_MEM           , 0               , 7, 0x00000FAE, 0),
  MAKE_INST(INST_CMC              , "cmc"              , I_EMIT          , 0               , 0               , 0, 0x000000F5, 0),
  MAKE_INST(INST_CMOVA            , "cmova"            , I_R_RM          , 0               , 0               , 0, 0x00000F47, 0),
  MAKE_INST(INST_CMOVAE           , "cmovae"           , I_R_RM          , 0               , 0               , 0, 0x00000F43, 0),
  MAKE_INST(INST_CMOVB            , "cmovb"            , I_R_RM          , 0               , 0               , 0, 0x00000F42, 0),
  MAKE_INST(INST_CMOVBE           , "cmovbe"           , I_R_RM          , 0               , 0               , 0, 0x00000F46, 0),
  MAKE_INST(INST_CMOVC            , "cmovc"            , I_R_RM          , 0               , 0               , 0, 0x00000F42, 0),
  MAKE_INST(INST_CMOVE            , "cmove"            , I_R_RM          , 0               , 0               , 0, 0x00000F44, 0),
  MAKE_INST(INST_CMOVG            , "cmovg"            , I_R_RM          , 0               , 0               , 0, 0x00000F4F, 0),
  MAKE_INST(INST_CMOVGE           , "cmovge"           , I_R_RM          , 0               , 0               , 0, 0x00000F4D, 0),
  MAKE_INST(INST_CMOVL            , "cmovl"            , I_R_RM          , 0               , 0               , 0, 0x00000F4C, 0),
  MAKE_INST(INST_CMOVLE           , "cmovle"           , I_R_RM          , 0               , 0               , 0, 0x00000F4E, 0),
  MAKE_INST(INST_CMOVNA           , "cmovna"           , I_R_RM          , 0               , 0               , 0, 0x00000F46, 0),
  MAKE_INST(INST_CMOVNAE          , "cmovnae"          , I_R_RM          , 0               , 0               , 0, 0x00000F42, 0),
  MAKE_INST(INST_CMOVNB           , "cmovnb"           , I_R_RM          , 0               , 0               , 0, 0x00000F43, 0),
  MAKE_INST(INST_CMOVNBE          , "cmovnbe"          , I_R_RM          , 0               , 0               , 0, 0x00000F47, 0),
  MAKE_INST(INST_CMOVNC           , "cmovnc"           , I_R_RM          , 0               , 0               , 0, 0x00000F43, 0),
  MAKE_INST(INST_CMOVNE           , "cmovne"           , I_R_RM          , 0               , 0               , 0, 0x00000F45, 0),
  MAKE_INST(INST_CMOVNG           , "cmovng"           , I_R_RM          , 0               , 0               , 0, 0x00000F4E, 0),
  MAKE_INST(INST_CMOVNGE          , "cmovnge"          , I_R_RM          , 0               , 0               , 0, 0x00000F4C, 0),
  MAKE_INST(INST_CMOVNL           , "cmovnl"           , I_R_RM          , 0               , 0               , 0, 0x00000F4D, 0),
  MAKE_INST(INST_CMOVNLE          , "cmovnle"          , I_R_RM          , 0               , 0               , 0, 0x00000F4F, 0),
  MAKE_INST(INST_CMOVNO           , "cmovno"           , I_R_RM          , 0               , 0               , 0, 0x00000F41, 0),
  MAKE_INST(INST_CMOVNP           , "cmovnp"           , I_R_RM          , 0               , 0               , 0, 0x00000F4B, 0),
  MAKE_INST(INST_CMOVNS           , "cmovns"           , I_R_RM          , 0               , 0               , 0, 0x00000F49, 0),
  MAKE_INST(INST_CMOVNZ           , "cmovnz"           , I_R_RM          , 0               , 0               , 0, 0x00000F45, 0),
  MAKE_INST(INST_CMOVO            , "cmovo"            , I_R_RM          , 0               , 0               , 0, 0x00000F40, 0),
  MAKE_INST(INST_CMOVP            , "cmovp"            , I_R_RM          , 0               , 0               , 0, 0x00000F4A, 0),
  MAKE_INST(INST_CMOVPE           , "cmovpe"           , I_R_RM          , 0               , 0               , 0, 0x00000F4A, 0),
  MAKE_INST(INST_CMOVPO           , "cmovpo"           , I_R_RM          , 0               , 0               , 0, 0x00000F4B, 0),
  MAKE_INST(INST_CMOVS            , "cmovs"            , I_R_RM          , 0               , 0               , 0, 0x00000F48, 0),
  MAKE_INST(INST_CMOVZ            , "cmovz"            , I_R_RM          , 0               , 0               , 0, 0x00000F44, 0),
  MAKE_INST(INST_CMP              , "cmp"              , I_ALU           , 0               , 0               , 7, 0x00000038, 0x00000080),
  MAKE_INST(INST_CMPPD            , "cmppd"            , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x66000FC2, 0),
  MAKE_INST(INST_CMPPS            , "cmpps"            , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x00000FC2, 0),
  MAKE_INST(INST_CMPSD            , "cmpsd"            , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0xF2000FC2, 0),
  MAKE_INST(INST_CMPSS            , "cmpss"            , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0xF3000FC2, 0),
  MAKE_INST(INST_CMPXCHG          , "cmpxchg"          , I_RM_R          , 0               , 0               , 0, 0x00000FB0, 0),
  MAKE_INST(INST_CMPXCHG16B       , "cmpxchg16b"       , I_M             , O_MEM           , 0               , 1, 0x00000FC7, 1 /* RexW */),
  MAKE_INST(INST_CMPXCHG8B        , "cmpxchg8b"        , I_M             , O_MEM           , 0               , 1, 0x00000FC7, 0),
  MAKE_INST(INST_COMISD           , "comisd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F2F, 0),
  MAKE_INST(INST_COMISS           , "comiss"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F2F, 0),
  MAKE_INST(INST_CPUID            , "cpuid"            , I_EMIT          , 0               , 0               , 0, 0x00000FA2, 0),
  MAKE_INST(INST_CRC32            , "crc32"            , I_CRC32         , 0               , 0               , 0, 0xF20F38F0, 0),
  MAKE_INST(INST_CVTDQ2PD         , "cvtdq2pd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000FE6, 0),
  MAKE_INST(INST_CVTDQ2PS         , "cvtdq2ps"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F5B, 0),
  MAKE_INST(INST_CVTPD2DQ         , "cvtpd2dq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000FE6, 0),
  MAKE_INST(INST_CVTPD2PI         , "cvtpd2pi"         , I_MMU_RMI       , O_MM            , O_XMM_MEM       , 0, 0x66000F2D, 0),
  MAKE_INST(INST_CVTPD2PS         , "cvtpd2ps"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F5A, 0),
  MAKE_INST(INST_CVTPI2PD         , "cvtpi2pd"         , I_MMU_RMI       , O_XMM           , O_MM_MEM        , 0, 0x66000F2A, 0),
  MAKE_INST(INST_CVTPI2PS         , "cvtpi2ps"         , I_MMU_RMI       , O_XMM           , O_MM_MEM        , 0, 0x00000F2A, 0),
  MAKE_INST(INST_CVTPS2DQ         , "cvtps2dq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F5B, 0),
  MAKE_INST(INST_CVTPS2PD         , "cvtps2pd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F5A, 0),
  MAKE_INST(INST_CVTPS2PI         , "cvtps2pi"         , I_MMU_RMI       , O_MM            , O_XMM_MEM       , 0, 0x00000F2D, 0),
  MAKE_INST(INST_CVTSD2SI         , "cvtsd2si"         , I_MMU_RMI       , O_G32_64        , O_XMM_MEM       , 0, 0xF2000F2D, 0),
  MAKE_INST(INST_CVTSD2SS         , "cvtsd2ss"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F5A, 0),
  MAKE_INST(INST_CVTSI2SD         , "cvtsi2sd"         , I_MMU_RMI       , O_XMM           , O_G32_64|O_MEM  , 0, 0xF2000F2A, 0),
  MAKE_INST(INST_CVTSI2SS         , "cvtsi2ss"         , I_MMU_RMI       , O_XMM           , O_G32_64|O_MEM  , 0, 0xF3000F2A, 0),
  MAKE_INST(INST_CVTSS2SD         , "cvtss2sd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F5A, 0),
  MAKE_INST(INST_CVTSS2SI         , "cvtss2si"         , I_MMU_RMI       , O_G32_64        , O_XMM_MEM       , 0, 0xF3000F2D, 0),
  MAKE_INST(INST_CVTTPD2DQ        , "cvttpd2dq"        , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000FE6, 0),
  MAKE_INST(INST_CVTTPD2PI        , "cvttpd2pi"        , I_MMU_RMI       , O_MM            , O_XMM_MEM       , 0, 0x66000F2C, 0),
  MAKE_INST(INST_CVTTPS2DQ        , "cvttps2dq"        , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F5B, 0),
  MAKE_INST(INST_CVTTPS2PI        , "cvttps2pi"        , I_MMU_RMI       , O_MM            , O_XMM_MEM       , 0, 0x00000F2C, 0),
  MAKE_INST(INST_CVTTSD2SI        , "cvttsd2si"        , I_MMU_RMI       , O_G32_64        , O_XMM_MEM       , 0, 0xF2000F2C, 0),
  MAKE_INST(INST_CVTTSS2SI        , "cvttss2si"        , I_MMU_RMI       , O_G32_64        , O_XMM_MEM       , 0, 0xF3000F2C, 0),
  MAKE_INST(INST_CWDE             , "cwde"             , I_EMIT          , 0               , 0               , 0, 0x00000099, 0),
  MAKE_INST(INST_DAA              , "daa"              , I_EMIT          , 0               , 0               , 0, 0x00000027, 0),
  MAKE_INST(INST_DAS              , "das"              , I_EMIT          , 0               , 0               , 0, 0x0000002F, 0),
  MAKE_INST(INST_DEC              , "dec"              , I_INC_DEC       , 0               , 0               , 1, 0x00000048, 0x000000FE),
  MAKE_INST(INST_DIV              , "div"              , I_RM            , 0               , 0               , 6, 0x000000F6, 0),
  MAKE_INST(INST_DIVPD            , "divpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F5E, 0),
  MAKE_INST(INST_DIVPS            , "divps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F5E, 0),
  MAKE_INST(INST_DIVSD            , "divsd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F5E, 0),
  MAKE_INST(INST_DIVSS            , "divss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F5E, 0),
  MAKE_INST(INST_DPPD             , "dppd"             , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A41, 0),
  MAKE_INST(INST_DPPS             , "dpps"             , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A40, 0),
  MAKE_INST(INST_EMMS             , "emms"             , I_EMIT          , 0               , 0               , 0, 0x00000F77, 0),
  MAKE_INST(INST_EXTRACTPS        , "extractps"        , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A17, 0),
  MAKE_INST(INST_F2XM1            , "f2xm1"            , I_EMIT          , 0               , 0               , 0, 0x0000D9F0, 0),
  MAKE_INST(INST_FABS             , "fabs"             , I_EMIT          , 0               , 0               , 0, 0x0000D9E1, 0),
  MAKE_INST(INST_FADD             , "fadd"             , I_X87_FPU       , 0               , 0               , 0, 0xD8C0DCC0, 0),
  MAKE_INST(INST_FADDP            , "faddp"            , I_X87_STI       , 0               , 0               , 0, 0x0000DEC0, 0),
  MAKE_INST(INST_FBLD             , "fbld"             , I_M             , O_MEM           , 0               , 4, 0x000000DF, 0),
  MAKE_INST(INST_FBSTP            , "fbstp"            , I_M             , O_MEM           , 0               , 6, 0x000000DF, 0),
  MAKE_INST(INST_FCHS             , "fchs"             , I_EMIT          , 0               , 0               , 0, 0x0000D9E0, 0),
  MAKE_INST(INST_FCLEX            , "fclex"            , I_EMIT          , 0               , 0               , 0, 0x9B00DBE2, 0),
  MAKE_INST(INST_FCMOVB           , "fcmovb"           , I_X87_STI       , 0               , 0               , 0, 0x0000DAC0, 0),
  MAKE_INST(INST_FCMOVBE          , "fcmovbe"          , I_X87_STI       , 0               , 0               , 0, 0x0000DAD0, 0),
  MAKE_INST(INST_FCMOVE           , "fcmove"           , I_X87_STI       , 0               , 0               , 0, 0x0000DAC8, 0),
  MAKE_INST(INST_FCMOVNB          , "fcmovnb"          , I_X87_STI       , 0               , 0               , 0, 0x0000DBC0, 0),
  MAKE_INST(INST_FCMOVNBE         , "fcmovnbe"         , I_X87_STI       , 0               , 0               , 0, 0x0000DBD0, 0),
  MAKE_INST(INST_FCMOVNE          , "fcmovne"          , I_X87_STI       , 0               , 0               , 0, 0x0000DBC8, 0),
  MAKE_INST(INST_FCMOVNU          , "fcmovnu"          , I_X87_STI       , 0               , 0               , 0, 0x0000DBD8, 0),
  MAKE_INST(INST_FCMOVU           , "fcmovu"           , I_X87_STI       , 0               , 0               , 0, 0x0000DAD8, 0),
  MAKE_INST(INST_FCOM             , "fcom"             , I_X87_FPU       , 0               , 0               , 2, 0xD8DCD0D0, 0),
  MAKE_INST(INST_FCOMI            , "fcomi"            , I_X87_STI       , 0               , 0               , 0, 0x0000DBF0, 0),
  MAKE_INST(INST_FCOMIP           , "fcomip"           , I_X87_STI       , 0               , 0               , 0, 0x0000DFF0, 0),
  MAKE_INST(INST_FCOMP            , "fcomp"            , I_X87_FPU       , 0               , 0               , 3, 0xD8DCD8D8, 0),
  MAKE_INST(INST_FCOMPP           , "fcompp"           , I_EMIT          , 0               , 0               , 0, 0x0000DED9, 0),
  MAKE_INST(INST_FCOS             , "fcos"             , I_EMIT          , 0               , 0               , 0, 0x0000D9FF, 0),
  MAKE_INST(INST_FDECSTP          , "fdecstp"          , I_EMIT          , 0               , 0               , 0, 0x0000D9F6, 0),
  MAKE_INST(INST_FDIV             , "fdiv"             , I_X87_FPU       , 0               , 0               , 6, 0xD8DCF0F8, 0),
  MAKE_INST(INST_FDIVP            , "fdivp"            , I_X87_STI       , 0               , 0               , 0, 0x0000DEF8, 0),
  MAKE_INST(INST_FDIVR            , "fdivr"            , I_X87_FPU       , 0               , 0               , 7, 0xD8DCF8F0, 0),
  MAKE_INST(INST_FDIVRP           , "fdivrp"           , I_X87_STI       , 0               , 0               , 0, 0x0000DEF0, 0),
  MAKE_INST(INST_FEMMS            , "femms"            , I_EMIT          , 0               , 0               , 0, 0x00000F0E, 0),
  MAKE_INST(INST_FFREE            , "ffree"            , I_X87_STI       , 0               , 0               , 0, 0x0000DDC0, 0),
  MAKE_INST(INST_FIADD            , "fiadd"            , I_X87_MEM       , O_FM_2_4        , 0               , 0, 0xDEDA0000, 0),
  MAKE_INST(INST_FICOM            , "ficom"            , I_X87_MEM       , O_FM_2_4        , 0               , 2, 0xDEDA0000, 0),
  MAKE_INST(INST_FICOMP           , "ficomp"           , I_X87_MEM       , O_FM_2_4        , 0               , 3, 0xDEDA0000, 0),
  MAKE_INST(INST_FIDIV            , "fidiv"            , I_X87_MEM       , O_FM_2_4        , 0               , 6, 0xDEDA0000, 0),
  MAKE_INST(INST_FIDIVR           , "fidivr"           , I_X87_MEM       , O_FM_2_4        , 0               , 7, 0xDEDA0000, 0),
  MAKE_INST(INST_FILD             , "fild"             , I_X87_MEM       , O_FM_2_4_8      , 0               , 0, 0xDFDBDF05, 0),
  MAKE_INST(INST_FIMUL            , "fimul"            , I_X87_MEM       , O_FM_2_4        , 0               , 1, 0xDEDA0000, 0),
  MAKE_INST(INST_FINCSTP          , "fincstp"          , I_EMIT          , 0               , 0               , 0, 0x0000D9F7, 0),
  MAKE_INST(INST_FINIT            , "finit"            , I_EMIT          , 0               , 0               , 0, 0x9B00DBE3, 0),
  MAKE_INST(INST_FIST             , "fist"             , I_X87_MEM       , O_FM_2_4        , 0               , 2, 0xDFDB0000, 0),
  MAKE_INST(INST_FISTP            , "fistp"            , I_X87_MEM       , O_FM_2_4_8      , 0               , 3, 0xDFDBDF07, 0),
  MAKE_INST(INST_FISTTP           , "fisttp"           , I_X87_MEM       , O_FM_2_4_8      , 0               , 1, 0xDFDBDD01, 0),
  MAKE_INST(INST_FISUB            , "fisub"            , I_X87_MEM       , O_FM_2_4        , 0               , 4, 0xDEDA0000, 0),
  MAKE_INST(INST_FISUBR           , "fisubr"           , I_X87_MEM       , O_FM_2_4        , 0               , 5, 0xDEDA0000, 0),
  MAKE_INST(INST_FLD              , "fld"              , I_X87_MEM_STI   , O_FM_4_8_10     , 0               , 0, 0x00D9DD00, 0xD9C0DB05),
  MAKE_INST(INST_FLD1             , "fld1"             , I_EMIT          , 0               , 0               , 0, 0x0000D9E8, 0),
  MAKE_INST(INST_FLDCW            , "fldcw"            , I_M             , O_MEM           , 0               , 5, 0x000000D9, 0),
  MAKE_INST(INST_FLDENV           , "fldenv"           , I_M             , O_MEM           , 0               , 4, 0x000000D9, 0),
  MAKE_INST(INST_FLDL2E           , "fldl2e"           , I_EMIT          , 0               , 0               , 0, 0x0000D9EA, 0),
  MAKE_INST(INST_FLDL2T           , "fldl2t"           , I_EMIT          , 0               , 0               , 0, 0x0000D9E9, 0),
  MAKE_INST(INST_FLDLG2           , "fldlg2"           , I_EMIT          , 0               , 0               , 0, 0x0000D9EC, 0),
  MAKE_INST(INST_FLDLN2           , "fldln2"           , I_EMIT          , 0               , 0               , 0, 0x0000D9ED, 0),
  MAKE_INST(INST_FLDPI            , "fldpi"            , I_EMIT          , 0               , 0               , 0, 0x0000D9EB, 0),
  MAKE_INST(INST_FLDZ             , "fldz"             , I_EMIT          , 0               , 0               , 0, 0x0000D9EE, 0),
  MAKE_INST(INST_FMUL             , "fmul"             , I_X87_FPU       , 0               , 0               , 1, 0xD8DCC8C8, 0),
  MAKE_INST(INST_FMULP            , "fmulp"            , I_X87_STI       , 0               , 0               , 0, 0x0000DEC8, 0),
  MAKE_INST(INST_FNCLEX           , "fnclex"           , I_EMIT          , 0               , 0               , 0, 0x0000DBE2, 0),
  MAKE_INST(INST_FNINIT           , "fninit"           , I_EMIT          , 0               , 0               , 0, 0x0000DBE3, 0),
  MAKE_INST(INST_FNOP             , "fnop"             , I_EMIT          , 0               , 0               , 0, 0x0000D9D0, 0),
  MAKE_INST(INST_FNSAVE           , "fnsave"           , I_M             , O_MEM           , 0               , 6, 0x000000DD, 0),
  MAKE_INST(INST_FNSTCW           , "fnstcw"           , I_M             , O_MEM           , 0               , 7, 0x000000D9, 0),
  MAKE_INST(INST_FNSTENV          , "fnstenv"          , I_M             , O_MEM           , 0               , 6, 0x000000D9, 0),
  MAKE_INST(INST_FNSTSW           , "fnstsw"           , I_X87_FSTSW     , O_MEM           , 0               , 7, 0x000000DD, 0x0000DFE0),
  MAKE_INST(INST_FPATAN           , "fpatan"           , I_EMIT          , 0               , 0               , 0, 0x0000D9F3, 0),
  MAKE_INST(INST_FPREM            , "fprem"            , I_EMIT          , 0               , 0               , 0, 0x0000D9F8, 0),
  MAKE_INST(INST_FPREM1           , "fprem1"           , I_EMIT          , 0               , 0               , 0, 0x0000D9F5, 0),
  MAKE_INST(INST_FPTAN            , "fptan"            , I_EMIT          , 0               , 0               , 0, 0x0000D9F2, 0),
  MAKE_INST(INST_FRNDINT          , "frndint"          , I_EMIT          , 0               , 0               , 0, 0x0000D9FC, 0),
  MAKE_INST(INST_FRSTOR           , "frstor"           , I_M             , O_MEM           , 0               , 4, 0x000000DD, 0),
  MAKE_INST(INST_FSAVE            , "fsave"            , I_M             , O_MEM           , 0               , 6, 0x9B0000DD, 0),
  MAKE_INST(INST_FSCALE           , "fscale"           , I_EMIT          , 0               , 0               , 0, 0x0000D9FD, 0),
  MAKE_INST(INST_FSIN             , "fsin"             , I_EMIT          , 0               , 0               , 0, 0x0000D9FE, 0),
  MAKE_INST(INST_FSINCOS          , "fsincos"          , I_EMIT          , 0               , 0               , 0, 0x0000D9FB, 0),
  MAKE_INST(INST_FSQRT            , "fsqrt"            , I_EMIT          , 0               , 0               , 0, 0x0000D9FA, 0),
  MAKE_INST(INST_FST              , "fst"              , I_X87_MEM_STI   , O_FM_4_8        , 0               , 2, 0x00D9DD02, 0xDDD00000),
  MAKE_INST(INST_FSTCW            , "fstcw"            , I_M             , O_MEM           , 0               , 7, 0x9B0000D9, 0),
  MAKE_INST(INST_FSTENV           , "fstenv"           , I_M             , O_MEM           , 0               , 6, 0x9B0000D9, 0),
  MAKE_INST(INST_FSTP             , "fstp"             , I_X87_MEM_STI   , O_FM_4_8_10     , 0               , 3, 0x00D9DD03, 0xDDD8DB07),
  MAKE_INST(INST_FSTSW            , "fstsw"            , I_X87_FSTSW     , O_MEM           , 0               , 7, 0x9B0000DD, 0x9B00DFE0),
  MAKE_INST(INST_FSUB             , "fsub"             , I_X87_FPU       , 0               , 0               , 4, 0xD8DCE0E8, 0),
  MAKE_INST(INST_FSUBP            , "fsubp"            , I_X87_STI       , 0               , 0               , 0, 0x0000DEE8, 0),
  MAKE_INST(INST_FSUBR            , "fsubr"            , I_X87_FPU       , 0               , 0               , 5, 0xD8DCE8E0, 0),
  MAKE_INST(INST_FSUBRP           , "fsubrp"           , I_X87_STI       , 0               , 0               , 0, 0x0000DEE0, 0),
  MAKE_INST(INST_FTST             , "ftst"             , I_EMIT          , 0               , 0               , 0, 0x0000D9E4, 0),
  MAKE_INST(INST_FUCOM            , "fucom"            , I_X87_STI       , 0               , 0               , 0, 0x0000DDE0, 0),
  MAKE_INST(INST_FUCOMI           , "fucomi"           , I_X87_STI       , 0               , 0               , 0, 0x0000DBE8, 0),
  MAKE_INST(INST_FUCOMIP          , "fucomip"          , I_X87_STI       , 0               , 0               , 0, 0x0000DFE8, 0),
  MAKE_INST(INST_FUCOMP           , "fucomp"           , I_X87_STI       , 0               , 0               , 0, 0x0000DDE8, 0),
  MAKE_INST(INST_FUCOMPP          , "fucompp"          , I_EMIT          , 0               , 0               , 0, 0x0000DAE9, 0),
  MAKE_INST(INST_FWAIT            , "fwait"            , I_EMIT          , 0               , 0               , 0, 0x000000DB, 0),
  MAKE_INST(INST_FXAM             , "fxam"             , I_EMIT          , 0               , 0               , 0, 0x0000D9E5, 0),
  MAKE_INST(INST_FXCH             , "fxch"             , I_X87_STI       , 0               , 0               , 0, 0x0000D9C9, 0),
  MAKE_INST(INST_FXRSTOR          , "fxrstor"          , I_M             , 0               , 0               , 1, 0x00000FAE, 0),
  MAKE_INST(INST_FXSAVE           , "fxsave"           , I_M             , 0               , 0               , 0, 0x00000FAE, 0),
  MAKE_INST(INST_FXTRACT          , "fxtract"          , I_EMIT          , 0               , 0               , 0, 0x0000D9F4, 0),
  MAKE_INST(INST_FYL2X            , "fyl2x"            , I_EMIT          , 0               , 0               , 0, 0x0000D9F1, 0),
  MAKE_INST(INST_FYL2XP1          , "fyl2xp1"          , I_EMIT          , 0               , 0               , 0, 0x0000D9F9, 0),
  MAKE_INST(INST_HADDPD           , "haddpd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F7C, 0),
  MAKE_INST(INST_HADDPS           , "haddps"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F7C, 0),
  MAKE_INST(INST_HSUBPD           , "hsubpd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F7D, 0),
  MAKE_INST(INST_HSUBPS           , "hsubps"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F7D, 0),
  MAKE_INST(INST_IDIV             , "idiv"             , I_RM            , 0               , 0               , 7, 0x000000F6, 0),
  MAKE_INST(INST_IMUL             , "imul"             , I_IMUL          , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_INC              , "inc"              , I_INC_DEC       , 0               , 0               , 0, 0x00000040, 0x000000FE),
  MAKE_INST(INST_INT3             , "int3"             , I_EMIT          , 0               , 0               , 0, 0x000000CC, 0),
  MAKE_INST(INST_JA               , "ja"               , I_J             , 0               , 0               , 0, 0x7       , 0),
  MAKE_INST(INST_JAE              , "jae"              , I_J             , 0               , 0               , 0, 0x3       , 0),
  MAKE_INST(INST_JB               , "jb"               , I_J             , 0               , 0               , 0, 0x2       , 0),
  MAKE_INST(INST_JBE              , "jbe"              , I_J             , 0               , 0               , 0, 0x6       , 0),
  MAKE_INST(INST_JC               , "jc"               , I_J             , 0               , 0               , 0, 0x2       , 0),
  MAKE_INST(INST_JE               , "je"               , I_J             , 0               , 0               , 0, 0x4       , 0),
  MAKE_INST(INST_JG               , "jg"               , I_J             , 0               , 0               , 0, 0xF       , 0),
  MAKE_INST(INST_JGE              , "jge"              , I_J             , 0               , 0               , 0, 0xD       , 0),
  MAKE_INST(INST_JL               , "jl"               , I_J             , 0               , 0               , 0, 0xC       , 0),
  MAKE_INST(INST_JLE              , "jle"              , I_J             , 0               , 0               , 0, 0xE       , 0),
  MAKE_INST(INST_JNA              , "jna"              , I_J             , 0               , 0               , 0, 0x6       , 0),
  MAKE_INST(INST_JNAE             , "jnae"             , I_J             , 0               , 0               , 0, 0x2       , 0),
  MAKE_INST(INST_JNB              , "jnb"              , I_J             , 0               , 0               , 0, 0x3       , 0),
  MAKE_INST(INST_JNBE             , "jnbe"             , I_J             , 0               , 0               , 0, 0x7       , 0),
  MAKE_INST(INST_JNC              , "jnc"              , I_J             , 0               , 0               , 0, 0x3       , 0),
  MAKE_INST(INST_JNE              , "jne"              , I_J             , 0               , 0               , 0, 0x5       , 0),
  MAKE_INST(INST_JNG              , "jng"              , I_J             , 0               , 0               , 0, 0xE       , 0),
  MAKE_INST(INST_JNGE             , "jnge"             , I_J             , 0               , 0               , 0, 0xC       , 0),
  MAKE_INST(INST_JNL              , "jnl"              , I_J             , 0               , 0               , 0, 0xD       , 0),
  MAKE_INST(INST_JNLE             , "jnle"             , I_J             , 0               , 0               , 0, 0xF       , 0),
  MAKE_INST(INST_JNO              , "jno"              , I_J             , 0               , 0               , 0, 0x1       , 0),
  MAKE_INST(INST_JNP              , "jnp"              , I_J             , 0               , 0               , 0, 0xB       , 0),
  MAKE_INST(INST_JNS              , "jns"              , I_J             , 0               , 0               , 0, 0x9       , 0),
  MAKE_INST(INST_JNZ              , "jnz"              , I_J             , 0               , 0               , 0, 0x5       , 0),
  MAKE_INST(INST_JO               , "jo"               , I_J             , 0               , 0               , 0, 0x0       , 0),
  MAKE_INST(INST_JP               , "jp"               , I_J             , 0               , 0               , 0, 0xA       , 0),
  MAKE_INST(INST_JPE              , "jpe"              , I_J             , 0               , 0               , 0, 0xA       , 0),
  MAKE_INST(INST_JPO              , "jpo"              , I_J             , 0               , 0               , 0, 0xB       , 0),
  MAKE_INST(INST_JS               , "js"               , I_J             , 0               , 0               , 0, 0x8       , 0),
  MAKE_INST(INST_JZ               , "jz"               , I_J             , 0               , 0               , 0, 0x4       , 0),
  MAKE_INST(INST_JMP              , "jmp"              , I_JMP           , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_JMP_PTR          , "jmp"              , I_JMP_PTR       , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_LDDQU            , "lddqu"            , I_MMU_RMI       , O_XMM           , O_MEM           , 0, 0xF2000FF0, 0),
  MAKE_INST(INST_LDMXCSR          , "ldmxcsr"          , I_M             , O_MEM           , 0               , 2, 0x00000FAE, 0),
  MAKE_INST(INST_LEA              , "lea"              , I_LEA           , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_LFENCE           , "lfence"           , I_EMIT          , 0               , 0               , 0, 0x000FAEE8, 0),
  MAKE_INST(INST_LOCK             , "lock"             , I_EMIT          , 0               , 0               , 0, 0x000000F0, 0),
  MAKE_INST(INST_MASKMOVDQU       , "maskmovdqu"       , I_MMU_RMI       , O_XMM           , O_XMM           , 0, 0x66000F57, 0),
  MAKE_INST(INST_MASKMOVQ         , "maskmovq"         , I_MMU_RMI       , O_MM            , O_MM            , 0, 0x00000FF7, 0),
  MAKE_INST(INST_MAXPD            , "maxpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F5F, 0),
  MAKE_INST(INST_MAXPS            , "maxps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F5F, 0),
  MAKE_INST(INST_MAXSD            , "maxsd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F5F, 0),
  MAKE_INST(INST_MAXSS            , "maxss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F5F, 0),
  MAKE_INST(INST_MFENCE           , "mfence"           , I_EMIT          , 0               , 0               , 0, 0x000FAEF0, 0),
  MAKE_INST(INST_MINPD            , "minpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F5D, 0),
  MAKE_INST(INST_MINPS            , "minps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F5D, 0),
  MAKE_INST(INST_MINSD            , "minsd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F5D, 0),
  MAKE_INST(INST_MINSS            , "minss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F5D, 0),
  MAKE_INST(INST_MONITOR          , "monitor"          , I_EMIT          , 0               , 0               , 0, 0x000F01C8, 0),
  MAKE_INST(INST_MOV              , "mov"              , I_MOV           , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_MOVAPD           , "movapd"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x66000F28, 0x66000F29),
  MAKE_INST(INST_MOVAPS           , "movaps"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x00000F28, 0x00000F29),
  MAKE_INST(INST_MOVBE            , "movbe"            , I_MOVBE         ,O_G16_32_64|O_MEM,O_G16_32_64|O_MEM, 0, 0x000F38F0, 0x000F38F1),
  MAKE_INST(INST_MOVD             , "movd"             , I_MMU_MOVD      , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_MOVDDUP          , "movddup"          , I_MMU_MOV       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F12, 0),
  MAKE_INST(INST_MOVDQ2Q          , "movdq2q"          , I_MMU_MOV       , O_MM            , O_XMM           , 0, 0xF2000FD6, 0),
  MAKE_INST(INST_MOVDQA           , "movdqa"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x66000F6F, 0x66000F7F),
  MAKE_INST(INST_MOVDQU           , "movdqu"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0xF3000F6F, 0xF3000F7F),
  MAKE_INST(INST_MOVHLPS          , "movhlps"          , I_MMU_MOV       , O_XMM           , O_XMM           , 0, 0x00000F12, 0),
  MAKE_INST(INST_MOVHPD           , "movhpd"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x66000F16, 0x66000F17),
  MAKE_INST(INST_MOVHPS           , "movhps"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x00000F16, 0x00000F17),
  MAKE_INST(INST_MOVLHPS          , "movlhps"          , I_MMU_MOV       , O_XMM           , O_XMM           , 0, 0x00000F16, 0),
  MAKE_INST(INST_MOVLPD           , "movlpd"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x66000F12, 0x66000F13),
  MAKE_INST(INST_MOVLPS           , "movlps"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x00000F12, 0x00000F13),
  MAKE_INST(INST_MOVMSKPD         , "movmskpd"         , I_MMU_MOV       , O_G32_64|O_NOREX, O_XMM           , 0, 0x66000F50, 0),
  MAKE_INST(INST_MOVMSKPS         , "movmskps"         , I_MMU_MOV       , O_G32_64|O_NOREX, O_XMM           , 0, 0x00000F50, 0),
  MAKE_INST(INST_MOVNTDQ          , "movntdq"          , I_MMU_MOV       , O_MEM           , O_XMM           , 0, 0         , 0x66000FE7),
  MAKE_INST(INST_MOVNTDQA         , "movntdqa"         , I_MMU_MOV       , O_XMM           , O_MEM           , 0, 0x660F382A, 0),
  MAKE_INST(INST_MOVNTI           , "movnti"           , I_MMU_MOV       , O_MEM           , O_G32_64        , 0, 0         , 0x00000FC3),
  MAKE_INST(INST_MOVNTPD          , "movntpd"          , I_MMU_MOV       , O_MEM           , O_XMM           , 0, 0         , 0x66000F2B),
  MAKE_INST(INST_MOVNTPS          , "movntps"          , I_MMU_MOV       , O_MEM           , O_XMM           , 0, 0         , 0x00000F2B),
  MAKE_INST(INST_MOVNTQ           , "movntq"           , I_MMU_MOV       , O_MEM           , O_MM            , 0, 0         , 0x00000FE7),
  MAKE_INST(INST_MOVQ             , "movq"             , I_MMU_MOVQ      , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_MOVQ2DQ          , "movq2dq"          , I_MMU_RMI       , O_XMM           , O_MM            , 0, 0xF3000FD6, 0),
  MAKE_INST(INST_MOVSD            , "movsd"            , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0xF2000F10, 0xF2000F11),
  MAKE_INST(INST_MOVSHDUP         , "movshdup"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F16, 0),
  MAKE_INST(INST_MOVSLDUP         , "movsldup"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F12, 0),
  MAKE_INST(INST_MOVSS            , "movss"            , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0xF3000F10, 0xF3000F11),
  MAKE_INST(INST_MOVSX            , "movsx"            , I_MOVSX_MOVZX   , 0               , 0               , 0, 0x00000FBE, 0),
  MAKE_INST(INST_MOVSXD           , "movsxd"           , I_MOVSXD        , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_MOVUPD           , "movupd"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x66000F10, 0x66000F11),
  MAKE_INST(INST_MOVUPS           , "movups"           , I_MMU_MOV       , O_XMM_MEM       , O_XMM_MEM       , 0, 0x00000F10, 0x00000F11),
  MAKE_INST(INST_MOVZX            , "movzx"            , I_MOVSX_MOVZX   , 0               , 0               , 0, 0x00000FB6, 0),
  MAKE_INST(INST_MOV_PTR          , "mov"              , I_MOV_PTR       , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_MPSADBW          , "mpsadbw"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A42, 0),
  MAKE_INST(INST_MUL              , "mul"              , I_RM            , 0               , 0               , 4, 0x000000F6, 0),
  MAKE_INST(INST_MULPD            , "mulpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F59, 0),
  MAKE_INST(INST_MULPS            , "mulps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F59, 0),
  MAKE_INST(INST_MULSD            , "mulsd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F59, 0),
  MAKE_INST(INST_MULSS            , "mulss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F59, 0),
  MAKE_INST(INST_MWAIT            , "mwait"            , I_EMIT          , 0               , 0               , 0, 0x000F01C9, 0),
  MAKE_INST(INST_NEG              , "neg"              , I_RM            , 0               , 0               , 3, 0x000000F6, 0),
  MAKE_INST(INST_NOP              , "nop"              , I_EMIT          , 0               , 0               , 0, 0x00000090, 0),
  MAKE_INST(INST_NOT              , "not"              , I_RM            , 0               , 0               , 2, 0x000000F6, 0),
  MAKE_INST(INST_OR               , "or"               , I_ALU           , 0               , 0               , 1, 0x00000008, 0x00000080),
  MAKE_INST(INST_ORPD             , "orpd"             , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F56, 0),
  MAKE_INST(INST_ORPS             , "orps"             , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F56, 0),
  MAKE_INST(INST_PABSB            , "pabsb"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F381C, 0),
  MAKE_INST(INST_PABSD            , "pabsd"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F381E, 0),
  MAKE_INST(INST_PABSW            , "pabsw"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F381D, 0),
  MAKE_INST(INST_PACKSSDW         , "packssdw"         , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F6B, 0),
  MAKE_INST(INST_PACKSSWB         , "packsswb"         , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F63, 0),
  MAKE_INST(INST_PACKUSDW         , "packusdw"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F382B, 0),
  MAKE_INST(INST_PACKUSWB         , "packuswb"         , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F67, 0),
  MAKE_INST(INST_PADDB            , "paddb"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FFC, 0),
  MAKE_INST(INST_PADDD            , "paddd"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FFE, 0),
  MAKE_INST(INST_PADDQ            , "paddq"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FD4, 0),
  MAKE_INST(INST_PADDSB           , "paddsb"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FEC, 0),
  MAKE_INST(INST_PADDSW           , "paddsw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FED, 0),
  MAKE_INST(INST_PADDUSB          , "paddusb"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FDC, 0),
  MAKE_INST(INST_PADDUSW          , "paddusw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FDD, 0),
  MAKE_INST(INST_PADDW            , "paddw"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FFD, 0),
  MAKE_INST(INST_PALIGNR          , "palignr"          , I_MMU_RM_IMM8   , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3A0F, 0),
  MAKE_INST(INST_PAND             , "pand"             , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FDB, 0),
  MAKE_INST(INST_PANDN            , "pandn"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FDF, 0),
  MAKE_INST(INST_PAUSE            , "pause"            , I_EMIT          , 0               , 0               , 0, 0xF3000090, 0),
  MAKE_INST(INST_PAVGB            , "pavgb"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FE0, 0),
  MAKE_INST(INST_PAVGW            , "pavgw"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FE3, 0),
  MAKE_INST(INST_PBLENDVB         , "pblendvb"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3810, 0),
  MAKE_INST(INST_PBLENDW          , "pblendw"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A0E, 0),
  MAKE_INST(INST_PCMPEQB          , "pcmpeqb"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F74, 0),
  MAKE_INST(INST_PCMPEQD          , "pcmpeqd"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F76, 0),
  MAKE_INST(INST_PCMPEQQ          , "pcmpeqq"          , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3829, 0),
  MAKE_INST(INST_PCMPEQW          , "pcmpeqw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F75, 0),
  MAKE_INST(INST_PCMPESTRI        , "pcmpestri"        , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A61, 0),
  MAKE_INST(INST_PCMPESTRM        , "pcmpestrm"        , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A60, 0),
  MAKE_INST(INST_PCMPGTB          , "pcmpgtb"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F64, 0),
  MAKE_INST(INST_PCMPGTD          , "pcmpgtd"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F66, 0),
  MAKE_INST(INST_PCMPGTQ          , "pcmpgtq"          , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3837, 0),
  MAKE_INST(INST_PCMPGTW          , "pcmpgtw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F65, 0),
  MAKE_INST(INST_PCMPISTRI        , "pcmpistri"        , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A63, 0),
  MAKE_INST(INST_PCMPISTRM        , "pcmpistrm"        , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A62, 0),
  MAKE_INST(INST_PEXTRB           , "pextrb"           , I_MMU_PEXTR     , O_G8|O_G32|O_MEM, O_XMM           , 0, 0x000F3A14, 0),
  MAKE_INST(INST_PEXTRD           , "pextrd"           , I_MMU_PEXTR     , O_G32     |O_MEM, O_XMM           , 0, 0x000F3A16, 0),
  MAKE_INST(INST_PEXTRQ           , "pextrq"           , I_MMU_PEXTR     , O_G32_64  |O_MEM, O_XMM           , 1, 0x000F3A16, 0),
  MAKE_INST(INST_PEXTRW           , "pextrw"           , I_MMU_PEXTR     , O_G32     |O_MEM, O_XMM | O_MM    , 0, 0x000F3A16, 0),
  MAKE_INST(INST_PF2ID            , "pf2id"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x1D),
  MAKE_INST(INST_PF2IW            , "pf2iw"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x1C),
  MAKE_INST(INST_PFACC            , "pfacc"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xAE),
  MAKE_INST(INST_PFADD            , "pfadd"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x9E),
  MAKE_INST(INST_PFCMPEQ          , "pfcmpeq"          , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xB0),
  MAKE_INST(INST_PFCMPGE          , "pfcmpge"          , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x90),
  MAKE_INST(INST_PFCMPGT          , "pfcmpgt"          , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xA0),
  MAKE_INST(INST_PFMAX            , "pfmax"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xA4),
  MAKE_INST(INST_PFMIN            , "pfmin"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x94),
  MAKE_INST(INST_PFMUL            , "pfmul"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xB4),
  MAKE_INST(INST_PFNACC           , "pfnacc"           , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x8A),
  MAKE_INST(INST_PFPNACC          , "pfpnacc"          , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x8E),
  MAKE_INST(INST_PFRCP            , "pfrcp"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x96),
  MAKE_INST(INST_PFRCPIT1         , "pfrcpit1"         , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xA6),
  MAKE_INST(INST_PFRCPIT2         , "pfrcpit2"         , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xB6),
  MAKE_INST(INST_PFRSQIT1         , "pfrsqit1"         , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xA7),
  MAKE_INST(INST_PFRSQRT          , "pfrsqrt"          , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x97),
  MAKE_INST(INST_PFSUB            , "pfsub"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x9A),
  MAKE_INST(INST_PFSUBR           , "pfsubr"           , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xAA),
  MAKE_INST(INST_PHADDD           , "phaddd"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3802, 0),
  MAKE_INST(INST_PHADDSW          , "phaddsw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3803, 0),
  MAKE_INST(INST_PHADDW           , "phaddw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3801, 0),
  MAKE_INST(INST_PHMINPOSUW       , "phminposuw"       , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3841, 0),
  MAKE_INST(INST_PHSUBD           , "phsubd"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3806, 0),
  MAKE_INST(INST_PHSUBSW          , "phsubsw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3807, 0),
  MAKE_INST(INST_PHSUBW           , "phsubw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3805, 0),
  MAKE_INST(INST_PI2FD            , "pi2fd"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x0D),
  MAKE_INST(INST_PI2FW            , "pi2fw"            , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0x0C),
  MAKE_INST(INST_PINSRB           , "pinsrb"           , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A20, 0),
  MAKE_INST(INST_PINSRD           , "pinsrd"           , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A22, 0),
  MAKE_INST(INST_PINSRW           , "pinsrw"           , I_MMU_RM_IMM8   , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FC4, 0),
  MAKE_INST(INST_PMADDUBSW        , "pmaddubsw"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3804, 0),
  MAKE_INST(INST_PMADDWD          , "pmaddwd"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FF5, 0),
  MAKE_INST(INST_PMAXSB           , "pmaxsb"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F383C, 0),
  MAKE_INST(INST_PMAXSD           , "pmaxsd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F383D, 0),
  MAKE_INST(INST_PMAXSW           , "pmaxsw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FEE, 0),
  MAKE_INST(INST_PMAXUB           , "pmaxub"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FDE, 0),
  MAKE_INST(INST_PMAXUD           , "pmaxud"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F383F, 0),
  MAKE_INST(INST_PMAXUW           , "pmaxuw"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F383E, 0),
  MAKE_INST(INST_PMINSB           , "pminsb"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3838, 0),
  MAKE_INST(INST_PMINSD           , "pminsd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3839, 0),
  MAKE_INST(INST_PMINSW           , "pminsw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FEA, 0),
  MAKE_INST(INST_PMINUB           , "pminub"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FDA, 0),
  MAKE_INST(INST_PMINUD           , "pminud"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F383B, 0),
  MAKE_INST(INST_PMINUW           , "pminuw"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F383A, 0),
  MAKE_INST(INST_PMOVMSKB         , "pmovmskb"         , I_MMU_RMI       , O_G32_64        , O_MM_XMM        , 0, 0x00000FD7, 0),
  MAKE_INST(INST_PMOVSXBD         , "pmovsxbd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3821, 0),
  MAKE_INST(INST_PMOVSXBQ         , "pmovsxbq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3822, 0),
  MAKE_INST(INST_PMOVSXBW         , "pmovsxbw"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3820, 0),
  MAKE_INST(INST_PMOVSXDQ         , "pmovsxdq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3825, 0),
  MAKE_INST(INST_PMOVSXWD         , "pmovsxwd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3823, 0),
  MAKE_INST(INST_PMOVSXWQ         , "pmovsxwq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3824, 0),
  MAKE_INST(INST_PMOVZXBD         , "pmovzxbd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3831, 0),
  MAKE_INST(INST_PMOVZXBQ         , "pmovzxbq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3832, 0),
  MAKE_INST(INST_PMOVZXBW         , "pmovzxbw"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3830, 0),
  MAKE_INST(INST_PMOVZXDQ         , "pmovzxdq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3835, 0),
  MAKE_INST(INST_PMOVZXWD         , "pmovzxwd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3833, 0),
  MAKE_INST(INST_PMOVZXWQ         , "pmovzxwq"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3834, 0),
  MAKE_INST(INST_PMULDQ           , "pmuldq"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3828, 0),
  MAKE_INST(INST_PMULHRSW         , "pmulhrsw"         , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F380B, 0),
  MAKE_INST(INST_PMULHUW          , "pmulhuw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FE4, 0),
  MAKE_INST(INST_PMULHW           , "pmulhw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FE5, 0),
  MAKE_INST(INST_PMULLD           , "pmulld"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3840, 0),
  MAKE_INST(INST_PMULLW           , "pmullw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FD5, 0),
  MAKE_INST(INST_PMULUDQ          , "pmuludq"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FF4, 0),
  MAKE_INST(INST_POP              , "pop"              , I_POP           , 0               , 0               , 0, 0x00000058, 0x0000008F),
  MAKE_INST(INST_POPAD            , "popad"            , I_EMIT          , 0               , 0               , 0, 0x00000061, 0),
  MAKE_INST(INST_POPCNT           , "popcnt"           , I_R_RM          , 0               , 0               , 0, 0xF3000FB8, 0),
  MAKE_INST(INST_POPFD            , "popfd"            , I_EMIT          , 0               , 0               , 0, 0x0000009D, 0),
  MAKE_INST(INST_POPFQ            , "popfq"            , I_EMIT          , 0               , 0               , 0, 0x0000009D, 0),
  MAKE_INST(INST_POR              , "por"              , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FEB, 0),
  MAKE_INST(INST_PREFETCH         , "prefetch"         , I_MMU_PREFETCH  , O_MEM           , O_IMM           , 0, 0         , 0),
  MAKE_INST(INST_PSADBW           , "psadbw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FF6, 0),
  MAKE_INST(INST_PSHUFB           , "pshufb"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3800, 0),
  MAKE_INST(INST_PSHUFD           , "pshufd"           , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x66000F70, 0),
  MAKE_INST(INST_PSHUFW           , "pshufw"           , I_MMU_RM_IMM8   , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F70, 0),
  MAKE_INST(INST_PSHUHW           , "pshuhw"           , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0xF3000F70, 0),
  MAKE_INST(INST_PSHULW           , "pshulw"           , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0xF2000F70, 0),
  MAKE_INST(INST_PSIGNB           , "psignb"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3808, 0),
  MAKE_INST(INST_PSIGND           , "psignd"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F380A, 0),
  MAKE_INST(INST_PSIGNW           , "psignw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x000F3809, 0),
  MAKE_INST(INST_PSLLD            , "pslld"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 6, 0x00000FF2, 0x00000F72),
  MAKE_INST(INST_PSLLDQ           , "pslldq"           , I_MMU_RMI       , O_XMM   , O_IMM                   , 7, 0         , 0x66000F73),
  MAKE_INST(INST_PSLLQ            , "psllq"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 6, 0x00000FF3, 0x00000F73),
  MAKE_INST(INST_PSLLW            , "psllw"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 6, 0x00000FF1, 0x00000F71),
  MAKE_INST(INST_PSRAD            , "psrad"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 4, 0x00000FE2, 0x00000F72),
  MAKE_INST(INST_PSRAW            , "psraw"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 4, 0x00000FE1, 0x00000F71),
  MAKE_INST(INST_PSRLD            , "psrld"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 2, 0x00000FD2, 0x00000F72),
  MAKE_INST(INST_PSRLDQ           , "psrldq"           , I_MMU_RMI       , O_XMM   , O_IMM                   , 3, 0         , 0x66000F73),
  MAKE_INST(INST_PSRLQ            , "psrlq"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 2, 0x00000FD3, 0x00000F73),
  MAKE_INST(INST_PSRLW            , "psrlw"            , I_MMU_RMI       , O_MM_XMM, O_IMM | O_MM_XMM_MEM    , 2, 0x00000FD1, 0x00000F71),
  MAKE_INST(INST_PSUBB            , "psubb"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FF8, 0),
  MAKE_INST(INST_PSUBD            , "psubd"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FFA, 0),
  MAKE_INST(INST_PSUBQ            , "psubq"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FFB, 0),
  MAKE_INST(INST_PSUBSB           , "psubsb"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FE8, 0),
  MAKE_INST(INST_PSUBSW           , "psubsw"           , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FE9, 0),
  MAKE_INST(INST_PSUBUSB          , "psubusb"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FD8, 0),
  MAKE_INST(INST_PSUBUSW          , "psubusw"          , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FD9, 0),
  MAKE_INST(INST_PSUBW            , "psubw"            , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FF9, 0),
  MAKE_INST(INST_PSWAPD           , "pswapd"           , I_MMU_RM_3DNOW  , O_MM            , O_MM_MEM        , 0, 0x00000F0F, 0xBB),
  MAKE_INST(INST_PTEST            , "ptest"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x660F3817, 0),
  MAKE_INST(INST_PUNPCKHBW        , "punpckhbw"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F68, 0),
  MAKE_INST(INST_PUNPCKHDQ        , "punpckhdq"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F6A, 0),
  MAKE_INST(INST_PUNPCKHQDQ       , "punpckhqdq"       , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F6D, 0),
  MAKE_INST(INST_PUNPCKHWD        , "punpckhwd"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F69, 0),
  MAKE_INST(INST_PUNPCKLBW        , "punpcklbw"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F60, 0),
  MAKE_INST(INST_PUNPCKLDQ        , "punpckldq"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F62, 0),
  MAKE_INST(INST_PUNPCKLQDQ       , "punpcklqdq"       , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F6C, 0),
  MAKE_INST(INST_PUNPCKLWD        , "punpcklwd"        , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000F61, 0),
  MAKE_INST(INST_PUSH             , "push"             , I_PUSH          , 0               , 0               , 6, 0x00000050, 0x000000FF),
  MAKE_INST(INST_PUSHAD           , "pushad"           , I_EMIT          , 0               , 0               , 0, 0x00000060, 0),
  MAKE_INST(INST_PUSHFD           , "pushfd"           , I_EMIT          , 0               , 0               , 0, 0x0000009C, 0),
  MAKE_INST(INST_PUSHFQ           , "pushfq"           , I_EMIT          , 0               , 0               , 0, 0x0000009C, 0),
  MAKE_INST(INST_PXOR             , "pxor"             , I_MMU_RMI       , O_MM_XMM        , O_MM_XMM_MEM    , 0, 0x00000FEF, 0),
  MAKE_INST(INST_RCL              , "rcl"              , I_ROT           , 0               , 0               , 2, 0         , 0),
  MAKE_INST(INST_RCPPS            , "rcpps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F53, 0),
  MAKE_INST(INST_RCPSS            , "rcpss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F53, 0),
  MAKE_INST(INST_RCR              , "rcr"              , I_ROT           , 0               , 0               , 3, 0         , 0),
  MAKE_INST(INST_RDTSC            , "rdtsc"            , I_EMIT          , 0               , 0               , 0, 0x00000F31, 0),
  MAKE_INST(INST_RDTSCP           , "rdtscp"           , I_EMIT          , 0               , 0               , 0, 0x000F01F9, 0),
  MAKE_INST(INST_RET              , "ret"              , I_RET           , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_ROL              , "rol"              , I_ROT           , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_ROR              , "ror"              , I_ROT           , 0               , 0               , 1, 0         , 0),
  MAKE_INST(INST_ROUNDPD          , "roundpd"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A09, 0),
  MAKE_INST(INST_ROUNDPS          , "roundps"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A08, 0),
  MAKE_INST(INST_ROUNDSD          , "roundsd"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A0B, 0),
  MAKE_INST(INST_ROUNDSS          , "roundss"          , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x660F3A0A, 0),
  MAKE_INST(INST_RSQRTPS          , "rsqrtps"          , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F52, 0),
  MAKE_INST(INST_RSQRTSS          , "rsqrtss"          , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F52, 0),
  MAKE_INST(INST_SAHF             , "sahf"             , I_EMIT          , 0               , 0               , 0, 0x0000009E, 0),
  MAKE_INST(INST_SAL              , "sal"              , I_ROT           , 0               , 0               , 4, 0         , 0),
  MAKE_INST(INST_SAR              , "sar"              , I_ROT           , 0               , 0               , 7, 0         , 0),
  MAKE_INST(INST_SBB              , "sbb"              , I_ALU           , 0               , 0               , 3, 0x00000018, 0x00000080),
  MAKE_INST(INST_SFENCE           , "sfence"           , I_M             , O_MEM           , 0               , 7, 0x00000FAE, 0),
  MAKE_INST(INST_SHL              , "shl"              , I_ROT           , 0               , 0               , 4, 0         , 0),
  MAKE_INST(INST_SHLD             , "shld"             , I_SHLD_SHRD     , 0               , 0               , 0, 0x00000FA4, 0),
  MAKE_INST(INST_SHR              , "shr"              , I_ROT           , 0               , 0               , 5, 0         , 0),
  MAKE_INST(INST_SHRD             , "shrd"             , I_SHLD_SHRD     , 0               , 0               , 0, 0x00000FAC, 0),
  MAKE_INST(INST_SHUFPS           , "shufps"           , I_MMU_RM_IMM8   , O_XMM           , O_XMM_MEM       , 0, 0x00000FC6, 0),
  MAKE_INST(INST_SQRTPD           , "sqrtpd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F51, 0),
  MAKE_INST(INST_SQRTPS           , "sqrtps"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F51, 0),
  MAKE_INST(INST_SQRTSD           , "sqrtsd"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F51, 0),
  MAKE_INST(INST_SQRTSS           , "sqrtss"           , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F51, 0),
  MAKE_INST(INST_STC              , "stc"              , I_EMIT          , 0               , 0               , 0, 0x000000F9, 0),
  MAKE_INST(INST_STD              , "std"              , I_EMIT          , 0               , 0               , 0, 0x000000FD, 0),
  MAKE_INST(INST_STMXCSR          , "stmxcsr"          , I_M             , O_MEM           , 0               , 3, 0x00000FAE, 0),
  MAKE_INST(INST_SUB              , "sub"              , I_ALU           , 0               , 0               , 5, 0x00000028, 0x00000080),
  MAKE_INST(INST_SUBPD            , "subpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F5C, 0),
  MAKE_INST(INST_SUBPS            , "subps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F5C, 0),
  MAKE_INST(INST_SUBSD            , "subsd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF2000F5C, 0),
  MAKE_INST(INST_SUBSS            , "subss"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0xF3000F51, 0),
  MAKE_INST(INST_TEST             , "test"             , I_TEST          , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_UCOMISD          , "ucomisd"          , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F2E, 0),
  MAKE_INST(INST_UCOMISS          , "ucomiss"          , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F2E, 0),
  MAKE_INST(INST_UD2              , "ud2"              , I_EMIT          , 0               , 0               , 0, 0x00000F0B, 0),
  MAKE_INST(INST_UNPCKHPD         , "unpckhpd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F15, 0),
  MAKE_INST(INST_UNPCKHPS         , "unpckhps"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F15, 0),
  MAKE_INST(INST_UNPCKLPD         , "unpcklpd"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F14, 0),
  MAKE_INST(INST_UNPCKLPS         , "unpcklps"         , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F14, 0),
  MAKE_INST(INST_XADD             , "xadd"             , I_RM_R          , 0               , 0               , 0, 0x00000FC0, 0),
  MAKE_INST(INST_XCHG             , "xchg"             , I_XCHG          , 0               , 0               , 0, 0         , 0),
  MAKE_INST(INST_XOR              , "xor"              , I_ALU           , 0               , 0               , 6, 0x00000030, 0x00000080),
  MAKE_INST(INST_XORPD            , "xorpd"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x66000F57, 0),
  MAKE_INST(INST_XORPS            , "xorps"            , I_MMU_RMI       , O_XMM           , O_XMM_MEM       , 0, 0x00000F57, 0)
};

#undef MAKE_INST

void Assembler::_emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  // Operands can't be non null, OP_NONE must be used instead
  ASMJIT_ASSERT(o1 && o2 && o3);

  // Check for buffer space (and grow if needed).
  if (!ensureSpace()) return;

  if (code >= _INST_COUNT) return;
  const InstructionDescription& id = x86instructions[code];

#if defined(ASMJIT_DEBUG_INSTRUCTION_MAP)
  ASMJIT_ASSERT(id.instruction == code);
#endif // ASMJIT_DEBUG_INSTRUCTION_MAP

  switch (id.group)
  {
    case I_EMIT:
    {
      _emitOpCode(id.opCode1);
      return;
    }

    case I_ALU:
    {
      UInt32 opCode = id.opCode1;
      UInt8 opReg = id.opCodeR;

      // Mem <- Reg
      if (o1->isMem() && o2->isReg())
      {
        _emitX86RM(opCode + (!o2->isRegType(REG_GPB)),
          o2->isRegType(REG_GPW), 
          o2->isRegType(REG_GPQ), 
          operand_cast<const Register&>(*o2).code(), 
          operand_cast<const BaseRegMem&>(*o1));
        return;
      }

      // Reg <- Reg|Mem
      if (o1->isReg() && o2->isRegMem())
      {
        _emitX86RM(opCode + 2 + (!o1->isRegType(REG_GPB)),
          o1->isRegType(REG_GPW), 
          o1->isRegType(REG_GPQ), 
          operand_cast<const Register&>(*o1).code(), 
          operand_cast<const BaseRegMem&>(*o2));
        return;
      }

      // AL, AX, EAX, RAX register shortcuts
      if (o1->isRegIndex(0) && o2->isImm())
      {
        if (o1->isRegType(REG_GPW))
          _emitByte(0x66); // 16 bit
        else if (o1->isRegType(REG_GPQ))
          _emitByte(0x48); // REX.W

        _emitByte((opReg << 3) | (0x04 + !o1->isRegType(REG_GPB)));
        _emitImmediate(
          operand_cast<const Immediate&>(*o2), o1->size() <= 4 ? o1->size() : 4);
        return;
      }

      if (o1->isRegMem() && o2->isImm())
      {
        const Immediate& imm = operand_cast<const Immediate&>(*o2);
        UInt8 immSize = isInt8(imm.value()) ? 1 : (o1->size() <= 4 ? o1->size() : 4);

        _emitX86RM(id.opCode2 + (o1->size() != 1 ? (immSize != 1 ? 1 : 3) : 0), 
          o1->size() == 2,
          o1->size() == 8,
          opReg, operand_cast<const BaseRegMem&>(*o1));
        _emitImmediate(
          operand_cast<const Immediate&>(*o2), immSize);
        return;
      }

      break;
    }

    case I_BSWAP:
    {
      if (o1->isReg())
      {
        const Register& dst = operand_cast<const Register&>(*o1);

#if defined(ASMJIT_X64)
        _emitRexR(dst.type() == REG_GPQ, 1, dst.code());
#endif // ASMJIT_X64
        _emitByte(0x0F);
        _emitModR(1, dst.code());
        return;
      }

      break;
    }

    case I_BT:
    {
      if (o1->isRegMem() && o2->isReg())
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);
        const Register& src = operand_cast<const Register&>(*o2);

        _emitX86RM(id.opCode1, 
          src.isRegType(REG_GPW),
          src.isRegType(REG_GPQ),
          src.code(),
          dst);
        return;
      }

      if (o1->isRegMem() && o2->isImm())
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);
        const Immediate& src = operand_cast<const Immediate&>(*o2);

        _emitX86RM(id.opCode2,
          src.size() == 2,
          src.size() == 8,
          id.opCodeR,
          dst);
        _emitImmediate(src, 1);
        return;
      }

      break;
    }

    case I_CALL:
    {
      if (o1->isRegMem(REG_GPN))
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);
        _emitX86RM(0xFF, 
          0, 
          0, 2, dst);
        return;
      }

      if (o1->isLabel())
      {
        Label* label = (Label*)(o1);

        if (label->isBound())
        {
          const SysInt rel32_size = 5;
          SysInt offs = label->position() - offset();
          ASMJIT_ASSERT(offs <= 0);

          _emitByte(0xE8);
          _emitInt32((Int32)(offs - rel32_size));
        }
        else
        {
          _emitByte(0xE8);
          _emitDisplacement(label);
        }
        return;
      }

      break;
    }
    
    case I_CRC32:
    {
      if (o1->isReg() && o2->isRegMem())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o2);
        ASMJIT_ASSERT(dst.type() == REG_GPD || dst.type() == REG_GPQ);

        _emitX86RM(id.opCode1 + (src.size() != 1),
          src.size() == 2, 
          dst.type() == 8, dst.code(), src);
        return;
      }

      break;
    }

    case I_IMUL:
    {
      // 1 operand
      if (o1->isRegMem() && o2->isNone() && o3->isNone())
      {
        const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o1);
        _emitX86RM(0xF6 + (src.size() != 1),
          src.size() == 2,
          src.size() == 8, 5, src);
        return;
      }
      // 2 operands
      else if (o1->isReg() && !o2->isNone() && o3->isNone())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        ASMJIT_ASSERT(!dst.isRegType(REG_GPW));

        if (o2->isRegMem())
        {
          const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o2);

          _emitX86RM(0x0FAF,
            dst.isRegType(REG_GPW), 
            dst.isRegType(REG_GPQ), dst.code(), operand_cast<const BaseRegMem&>(src));
          return;
        }
        else if (o2->isImm())
        {
          const Immediate& imm = operand_cast<const Immediate&>(*o2);

          if (isInt8(imm.value()) && imm.relocMode() == RELOC_NONE)
          {
            _emitX86RM(0x6B,
              dst.isRegType(REG_GPW),
              dst.isRegType(REG_GPQ), dst.code(), dst);
            _emitImmediate(imm, 1);
          }
          else
          {
            _emitX86RM(0x69, 
              dst.isRegType(REG_GPW), 
              dst.isRegType(REG_GPQ), dst.code(), dst);
            _emitImmediate(imm, dst.isRegType(REG_GPW) ? 2 : 4);
          }
          return;
        }
      }
      // 3 operands
      else if (o1->isReg() && o2->isRegMem() && o3->isImm())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o2);
        const Immediate& imm = operand_cast<const Immediate&>(*o3);

        if (isInt8(imm.value()) && imm.relocMode() == RELOC_NONE)
        {
          _emitX86RM(0x6B,
            dst.isRegType(REG_GPW),
            dst.isRegType(REG_GPQ), dst.code(), src);
          _emitImmediate(imm, 1);
        }
        else
        {
          _emitX86RM(0x69, 
            dst.isRegType(REG_GPW), 
            dst.isRegType(REG_GPQ), dst.code(), src);
          _emitImmediate(imm, dst.isRegType(REG_GPW) ? 2 : 4);
        }
        return;
      }

      break;
    }
    
    case I_INC_DEC:
    {
      if (o1->isRegMem())
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);

        // INC [r16|r32] in 64 bit mode is not encodable.
#if defined(ASMJIT_X86)
        if ((dst.isReg()) && (dst.isRegType(REG_GPW) || dst.isRegType(REG_GPD)))
        {
          _emitX86Inl(id.opCode1,
            dst.isRegType(REG_GPW), 
            0, operand_cast<const BaseReg&>(dst).code());
          return;
        }
#endif // ASMJIT_X86

        _emitX86RM(id.opCode2 + (dst.size() != 1),
          dst.size() == 2,
          dst.size() == 8, (UInt8)id.opCodeR, dst);
        return; 
      }

      break;
    }

    case I_J:
    {
      if (o1->isLabel())
      {
        Label* label = (Label*)(o1);

        if (o2->isImm() && operand_cast<const Immediate&>(*o2).value() != HINT_NONE)
        {
          UInt8 hint = operand_cast<const Immediate&>(*o2).value() & 0xFF;
          _emitByte(hint);
        }

        if (label->isBound())
        {
          const SysInt rel8_size = 2;
          const SysInt rel32_size = 6;
          SysInt offs = label->position() - offset();

          ASMJIT_ASSERT(offs <= 0);

          if (isInt8(offs - rel8_size))
          {
            _emitByte(0x70 | (UInt8)id.opCode1);
            _emitByte((UInt8)(Int8)(offs - rel8_size));
          }
          else
          {
            _emitByte(0x0F);
            _emitByte(0x80 | (UInt8)id.opCode1);
            _emitInt32((Int32)(offs - rel32_size));
          }
        }
        else
        {
          _emitByte(0x0F);
          _emitByte(0x80 | (UInt8)id.opCode1);
          _emitDisplacement(label);
        }
        return;
      }

      break;
    }

    case I_JMP:
    {
      if (o1->isRegMem())
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);

        _emitX86RM(0xFF,
          0,
          0, 4, dst);
        return; 
      }

      if (o1->isLabel())
      {
        Label* label = (Label*)(o1);

        if (label->isBound())
        {
          const SysInt rel8_size = 2;
          const SysInt rel32_size = 5;
          SysInt offs = label->position() - offset();

          ASMJIT_ASSERT(offs <= 0);

          if (isInt8(offs - rel8_size))
          {
            _emitByte(0xEB);
            _emitByte((UInt8)(Int8)(offs - rel8_size));
          }
          else
          {
            _emitByte(0xE9);
            _emitInt32((Int32)(offs - rel32_size));
          }
        }
        else
        {
          _emitByte(0xE9);
          _emitDisplacement(label);
        }
        return;
      }

      break;
    }

    case I_JMP_PTR:
    {
      if (o1->isImm() && o2->isReg())
      {
        void* ptr = (void*)operand_cast<const Immediate&>(*o1).value();
        const Register& temporary = operand_cast<const Register&>(*o2);

        ASMJIT_ASSERT(temporary.isRegType(REG_GPN));

        // Emit mov and absolute jmp
        SysInt jmpStart = offset();
        mov(temporary, operand_cast<const Immediate&>(*o1));
        SysInt jmpAddress = offset() - sizeof(SysInt);
        jmp(temporary);
        SysInt jmpSize = offset() - jmpStart;

        // Store information about relocation
        RelocInfo ri;
        ri.offset = jmpStart;
        ri.size = 4;
        ri.mode = RELOC_JMP_RELATIVE;

        // Data contains informations in two bytes:
        // [0] - Offset relative to jmpStart where absolute address is read by 
        //       relocCode()
        // [1] - total size of jmp instructions generated by jmp_rel
        ri.data = (UInt16)((jmpAddress - jmpStart) << 8) | (UInt16)(jmpSize);
        _relocations.append(ri);
        return; 
      }

      break;
    }
    
    case I_LEA:
    {
      if (o1->isReg() && o2->isMem())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        const Mem& src = operand_cast<const Mem&>(*o2);
        _emitX86RM(0x8D, 
          dst.isRegType(REG_GPW), 
          dst.isRegType(REG_GPQ), dst.code(), src);
        return; 
      }

      break;
    }

    case I_M:
    {
      if (o1->isMem())
      {
        _emitX86RM(id.opCode1, 0, (UInt8)id.opCode2, id.opCodeR, operand_cast<const Mem&>(*o1));
        return;
      }
      break;
    }
    
    case I_MOV:
    { 
      const Operand& dst = *o1;
      const Operand& src = *o2;

      switch (dst.op() << 4 | src.op())
      {
        // Reg <- Reg/Mem
        case (OP_REG << 4) | OP_REG:
          ASMJIT_ASSERT(src.isRegType(REG_GPB) || src.isRegType(REG_GPW) ||
                        src.isRegType(REG_GPD) || src.isRegType(REG_GPQ));
          // ... fall through ...
        case (OP_REG << 4) | OP_MEM:
          ASMJIT_ASSERT(dst.isRegType(REG_GPB) || dst.isRegType(REG_GPW) ||
                        dst.isRegType(REG_GPD) || dst.isRegType(REG_GPQ));

          _emitX86RM(0x0000008A + !dst.isRegType(REG_GPB),
            dst.isRegType(REG_GPW),
            dst.isRegType(REG_GPQ),
            operand_cast<const Register&>(dst).code(),
            operand_cast<const BaseRegMem&>(src));
          return;

        // Reg <- Imm
        case (OP_REG << 4) | OP_IMM:
          _emitX86Inl((dst.size() == 1 ? 0xB0 : 0xB8), 
            dst.isRegType(REG_GPW), 
            dst.isRegType(REG_GPQ), 
            operand_cast<const Register&>(dst).code());
          _emitImmediate(operand_cast<const Immediate&>(src), 
            operand_cast<const Register&>(dst).size());
          return;

        // Mem <- Reg
        case (OP_MEM << 4) | OP_REG:
          ASMJIT_ASSERT(src.isRegType(REG_GPB) || src.isRegType(REG_GPW) ||
                        src.isRegType(REG_GPD) || src.isRegType(REG_GPQ));

          _emitX86RM(0x88 + !src.isRegType(REG_GPB),
            src.isRegType(REG_GPW),
            src.isRegType(REG_GPQ),
            operand_cast<const Register&>(src).code(), 
            operand_cast<const BaseRegMem&>(dst));
          return;

        // Mem <- Imm
        case (OP_MEM << 4) | OP_IMM:
          _emitX86RM(0xC6 + (dst.size() != 1),
            dst.size() == 2,
            dst.size() == 8,
            0,
            operand_cast<const BaseRegMem&>(dst));
          _emitImmediate(operand_cast<const Immediate&>(src), 
            dst.size() <= 4 ? dst.size() : 4);
          return;
      }

      ASMJIT_CRASH();
      return; 
    }

    case I_MOV_PTR:
    {
      if ((o1->isReg() && o2->isImm()) || (o1->isImm() && o2->isReg()))
      {
        bool reverse = o1->op() == OP_REG;
        UInt8 opCode = !reverse ? 0xA0 : 0xA2;
        const Register& reg = operand_cast<const Register&>(!reverse ? *o1 : *o2);
        const Immediate& imm = operand_cast<const Immediate&>(!reverse ? *o2 : *o1);

        if (reg.isRegType(REG_GPW)) _emitByte(0x66);
#if defined(ASMJIT_X64)
        _emitRexR(reg.size() == 8, 0, 0);
#endif // ASMJIT_X64
        _emitByte(opCode + (reg.size() != 1));
        _emitImmediate(imm, sizeof(SysInt));
        return; 
      }

      break;
    }

    case I_MOVSX_MOVZX:
    {
      if (o1->isReg() && o2->isRegMem())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o2);

        if (dst.isRegType(REG_GPB)) goto illegalInstruction;
        if (src.size() != 1 && src.size() != 2) goto illegalInstruction;
        if (src.size() == 2 && dst.isRegType(REG_GPW)) goto illegalInstruction;

        _emitX86RM(id.opCode1 + (src.size() != 1),
          dst.isRegType(REG_GPW),
          dst.isRegType(REG_GPQ),
          dst.code(),
          src);
        return; 
      }

      break;
    }
    
#if defined(ASMJIT_X64)
    case I_MOVSXD:
    { 
      if (o1->isReg() && o2->isRegMem())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o2);
        _emitX86RM(0x00000063,
          0,
          1, dst.code(), src);
        return; 
      }

      break;
    }
#endif // ASMJIT_X64

    case I_PUSH:
    {
      // This section is only for immediates, memory/register operands are handled in I_POP.
      if (o1->isImm())
      {
        const Immediate& imm = operand_cast<const Immediate&>(*o1);

        if (isInt8(imm.value()) && imm.relocMode() == RELOC_NONE)
        {
          _emitByte(0x6A);
          _emitImmediate(imm, 1);
        }
        else
        {
          _emitByte(0x68);
          _emitImmediate(imm, 4);
        }
        return;
      }

      // ... goto I_POP ...
    }

    case I_POP:
    {
      if (o1->isReg())
      {
        ASMJIT_ASSERT(o1->isRegType(REG_GPW) || o1->isRegType(REG_GPN));
        _emitX86Inl(id.opCode1, o1->isRegType(REG_GPW), 0, operand_cast<const Register&>(*o1).code());
        return;
      }

      if (o1->isMem())
      {
        _emitX86RM(id.opCode2, o1->size() == 2, 0, id.opCodeR, operand_cast<const BaseRegMem&>(*o1));
        return;
      }

      break;
    }

    case I_R_RM:
    {
      if (o1->isReg() && o2->isRegMem())
      {
        const Register& dst = operand_cast<const Register&>(*o1);
        ASMJIT_ASSERT(dst.type() != REG_GPB);
        const BaseRegMem& src = operand_cast<const BaseRegMem&>(*o2);

        _emitX86RM(id.opCode1,
          dst.type() == REG_GPW, 
          dst.type() == REG_GPQ, dst.code(), src);
        return;
      }

      break;
    }

    case I_RM:
    {
      if (o1->isRegMem())
      {
        const BaseRegMem& op = operand_cast<const BaseRegMem&>(*o1);
        _emitX86RM(id.opCode1 + (op.size() != 1),
          op.size() == 2,
          op.size() == 8, (UInt8)id.opCodeR, op);
        return;
      }

      break;
    }
    
    case I_RM_R:
    {
      if (o1->isRegMem() && o2->isReg())
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);
        const Register& src = operand_cast<const Register&>(*o2);
        _emitX86RM(id.opCode1 + src.type() != REG_GPB,
          src.type() == REG_GPW, 
          src.type() == REG_GPQ, src.code(), dst);
        return;
      }

      break;
    }
    
    case I_RET:
    {
      if (o1->isNone())
      {
        _emitByte(0xC3);
        return;
      }
      else if (o1->isImm())
      {
        const Immediate& imm = operand_cast<const Immediate&>(*o1);
        ASMJIT_ASSERT(isUInt16(imm.value()));

        if (imm.value() == 0 && imm.relocMode() == RELOC_NONE)
        {
          _emitByte(0xC3);
        }
        else
        {
          _emitByte(0xC2);
          _emitImmediate(imm, 2);
        }
        return;
      }

      break;
    }

    case I_ROT:
    {
      if (o1->isRegMem() && (o2->isRegCode(REG_CL) || o2->isImm()))
      {
        // generate opcode. For these operations is base 0xC0 or 0xD0.
        bool useImm8 = (o2->isImm() && 
                       (operand_cast<const Immediate&>(*o2).value() != 1 || 
                        operand_cast<const Immediate&>(*o2).relocMode() != RELOC_NONE));
        UInt32 opCode = useImm8 ? 0xC0 : 0xD0;

        // size and operand type modifies the opcode
        if (o1->size() != 1) opCode |= 0x01;
        if (o2->op() == OP_REG) opCode |= 0x02;

        _emitX86RM(opCode,
          o1->size() == 2,
          o1->size() == 8,
          id.opCodeR, operand_cast<const BaseRegMem&>(*o1));
        if (useImm8) _emitImmediate(operand_cast<const Immediate&>(*o2), 1);
        return;
      }

      break;
    }

    case I_SHLD_SHRD:
    {
      if (o1->isRegMem() && o2->isReg() && (o3->isImm() || (o3->isReg() && o3->isRegCode(REG_CL))))
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);
        const Register& src1 = operand_cast<const Register&>(*o2);
        const Operand& src2 = operand_cast<const Operand&>(*o3);

        ASMJIT_ASSERT(dst.size() == src1.size());

        _emitX86RM(id.opCode1 + src2.isReg(),
          src1.isRegType(REG_GPW),
          src1.isRegType(REG_GPQ),
          src1.code(), dst);
        if (src2.isImm()) 
          _emitImmediate(operand_cast<const Immediate&>(src2), 1);
        return;
      }

      break;
    }

    case I_TEST:
    {
      if (o1->isRegMem() && o2->isReg())
      {
        ASMJIT_ASSERT(o1->size() == o2->size());
        _emitX86RM(0x84 + (o2->size() != 1),
          o2->size() == 2, o2->size() == 8, 
          operand_cast<const BaseReg&>(*o2).code(), 
          operand_cast<const BaseRegMem&>(*o1));
        return;
      }

      if (o1->isRegIndex(0) && o2->isImm())
      {
        if (o1->size() == 2) _emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
        _emitRexRM(o1->size() == 8, 0, operand_cast<const BaseRegMem&>(*o1));
#endif // ASMJIT_X64
        _emitByte(0xA8 + (o1->size() != 1));
        _emitImmediate(operand_cast<const Immediate&>(*o2), o1->size() <= 4 ? o1->size() : 4);
        return;
      }

      if (o1->isRegMem() && o2->isImm())
      {
        if (o1->size() == 2) _emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
        _emitRexRM(o1->size() == 8, 0, operand_cast<const BaseRegMem&>(*o1));
#endif // ASMJIT_X64
        _emitByte(0xF6 + (o1->size() != 1));
        _emitModRM(0, operand_cast<const BaseRegMem&>(*o1));
        _emitImmediate(operand_cast<const Immediate&>(*o2), o1->size() <= 4 ? o1->size() : 4);
        return;
      }

      break;
    }

    case I_XCHG:
    {
      if (o1->isRegMem() && o2->isReg())
      {
        const BaseRegMem& dst = operand_cast<const BaseRegMem&>(*o1);
        const Register& src = operand_cast<const Register&>(*o2);

        if (src.isRegType(REG_GPW)) _emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
        _emitRexRM(src.isRegType(REG_GPQ), src.code(), dst);
#endif // ASMJIT_X64

        // Special opcode for index 0 registers (AX, EAX, RAX vs register)
        if ((dst.op() == OP_REG && dst.size() > 1) && 
            (operand_cast<const Register&>(dst).code() == 0 || 
             operand_cast<const Register&>(src).code() == 0 ))
        {
          int index = operand_cast<const Register&>(dst).code() | src.code();
          _emitByte(0x90 + index);
          return;
        }

        _emitByte(0x86 + src.isRegType(REG_GPB));
        _emitModRM(src.code(), dst);
        return; 
      }

      break;
    }

    case I_MOVBE:
    {
      if (o1->isReg() && o2->isMem())
      {
        _emitX86RM(0x000F38F0,
          o1->isRegType(REG_GPW), 
          o1->isRegType(REG_GPQ), 
          operand_cast<const Register&>(*o1).code(), 
          operand_cast<const Mem&>(*o2));
        return;
      }

      if (o1->isMem() && o2->isReg())
      {
        _emitX86RM(0x000F38F1,
          o2->isRegType(REG_GPW),
          o2->isRegType(REG_GPQ),
          operand_cast<const Register&>(*o2).code(),
          operand_cast<const Mem&>(*o1));
        return;
      }

      break;
    }

    case I_X87_FPU:
    {
      if (o1->isRegType(REG_X87))
      {
        UInt8 i1 = operand_cast<const X87Register&>(*o1).index();
        UInt8 i2 = 0;

        if (code != INST_FCOM && code != INST_FCOMP)
        {
          if (!o2->isRegType(REG_X87)) goto illegalInstruction;
          i2 = operand_cast<const X87Register&>(*o2).index();
        }
        else if (i1 != 0 && i2 != 0)
        {
          goto illegalInstruction;
        }

        _emitByte(i1 == 0
          ? ((id.opCode1 & 0xFF000000) >> 24) 
          : ((id.opCode1 & 0x00FF0000) >> 16));
        _emitByte(i1 == 0 
          ? ((id.opCode1 & 0x0000FF00) >>  8) + i2
          : ((id.opCode1 & 0x000000FF)      ) + i1);
        return;
      }

      if (o1->isMem() && (o1->size() == 4 || o1->size() == 8) && o2->isNone())
      {
        _emitByte(o1->size() == 4 
          ? ((id.opCode1 & 0xFF000000) >> 24) 
          : ((id.opCode1 & 0x00FF0000) >> 16));
        _emitModM(id.opCodeR, operand_cast<const Mem&>(*o1));
        return;
      }

      break;
    }

    case I_X87_STI:
    {
      if (o1->isRegType(REG_X87))
      {
        UInt8 i = operand_cast<const X87Register&>(*o1).index();
        _emitByte((UInt8)((id.opCode1 & 0x0000FF00) >> 8));
        _emitByte((UInt8)((id.opCode1 & 0x000000FF) + i));
        return;
      }
      break;
    }

    case I_X87_FSTSW:
    {
      if (o1->isReg() && 
          operand_cast<const BaseReg&>(*o1).type() <= REG_GPQ && 
          operand_cast<const BaseReg&>(*o1).index())
      {
        _emitOpCode(id.opCode2);
        return;
      }

      if (o1->isMem())
      {
        _emitX86RM(id.opCode1, 0, 0, id.opCodeR, operand_cast<const Mem&>(*o1));
        return;
      }

      break;
    }

    case I_X87_MEM_STI:
    {
      if (o1->isRegType(REG_X87))
      {
        _emitByte((UInt8)((id.opCode2 & 0xFF000000) >> 24));
        _emitByte((UInt8)((id.opCode2 & 0x00FF0000) >> 16) + 
          operand_cast<const X87Register&>(*o1).index());
        return;
      }

      // ... fall through to I_X87_MEM ...
    }

    case I_X87_MEM:
    {
      if (!o1->isMem()) goto illegalInstruction;

      UInt8 opCode = 0x00, mod = 0;

      if (o1->size() == 2 && id.o1Flags & O_FM_2)
      {
        opCode = (UInt8)((id.opCode1 & 0xFF000000) >> 24); 
        mod    = id.opCodeR;
      }
      if (o1->size() == 4 && id.o1Flags & O_FM_4)
      {
        opCode = (UInt8)((id.opCode1 & 0x00FF0000) >> 16); 
        mod    = id.opCodeR;
      }
      if (o1->size() == 8 && id.o1Flags & O_FM_8)
      {
        opCode = (UInt8)((id.opCode1 & 0x0000FF00) >>  8); 
        mod    = (UInt8)((id.opCode1 & 0x000000FF)      );
      }

      if (opCode)
      {
        _emitByte(opCode);
        _emitModM(mod, operand_cast<const Mem&>(*o1));
        return;
      }

      break;
    }

    case I_MMU_MOV:
    {
      ASMJIT_ASSERT(id.o1Flags != 0);
      ASMJIT_ASSERT(id.o2Flags != 0);

      // Check parameters (X)MM|GP32_64 <- (X)MM|GP32_64|Mem|Imm
      if ((o1->isMem()            && (id.o1Flags & O_MEM) == 0) ||
          (o1->isRegType(REG_MM ) && (id.o1Flags & O_MM ) == 0) ||
          (o1->isRegType(REG_XMM) && (id.o1Flags & O_XMM) == 0) ||
          (o1->isRegType(REG_GPD) && (id.o1Flags & O_G32) == 0) ||
          (o1->isRegType(REG_GPQ) && (id.o1Flags & O_G64) == 0) ||
          (o2->isRegType(REG_MM ) && (id.o2Flags & O_MM ) == 0) ||
          (o2->isRegType(REG_XMM) && (id.o2Flags & O_XMM) == 0) ||
          (o2->isRegType(REG_GPD) && (id.o2Flags & O_G32) == 0) ||
          (o2->isRegType(REG_GPQ) && (id.o2Flags & O_G64) == 0) ||
          (o2->isMem()            && (id.o2Flags & O_MEM) == 0) )
      {
        goto illegalInstruction;
      }

      // Illegal
      if (o1->isMem() && o2->isMem()) goto illegalInstruction;

      UInt8 rexw = ((id.o1Flags|id.o2Flags) & O_NOREX) 
        ? 0 
        : o1->isRegType(REG_GPQ) | o1->isRegType(REG_GPQ);

      // (X)MM|Reg <- (X)MM|Reg
      if (o1->isReg() && o2->isReg())
      {
        _emitMmu(id.opCode1, rexw,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const BaseReg&>(*o2)); 
        return;
      }
      
      // (X)MM|Reg <- Mem
      if (o1->isReg() && o2->isMem())
      {
        _emitMmu(id.opCode1, rexw,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const Mem&>(*o2)); 
        return;
      }

      // Mem <- (X)MM|Reg
      if (o1->isMem() && o2->isReg())
      {
        _emitMmu(id.opCode2, rexw,
          operand_cast<const BaseReg&>(*o2).code(),
          operand_cast<const Mem&>(*o1)); 
        return;
      }

      break;
    }

    case I_MMU_MOVD:
    {
      if ((o1->isRegType(REG_MM) || o1->isRegType(REG_XMM)) && (o2->isRegType(REG_GPD) || o2->isMem()))
      {
        _emitMmu(o1->isRegType(REG_XMM) ? 0x66000F6E : 0x00000F6E, 0,
          operand_cast<const BaseReg&>(*o1).code(), 
          operand_cast<const BaseRegMem&>(*o2));
        return;
      }

      if ((o1->isRegType(REG_GPD) || o1->isMem()) && (o2->isRegType(REG_MM) || o2->isRegType(REG_XMM)))
      {
        _emitMmu(o1->isRegType(REG_XMM) ? 0x66000F7E : 0x00000F7E, 0,
          operand_cast<const BaseReg&>(*o2).code(), 
          operand_cast<const BaseRegMem&>(*o1));
        return;
      }

      break;
    }

    case I_MMU_MOVQ:
    {
      if (o1->isRegType(REG_MM) && o2->isRegType(REG_MM))
      {
        _emitMmu(0x00000F6F, 0,
          operand_cast<const MMRegister&>(*o1).code(),
          operand_cast<const MMRegister&>(*o2));
        return;
      }

      if (o1->isRegType(REG_XMM) && o2->isRegType(REG_XMM))
      {
        _emitMmu(0xF3000F7E, 0,
          operand_cast<const XMMRegister&>(*o1).code(),
          operand_cast<const XMMRegister&>(*o2));
        return;
      }

      // Convenience - movdq2q
      if (o1->isRegType(REG_MM) && o2->isRegType(REG_XMM))
      {
        _emitMmu(0xF2000FD6, 0,
          operand_cast<const MMRegister&>(*o1).code(),
          operand_cast<const XMMRegister&>(*o2));
        return;
      }

      // Convenience - movq2dq
      if (o1->isRegType(REG_XMM) && o2->isRegType(REG_MM))
      {
        _emitMmu(0xF3000FD6, 0,
          operand_cast<const XMMRegister&>(*o1).code(),
          operand_cast<const MMRegister&>(*o2));
        return;
      }

      if (o1->isRegType(REG_MM) && o2->isMem())
      {
        _emitMmu(0x00000F6F, 0,
          operand_cast<const MMRegister&>(*o1).code(),
          operand_cast<const Mem&>(*o2));
        return;
      }

      if (o1->isRegType(REG_XMM) && o2->isMem())
      {
        _emitMmu(0xF3000F7E, 0,
          operand_cast<const XMMRegister&>(*o1).code(),
          operand_cast<const Mem&>(*o2));
        return;
      }

      if (o1->isMem() && o2->isRegType(REG_MM))
      {
        _emitMmu(0x00000F7F, 0,
          operand_cast<const MMRegister&>(*o2).code(),
          operand_cast<const Mem&>(*o1));
        return;
      }

      if (o1->isMem() && o2->isRegType(REG_XMM))
      {
        _emitMmu(0x66000FD6, 0,
          operand_cast<const XMMRegister&>(*o2).code(),
          operand_cast<const Mem&>(*o1));
        return;
      }

#if defined(ASMJIT_X64)
      if ((o1->isRegType(REG_MM) || o1->isRegType(REG_XMM)) && (o2->isRegType(REG_GPQ) || o2->isMem()))
      {
        _emitMmu(o1->isRegType(REG_XMM) ? 0x66000F6E : 0x00000F6E, 1,
          operand_cast<const BaseReg&>(*o1).code(), 
          operand_cast<const BaseRegMem&>(*o2));
        return;
      }

      if ((o1->isRegType(REG_GPQ) || o1->isMem()) && (o2->isRegType(REG_MM) || o2->isRegType(REG_XMM)))
      {
        _emitMmu(o2->isRegType(REG_XMM) ? 0x66000F7E : 0x00000F7E, 1,
          operand_cast<const BaseReg&>(*o2).code(), 
          operand_cast<const BaseRegMem&>(*o1));
        return;
      }
#endif // ASMJIT_X64

      break;
    }

    case I_MMU_PREFETCH:
    {
      if (o1->isMem() && o2->isImm())
      {
        const Mem& mem = operand_cast<const Mem&>(*o1);
        const Immediate& hint = operand_cast<const Immediate&>(*o2);

        _emitMmu(0x00000F18, 0, (UInt8)hint.value(), mem);
        return;
      }

      break;
    }

    case I_MMU_PEXTR:
    {
      if (!(o1->isRegMem() && 
           (o2->isRegType(REG_XMM) || (code == INST_PEXTRW && o2->isRegType(REG_MM))) && 
            o3->isImm()))
      {
        goto illegalInstruction;
      }

      UInt32 opCode = id.opCode1;
      UInt8 isGpdGpq = o1->isRegType(REG_GPD) | o1->isRegType(REG_GPQ);

      if (code == INST_PEXTRB && (o1->size() != 0 && o1->size() != 1) && !isGpdGpq) goto illegalInstruction;
      if (code == INST_PEXTRW && (o1->size() != 0 && o1->size() != 2) && !isGpdGpq) goto illegalInstruction;
      if (code == INST_PEXTRD && (o1->size() != 0 && o1->size() != 4) && !isGpdGpq) goto illegalInstruction;
      if (code == INST_PEXTRQ && (o1->size() != 0 && o1->size() != 8) && !isGpdGpq) goto illegalInstruction;

      if (o2->isRegType(REG_XMM)) opCode |= 0x66000000;

      if (o1->isReg())
      {
        _emitMmu(opCode, id.opCodeR | o1->isRegType(REG_GPQ),
          operand_cast<const BaseReg&>(*o2).code(),
          operand_cast<const BaseReg&>(*o1));
        _emitImmediate(
          operand_cast<const Immediate&>(*o3), 1);
        return;
      }

      if (o1->isMem())
      {
        _emitMmu(opCode, id.opCodeR,
          operand_cast<const BaseReg&>(*o2).code(),
          operand_cast<const Mem&>(*o1)); 
        _emitImmediate(
          operand_cast<const Immediate&>(*o3), 1);
        return;
      }

      break;
    }

    case I_MMU_RMI:
    {
      ASMJIT_ASSERT(id.o1Flags != 0);
      ASMJIT_ASSERT(id.o2Flags != 0);

      // Check parameters (X)MM|GP32_64 <- (X)MM|GP32_64|Mem|Imm
      if (!o1->isReg() ||
          (o1->isRegType(REG_MM ) && (id.o1Flags & O_MM ) == 0) ||
          (o1->isRegType(REG_XMM) && (id.o1Flags & O_XMM) == 0) ||
          (o1->isRegType(REG_GPD) && (id.o1Flags & O_G32) == 0) ||
          (o1->isRegType(REG_GPQ) && (id.o1Flags & O_G64) == 0) ||
          (o2->isRegType(REG_MM ) && (id.o2Flags & O_MM ) == 0) ||
          (o2->isRegType(REG_XMM) && (id.o2Flags & O_XMM) == 0) ||
          (o2->isRegType(REG_GPD) && (id.o2Flags & O_G32) == 0) ||
          (o2->isRegType(REG_GPQ) && (id.o2Flags & O_G64) == 0) ||
          (o2->isMem()            && (id.o2Flags & O_MEM) == 0) ||
          (o2->isImm()            && (id.o2Flags & O_IMM) == 0))
      {
        goto illegalInstruction;
      }

      UInt8 prefix = ((id.o1Flags & O_MM_XMM) == O_MM_XMM && o1->isRegType(REG_XMM)) 
        ? 0x00000000 
        : 0x66000000;
      UInt8 rexw = ((id.o1Flags|id.o2Flags) & O_NOREX) 
        ? 0 
        : o1->isRegType(REG_GPQ) | o1->isRegType(REG_GPQ);

      // (X)MM <- (X)MM (opcode1)
      if (o2->isReg())
      {
        if ((id.o2Flags & (O_MM_XMM | O_G32_64)) == 0) goto illegalInstruction;
        _emitMmu(id.opCode1 | prefix, rexw,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const BaseReg&>(*o2)); 
        return;
      }
      // (X)MM <- Mem (opcode1)
      if (o2->isMem())
      {
        if ((id.o2Flags & O_MEM) == 0) goto illegalInstruction;
        _emitMmu(id.opCode1 | prefix, rexw,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const Mem&>(*o2)); 
        return;
      }
      // (X)MM <- Imm (opcode2+opcodeR)
      if (o2->isImm())
      {
        if ((id.o2Flags & O_IMM) == 0) goto illegalInstruction;
        _emitMmu(id.opCode2 | prefix, rexw,
          id.opCodeR,
          operand_cast<const BaseReg&>(*o1));
        _emitImmediate(
          operand_cast<const Immediate&>(*o2), 1);
        return;
      }

      break;
    }

    case I_MMU_RM_IMM8:
    {
      ASMJIT_ASSERT(id.o1Flags != 0);
      ASMJIT_ASSERT(id.o2Flags != 0);

      // Check parameters (X)MM|GP32_64 <- (X)MM|GP32_64|Mem|Imm
      if (!o1->isReg() ||
          (o1->isRegType(REG_MM ) && (id.o1Flags & O_MM ) == 0) ||
          (o1->isRegType(REG_XMM) && (id.o1Flags & O_XMM) == 0) ||
          (o1->isRegType(REG_GPD) && (id.o1Flags & O_G32) == 0) ||
          (o1->isRegType(REG_GPQ) && (id.o1Flags & O_G64) == 0) ||
          (o2->isRegType(REG_MM ) && (id.o2Flags & O_MM ) == 0) ||
          (o2->isRegType(REG_XMM) && (id.o2Flags & O_XMM) == 0) ||
          (o2->isRegType(REG_GPD) && (id.o2Flags & O_G32) == 0) ||
          (o2->isRegType(REG_GPQ) && (id.o2Flags & O_G64) == 0) ||
          (o2->isMem()            && (id.o2Flags & O_MEM) == 0) ||
          !o3->isImm())
      {
        goto illegalInstruction;
      }

      UInt8 prefix = ((id.o1Flags & O_MM_XMM) == O_MM_XMM && o1->isRegType(REG_XMM)) 
        ? 0x00000000 
        : 0x66000000;
      UInt8 rexw = ((id.o1Flags|id.o2Flags) & O_NOREX) 
        ? 0 
        : o1->isRegType(REG_GPQ) | o1->isRegType(REG_GPQ);

      // (X)MM <- (X)MM (opcode1)
      if (o2->isReg())
      {
        if ((id.o2Flags & (O_MM_XMM | O_G32_64)) == 0) goto illegalInstruction;
        _emitMmu(id.opCode1 | prefix, rexw,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const BaseReg&>(*o2)); 
        _emitImmediate(operand_cast<const Immediate &>(*o3), 1);
        return; 
      }
      // (X)MM <- Mem (opcode1)
      if (o2->isMem())
      {
        if ((id.o2Flags & O_MEM) == 0) goto illegalInstruction;
        _emitMmu(id.opCode1 | prefix, rexw,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const Mem&>(*o2)); 
        _emitImmediate(operand_cast<const Immediate &>(*o3), 1);
        return;
      }

      break;
    }

    case I_MMU_RM_3DNOW:
    {
      if (o1->isRegType(REG_MM) && (o2->isRegType(REG_MM) || o2->isMem()))
      {
        _emitMmu(id.opCode1, 0,
          operand_cast<const BaseReg&>(*o1).code(),
          operand_cast<const Mem&>(*o2)); 
        _emitByte((UInt8)id.opCode2);
        return;
      }

      break;
    }
  }

illegalInstruction:
  // It's illegal to be here
  ASMJIT_CRASH();
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Align]
// ----------------------------------------------------------------------------

void Assembler::align(SysInt m)
{
  if (!ensureSpace()) return;
  if (!m) return;

  ASMJIT_ASSERT(m == 1 || m == 2 || m == 4 || m == 8 || m == 16 || m == 32);
  while ((offset() & (m - 1)) != 0) _emitByte(0x90);
}

// ----------------------------------------------------------------------------
// [AsmJit::Assembler - Bind]
// ----------------------------------------------------------------------------

void Assembler::bind(Label* label)
{
  // label can only be bound once
  ASMJIT_ASSERT(!label->isBound());
  bindTo(label, offset());
}

void Assembler::bindTo(Label* label, SysInt pos)
{
  if (label->isLinked())
  {
    Int32 _pos = (Int32)pos;
    Int32 prev = (Int32)label->position();

    for (;;)
    {
      Int32 chain = getInt32At(prev);
      setInt32At(prev, _pos - (prev + 4));
      if (chain == 0) break;
      prev -= chain;
    }
  }

  label->set(LABEL_BOUND, pos);
}

} // AsmJit namespace
