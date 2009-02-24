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

// [Guard]
#ifndef _ASMJITDEFSX86X64_H
#define _ASMJITDEFSX86X64_H

// [Dependencies]
#include "AsmJitConfig.h"
#include "AsmJitUtil.h"

#include <stdlib.h>
#include <string.h>

namespace AsmJit {

// ============================================================================
// [Constants]
// ============================================================================

//! @brief Operand types that can be encoded in @c Op operand.
enum OP
{
  //! @brief Operand is none, used only internally.
  OP_NONE = 0,
  //! @brief Operand is register.
  OP_REG = 1,
  //! @brief Operand is memory.
  OP_MEM = 2,
  //! @brief Operand is immediate.
  OP_IMM = 3,
  //! @brief Operand is label.
  OP_LABEL = 4,
};

//! @brief Size of registers and pointers.
enum SIZE
{
  //! @brief 1 byte size.
  SIZE_BYTE   = 1,
  //! @brief 2 bytes size.
  SIZE_WORD   = 2,
  //! @brief 4 bytes size.
  SIZE_DWORD  = 4,
  //! @brief 8 bytes size.
  SIZE_QWORD  = 8,
  //! @brief 10 bytes size.
  SIZE_TWORD  = 10,
  //! @brief 16 bytes size.
  SIZE_DQWORD = 16
};

//! @brief Valid X86 register IDs.
//!
//! These codes are real, don't miss with @c REG enum! and don't use these
//! values if you are not writing @c AsmJit::Serializer backend.
enum RID
{
  RID_EAX = 0,
  RID_ECX = 1,
  RID_EDX = 2,
  RID_EBX = 3,
  RID_ESP = 4,
  RID_EBP = 5,
  RID_ESI = 6,
  RID_EDI = 7,
  // to check if register is invalid, use reg >= R_INVALID
  RID_INVALID = 8
};

//! @brief Pseudo (not real X86) register codes used for generating opcodes.
//!
//! From this register code can be generated real x86 register ID, type of
//! register and size of register.
enum REG
{
  //! @brief Mask for register type.
  REGTYPE_MASK = 0xF0,
  //! @brief Mask for register code (index).
  REGCODE_MASK = 0x0F,

  // First nibble contains register type (mask 0xF0), Second nibble contains
  // register index code.

  // 8 bit, 16 bit and 32 bit general purpose registers
  REG_GPB = 0x00,
  REG_GPW = 0x10,
  REG_GPD = 0x20,

  // 64 bit registers (RAX, RBX, ...), not available in 32 bit mode
  REG_GPQ = 0x30,

  // native 32 bit or 64 bit registers
#if defined(ASMJIT_X86)
  REG_GPN = REG_GPD,
#else
  REG_GPN = REG_GPQ,
#endif

  // X87 (FPU) registers
  REG_X87 = 0x50,

  // 64 bit mmx registers
  REG_MM = 0x60,
  // 128 bit sse registers
  REG_XMM = 0x70,

  // 8/16 bit registers
  REG_AL   = REG_GPB , REG_CL   , REG_DL   , REG_BL   , REG_AH   , REG_CH   , REG_DH   , REG_BH   ,
#if defined(ASMJIT_X64)
  REG_R8B            , REG_R9B  , REG_R10B , REG_R11B , REG_R12B , REG_R13B , REG_R14B , REG_R15B ,
#endif // ASMJIT_X64
  REG_AX   = REG_GPW , REG_CX   , REG_DX   , REG_BX   , REG_SP   , REG_BP   , REG_SI   , REG_DI   ,
#if defined(ASMJIT_X64)
  REG_R8W            , REG_R9W  , REG_R10W , REG_R11W , REG_R12W , REG_R13W , REG_R14W , REG_R15W ,
#endif // ASMJIT_X64

  // 32 bit registers
  REG_EAX  = REG_GPD , REG_ECX  , REG_EDX  , REG_EBX  , REG_ESP  , REG_EBP  , REG_ESI  , REG_EDI  ,
#if defined(ASMJIT_X64)
  REG_R8D            , REG_R9D  , REG_R10D , REG_R11D , REG_R12D , REG_R13D , REG_R14D , REG_R15D ,
#endif // ASMJIT_X64

  // 64 bit registers
#if defined(ASMJIT_X64)
  REG_RAX  = REG_GPQ , REG_RCX  , REG_RDX  , REG_RBX  , REG_RSP  , REG_RBP  , REG_RSI  , REG_RDI  ,
  REG_R8             , REG_R9   , REG_R10  , REG_R11  , REG_R12  , REG_R13  , REG_R14  , REG_R15  ,
#endif // ASMJIT_X64

  // MMX registers 
  REG_MM0  = REG_MM , REG_MM1  , REG_MM2  , REG_MM3  , REG_MM4  , REG_MM5  , REG_MM6  , REG_MM7  ,

  // SSE registers
  REG_XMM0 = REG_XMM , REG_XMM1 , REG_XMM2 , REG_XMM3 , REG_XMM4 , REG_XMM5 , REG_XMM6 , REG_XMM7 ,
#if defined(ASMJIT_X64)
  REG_XMM8           , REG_XMM9 , REG_XMM10, REG_XMM11, REG_XMM12, REG_XMM13, REG_XMM14, REG_XMM15,
#endif // ASMJIT_X64

  // native registers (depends if processor runs in 32 bit or 64 bit mode)
#if defined(ASMJIT_X86)
  REG_NAX  = REG_GPD , REG_NCX  , REG_NDX  , REG_NBX  , REG_NSP  , REG_NBP  , REG_NSI  , REG_NDI  ,
#else
  REG_NAX  = REG_GPQ , REG_NCX  , REG_NDX  , REG_NBX  , REG_NSP  , REG_NBP  , REG_NSI  , REG_NDI  ,
#endif

  //! @brief Invalid register code.
  NO_REG = 0xFF
};

//! @brief Prefetch hints.
enum PREFETCH_HINT
{
  PREFETCH_T0  = 1,
  PREFETCH_T1  = 2,
  PREFETCH_T2  = 3,
  PREFETCH_NTA = 0
};

//! @brief Condition codes.
enum CONDITION
{
  //! @brief No condition code.
  C_NO_CONDITION  = -1,

  // Condition codes from processor manuals.
  C_A             = 0x7,
  C_AE            = 0x3,
  C_B             = 0x2,
  C_BE            = 0x6,
  C_C             = 0x2,
  C_E             = 0x4,
  C_G             = 0xF,
  C_GE            = 0xD,
  C_L             = 0xC,
  C_LE            = 0xE,
  C_NA            = 0x6,
  C_NAE           = 0x2,
  C_NB            = 0x3,
  C_NBE           = 0x7,
  C_NC            = 0x3,
  C_NE            = 0x5,
  C_NG            = 0xE,
  C_NGE           = 0xC,
  C_NL            = 0xD,
  C_NLE           = 0xF,
  C_NO            = 0x1,
  C_NP            = 0xB,
  C_NS            = 0x9,
  C_NZ            = 0x5,
  C_O             = 0x0,
  C_P             = 0xA,
  C_PE            = 0xA,
  C_PO            = 0xB,
  C_S             = 0x8,
  C_Z             = 0x4,

  // Simplified condition codes
  C_OVERFLOW      = 0x0,
  C_NO_OVERFLOW   = 0x1,
  C_BELOW         = 0x2,
  C_ABOVE_EQUAL   = 0x3,
  C_EQUAL         = 0x4,
  C_NOT_EQUAL     = 0x5,
  C_BELOW_EQUAL   = 0x6,
  C_ABOVE         = 0x7,
  C_SIGN          = 0x8,
  C_NOT_SIGN      = 0x9,
  C_PARITY_EVEN   = 0xA,
  C_PARITY_ODD    = 0xB,
  C_LESS          = 0xC,
  C_GREATER_EQUAL = 0xD,
  C_LESS_EQUAL    = 0xE,
  C_GREATER       = 0xF,

  // aliases
  C_ZERO          = 0x4,
  C_NOT_ZERO      = 0x5,
  C_NEGATIVE      = 0x8,
  C_POSITIVE      = 0x9,

  // x87 floating point only
  C_FP_UNORDERED  = 16,
  C_FP_NOT_UNORDERED = 17
};

//! @brief  Returns the equivalent of !cc.
//!
//! Negation of the default no_condition (-1) results in a non-default
//! no_condition value (-2). As long as tests for no_condition check
//! for condition < 0, this will work as expected.
static inline CONDITION negateCondition(CONDITION cc)
{
  return static_cast<CONDITION>(cc ^ 1);
}

//! @brief Corresponds to transposing the operands of a comparison.
static inline CONDITION reverseCondition(CONDITION cc)
{
  switch (cc) {
    case C_BELOW:
      return C_ABOVE;
    case C_ABOVE:
      return C_BELOW;
    case C_ABOVE_EQUAL:
      return C_BELOW_EQUAL;
    case C_BELOW_EQUAL:
      return C_ABOVE_EQUAL;
    case C_LESS:
      return C_GREATER;
    case C_GREATER:
      return C_LESS;
    case C_GREATER_EQUAL:
      return C_LESS_EQUAL;
    case C_LESS_EQUAL:
      return C_GREATER_EQUAL;
    default:
      return cc;
  };
}

//! @brief Scale, can be used for addressing.
//!
//! See @c Op and addressing methods like @c byte_ptr(), @c word_ptr(),
//! @c dword_ptr(), etc...
enum SCALE
{
  //! @brief Scale 1 times (no scale).
  TIMES_1 = 0,
  //! @brief Scale 2 times (same as shifting to left by 1).
  TIMES_2 = 1,
  //! @brief Scale 4 times (same as shifting to left by 2).
  TIMES_4 = 2,
  //! @brief Scale 8 times (same as shifting to left by 3).
  TIMES_8 = 3
};

//! @brief Condition hint.
enum HINT
{
  //! @brief No hint.
  HINT_NONE = 0,
  //! @brief Condition will be taken (likely).
  HINT_TAKEN = 0x3E,
  //! @brief Condition will be not taken (unlikely).
  HINT_NOT_TAKEN = 0x2E
};

//! @brief Floating point status.
enum FP_STATUS
{
  FP_C0 = 0x100,
  FP_C1 = 0x200,
  FP_C2 = 0x400,
  FP_C3 = 0x4000,
  FP_CC_MASK = 0x4500
};

//! @brief Floating point control word.
enum FP_CW
{
  FP_CW_INVOPEX_MASK  = 0x001,
  FP_CW_DENOPEX_MASK  = 0x002,
  FP_CW_ZERODIV_MASK  = 0x004,
  FP_CW_OVFEX_MASK    = 0x008,
  FP_CW_UNDFEX_MASK   = 0x010,
  FP_CW_PRECEX_MASK   = 0x020,
  FP_CW_PRECC_MASK    = 0x300,
  FP_CW_ROUNDC_MASK   = 0xC00,

  // values for precision control
  FP_CW_PREC_SINGLE   = 0x000,
  FP_CW_PREC_DOUBLE   = 0x200,
  FP_CW_PREC_EXTENDED = 0x300,

  // values for rounding control
  FP_CW_ROUND_NEAREST = 0x000,
  FP_CW_ROUND_DOWN    = 0x400,
  FP_CW_ROUND_UP      = 0x800,
  FP_CW_ROUND_TOZERO  = 0xC00
};

//! @brief Relocation info.
enum RELOC_MODE
{
  //! @brief No relocation.
  RELOC_NONE = 0,

  //! @brief Overwrite relocation (immediates as constants).
  RELOC_OVERWRITE = 1,

  //! @brief Internal, used by @c AsmJit::Assembler::jmp_rel() and 
  //! @c AsmJit::Assembler::j_rel()
  RELOC_JMP_RELATIVE = 10
};

enum LABEL_STATE
{
  LABEL_UNUSED = 0,
  LABEL_LINKED = 1,
  LABEL_BOUND = 2
};

// ============================================================================
// [AsmJit::INST_CODE]
// ============================================================================

//! @brief Instruction codes (AsmJit specific)
enum INST_CODE
{
  INST_ADC,           // X86/X64
  INST_ADD,           // X86/X64
  INST_ADDPD,
  INST_ADDPS,
  INST_ADDSD,
  INST_ADDSS,
  INST_ADDSUBPD,
  INST_ADDSUBPS,
  INST_AMD_PREFETCH,
  INST_AMD_PREFETCHW,
  INST_AND,           // X86/X64
  INST_ANDNPD,
  INST_ANDNPS,
  INST_ANDPD,
  INST_ANDPS,
  INST_BLENDPD,
  INST_BLENDPS,
  INST_BLENDVPD,
  INST_BLENDVPS,
  INST_BSF,           // X86/X64
  INST_BSR,           // X86/X64
  INST_BSWAP,         // X86/X64 (i486)
  INST_BT,            // X86/X64
  INST_BTC,           // X86/X64
  INST_BTR,           // X86/X64
  INST_BTS,           // X86/X64
  INST_CALL,          // X86/X64
  INST_CBW,           // X86/X64
  INST_CDQE,          // X64 only
  INST_CLC,           // X86/X64
  INST_CLD,           // X86/X64
  INST_CLFLUSH,
  INST_CMC,           // X86/X64
  INST_CMOV,          // Begin (cmovcc) (i586)
  INST_CMOVA=INST_CMOV,//X86/X64 (cmovcc) (i586)
  INST_CMOVAE,        // X86/X64 (cmovcc) (i586)
  INST_CMOVB,         // X86/X64 (cmovcc) (i586)
  INST_CMOVBE,        // X86/X64 (cmovcc) (i586)
  INST_CMOVC,         // X86/X64 (cmovcc) (i586)
  INST_CMOVE,         // X86/X64 (cmovcc) (i586)
  INST_CMOVG,         // X86/X64 (cmovcc) (i586)
  INST_CMOVGE,        // X86/X64 (cmovcc) (i586)
  INST_CMOVL,         // X86/X64 (cmovcc) (i586)
  INST_CMOVLE,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNA,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNAE,       // X86/X64 (cmovcc) (i586)
  INST_CMOVNB,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNBE,       // X86/X64 (cmovcc) (i586)
  INST_CMOVNC,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNE,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNG,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNGE,       // X86/X64 (cmovcc) (i586)
  INST_CMOVNL,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNLE,       // X86/X64 (cmovcc) (i586)
  INST_CMOVNO,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNP,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNS,        // X86/X64 (cmovcc) (i586)
  INST_CMOVNZ,        // X86/X64 (cmovcc) (i586)
  INST_CMOVO,         // X86/X64 (cmovcc) (i586)
  INST_CMOVP,         // X86/X64 (cmovcc) (i586)
  INST_CMOVPE,        // X86/X64 (cmovcc) (i586)
  INST_CMOVPO,        // X86/X64 (cmovcc) (i586)
  INST_CMOVS,         // X86/X64 (cmovcc) (i586)
  INST_CMOVZ,         // X86/X64 (cmovcc) (i586)
  INST_CMP,           // X86/X64
  INST_CMPPD,
  INST_CMPPS,
  INST_CMPSD,
  INST_CMPSS,
  INST_CMPXCHG,       // X86/X64 (i486)
  INST_CMPXCHG16B,    // X64 only
  INST_CMPXCHG8B,     // X86/X64 (i586)
  INST_COMISD,
  INST_COMISS,
  INST_CPUID,         // X86/X64 (i486)
  INST_CRC32,
  INST_CVTDQ2PD,
  INST_CVTDQ2PS,
  INST_CVTPD2DQ,
  INST_CVTPD2PI,
  INST_CVTPD2PS,
  INST_CVTPI2PD,
  INST_CVTPI2PS,
  INST_CVTPS2DQ,
  INST_CVTPS2PD,
  INST_CVTPS2PI,
  INST_CVTSD2SI,
  INST_CVTSD2SS,
  INST_CVTSI2SD,
  INST_CVTSI2SS,
  INST_CVTSS2SD,
  INST_CVTSS2SI,
  INST_CVTTPD2DQ,
  INST_CVTTPD2PI,
  INST_CVTTPS2DQ,
  INST_CVTTPS2PI,
  INST_CVTTSD2SI,
  INST_CVTTSS2SI,
  INST_CWDE,          // X86/X64
  INST_DAA,           // X86 only
  INST_DAS,           // X86 only
  INST_DEC,           // X86/X64
  INST_DIV,           // X86/X64
  INST_DIVPD,
  INST_DIVPS,
  INST_DIVSD,
  INST_DIVSS,
  INST_DPPD,
  INST_DPPS,
  INST_EMMS,
  INST_EXTRACTPS,
  INST_F2XM1,         // X87
  INST_FABS,          // X87
  INST_FADD,          // X87
  INST_FADDP,         // X87
  INST_FBLD,          // X87
  INST_FBSTP,         // X87
  INST_FCHS,          // X87
  INST_FCLEX,         // X87
  INST_FCMOVB,        // X87
  INST_FCMOVBE,       // X87
  INST_FCMOVE,        // X87
  INST_FCMOVNB,       // X87
  INST_FCMOVNBE,      // X87
  INST_FCMOVNE,       // X87
  INST_FCMOVNU,       // X87
  INST_FCMOVU,        // X87
  INST_FCOM,          // X87
  INST_FCOMI,         // X87
  INST_FCOMIP,        // X87
  INST_FCOMP,         // X87
  INST_FCOMPP,        // X87
  INST_FCOS,          // X87
  INST_FDECSTP,       // X87
  INST_FDIV,          // X87
  INST_FDIVP,         // X87
  INST_FDIVR,         // X87
  INST_FDIVRP,        // X87
  INST_FEMMS,         // 3dNow!
  INST_FFREE,         // X87
  INST_FIADD,         // X87
  INST_FICOM,         // X87
  INST_FICOMP,        // X87
  INST_FIDIV,         // X87
  INST_FIDIVR,        // X87
  INST_FILD,          // X87
  INST_FIMUL,         // X87
  INST_FINCSTP,       // X87
  INST_FINIT,         // X87
  INST_FIST,          // X87
  INST_FISTP,         // X87
  INST_FISTTP,
  INST_FISUB,         // X87
  INST_FISUBR,        // X87
  INST_FLD,           // X87
  INST_FLD1,          // X87
  INST_FLDCW,         // X87
  INST_FLDENV,        // X87
  INST_FLDL2E,        // X87
  INST_FLDL2T,        // X87
  INST_FLDLG2,        // X87
  INST_FLDLN2,        // X87
  INST_FLDPI,         // X87
  INST_FLDZ,          // X87
  INST_FMUL,          // X87
  INST_FMULP,         // X87
  INST_FNCLEX,        // X87
  INST_FNINIT,        // X87
  INST_FNOP,          // X87
  INST_FNSAVE,        // X87
  INST_FNSTCW,        // X87
  INST_FNSTENV,       // X87
  INST_FNSTSW,        // X87
  INST_FPATAN,        // X87
  INST_FPREM,         // X87
  INST_FPREM1,        // X87
  INST_FPTAN,         // X87
  INST_FRNDINT,       // X87
  INST_FRSTOR,        // X87
  INST_FSAVE,         // X87
  INST_FSCALE,        // X87
  INST_FSIN,          // X87
  INST_FSINCOS,       // X87
  INST_FSQRT,         // X87
  INST_FST,           // X87
  INST_FSTCW,         // X87
  INST_FSTENV,        // X87
  INST_FSTP,          // X87
  INST_FSTSW,         // X87
  INST_FSUB,          // X87
  INST_FSUBP,         // X87
  INST_FSUBR,         // X87
  INST_FSUBRP,        // X87
  INST_FTST,          // X87
  INST_FUCOM,         // X87
  INST_FUCOMI,        // X87
  INST_FUCOMIP,       // X87
  INST_FUCOMP,        // X87
  INST_FUCOMPP,       // X87
  INST_FWAIT,         // X87
  INST_FXAM,          // X87
  INST_FXCH,          // X87
  INST_FXRSTOR,       // X87
  INST_FXSAVE,        // X87
  INST_FXTRACT,       // X87
  INST_FYL2X,         // X87
  INST_FYL2XP1,       // X87
  INST_HADDPD,
  INST_HADDPS,
  INST_HSUBPD,
  INST_HSUBPS,
  INST_IDIV,          // X86/X64
  INST_IMUL,          // X86/X64
  INST_INC,           // X86/X64
  INST_INT3,          // X86/X64
  INST_J,             // Begin (jcc)
  INST_JA = INST_J,   // X86/X64 (jcc)
  INST_JAE,           // X86/X64 (jcc)
  INST_JB,            // X86/X64 (jcc)
  INST_JBE,           // X86/X64 (jcc)
  INST_JC,            // X86/X64 (jcc)
  INST_JE,            // X86/X64 (jcc)
  INST_JG,            // X86/X64 (jcc)
  INST_JGE,           // X86/X64 (jcc)
  INST_JL,            // X86/X64 (jcc)
  INST_JLE,           // X86/X64 (jcc)
  INST_JNA,           // X86/X64 (jcc)
  INST_JNAE,          // X86/X64 (jcc)
  INST_JNB,           // X86/X64 (jcc)
  INST_JNBE,          // X86/X64 (jcc)
  INST_JNC,           // X86/X64 (jcc)
  INST_JNE,           // X86/X64 (jcc)
  INST_JNG,           // X86/X64 (jcc)
  INST_JNGE,          // X86/X64 (jcc)
  INST_JNL,           // X86/X64 (jcc)
  INST_JNLE,          // X86/X64 (jcc)
  INST_JNO,           // X86/X64 (jcc)
  INST_JNP,           // X86/X64 (jcc)
  INST_JNS,           // X86/X64 (jcc)
  INST_JNZ,           // X86/X64 (jcc)
  INST_JO,            // X86/X64 (jcc)
  INST_JP,            // X86/X64 (jcc)
  INST_JPE,           // X86/X64 (jcc)
  INST_JPO,           // X86/X64 (jcc)
  INST_JS,            // X86/X64 (jcc)
  INST_JZ,            // X86/X64 (jcc)
  INST_JMP,           // X86/X64 (jmp)
  INST_JMP_PTR,       // X86/X64 (jmp)
  INST_LDDQU,
  INST_LDMXCSR,
  INST_LEA,           // X86/X64
  INST_LFENCE,
  INST_LOCK,          // X86/X64
  INST_MASKMOVDQU,
  INST_MASKMOVQ,      // MMX Extensions
  INST_MAXPD,
  INST_MAXPS,
  INST_MAXSD,
  INST_MAXSS,
  INST_MFENCE,
  INST_MINPD,
  INST_MINPS,
  INST_MINSD,
  INST_MINSS,
  INST_MONITOR,
  INST_MOV,           // X86/X64
  INST_MOVAPD,
  INST_MOVAPS,
  INST_MOVBE,
  INST_MOVD,
  INST_MOVDDUP,
  INST_MOVDQ2Q,
  INST_MOVDQA,
  INST_MOVDQU,
  INST_MOVHLPS,
  INST_MOVHPD,
  INST_MOVHPS,
  INST_MOVLHPS,
  INST_MOVLPD,
  INST_MOVLPS,
  INST_MOVMSKPD,
  INST_MOVMSKPS,
  INST_MOVNTDQ,
  INST_MOVNTDQA,
  INST_MOVNTI,
  INST_MOVNTPD,
  INST_MOVNTPS,
  INST_MOVNTQ,        // MMX Extensions
  INST_MOVQ,
  INST_MOVQ2DQ,
  INST_MOVSD,
  INST_MOVSHDUP,
  INST_MOVSLDUP,
  INST_MOVSS,
  INST_MOVSX,         // X86/X64
  INST_MOVSXD,        // X86/X64
  INST_MOVUPD,
  INST_MOVUPS,
  INST_MOVZX,         // X86/X64
  INST_MOV_PTR,       // X86/X64
  INST_MPSADBW,
  INST_MUL,           // X86/X64
  INST_MULPD,
  INST_MULPS,
  INST_MULSD,
  INST_MULSS,
  INST_MWAIT,
  INST_NEG,           // X86/X64
  INST_NOP,           // X86/X64
  INST_NOT,           // X86/X64
  INST_OR,            // X86/X64
  INST_ORPD,
  INST_ORPS,
  INST_PABSB,
  INST_PABSD,
  INST_PABSW,
  INST_PACKSSDW,
  INST_PACKSSWB,
  INST_PACKUSDW,
  INST_PACKUSWB,
  INST_PADDB,
  INST_PADDD,
  INST_PADDQ,
  INST_PADDSB,
  INST_PADDSW,
  INST_PADDUSB,
  INST_PADDUSW,
  INST_PADDW,
  INST_PALIGNR,
  INST_PAND,
  INST_PANDN,
  INST_PAUSE,
  INST_PAVGB,         // MMX Extensions
  INST_PAVGW,         // MMX Extensions
  INST_PBLENDVB,
  INST_PBLENDW,
  INST_PCMPEQB,
  INST_PCMPEQD,
  INST_PCMPEQQ,
  INST_PCMPEQW,
  INST_PCMPESTRI,
  INST_PCMPESTRM,
  INST_PCMPGTB,
  INST_PCMPGTD,
  INST_PCMPGTQ,
  INST_PCMPGTW,
  INST_PCMPISTRI,
  INST_PCMPISTRM,
  INST_PEXTRB,
  INST_PEXTRD,
  INST_PEXTRQ,
  INST_PEXTRW,        // MMX Extensions
  INST_PF2ID,         // 3dNow!
  INST_PF2IW,         // 3dNow! Extensions
  INST_PFACC,         // 3dNow!
  INST_PFADD,         // 3dNow!
  INST_PFCMPEQ,       // 3dNow!
  INST_PFCMPGE,       // 3dNow!
  INST_PFCMPGT,       // 3dNow!
  INST_PFMAX,         // 3dNow!
  INST_PFMIN,         // 3dNow!
  INST_PFMUL,         // 3dNow!
  INST_PFNACC,        // 3dNow! Extensions
  INST_PFPNACC,       // 3dNow! Extensions
  INST_PFRCP,         // 3dNow!
  INST_PFRCPIT1,      // 3dNow!
  INST_PFRCPIT2,      // 3dNow!
  INST_PFRSQIT1,      // 3dNow!
  INST_PFRSQRT,       // 3dNow!
  INST_PFSUB,         // 3dNow!
  INST_PFSUBR,        // 3dNow!
  INST_PHADDD,
  INST_PHADDSW,
  INST_PHADDW,
  INST_PHMINPOSUW,
  INST_PHSUBD,
  INST_PHSUBSW,
  INST_PHSUBW,
  INST_PI2FD,         // 3dNow!
  INST_PI2FW,         // 3dNow! Extensions
  INST_PINSRB,
  INST_PINSRD,
  INST_PINSRW,        // MMX Extensions
  INST_PMADDUBSW,
  INST_PMADDWD,
  INST_PMAXSB,
  INST_PMAXSD,
  INST_PMAXSW,        // MMX Extensions
  INST_PMAXUB,        // MMX Extensions
  INST_PMAXUD,
  INST_PMAXUW,
  INST_PMINSB,
  INST_PMINSD,
  INST_PMINSW,        // MMX Extensions
  INST_PMINUB,        // MMX Extensions
  INST_PMINUD,
  INST_PMINUW,
  INST_PMOVMSKB,      // MMX Extensions
  INST_PMOVSXBD,
  INST_PMOVSXBQ,
  INST_PMOVSXBW,
  INST_PMOVSXDQ,
  INST_PMOVSXWD,
  INST_PMOVSXWQ,
  INST_PMOVZXBD,
  INST_PMOVZXBQ,
  INST_PMOVZXBW,
  INST_PMOVZXDQ,
  INST_PMOVZXWD,
  INST_PMOVZXWQ,
  INST_PMULDQ,
  INST_PMULHRSW,
  INST_PMULHUW,       // MMX Extensions
  INST_PMULHW,
  INST_PMULLD,
  INST_PMULLW,
  INST_PMULUDQ,
  INST_POP,           // X86/X64
  INST_POPAD,         // X86 only
  INST_POPCNT,
  INST_POPFD,         // X86 only
  INST_POPFQ,         // X64 only
  INST_POR,
  INST_PREFETCH,      // MMX Extensions
  INST_PSADBW,        // MMX Extensions
  INST_PSHUFB,
  INST_PSHUFD,
  INST_PSHUFW,        // MMX Extensions
  INST_PSHUHW,
  INST_PSHULW,
  INST_PSIGNB,
  INST_PSIGND,
  INST_PSIGNW,
  INST_PSLLD,
  INST_PSLLDQ,
  INST_PSLLQ,
  INST_PSLLW,
  INST_PSRAD,
  INST_PSRAW,
  INST_PSRLD,
  INST_PSRLDQ,
  INST_PSRLQ,
  INST_PSRLW,
  INST_PSUBB,
  INST_PSUBD,
  INST_PSUBQ,
  INST_PSUBSB,
  INST_PSUBSW,
  INST_PSUBUSB,
  INST_PSUBUSW,
  INST_PSUBW,
  INST_PSWAPD,        // 3dNow! Extensions
  INST_PTEST,
  INST_PUNPCKHBW,
  INST_PUNPCKHDQ,
  INST_PUNPCKHQDQ,
  INST_PUNPCKHWD,
  INST_PUNPCKLBW,
  INST_PUNPCKLDQ,
  INST_PUNPCKLQDQ,
  INST_PUNPCKLWD,
  INST_PUSH,          // X86/X64
  INST_PUSHAD,        // X86 only
  INST_PUSHFD,        // X86 only
  INST_PUSHFQ,        // X64 only
  INST_PXOR,
  INST_RCL,           // X86/X64
  INST_RCPPS,
  INST_RCPSS,
  INST_RCR,           // X86/X64
  INST_RDTSC,         // X86/X64
  INST_RDTSCP,        // X86/X64
  INST_RET,           // X86/X64
  INST_ROL,           // X86/X64
  INST_ROR,           // X86/X64
  INST_ROUNDPD,
  INST_ROUNDPS,
  INST_ROUNDSD,
  INST_ROUNDSS,
  INST_RSQRTPS,
  INST_RSQRTSS,
  INST_SAHF,          // X86 only
  INST_SAL,           // X86/X64
  INST_SAR,           // X86/X64
  INST_SBB,           // X86/X64
  INST_SFENCE,        // MMX Extensions
  INST_SHL,           // X86/X64
  INST_SHLD,          // X86/X64
  INST_SHR,           // X86/X64
  INST_SHRD,          // X86/X64
  INST_SHUFPS,
  INST_SQRTPD,
  INST_SQRTPS,
  INST_SQRTSD,
  INST_SQRTSS,
  INST_STC,           // X86/X64
  INST_STD,           // X86/X64
  INST_STMXCSR,
  INST_SUB,           // X86/X64
  INST_SUBPD,
  INST_SUBPS,
  INST_SUBSD,
  INST_SUBSS,
  INST_TEST,          // X86/X64
  INST_UCOMISD,
  INST_UCOMISS,
  INST_UD2,           // X86/X64
  INST_UNPCKHPD,
  INST_UNPCKHPS,
  INST_UNPCKLPD,
  INST_UNPCKLPS,
  INST_XADD,          // X86/X64 (i486)
  INST_XCHG,          // X86/X64 (i386)
  INST_XOR,           // X86/X64
  INST_XORPD,
  INST_XORPS,

  _INST_COUNT
};

// ============================================================================
// [AsmJit::RelocInfo]
// ============================================================================

//! @brief Structure used internally for relocations.
struct RelocInfo
{
  SysUInt offset;
  UInt8 size;
  UInt8 mode;
  UInt16 data;
};

// ============================================================================
// [AsmJit::Operand]
// ============================================================================

struct _DontInitialize {};
struct _Initialize {};

//! @brief Operand, abstract class for register, memory location and immediate 
//! value.
struct Operand
{
  inline Operand() { memset(this, 0, sizeof(Operand)); }
  inline Operand(const Operand& other) { _init(other); }

  inline Operand(const _DontInitialize&) : _operandId(0) {}

  //! @brief Return type of operand, see @c OP.
  inline UInt8 op() const { return _op.op; }
  //! @brief Return size of operand in bytes.
  inline UInt8 size() const { return _op.size; }

  //! @brief Return @c true if operand is none (@c OP_NONE).
  inline UInt8 isNone() const { return _op.op == OP_NONE; }
  //! @brief Return @c true if operand is any (general purpose, mmx or sse) register (@c OP_REG).
  inline UInt8 isReg() const { return _op.op == OP_REG; }
  //! @brief Return @c true if operand is memory address (@c OP_MEM).
  inline UInt8 isMem() const { return _op.op == OP_MEM; }
  //! @brief Return @c true if operand is immediate (@c OP_IMM).
  inline UInt8 isImm() const { return _op.op == OP_IMM; }
  //! @brief Return @c true if operand is label (@c OP_LABEL).
  inline UInt8 isLabel() const { return _op.op == OP_LABEL; }

  //! @brief Return @c true if operand is register and type of register is @a regType.
  inline UInt8 isRegType(UInt8 regType) const { return isReg() & ((_reg.code & REGTYPE_MASK) == regType); }
  //! @brief Return @c true if operand is register and code of register is @a regCode.
  inline UInt8 isRegCode(UInt8 regCode) const { return isReg() & (_reg.code == regCode); }
  //! @brief Return @c true if operand is register and index of register is @a regIndex.
  inline UInt8 isRegIndex(UInt8 regIndex) const { return isReg() & ((_reg.code & REGCODE_MASK) == (regIndex & REGCODE_MASK)); }

  //! @brief Return @c true if operand is any register or memory.
  inline UInt8 isRegMem() const { return isMem() | isReg(); }
  //! @brief Return @c true if operand is register of @a regType type or memory.
  inline UInt8 isRegMem(UInt8 regType) const { return isMem() | isRegType(regType); }

  //! @brief Return operand Id (Operand Id's are for @c Compiler class).
  inline UInt32 operandId() const { return _operandId; }
  //! @brief Return clears operand Id (@c Compiler will not recognize it after clearing).
  inline void clearId() { _operandId = 0; }

  //! @brief Generic operand data shared between all operands.
  struct GenData
  {
    //! @brief Type of operand, see @c OP.
    UInt8 op;
    //! @brief Size of operand (register, address or immediate size).
    UInt8 size;
    UInt8 unused2;
    UInt8 unused3;
    SysInt unused4;
  };

  //! @brief Register data.
  struct RegData
  {
    //! @brief Type of operand, see @c OP.
    UInt8 op;
    //! @brief Size of register.
    UInt8 size;
    //! @brief Register code, see @c REG.
    UInt8 code;
    //! @brief Not used.
    UInt8 unused3;
    //! @brief Not used.
    SysInt unused5;
  };

  //! @brief Memory address data.
  struct MemData
  {
    //! @brief Type of operand, see @c OP.
    UInt8 op;
    //! @brief Size of pointer.
    UInt8 size;
    //! @brief Base register index, see @c REG.
    UInt8 base;
    //! @brief Index register index, see @c REG.
    //!
    //! Index register is a bit complicated here, because we need to store here 
    //! more informations than only register index (to save operand size).
    //! First 3 bits are shift (this very likely SIB byte), next one bit means
    //! if index is used or not. Last 4 bytes are representing index register
    //! code (0-15 is sufficient for 32 bit platform and 64 bit platform).
    //!
    //! To get if this is base+index+displacement address use
    //!   index & 0x10
    //!
    //! To get shift use:
    //!   index >> 5
    //!
    //! To get index register use:
    //!   index & 0xF
    //!
    //! See @c Mem implementation for details
    UInt8 index;
    //! @brief Displacement.
    SysInt displacement;
  };

  //! @brief Immediate value data.
  struct ImmData
  {
    //! @brief Type of operand, see @c OP.
    UInt8 op;
    //! @brief Size of immediate (or 0 to autodetect).
    UInt8 size;
    //! @brief @c true if immediate is unsigned.
    UInt8 isUnsigned;
    //! @brief Not used.
    UInt8 relocMode;
    //! @brief Immediate value.
    SysInt value;
  };

  //! @brief Label data.
  struct LblData
  {
    //! @brief Type of operand, see @c OP.
    UInt8 op;
    //! @brief State of label, see @c LABEL_STATE.
    UInt8 state;
    //! @brief Label Id (0 means unknown).
    UInt16 id;
    //! @brief Position (always positive, information depends to @c state).
    SysInt position;
  };

  union
  {
    //! @brief Generic operand data.
    GenData _op;
    //! @brief Register operand data.
    RegData _reg;
    //! @brief Memory operand data.
    MemData _mem;
    //! @brief Immediate operand data.
    ImmData _imm;
    //! @brief Label data.
    LblData _lbl;
  };

  //! @brief Compiler operand id (do not modify manually).
  UInt32 _operandId;

  inline void _init(const Operand& other)
  {
    memcpy(this, &other, sizeof(Operand));
  }

  inline void _copy(const Operand& other)
  {
    memcpy(this, &other, sizeof(Operand));
  }

  //! @brief Private method to init whole operand.
  //! 
  //! If all parameters are constants then compiler can generate very small 
  //! code (3 instructions) that's much better that code that generates for
  //! each operand normally.
  inline void _initAll(UInt8 i8_0, UInt8 i8_1, UInt8 i8_2, UInt8 i8_3, SysInt i32_64)
  {
    *reinterpret_cast<UInt32*>((UInt8*)this) = 
      ((UInt32)i8_0      ) |
      ((UInt32)i8_1 <<  8) |
      ((UInt32)i8_2 << 16) |
      ((UInt32)i8_3 << 24) ;
    *reinterpret_cast<SysInt*>(&this->_op.unused4) = i32_64;
  }

  friend struct Compiler;
};

// ============================================================================
// [AsmJit::BaseRegMem]
// ============================================================================

//! @brief Base class for registers and memory location.
//!
//! Reason for this class is that internally most functions supports
//! source or destination operands that can be registers or memory location.
struct BaseRegMem : public Operand
{
  inline BaseRegMem(const _DontInitialize& dontInitialize) : Operand(dontInitialize) {}
  inline BaseRegMem(const BaseRegMem& other) : Operand(other) {}
  inline BaseRegMem& operator=(const BaseRegMem& other) { _copy(other); }
};

// ============================================================================
// [AsmJit::BaseReg]
// ============================================================================

//! @brief Base class for all registers.
struct BaseReg : public BaseRegMem
{
  BaseReg(UInt8 code, UInt8 size) : BaseRegMem(_DontInitialize())
  { _initAll(OP_REG, size, code, 0, 0); }

  inline BaseReg(const BaseReg& other) : BaseRegMem(other)
  {}

  inline BaseReg& operator=(const BaseReg& other)
  { _copy(other); }

  //! @brief Return register type, see @c REG.
  inline UInt8 type() const { return (UInt8)(_reg.code & REGTYPE_MASK); }
  //! @brief Return register code, see @c REG.
  inline UInt8 code() const { return (UInt8)(_reg.code); }
  //! @brief Return register index (value from 0 to 7/15).
  inline UInt8 index() const { return (UInt8)(_reg.code & REGCODE_MASK); }

  inline UInt8 isRegCode(UInt8 code) const
  { return _reg.code == code; }

  inline UInt8 isRegType(UInt8 type) const 
  { return (_reg.code & REGTYPE_MASK) == type; }

  inline UInt8 isRegIndex(UInt8 index) const
  { return (_reg.code & REGCODE_MASK) == index; }

  //! @brief Set register code.
  inline void setCode(UInt8 code) { _reg.code = code; }
};

// ============================================================================
// [AsmJit::Register]
// ============================================================================

//! @brief General purpose register.
//!
//! This class is for all general purpose registers (64, 32, 16 and 8 bit).
struct Register : public BaseReg
{
  inline Register(const _Initialize&, UInt8 code) :
    BaseReg(code, static_cast<UInt8>(1U << ((code & REGTYPE_MASK) >> 4)))
  {}

  inline Register(const Register& other) : 
    BaseReg(other)
  {}

  inline Register& operator=(const Register& other)
  { _copy(other); }

  inline bool operator==(const Register& other) const { return code() == other.code(); }
  inline bool operator!=(const Register& other) const { return code() != other.code(); }
};

// ============================================================================
// [AsmJit::X87Register]
// ============================================================================

//! @brief 80-bit x87 floating point register.
//!
//! To create instance of x87 register, use @c st() function.
struct X87Register : public BaseReg
{
  inline X87Register(const _Initialize&, UInt8 code) : 
    BaseReg(code | REG_X87, 10)
  {}

  inline X87Register(const X87Register& other) : 
    BaseReg(other)
  {}

  inline X87Register& operator=(const X87Register& other)
  { _copy(other); }

  inline bool operator==(const X87Register& other) const { return code() == other.code(); }
  inline bool operator!=(const X87Register& other) const { return code() != other.code(); }
};

// ============================================================================
// [AsmJit::MMRegister]
// ============================================================================

//! @brief 64 bit MMX register.
struct MMRegister : public BaseReg
{
  inline MMRegister(const _Initialize&, UInt8 code) :
    BaseReg(code, 8)
  {}

  inline MMRegister(const MMRegister& other) :
    BaseReg(other)
  {}

  inline MMRegister& operator=(const MMRegister& other)
  { _copy(other); }

  inline bool operator==(const MMRegister& other) const { return code() == other.code(); }
  inline bool operator!=(const MMRegister& other) const { return code() != other.code(); }
};

// ============================================================================
// [AsmJit::XMMRegister]
// ============================================================================

//! @brief 128 bit SSE register.
struct XMMRegister : public BaseReg
{
  inline XMMRegister(const _Initialize&, UInt8 code) : 
    BaseReg(code, 16)
  {
  }

  inline XMMRegister(const XMMRegister& other) :
    BaseReg(other)
  {
  }

  inline XMMRegister& operator=(const XMMRegister& other)
  { _copy(other); }

  inline bool operator==(const XMMRegister& other) const { return code() == other.code(); }
  inline bool operator!=(const XMMRegister& other) const { return code() != other.code(); }
};

// ============================================================================
// [AsmJit::Registers]
// ============================================================================

//! @brief 8 bit General purpose register.
static const Register al(_Initialize(), REG_AL);
//! @brief 8 bit General purpose register.
static const Register cl(_Initialize(), REG_CL);
//! @brief 8 bit General purpose register.
static const Register dl(_Initialize(), REG_DL);
//! @brief 8 bit General purpose register.
static const Register bl(_Initialize(), REG_BL);
//! @brief 8 bit General purpose register.
static const Register ah(_Initialize(), REG_AH);
//! @brief 8 bit General purpose register.
static const Register ch(_Initialize(), REG_CH);
//! @brief 8 bit General purpose register.
static const Register dh(_Initialize(), REG_DH);
//! @brief 8 bit General purpose register.
static const Register bh(_Initialize(), REG_BH);

#if defined(ASMJIT_X64)
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r8b(_Initialize(), REG_R8B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r9b(_Initialize(), REG_R9B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r10b(_Initialize(), REG_R10B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r11b(_Initialize(), REG_R11B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r12b(_Initialize(), REG_R12B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r13b(_Initialize(), REG_R13B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r14b(_Initialize(), REG_R14B);
//! @brief 8 bit General purpose register (64 bit mode only).
static const Register r15b(_Initialize(), REG_R15B);
#endif // ASMJIT_X64

//! @brief 16 bit General purpose register.
static const Register ax(_Initialize(), REG_AX);
//! @brief 16 bit General purpose register.
static const Register cx(_Initialize(), REG_CX);
//! @brief 16 bit General purpose register.
static const Register dx(_Initialize(), REG_DX);
//! @brief 16 bit General purpose register.
static const Register bx(_Initialize(), REG_BX);
//! @brief 16 bit General purpose register.
static const Register sp(_Initialize(), REG_SP);
//! @brief 16 bit General purpose register.
static const Register bp(_Initialize(), REG_BP);
//! @brief 16 bit General purpose register.
static const Register si(_Initialize(), REG_SI);
//! @brief 16 bit General purpose register.
static const Register di(_Initialize(), REG_DI);

#if defined(ASMJIT_X64)
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r8w(_Initialize(), REG_R8W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r9w(_Initialize(), REG_R9W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r10w(_Initialize(), REG_R10W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r11w(_Initialize(), REG_R11W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r12w(_Initialize(), REG_R12W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r13w(_Initialize(), REG_R13W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r14w(_Initialize(), REG_R14W);
//! @brief 16 bit General purpose register (64 bit mode only).
static const Register r15w(_Initialize(), REG_R15W);
#endif // ASMJIT_X64

//! @brief 32 bit General purpose register.
static const Register eax(_Initialize(), REG_EAX);
//! @brief 32 bit General purpose register.
static const Register ecx(_Initialize(), REG_ECX);
//! @brief 32 bit General purpose register.
static const Register edx(_Initialize(), REG_EDX);
//! @brief 32 bit General purpose register.
static const Register ebx(_Initialize(), REG_EBX);
//! @brief 32 bit General purpose register.
static const Register esp(_Initialize(), REG_ESP);
//! @brief 32 bit General purpose register.
static const Register ebp(_Initialize(), REG_EBP);
//! @brief 32 bit General purpose register.
static const Register esi(_Initialize(), REG_ESI);
//! @brief 32 bit General purpose register.
static const Register edi(_Initialize(), REG_EDI);

#if defined(ASMJIT_X64)
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rax(_Initialize(), REG_RAX);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rcx(_Initialize(), REG_RCX);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rdx(_Initialize(), REG_RDX);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rbx(_Initialize(), REG_RBX);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rsp(_Initialize(), REG_RSP);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rbp(_Initialize(), REG_RBP);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rsi(_Initialize(), REG_RSI);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register rdi(_Initialize(), REG_RDI);

//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r8(_Initialize(), REG_R8);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r9(_Initialize(), REG_R9);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r10(_Initialize(), REG_R10);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r11(_Initialize(), REG_R11);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r12(_Initialize(), REG_R12);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r13(_Initialize(), REG_R13);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r14(_Initialize(), REG_R14);
//! @brief 64 bit General purpose register (64 bit mode only).
static const Register r15(_Initialize(), REG_R15);
#endif // ASMJIT_X64

//! @brief 32 bit General purpose register.
static const Register nax(_Initialize(), REG_NAX);
//! @brief 32 bit General purpose register.
static const Register ncx(_Initialize(), REG_NCX);
//! @brief 32 bit General purpose register.
static const Register ndx(_Initialize(), REG_NDX);
//! @brief 32 bit General purpose register.
static const Register nbx(_Initialize(), REG_NBX);
//! @brief 32 bit General purpose register.
static const Register nsp(_Initialize(), REG_NSP);
//! @brief 32 bit General purpose register.
static const Register nbp(_Initialize(), REG_NBP);
//! @brief 32 bit General purpose register.
static const Register nsi(_Initialize(), REG_NSI);
//! @brief 32 bit General purpose register.
static const Register ndi(_Initialize(), REG_NDI);

//! @brief 64 bit MMX register.
static const MMRegister mm0(_Initialize(), REG_MM0);
//! @brief 64 bit MMX register.
static const MMRegister mm1(_Initialize(), REG_MM1);
//! @brief 64 bit MMX register.
static const MMRegister mm2(_Initialize(), REG_MM2);
//! @brief 64 bit MMX register.
static const MMRegister mm3(_Initialize(), REG_MM3);
//! @brief 64 bit MMX register.
static const MMRegister mm4(_Initialize(), REG_MM4);
//! @brief 64 bit MMX register.
static const MMRegister mm5(_Initialize(), REG_MM5);
//! @brief 64 bit MMX register.
static const MMRegister mm6(_Initialize(), REG_MM6);
//! @brief 64 bit MMX register.
static const MMRegister mm7(_Initialize(), REG_MM7);

//! @brief 128 bit SSE register.
static const XMMRegister xmm0(_Initialize(), REG_XMM0);
//! @brief 128 bit SSE register.
static const XMMRegister xmm1(_Initialize(), REG_XMM1);
//! @brief 128 bit SSE register.
static const XMMRegister xmm2(_Initialize(), REG_XMM2);
//! @brief 128 bit SSE register.
static const XMMRegister xmm3(_Initialize(), REG_XMM3);
//! @brief 128 bit SSE register.
static const XMMRegister xmm4(_Initialize(), REG_XMM4);
//! @brief 128 bit SSE register.
static const XMMRegister xmm5(_Initialize(), REG_XMM5);
//! @brief 128 bit SSE register.
static const XMMRegister xmm6(_Initialize(), REG_XMM6);
//! @brief 128 bit SSE register.
static const XMMRegister xmm7(_Initialize(), REG_XMM7);

#if defined(ASMJIT_X64)
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm8(_Initialize(), REG_XMM8);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm9(_Initialize(), REG_XMM9);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm10(_Initialize(), REG_XMM10);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm11(_Initialize(), REG_XMM11);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm12(_Initialize(), REG_XMM12);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm13(_Initialize(), REG_XMM13);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm14(_Initialize(), REG_XMM14);
//! @brief 128 bit SSE register (64 bit mode only).
static const XMMRegister xmm15(_Initialize(), REG_XMM15);
#endif // ASMJIT_X64

static inline Register mk_gpb(UInt8 index) { return Register(_Initialize(), static_cast<UInt8>(index | REG_GPB)); }
static inline Register mk_gpw(UInt8 index) { return Register(_Initialize(), static_cast<UInt8>(index | REG_GPW)); }
static inline Register mk_gpd(UInt8 index) { return Register(_Initialize(), static_cast<UInt8>(index | REG_GPD)); }
#if defined(ASMJIT_X64)
static inline Register mk_gpq(UInt8 index) { return Register(_Initialize(), static_cast<UInt8>(index | REG_GPQ)); }
#endif
static inline Register mk_gpn(UInt8 index) { return Register(_Initialize(), static_cast<UInt8>(index | REG_GPN)); }
static inline MMRegister mk_mm(UInt8 index) { return MMRegister(_Initialize(), static_cast<UInt8>(index | REG_MM)); }
static inline XMMRegister mk_xmm(UInt8 index) { return XMMRegister(_Initialize(), static_cast<UInt8>(index | REG_XMM)); }

//! @brief returns x87 register with index @a i.
static inline X87Register st(int i)
{
  ASMJIT_ASSERT(i >= 0 && i < 8);
  return X87Register(_Initialize(), static_cast<UInt8>(i));
}

// ============================================================================
// [AsmJit::Mem]
// ============================================================================

//! @brief Memory location operand.
struct Mem : public BaseRegMem
{
  inline Mem(const Register& base, SysInt displacement, UInt8 size = 0) : 
    BaseRegMem(_DontInitialize())
  {
    _initAll(OP_MEM, size, base.index() | 0x10, 0x00, displacement);
  }

  inline Mem(const Register& base, const Register& index, UInt32 shift, SysInt displacement, UInt8 size = 0) : 
    BaseRegMem(_DontInitialize())
  {
    ASMJIT_ASSERT(shift <= 3);
    _initAll(OP_MEM, size, base.index() | 0x10, (shift << 5) | 0x10 | index.index(), displacement);
  }

  inline Mem(const Mem& other) :
    BaseRegMem(other)
  {}

  inline Mem& operator=(const Mem& other)
  { _copy(other); }

  //! @brief Return if address has base register. 
  inline bool hasBase() const { return (_mem.base & 0x10) != 0; }

  //! @brief Return if address has index register.
  //!
  //! @note It's illegal to have index register and not base one.
  inline bool hasIndex() const { return (_mem.index & 0x10) != 0; }

  //! @brief Address base register or @c NO_REG.
  inline UInt8 base() const { return _mem.base & 0xF; }

  //! @brief Address index register or @c NO_REG.
  inline UInt8 index() const { return _mem.index & 0xF; }

  //! @brief Address index scale (0, 1, 2 or 3).
  inline UInt32 shift() const { return _mem.index >> 5; }

  //! @brief Address relative displacement.
  inline SysInt displacement() const { return _mem.displacement; }

  //! @brief Set address relative displacement.
  inline void setDisplacement(SysInt displacement) { _mem.displacement = displacement; }
};

// [base + displacement]

//! @brief Create pointer operand with not specified size.
static inline Mem ptr(const Register& base, SysInt disp = 0){ return Mem(base, disp, 0); }
//! @brief Create byte pointer operand.
static inline Mem byte_ptr(const Register& base, SysInt disp = 0){ return Mem(base, disp, SIZE_BYTE); }
//! @brief Create word (2 Bytes) pointer operand.
static inline Mem word_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_WORD); }
//! @brief Create dword (4 Bytes) pointer operand.
static inline Mem dword_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_DWORD); }
//! @brief Create qword (8 Bytes) pointer operand.
static inline Mem qword_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_QWORD); }
//! @brief Create tword (10 Bytes) pointer operand (used for 80 bit floating points).
static inline Mem tword_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_TWORD); }
//! @brief Create dqword (16 Bytes) pointer operand.
static inline Mem dqword_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_DQWORD); }

//! @brief Create mmword (8 bytes) pointer operand
//!
//! @note This constructor is provided only for convenience for mmx programming.
static inline Mem mmword_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_QWORD); }
//! @brief Create xmmword (16 bytes) pointer operand
//!
//! @note This constructor is provided only for convenience for sse programming.
static inline Mem xmmword_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, SIZE_DQWORD); }

//! @brief Create system dependent pointer operand (32 bit or 64 bit).
static inline Mem sysint_ptr(const Register& base, SysInt disp = 0) { return Mem(base, disp, sizeof(SysInt)); }

// [base + (index << shift) + displacement]

//! @brief Create pointer operand with not specified size.
static inline Mem ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, 0); }
//! @brief Create byte pointer operand.
static inline Mem byte_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_BYTE); }
//! @brief Create word (2 Bytes) pointer operand.
static inline Mem word_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_WORD); }
//! @brief Create dword (4 Bytes) pointer operand.
static inline Mem dword_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_DWORD); }
//! @brief Create qword (8 Bytes) pointer operand.
static inline Mem qword_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_QWORD); }
//! @brief Create tword (10 Bytes) pointer operand (used for 80 bit floating points).
static inline Mem tword_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_TWORD); }
//! @brief Create dqword (16 Bytes) pointer operand.
static inline Mem dqword_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_DQWORD); }

//! @brief Create mmword (8 Bytes) pointer operand).
//!
//! @note This constructor is provided only for convenience for mmx programming.
static inline Mem mmword_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_QWORD); }
//! @brief Create xmmword (16 Bytes) pointer operand.
//!
//! @note This constructor is provided only for convenience for sse programming.
static inline Mem xmmword_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, SIZE_DQWORD); }

//! @brief Create system dependent pointer operand (32 bit or 64 bit).
static inline Mem sysint_ptr(const Register& base, const Register& index, UInt32 shift, SysInt disp = 0) { return Mem(base, index, shift, disp, sizeof(SysInt)); }

// ============================================================================
// [AsmJit::Immediate]
// ============================================================================

//! @brief Immediate operand.
//!
//! Immediate operand is value that is inlined in binary instruction stream.
//!
//! To create immediate operand, use @c imm() and @c uimm() constructors 
//! or constructors provided by @c Immediate class itself.
struct Immediate : public Operand
{
  Immediate(SysInt i) : Operand(_DontInitialize())
  {
    _initAll(OP_IMM, 0, 0, RELOC_NONE, i);
  }
  
  Immediate(SysInt i, UInt8 isUnsigned) : Operand(_DontInitialize())
  {
    _initAll(OP_IMM, 0, isUnsigned, RELOC_NONE, i);
  }
  
  inline Immediate(const Immediate& other) : Operand(other) {}

  inline Immediate& operator=(SysInt val)
  { setValue(val); return *this; }

  inline Immediate& operator=(const Immediate& other)
  { _copy(other); }

  //! @brief Return true if immediate is unsigned value.
  inline UInt8 isUnsigned() const { return _imm.isUnsigned; }

  //! @brief Return relocation mode.
  inline UInt8 relocMode() const { return _imm.relocMode; }

  //! @brief Return signed immediate value.
  inline SysInt value() const { return _imm.value; }

  //! @brief Return unsigned immediate value.
  inline SysUInt uvalue() const { return (SysUInt)_imm.value; }

  //! @brief Set immediate value as signed type to @a val.
  inline void setValue(SysInt val, UInt8 isUnsigned = false)
  {
    _imm.value = val; 
    _imm.isUnsigned = isUnsigned;
  }

  //! @brief Set immediate value as unsigned type to @a val.
  inline void setUValue(SysUInt val)
  {
    _imm.value = (SysInt)val;
    _imm.isUnsigned = true;
  }
};

//! @brief Create signed immediate value operand.
static inline Immediate imm(SysInt i) { return Immediate(i, false); }

//! @brief Create unsigned immediate value operand.
static inline Immediate uimm(SysUInt i) { return Immediate((SysInt)i, true); }

// ============================================================================
// [AsmJit::Label]
// ============================================================================

//! @brief Label.
//!
//! Label represents locations typically used as jump targets, but may be also
//! used as position where are stored constants or static variables.
struct Label : public Operand
{
  //! @brief Create new unused label.
  inline Label(UInt16 id = 0) 
  {
    _lbl.op = OP_LABEL;
    _lbl.state = LABEL_UNUSED;
    _lbl.id = id;
    _lbl.position = -1;
  }

  //! @brief Destroy label. If label is linked to some location (not bound), 
  //! assertion is raised (because generated code is invalid in this case).
  inline ~Label() { ASMJIT_ASSERT(!isLinked()); }

  //! @brief Unuse label (unbound or unlink) - Use with caution.
  inline void unuse()
  { _initAll(OP_LABEL, LABEL_UNUSED, 0, 0, -1); }

  //! @brief Return label state, see @c LABEL_STATE. */
  inline UInt8 state() const { return _lbl.state; }
  //! @brief Return label Id.
  inline UInt16 labelId() const { return _lbl.id; }
  //! @brief Returns @c true if label is unused (not bound or linked).
  inline bool isUnused() const { return _lbl.state == LABEL_UNUSED; }
  //! @brief Returns @c true if label is linked.
  inline bool isLinked() const { return _lbl.state == LABEL_LINKED; }
  //! @brief Returns @c true if label is bound.
  inline bool isBound()  const { return _lbl.state == LABEL_BOUND; }

  //! @brief Returns the position of bound or linked labels, -1 if label 
  //! is unused.
  inline SysInt position() const { return _lbl.position; }

  //! @brief Set label Id.
  inline void setId(UInt16 id) 
  {
    _lbl.id = id;
  }

  //! @brief Set state and position
  inline void setStatePos(UInt8 state, SysInt position)
  {
    _lbl.state = state; 
    _lbl.position = position;
  }
};

// ============================================================================
// [AsmJit::Relocable]
// ============================================================================

//! @brief Relocable immediate operand.
struct Relocable : public Immediate
{
  inline Relocable(SysInt i, UInt8 isUnsigned = false) : Immediate(i, isUnsigned)
  { _imm.relocMode = RELOC_OVERWRITE; }

  inline Relocable(const Relocable& other) : Immediate(other)
  { _imm.relocMode = RELOC_OVERWRITE; }

  inline Relocable& operator=(const SysInt i)
  { setValue(i); return *this; }

  inline Relocable& operator=(const Relocable& other)
  { _copy(other); }

  inline void discard()
  { _relocations.clear(); }

  mutable PodVector<RelocInfo> _relocations;
};

// ============================================================================
// [AsmJit::operand_cast<>]
// ============================================================================

// operand cast
template<typename To> static inline To operand_cast(Operand& op) { return reinterpret_cast<To>(op); }
template<typename To> static inline To operand_cast(const Operand& op) { return reinterpret_cast<To>(op); }

#define MAKE_OPERAND_CAST(To, Expect) \
template<> static inline To& operand_cast(Operand& op) \
{ \
  ASMJIT_ASSERT(Expect); \
  return reinterpret_cast<To&>(op); \
} \
\
template<> static inline const To& operand_cast(const Operand& op) \
{ \
  ASMJIT_ASSERT(Expect); \
  return reinterpret_cast<const To&>(op); \
}

MAKE_OPERAND_CAST(BaseReg, (op.op() == OP_REG));
MAKE_OPERAND_CAST(Register, (op.op() == OP_REG && reinterpret_cast<const Register&>(op).type() <= REG_GPQ));
MAKE_OPERAND_CAST(X87Register, (op.op() == OP_REG && reinterpret_cast<const Register&>(op).type() == REG_X87));
MAKE_OPERAND_CAST(MMRegister, (op.op() == OP_REG && reinterpret_cast<const Register&>(op).type() == REG_MM));
MAKE_OPERAND_CAST(XMMRegister, (op.op() == OP_REG && reinterpret_cast<const Register&>(op).type() == REG_XMM));
MAKE_OPERAND_CAST(Mem, (op.op() == OP_MEM));
MAKE_OPERAND_CAST(BaseRegMem, (op.op() == OP_MEM || op.op() == OP_REG));
MAKE_OPERAND_CAST(Immediate, (op.op() == OP_IMM));
MAKE_OPERAND_CAST(Relocable, (op.op() == OP_IMM && reinterpret_cast<const Immediate&>(op).relocMode() != RELOC_NONE));

#undef MAKE_OPERAND_CAST

// ============================================================================
// [AsmJit::mm_shuffle]
// ============================================================================

//! @brief Create Shuffle Constant for SSE shuffle instrutions.
static inline UInt8 mm_shuffle(UInt8 z, UInt8 y, UInt8 x, UInt8 w)
{ return (z << 6) | (y << 4) | (x << 2) | w; }

} // AsmJit namespace

// [Guard]
#endif // _ASMJITDEFSX86X64_H
