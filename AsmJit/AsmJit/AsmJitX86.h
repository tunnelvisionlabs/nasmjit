// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2006-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
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

// TODOs:
// [ ] FPU instructions and format is not well defined. Some instructions
//     allows to store result in st(i) and others to get st(0) and st(i).
//     This should be clear in the future.
// [ ] Add SSE4a instruction set (AMD).
// [ ] Add AMD 3dNow.
// [ ] Add AMD 3dNow extensions.
// [ ] X64 instructions not ported:
//     test, j, jmp, call

// [Guard]
#ifndef _ASMJIT_X86_H
#define _ASMJIT_X86_H

// [Dependencies]
#include "AsmJitConfig.h"

// [AsmJit::]
namespace AsmJit {

//! @addtogroup AsmJit_Main
//! @{

// Forward Declarations
struct Displacement;
struct Label;
struct X86; 

// Constants

//! @brief Operand types that can be encoded in @c Op operand.
enum OP
{
  OP_REG = 0x1,
  OP_MEM = 0x2,
  OP_IMM = 0x4,
};

//! @brief Size of registers and pointers.
enum SIZE
{
  SIZE_BYTE   = 1,
  SIZE_WORD   = 2,
  SIZE_DWORD  = 4,
  SIZE_QWORD  = 8,
  SIZE_DQWORD = 16
};

//! @brief Pseudo (not real X86) register codes used for generating opcodes.
//!
//! From this register code can be generated real x86 register ID, type of
//! register and size of register.
enum REG
{
  REGTYPE_MASK = 0xF0,
  REGCODE_MASK = 0x0F,

  // First nibble contains register type and size (mask 0xF0)
  // Register type is whole nibble, but to get register size, it's needed
  // this formula:
  //     1 << ((x & 70) >> 4)
  // It filters one bit from our nibble and shifts 1 by result to right

  // 8 bit, 16 bit and 32 bit generap purpose register types
  REG_GPB = 0x00,
  REG_GPW = 0x10,
  REG_GPD = 0x20,

  // 64 bit registers (RAX, RBX, ...), not available in 32 bit mode
  REG_GPQ = 0x30,

  // native 32 bit or 64 bit register type
#if defined(ASMJIT_X86)
  REG_GPN = REG_GPD,
#else
  REG_GPN = REG_GPQ,
#endif
  // 64 bit mmx register type
  REG_MMX = 0xB0,
  // 128 bit sse register type
  REG_SSE = 0xC0,

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
  REG_MM0  = REG_MMX , REG_MM1  , REG_MM2  , REG_MM3  , REG_MM4  , REG_MM5  , REG_MM6  , REG_MM7  ,

  // SSE registers
  REG_XMM0 = REG_SSE , REG_XMM1 , REG_XMM2 , REG_XMM3 , REG_XMM4 , REG_XMM5 , REG_XMM6 , REG_XMM7 ,
#if defined(ASMJIT_X64)
  REG_XMM8           , REG_XMM9 , REG_XMM10, REG_XMM11, REG_XMM12, REG_XMM13, REG_XMM14, REG_XMM15,
#endif // ASMJIT_X64

  // native registers (depends if processor runs in 32 bit or 64 bit mode)
#if defined(ASMJIT_X86)
  REG_NAX  = REG_GPD , REG_NCX  , REG_NDX  , REG_NBX  , REG_NSP  , REG_NBP  , REG_NSI  , REG_NDI  ,
#else
  REG_NAX  = REG_GPQ , REG_NCX  , REG_NDX  , REG_NBX  , REG_NSP  , REG_NBP  , REG_NSI  , REG_NDI  ,
#endif

  // invalid register code
  NO_REG = 0xFF
};

//! @brief Prefetch hints.
enum PREFETCH_HINT
{
  PREFETCH_T0 = 1,
  PREFETCH_T1 = 2,
  PREFETCH_T2 = 3,
  PREFETCH_NTA = 0
};

//! @brief Condition codes
enum CONDITION
{
  //! @brief Any value < 0 is considered no condition.
  C_NO_CONDITION  = -1,

  C_OVERFLOW      =  0,
  C_NO_OVERFLOW   =  1,
  C_BELOW         =  2,
  C_ABOVE_EQUAL   =  3,
  C_EQUAL         =  4,
  C_NOT_EQUAL     =  5,
  C_BELOW_EQUAL   =  6,
  C_ABOVE         =  7,
  C_SIGN          =  8,
  C_NOT_SIGN      =  9,
  C_PARITY_EVEN   = 10,
  C_PARITY_ODD    = 11,
  C_LESS          = 12,
  C_GREATER_EQUAL = 13,
  C_LESS_EQUAL    = 14,
  C_GREATER       = 15,

  // aliases
  C_ZERO          = C_EQUAL,
  C_NOT_ZERO      = C_NOT_EQUAL,
  C_NEGATIVE      = C_SIGN,
  C_POSITIVE      = C_NOT_SIGN,

  // floating point only
  C_FP_UNORDERED  = 100,
  C_FP_NOT_UNORDERED = 101
};

//! @brief Scale, can be used for addressing.
//!
//! See @c Op and addressing methods like @c byte_ptr(), @c word_ptr(),
//! @c dword_ptr(), etc...
enum SCALE
{
  TIMES_1 = 0,
  TIMES_2 = 1,
  TIMES_4 = 2,
  TIMES_8 = 3
};

//! @brief Condition hint
enum HINT
{
  HINT_NONE = 0,
  HINT_NOT_TAKEN = 0x2E,
  HINT_TAKEN = 0x3E
};

//! @brief Floating point status
enum FP_STATUS
{
  FP_C0 = 0x100,
  FP_C1 = 0x200,
  FP_C2 = 0x400,
  FP_C3 = 0x4000,
  FP_CC_MASK = 0x4500
};

//! @brief Floating point control word
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

// Helpers

//! @brief BitField is a help template for encoding and decode bitfield with
//! unsigned content.
//!
//! @internal
template<class T, int shift, int size>
struct BitField
{
  // Tells whether the provided value fits into the bit field.
  static bool isValid(T value) {
    return (static_cast<SysUInt>(value) & ~((1U << (size)) - 1)) == 0;
  }

  // Returns a UInt32 mask of bit field.
  static SysUInt mask() {
    return (1U << (size + shift)) - (1U << shift);
  }

  // Returns a UInt32 with the bit field value encoded.
  static SysUInt encode(T value)
  {
    ASMJIT_ASSERT(isValid(value));
    return static_cast<SysUInt>(value) << shift;
  }

  // Extracts the bit field from the value.
  static T decode(SysUInt value)
  {
    return static_cast<T>((value >> shift) & ((1U << (size)) - 1));
  }
};

//! @brief Returns @c true if a given integer @a x is signed 8 bit integer
inline bool isInt8(SysInt x) { return x >= -128 && x <= 127; }
//! @brief Returns @c true if a given integer @a x is unsigned 8 bit integer
inline bool isUInt8(SysInt x) { return x >= 0 && x <= 255; }

//! @brief Returns @c true if a given integer @a x is signed 16 bit integer
inline bool isInt16(SysInt x) { return x >= -32768 && x <= 32767; }
//! @brief Returns @c true if a given integer @a x is unsigned 16 bit integer
inline bool isUInt16(SysInt x) { return x >= 0 && x <= 65535; }

//! @brief Returns @c true if a given integer @a x is signed 16 bit integer
inline bool isInt32(SysInt x) { return x >= ASMJIT_INT64_C(-2147483648) && x <= ASMJIT_INT64_C(2147483647); }
//! @brief Returns @c true if a given integer @a x is unsigned 16 bit integer
inline bool isUInt32(SysInt x) { return x >= 0 && x <= ASMJIT_INT64_C(4294967295); }

//! @brief used to cast float to integer and vica versa.
//!
//! @internal
union IntFloatUnion
{
  int i;
  float f;
};

//! @brief Binary casts integer to float.
inline float intAsFloat(int x)
{
  IntFloatUnion u;
  u.i = x;
  return u.f;
}

//! @brief Binary casts float to integer.
inline int floatAsInt(float f)
{
  IntFloatUnion u;
  u.f = f;
  return u.i;
}

//! @brief 32 bit general purpose register.
struct Register
{
  inline int reg() const { return _code; }
  inline int regType() const { return _code & REGTYPE_MASK; }
  inline int regCode() const { return _code & REGCODE_MASK; }
  inline int regSize() const { return 1 << ((_code & 0x70) >> 4); }

  inline bool operator==(const Register& other) const { return _code == other._code; }
  inline bool operator!=(const Register& other) const { return _code != other._code; }

  UInt8 _code;
};

// 8 bit general purpose registers

//! @brief 8 bit General purpose register that shared place with @c ax and @c eax.
ASMJIT_VAR const Register al;
//! @brief 8 bit General purpose register that shared place with @c cx and @c ecx.
ASMJIT_VAR const Register cl;
//! @brief 8 bit General purpose register that shared place with @c dx and @c edx.
ASMJIT_VAR const Register dl;
//! @brief 8 bit General purpose register that shared place with @c bx and @c ebx.
ASMJIT_VAR const Register bl;
//! @brief 8 bit General purpose register that shared place with @c ax and @c eax.
ASMJIT_VAR const Register ah;
//! @brief 8 bit General purpose register that shared place with @c cx and @c ecx.
ASMJIT_VAR const Register ch;
//! @brief 8 bit General purpose register that shared place with @c dx and @c edx.
ASMJIT_VAR const Register dh;
//! @brief 8 bit General purpose register that shared place with @c bx and @c ebx.
ASMJIT_VAR const Register bh;

#if defined(ASMJIT_X64)
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r8b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r9b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r10b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r11b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r12b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r13b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r14b;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r15b;
#endif // ASMJIT_X64

// 16 bit general purpose registers

//! @brief 16 bit General purpose register that shared place with @c ax and @c eax.
ASMJIT_VAR const Register ax;
//! @brief 16 bit General purpose register that shared place with @c ax and @c ecx.
ASMJIT_VAR const Register cx;
//! @brief 16 bit General purpose register that shared place with @c ax and @c edx.
ASMJIT_VAR const Register dx;
//! @brief 16 bit General purpose register that shared place with @c ax and @c ebx.
ASMJIT_VAR const Register bx;
//! @brief 16 bit General purpose register that shared place with @c ax and @c esp.
ASMJIT_VAR const Register sp;
//! @brief 16 bit General purpose register that shared place with @c ax and @c ebp.
ASMJIT_VAR const Register bp;
//! @brief 16 bit General purpose register that shared place with @c ax and @c esi.
ASMJIT_VAR const Register si;
//! @brief 16 bit General purpose register that shared place with @c ax and @c edi.
ASMJIT_VAR const Register di;

// 32 bit general purpose registers

//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register eax;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register ecx;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register edx;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register ebx;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register esp;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register ebp;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register esi;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register edi;

#if defined(ASMJIT_X64)
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r8d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r9d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r10d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r11d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r12d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r13d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r14d;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register r15d;
#endif // ASMJIT_X64

// 64 bit mode general purpose registers
#if defined(ASMJIT_X64)
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rax;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rcx;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rdx;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rbx;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rsp;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rbp;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rsi;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register rdi;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r8;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r9;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r10;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r11;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r12;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r13;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r14;
//! @brief 64 bit General purpose register.
ASMJIT_VAR const Register r15;
#endif // ASMJIT_X64

// native general purpose registers
// (shared between 32 bit mode and 64 bit mode)

//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register nax;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register ncx;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register ndx;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register nbx;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register nsp;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register nbp;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register nsi;
//! @brief 32 bit General purpose register.
ASMJIT_VAR const Register ndi;

//! @brief 64 bit MMX register.
struct MMRegister
{
  inline int reg() const { return _code; }
  inline int regType() const { return _code & REGTYPE_MASK; }
  inline int regCode() const { return _code & REGCODE_MASK; }
  inline int regSize() const { return 8; }

  inline bool operator==(const MMRegister& other) const { return _code == other._code; }
  inline bool operator!=(const MMRegister& other) const { return _code != other._code; }

  UInt8 _code;
};

//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm0;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm1;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm2;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm3;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm4;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm5;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm6;
//! @brief 64 bit MMX register.
ASMJIT_VAR const MMRegister mm7;

//! @brief 128 bit SSE register.
struct XMMRegister
{
  inline int reg() const { return _code; }
  inline int regType() const { return _code & REGTYPE_MASK; }
  inline int regCode() const { return _code & REGCODE_MASK; }
  inline int regSize() const { return 16; }

  inline bool operator==(const XMMRegister& other) const { return _code == other._code; }
  inline bool operator!=(const XMMRegister& other) const { return _code != other._code; }

  UInt8 _code;
};

//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm0;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm1;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm2;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm3;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm4;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm5;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm6;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm7;

// 64 bit mode extended
#if defined(ASMJIT_X64)
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm8;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm9;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm10;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm11;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm12;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm13;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm14;
//! @brief 128 bit SSE register.
ASMJIT_VAR const XMMRegister xmm15;
#endif // ASMJIT_X64

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

//! @brief Displacement.
//!
//! A Displacement describes the 32bit immediate field of an instruction which
//! may be used together with a Label in order to refer to a yet unknown code
//! position. Displacements stored in the instruction stream are used to describe
//! the instruction and to chain a list of instructions using the same Label.
//! A Displacement contains 2 different fields:
//!
//! next field: position of next displacement in the chain (0 = end of list)
//1 type field: instruction type
//!
//! A next value of null (0) indicates the end of a chain (note that there can
//! be no displacement at position zero, because there is always at least one
//! instruction byte before the displacement).
//!
//! Displacement _data field layout
//!
//! |31.....1| ......0|
//! [  next  |  type  |
//!
//! @internal
struct Displacement
{
  enum Type
  {
    UNCONDITIONAL_JUMP,
    OTHER
  };

  explicit Displacement(SysInt data)
  {
    _data = data;
  }

  Displacement(Label* L, Type type)
  {
    init(L, type);
  }

  inline SysInt data() const
  {
    return _data;
  }

  inline Type type() const
  {
    return TypeField::decode(_data);
  }

  inline void next(Label* L) const;
  inline void linkTo(Label* L);

private:
  SysInt _data;

  struct TypeField: public BitField<Type, 0, 1> {};
  struct NextField: public BitField<SysInt,  1, (sizeof(SysInt)*8)-1> {};

  inline void init(Label* L, Type type);

  friend struct Label;
  friend struct X86;
};

//! @brief Labels represent pc locations.
//!
//! They are typically jump or call targets. After declaration, a label 
//! can be freely used to denote known or (yet) unknown pc location. 
//! X86::bind() is used to bind a label to the current pc.
//!
//! @note A label can be bound only once.
struct Label
{
  //! @brief Creates new unused label.
  inline Label() { unuse(); }
  //! @brief Destroys label. If label is linked to some location, assertion is raised.
  inline ~Label() { ASMJIT_ASSERT(!isLinked()); }

  //! @brief Returns Unuse label (unbound or unlink).
  inline void unuse() { _pos = 0; }

  //! @brief Returns @c true if label is bound.
  inline bool isBound()  const { return _pos <  0; }
  //! @brief Returns @c true if label is unused (not bound or linked).
  inline bool isUnused() const { return _pos == 0; }
  //! @brief Returns @c true if label is linked.
  inline bool isLinked() const { return _pos >  0; }

  //! @brief Returns the position of bound or linked labels. Cannot be 
  //! used for unused labels.
  inline SysInt pos() const
  {
    if (_pos < 0) return -_pos - 1;
    if (_pos > 0) return  _pos - 1;
    ASMJIT_ASSERT(0);
    return 0;
  }

private:
  //! pos_ encodes both the binding state (via its sign)
  //! and the binding position (via its value) of a label.
  //!
  //! pos_ <  0  bound label, pos() returns the jump target position
  //! pos_ == 0  unused label
  //! pos_ >  0  linked label, pos() returns the last reference position
  SysInt _pos;

  //! @brief Bind label to position @a pos.
  inline void bindTo(SysInt pos) 
  {
    _pos = -pos - 1;
    ASMJIT_ASSERT(isBound());
  }

  //! @brief Link label to position @a pos.
  inline void linkTo(SysInt pos)
  {
    _pos =  pos + 1;
    ASMJIT_ASSERT(isLinked());
  }

  friend struct Displacement;
  friend struct X86;
};

inline void Displacement::next(Label* L) const
{
  SysInt n = NextField::decode(_data);
  n > 0 ? L->linkTo(n) : L->unuse();
}

inline void Displacement::linkTo(Label* L)
{
  init(L, type());
}

inline void Displacement::init(Label* L, Type type)
{
  ASMJIT_ASSERT(!L->isBound());

  SysInt next = 0;
  if (L->isLinked())
  {
    next = L->pos();
    // Displacements must be at positions > 0
    ASMJIT_ASSERT(next > 0);
  }

  // Ensure that we _never_ overflow the next field.
  // ASMJIT_ASSERT(NextField::isValid(Assembler::kMaximalBufferSize));
  _data = NextField::encode(next) | TypeField::encode(type);
}

// [Operand implementation]

//! @brief Operand.
//!
//! Operands are used in instructions. Typically instruction supports two
//! operands (destination and source). In most instructions the destination
//! is memory/register and source is memory/register/immediate. Memory vs
//! memory functions are not possible in X86 and AsmJit will assert trying
//! to do that.
//!
//! To create operand do not use Op's constructors, instead use methods for
//! that like @c imm() for creating immediate operand or @c byte_ptr(), 
//! @c word_ptr(), @c dword_ptr(), @c qword_ptr() and dqword_ptr() methods.
//! Register operands are created by constructor (implicit), so for registers
//! you don't need to call for example Op(eax).
struct Op
{
  // [Construction / Destruction]

  // these constructors are provided for autocasting, it enables to use
  // registers in code without using wrappers, for example:
  //   mov(eax, ebx);
  //   mov(xmm0, xmm1);
  // ...
  inline Op(const Register& reg)
  {
    _op[0] = OP_REG;
    _op[1] = reg.reg();
    _op[2] = 0xFF;
    _op[3] = 0xFF;
    _val = 0;
    _size = reg.regSize();
  }

  inline Op(const MMRegister& reg)
  {
    _op[0] = OP_REG;
    _op[1] = reg.reg();
    _op[2] = 0xFF;
    _op[3] = 0xFF;
    _val = 0xFFFFFFFF;
    _size = 8;
  }

  inline Op(const XMMRegister& reg)
  {
    _op[0] = OP_REG;
    _op[1] = reg.reg();
    _op[2] = 0xFF;
    _op[3] = 0xFF;
    _val = -1;
    _size = 16;
  }

  // [base + displacement]
  inline Op(const Register& base, SysInt disp, int size)
  {
    _op[0] = OP_MEM;
    _op[1] = base.reg();
    _op[2] = 0xFF;
    _op[3] = 0xFF;
    _val = disp;
    _size = size;
  }

  // [base + (index << shift) + displacement]
  inline Op(const Register& base, const Register& index, Int32 shift, SysInt disp, int size)
  {
    _op[0] = OP_MEM;
    _op[1] = base.reg();
    _op[2] = index.reg();
    _op[3] = shift;
    _val = disp;
    _size = size;
  }

  // Immediate
  inline Op(SysInt imm)
  {
    _op[0] = OP_IMM;
    _op[1] = 0xFF;
    _op[2] = 0xFF;
    _op[3] = 0xFF;
    _val = imm;
#if defined(ASMJIT_X86)
    _size = 4;
#else
    _size = isInt32(imm) ? 4 : 8;
#endif // ASMJIT
  }

  // [Operand]

  inline int op() const
  {
    return _op[0];
  }

  // [Size]

  inline int size() const
  {
    return _size;
  }

  // [Register]

  inline int reg() const
  {
    ASMJIT_ASSERT(op() == OP_REG);
    return _op[1];
  }

  inline int regType() const
  {
    ASMJIT_ASSERT(op() == OP_REG);
    return _op[1] & REGTYPE_MASK;
  }

  inline int regCode() const
  {
    ASMJIT_ASSERT(op() == OP_REG);
    return _op[1] & REGCODE_MASK;
  }

  inline int regSize() const
  {
    ASMJIT_ASSERT(op() == OP_REG);
    return 1 << (_op[1] >> 4);
  }

  // [Memory]

  inline bool hasIndex() const
  {
    return _op[2] != 0xFF;
  }

  inline int baseRegCode() const
  {
    ASMJIT_ASSERT(op() == OP_MEM);
    return _op[1] & REGCODE_MASK;
  }

  inline int indexRegCode() const
  {
    ASMJIT_ASSERT(op() == OP_MEM);
    return _op[2] & REGCODE_MASK;
  }

  inline int indexShift() const
  {
    ASMJIT_ASSERT(op() == OP_MEM);
    return _op[3];
  }

  inline SysInt disp() const
  {
    ASMJIT_ASSERT(op() == OP_MEM);
    return _val;
  }

  // [Immediate]

  inline SysInt imm() const
  {
    ASMJIT_ASSERT(op() == OP_IMM);
    return _val;
  }

  // [Members]

  //! @brief Base data. Only _op[0] is consistent, _op[1-3] is dependent
  //! to operation type.
  //!
  //! _op[0] Type of operand (consistent)
  //!
  //! _op[1] OP_REG: Register if operand is register. 
  //!        OP_MEM: Base register for memory operand.
  //!        OP_IMM: Not used (0xFF)
  //!
  //! _op[2] OP_REG: Not used (0xFF)
  //!        OP_MEM: Index register for memory operand
  //!        OP_IMM: Not used (0xFF)
  //!
  //! _op[3] OP_REG: Not used (0xFF)
  //!        OP_MEM: Shift for memory operand
  //!        OP_IMM: Not used (0xFF)
  //!
  //! @sa @c Op
  UInt8 _op[4];

  //! @brief Value depends to operand type:
  //!
  //! OP_REG: Not used (-1)
  //! OP_MEM: Displacement
  //! OP_IMM: Immediate (8 bit or 32 bit value)
  SysInt _val;

  //! @brief Size of memory pointer, register or immediate in BYTES.
  UInt32 _size;
};

// [base + displacement]

//! @brief Create pointer operand with not specified size.
inline Op ptr(const Register& base, SysInt disp = 0){ return Op(base, disp, 0); }
//! @brief Create byte pointer operand.
inline Op byte_ptr(const Register& base, SysInt disp = 0){ return Op(base, disp, SIZE_BYTE); }
//! @brief Create word pointer operand.
inline Op word_ptr(const Register& base, SysInt disp = 0) { return Op(base, disp, SIZE_WORD); }
//! @brief Create dword pointer operand.
inline Op dword_ptr(const Register& base, SysInt disp = 0) { return Op(base, disp, SIZE_DWORD); }
//! @brief Create qword pointer operand.
inline Op qword_ptr(const Register& base, SysInt disp = 0) { return Op(base, disp, SIZE_QWORD); }
//! @brief Create dqword pointer operand.
inline Op dqword_ptr(const Register& base, SysInt disp = 0) { return Op(base, disp, SIZE_DQWORD); }

// [base + (index << shift) + displacement]

//! @brief Create byte pointer operand with not specified size.
inline Op ptr(const Register& base, const Register& index, Int32 shift, SysInt disp = 0) { return Op(base, index, shift, disp, 0); }
//! @brief Create byte pointer operand.
inline Op byte_ptr(const Register& base, const Register& index, Int32 shift, SysInt disp = 0) { return Op(base, index, shift, disp, SIZE_BYTE); }
//! @brief Create word pointer operand.
inline Op word_ptr(const Register& base, const Register& index, Int32 shift, SysInt disp = 0) { return Op(base, index, shift, disp, SIZE_WORD); }
//! @brief Create dword pointer operand.
inline Op dword_ptr(const Register& base, const Register& index, Int32 shift, SysInt disp = 0) { return Op(base, index, shift, disp, SIZE_DWORD); }
//! @brief Create qword pointer operand.
inline Op qword_ptr(const Register& base, const Register& index, Int32 shift, SysInt disp = 0) { return Op(base, index, shift, disp, SIZE_QWORD); }
//! @brief Create dqword pointer operand.
inline Op dqword_ptr(const Register& base, const Register& index, Int32 shift, SysInt disp = 0) { return Op(base, index, shift, disp, SIZE_DQWORD); }

//! @brief Create immediate value operand.
inline Op imm(SysInt imm) { return Op(imm); }

//! @brief X86 Assembler.
//!
//! This class is the main class in AsmJit for generating X86 binary. In 
//! creates internal buffer, where opcodes are stored (don't worry about it,
//! it's auto growing buffer) and contains methods for generating opcodes
//! with compile time and runtime time (DEBUG) checks. Buffer is allocated 
//! first time you emit instruction. Newly constructed instance never allocates
//! any memory (in constructor).
//!
//! Buffer is allocated by @c ASMJIT_MALLOC, @c ASMJIT_REALLOC and 
//! freed by @c ASMJIT_FREE macros. It's designed to fit your memory 
//! management model and not to dictate you own. Default functions are from
//! standard C library - malloc, realloc and free.
//!
//! While you are over from emitting instructions, you can get size of code
//! by codeSize() or offset() methods. These methods returns you code size
//! (or more precisely current code offset) in bytes. Use takeCode() to take
//! internal buffer (all pointers in X86 instance will be zeroed and current
//! buffer returned) to use it. If you don't take it, X86 destructor will
//! free it. To run code, don't use malloc()'ed memory, but use 
//! @c AsmJit::VM::alloc() to get memory for executing (specify canExecute to
//! true). Code generated from X86 can be memcpy'ed to that buffer and called.
//!
//! To generate instruction stream, use methods provided by X86 class. For 
//! example to generate X86 function, you need to do this:
//!
//! @verbatim
//! using namespace AsmJit;
//!
//! // Create function by dynamic way (32 bit code)
//! X86 a;
//!
//! // int f() - prolog
//! a.push(ebp);
//! a.mov(ebp, esp);
//!
//! // Mov 1024 to EAX, EAX is also return value.
//! a.mov(eax, imm(1024));
//!
//! // int f() - epilog
//! a.mov(esp, ebp);
//! a.pop(ebp);
//! a.ret();
//!
//! @endverbatim
//!
//! Code generated by this function can be executed by this way:
//!
//! @verbatim
//! // Alloc execute enabled memory and call generated function, vsize will
//! // contain size of allocated virtual memory block.
//! SysInt vsize;
//! void *vmem = VM::alloc(a.codeSize(), &vsize, true /* canExecute */);
//!
//! // Copy code to vmem. Code should be position indenpendent, if not,
//! // use reloc().
//! memcpy(vmem, a.pData, a.codeSize());
//!
//! // Cast vmem to void() function and call it
//! reinterpret_cast<void (*)()>(vmem)();
//!
//! // Memory should be freed, but use VM::free() to do that.
//! VM::free(vmem, vsize);
//! @endverbatim
//!
//! NOTE: This was very primitive example how to call generated code. In
//! real production code you will never alloc and free code for one run,
//! you will alloc memory, where you copy generated code, and you will run
//! it many times from that place.
//!
//! Using labels
//!
//! While generating assembler code, you will usually need to create complex
//! code with labels. Labels are fully supported and you can call jump or j..
//! instructions to yet uninitialized label. Each label expects to be bound
//! into offset. To bind label to specific offset, use @c bind() methos.
//!
//! @verbatim
//! // Example: Usage of Label (32 bit code)
//! //
//! // Create simple DWORD memory copy function:
//! // STDCALL void copy32(void* DST, const void* SRC, sysuint_t COUNT);
//! using namespace AsmJit;
//!
//! // X86 assembler
//! X86 a;
//! 
//! // Constants
//! const int arg_offset = 8; // Arguments offset (STDCALL EBP)
//! const int arg_size = 12;  // Arguments size
//! 
//! // Labels
//! Label L_Loop;
//! 
//! // Prolog
//! a.push(ebp);
//! a.mov(ebp, esp);
//! a.push(esi);
//! a.push(edi);
//! 
//! // Fetch arguments
//! a.mov(esi, dword_ptr(ebp, arg_offset + 0)); // get DST
//! a.mov(edi, dword_ptr(ebp, arg_offset + 4)); // get SRC
//! a.mov(ecx, dword_ptr(ebp, arg_offset + 8)); // get COUNT
//! 
//! // Bind L_Loop label to here
//! a.bind(&L_Loop);
//! 
//! a.mov(eax, dword_ptr(edi));
//! a.mov(dword_ptr(esi), eax);
//! 
//! // Repeat loop until ecx == 0
//! a.dec(ecx);
//! // See @c CONDITION codes, if ecx != 0, jump to L_Loop
//! a.j(C_ZERO, &L_Loop);
//! 
//! // Epilog
//! a.pop(edi);
//! a.pop(esi);
//! a.mov(esp, ebp);
//! a.pop(ebp);
//! 
//! // Return: STDCALL convention is to pop stack in called function
//! a.ret(arg_size);
//! @endverbatim
struct ASMJIT_API X86
{
  // -------------------------------------------------------------------------
  // [Construction / Destruction]
  // -------------------------------------------------------------------------

  //! @brief Creates X86 jit assembler instance.
  X86();
  //! @brief Destroys X86 jit assembler instance
  ~X86();

  // -------------------------------------------------------------------------
  // [Constants (for OPCODE generation)]
  // -------------------------------------------------------------------------

  //! @brief Valid X86 register IDs.
  //!
  //! These codes are real, don't miss with @c REG enum!
  enum R
  {
    R_EAX = 0,
    R_ECX = 1,
    R_EDX = 2,
    R_EBX = 3,
    R_ESP = 4,
    R_EBP = 5,
    R_ESI = 6,
    R_EDI = 7,
    // to check if register is invalid, use reg >= R_INVALID
    R_INVALID = 8
  };

  // -------------------------------------------------------------------------
  // [Internal Buffer]
  // -------------------------------------------------------------------------

  //! @brief Ensure space for next instruction
  inline bool ensureSpace() { return (pCur >= pMax) ? grow() : true; }
  //! @brief Return size of currently generated code.
  inline SysInt codeSize() const { return (SysInt)(pCur - pData); }
  //! @brief Return current offset in buffer (same as codeSize()).
  inline SysInt offset() const { return (SysInt)(pCur - pData); }
  //! @brief Return capacity of internal code buffer.
  inline SysInt capacity() const { return _capacity; }

  //! @brief Reallocate internal buffer.
  //!
  //! It's only used for growing, buffer is never reallocated to smaller 
  //! number than current capacity() is.
  bool realloc(SysInt to);
  //! @brief Used to grow the buffer.
  //!
  //! It will typically realloc to twice size of capacity(), but if capacity()
  //! is large, it will use smaller steps.
  bool grow();
  //! @brief Free internal buffer and NULL all pointers.
  void free();
  //! @brief Return internal buffer and NULL all pointers.
  UInt8* takeCode();

  // -------------------------------------------------------------------------
  // [Setters / Getters]
  // -------------------------------------------------------------------------

  //! @brief Return byte at position @a pos.
  inline UInt8 getByteAt(SysInt pos)
  {
    return *reinterpret_cast<UInt8*>(pData + pos);
  }

  //! @brief Return integer (dword size) at position @a pos.
  inline UInt32 getIntAt(SysInt pos)
  {
    return *reinterpret_cast<UInt32*>(pData + pos);
  }

  //! @brief Displacement at position @a pos.
  inline Displacement getDispAt(Label* L)
  {
    return Displacement(getIntAt(L->pos()));
  }

  //! @brief Set byte at position @a pos.
  inline void setByteAt(SysInt pos, int imm8)
  {
    *reinterpret_cast<UInt8*>(pData + pos) = imm8;
  }

  //! @brief Set integer (dword size) at position @a pos.
  inline void setIntAt(SysInt pos, int imm32)
  {
    *reinterpret_cast<int*>(pData + pos) = imm32;
  }

  //! @brief Set displacement at label @a L position.
  inline void setDispAt(Label* L, Displacement disp)
  {
    setIntAt(L->pos(), disp.data());
  }

  // -------------------------------------------------------------------------
  // Emitters 
  //
  // These emitters are not protecting buffer from overrun, this must be 
  // done for each instruction by this:
  //   if (!ensureSpace()) return;
  // -------------------------------------------------------------------------

  //! @brief Emit Byte to internal buffer.
  inline void emitByte(UInt8 x)
  {
    *pCur++ = x;
  }

  //! @brief Emit Word (2 bytes) to internal buffer.
  inline void emitWord(UInt16 x)
  {
    *(UInt16 *)pCur = x;
    pCur += 2;
  }

  //! @brief Emit DWord (4 bytes) to internal buffer.
  inline void emitDWord(UInt32 x)
  {
    *(UInt32 *)pCur = x;
    pCur += 4;
  }

  //! @brief Emit QWord (8 bytes) to internal buffer.
  inline void emitQWord(UInt64 x)
  {
    *(UInt64 *)pCur = x;
    pCur += 8;
  }

  //! @brief Emit immediate operand of Byte size.
  inline void emitByteImmediate(const Op& imm)
  {
    ASMJIT_ASSERT(imm.op() == OP_IMM);
    emitByte(imm.imm());
  }

  //! @brief Emit immediate operand of Word size.
  inline void emitWordImmediate(const Op& imm)
  {
    ASMJIT_ASSERT(imm.op() == OP_IMM);
    emitWord(imm.imm());
  }

  //! @brief Emit immediate operand of DWord size.
  inline void emitDWordImmediate(const Op& imm)
  {
    ASMJIT_ASSERT(imm.op() == OP_IMM);
    emitDWord(imm.imm());
  }

  //! @brief Emit immediate operand of DWord size.
  inline void emitQWordImmediate(const Op& imm)
  {
    ASMJIT_ASSERT(imm.op() == OP_IMM);
    emitQWord(imm.imm());
  }

  //! @brief Emit MODR/M byte.
  inline void emitMod(UInt8 m, UInt8 o, UInt8 r)
  {
    *pCur++ = ((m & 0x03) << 6) | ((o & 0x07) << 3) | (r & 0x07);
  }

  //! @brief Emit Register / Register - calls emitMod(3, o, r)
  inline void emitReg(UInt8 o, UInt8 r)
  {
    emitMod(3, o, r);
  }


#if 0
	void if16bit(const Operand& reg1, const Operand& reg2)
	{
		// except movsx(16bit, 32/64bit)
		if ((reg1.isBit(16) && !reg2.isBit(i32e)) || (reg2.isBit(16) && !reg1.isBit(i32e))) db(0x66);
	}
	void rexAddr(const Address& addr, const Reg& reg = Reg())
	{
#ifdef XBYAK64
		if (addr.is32bit_) db(0x67);
#endif
		if16bit(reg, addr);
		uint32 rex = addr.getRex() | reg.getRex();
		if (reg.isREG(64)) rex |= 0x48;
		if (rex) db(rex);
	}
	void rex(const Operand& op1, const Operand& op2 = Operand())
	{
		if (op1.isMEM()) {
			rexAddr(static_cast<const Address&>(op1), static_cast<const Reg&>(op2));
		} else if (op2.isMEM()) {
			rexAddr(static_cast<const Address&>(op2), static_cast<const Reg&>(op1));
		} else {
			const Reg& reg1 = static_cast<const Reg&>(op1);
			const Reg& reg2 = static_cast<const Reg&>(op2);
			// ModRM(reg, base);
			if16bit(reg1, reg2);
			uint8 rex = reg2.getRex(Reg(), reg1);
			if (reg1.isREG(64) || reg2.isREG(64)) rex |= 0x48;
			if (rex) db(rex);
		}
	}
#endif

#if defined(ASMJIT_X64)
  //! @brief Emits REX.W prefix in 64 bit mode.
  //!
  //! @param w Default operand size(0=Default, 1=64 bits).
  //! @param r Register field (1=high bit extension of the ModR/M REG field).
  //! @param x Index field (1=high bit extension of the SIB Index field).
  //! @param b Base field (1=high bit extension of the ModR/M or SIB Base field).
  //!
  //! @internal
  void _emitRex(UInt8 w, UInt8 r, UInt8 x, UInt8 b)
  {
    if (w || r || x || b)
    {
      emitByte(0x40 | ((!!w) << 3) | ((!!r) << 2) | ((!!x) << 1) | (!!b));
    }
  }
#endif // ASMJIT_X64

  //! @brief Emits REX.W prefix in 64 bit mode.
  //!
  //! This is convenience method that internally uses @c _emitRex().
  //!
  //! @internal
  void emitRex(UInt8 w, int dstCode, const Op& op)
  {
#if defined(ASMJIT_X64)
    if (op.op() == OP_REG)
    {
      _emitRex(w, dstCode & 0x8, 0 /* ignored */, op.regCode() & 0x8);
    }
    else if (op.op() == OP_MEM)
    {
      _emitRex(w, dstCode & 0x8, op.indexRegCode() & 0x8, op.baseRegCode() & 0x8);
    }
#endif // ASMJIT_X64
  }

  //! @brief Emits Register / Memory.
  //! @internal.
  void _emitMem(UInt8 o, Int32 disp)
  {
    emitMod(0, o, 5);
    emitDWord(disp);
  }

  //! @brief Emit register / memory address.
  //! @internal.
  //!
  //! This function is called internally from @c emitMem(). It has no sence
  //! to call it manually.
  void _emitMem(UInt8 o, UInt8 baseReg, Int32 disp)
  {
    if (baseReg == R_ESP)
    {
      if (disp == 0)
      {
        emitMod(0, o, R_ESP);
        emitMod(0, R_ESP, R_ESP);
      }
      else if (isInt8(disp))
      {
        emitMod(1, o, R_ESP);
        emitMod(0, R_ESP, R_ESP);
        emitByte(disp);
      }
      else
      {
        emitMod(2, o, R_ESP);
        emitMod(0, R_ESP, R_ESP);
        emitDWord(disp);
      }
    }
    else if (baseReg != R_EBP && disp == 0)
    {
      emitMod(0, o, baseReg);
    }
    else if (isInt8(disp))
    {
      emitMod(1, o, baseReg);
      emitByte(disp);
    }
    else
    {
      emitMod(2, o, baseReg);
      emitDWord(disp);
    }
  }

  //! @brief Emit register / memory address.
  //! @internal.
  //!
  //! This function is called internally from @c emitMem(). It has no sence
  //! to call it manually.
  void _emitMem(UInt8 o, UInt8 baseReg, Int32 disp, UInt8 indexReg, int shift)
  {
    if (baseReg >= R_INVALID)
    {
      emitMod(0, o, 4);
      emitMod(shift, indexReg, 5);
      emitDWord(disp);
    }
    else if (disp == 0 && baseReg != R_EBP)
    {
      emitMod(0, o, 4);
      emitMod(shift, indexReg, baseReg);
    }
    else if (isInt8(disp))
    {
      emitMod(1, o, 4);
      emitMod(shift, indexReg, baseReg);
      emitByte(disp);
    }
    else
    {
      emitMod(2, o, 4);
      emitMod(shift, indexReg, 5);
      emitDWord(disp);
    }
  }

  //! @brief Emit register / memory address combination to buffer.
  //! 
  //! This method can hangle addresses from simple to complex ones with 
  //! index and displacement.
  //!
  //! @note mem Operand must be @c OP_MEM type.
  void emitMem(UInt8 o, const Op& mem)
  {
    ASMJIT_ASSERT(mem.op() == OP_MEM);

    if (mem.hasIndex())
      _emitMem(o, mem.baseRegCode(), mem.disp(), mem.indexRegCode(), mem.indexShift());
    else
      _emitMem(o, mem.baseRegCode(), mem.disp());
  }

  //! @brief Emit register / register or register / memory to buffer.
  //!
  //! This function internally calls @c emitMem() or emitReg() that depends
  //! to @a op type.
  //!
  //! @note @a o is usually real register ID (see @c R) but some instructions
  //! have specific format and in that cases @a o is constant from opcode
  //! documentation.
  void emitOp(UInt8 o, const Op& op)
  {
    if (op.op() == OP_REG)
      emitReg(o, op.regCode());
    else if (op.op() == OP_MEM)
      emitMem(o, op);
    else
      ASMJIT_ILLEGAL();
  }
#if 0
  //! @brief Emit register / immediate or memory / immediate to buffer.
  void emitImm(UInt8 o, const Op& dst, const Op& imm)
  {
    // reg (BYTE) <- imm
    if (dst.op() == OP_REG && dst.regType() == REG_GPB)
    {
      // short form if the destination is 'al'.
      ASMJIT_ASSERT(isInt8(imm.imm()));

      if (dst.reg() == REG_AL)
      {
        // short form if the destination is 'al'.
        emitByte((o << 3) | 0x04);
        emitByte(imm.imm() & 0xFF);
      }
      else
      {
        emitByte(0x80);
        emitReg(o, dst.regCode());
        emitByte(imm.imm() & 0xFF);
      }
      return;
    }

    // reg (DWORD) <- imm
    if (dst.op() == OP_REG && dst.regType() == REG_GPD)
    {
      if (dst.reg() == REG_EAX)
      {
        // short form if the destination is 'eax'.
        emitByte((o << 3) | 0x05);
        emitDWordImmediate(imm);
      }
      else
      {
        emitByte(0x81);
        emitReg(o, dst.regCode());
        emitDWordImmediate(imm);
      }
      return;
    }

    // Mem <- Imm
    if (isInt8(imm.imm()))
    {
      // using a sign-extended 8-bit immediate.
      emitByte(0x83);
      emitOp(o, dst);
      emitByte(imm.imm() & 0xFF);
    }
    else
    {
      // using a literal 32-bit immediate.
      emitByte(0x81);
      emitOp(o, dst);
      emitDWordImmediate(imm);
    }
  }
#endif
  //! @brief Emit complete bit operation.
  //!
  //! Used for RCL, RCR, ROL, ROR, SAL, SAR, SHL, SHR.
  void _emitBitArith(const Op& dst, const Op& src, UInt8 ModR)
  {
    ASMJIT_ASSERT((dst.op() == OP_REG) ||
                  (dst.op() == OP_MEM));
    ASMJIT_ASSERT((src.op() == OP_REG && src.reg() == REG_CL) ||
                  (src.op() == OP_IMM));

    if (!ensureSpace()) return;

    // generate opcode. For these operations is base 0xC0 or 0xD0.
    bool useImm8 = (src.op() == OP_IMM && src.imm() != 1);
    UInt8 opCode = useImm8 ? 0xC0 : 0xD0;

    // size and operand type modifies the opcode
    if (dst.size() != 1) opCode |= 0x01;
    if (src.op() == OP_REG) opCode |= 0x02;

    if (dst.size() == 2) emitByte(0x66); // 16 bit
    emitRex(dst.size() == 8, ModR, dst);
    emitByte(opCode);
    emitOp(ModR, dst);
    if (useImm8) emitByte(src.imm());
  }

  //! @brief Emits SHLD or SHRD instruction.
  void _emitShldShrd(const Op& dst, const Register& src1, const Op& src2, UInt8 opCode)
  {
    ASMJIT_ASSERT(dst .op() == OP_MEM || (dst .op() == OP_REG));
    ASMJIT_ASSERT(src2.op() == OP_IMM || (src2.op() == OP_REG && src2.reg() == REG_CL));
    ASMJIT_ASSERT(dst.size() == src1.regSize());

    if (src1.regCode() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(src1.regType() == REG_GPQ, src1.regCode(), dst);
#endif // ASMJIT_X64
    emitByte(0x0F);
    emitByte(opCode + (src2.op() == OP_REG));
    emitOp(src1.regCode(), dst);
    if (src2.op() == OP_IMM) emitByte((UInt8)src2.imm());
  }

  //! @brief Emit complete arithmetic operation.
  //!
  //! This function is used from small number of functions to emit arithmetic
  //! instruction (for example @c adc(), @c add(), @c or_()).
  void _emitArithOp(const Op& _dst, const Op& _src, UInt8 opcode, UInt8 o)
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
            emitRex(0, dst->regCode(), *src);
#endif // ASMJIT_X64
            emitByte(opcode + 2);
            emitOp(dst->regCode(), *src);
            return;
          case REG_GPW:
            emitByte(0x66); // 16 bit prefix
            // ... fall through ...
          case REG_GPD:
#if defined(ASMJIT_X64)
            emitRex(0, dst->regCode(), *src);
#endif // ASMJIT_X64
            emitByte(opcode + 3);
            emitOp(dst->regCode(), *src);
            return;
#if defined(ASMJIT_X64)
          case REG_GPQ:
            emitRex(1, dst->regCode(), *src);
            emitByte(opcode + 3);
            emitOp(dst->regCode(), *src);
            return;
#endif // ASMJIT_X86
        }
        break;

      // Reg <- Imm
      case (OP_REG << 4) | OP_IMM:
      {
        bool imm8Bit = isInt8(src->imm());

        // AL, AX, EAX, RAX register shortcuts
        switch (dst->reg())
        {
          // TODO: Check for REX prefixes here!
          case REG_AL:
            // short form if the destination is 'al'.
            emitByte((o << 3) | 0x04);
            emitByte((Int8)src->imm());
            return;
          case REG_AX:
            emitByte(0x66); // 16 bit
            emitByte((o << 3) | 0x05);
            emitWord((Int16)src->imm());
            return;
          case REG_EAX:
            emitByte((o << 3) | 0x05);
            emitDWord((Int32)src->imm());
            return;
#if defined(ASMJIT_X64)
          case REG_RAX:
            emitByte(0x48); // REX.W
            emitByte((o << 3) | 0x05);
            emitDWord((Int32)src->imm());
            return;
#endif // ASMJIT_X64
        }
      }

      // ... fall through ...

      // Reg/Mem <- Imm
      case (OP_MEM << 4) | OP_IMM:
      {
        bool imm8Bit = isInt8(src->imm());

        // all others
        switch (dst->size())
        {
          case 1:
#if defined(ASMJIT_X64)
            emitRex(0, o, *dst); // REX
#endif // ASMJIT_X64
            emitByte(0x80);
            emitOp(o, *dst);
            emitByteImmediate(*src);
            return;

          case 2:
            emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
            emitRex(0, o, *dst); // REX
#endif // ASMJIT_X64
            emitByte(imm8Bit ? 0x03 : 0x81);
            emitOp(o, *dst);
            if (imm8Bit)
              emitByteImmediate(*src);
            else
              emitWordImmediate(*src);
            return;

          case 4:
#if defined(ASMJIT_X64)
            emitRex(0, o, *dst); // REX
#endif // ASMJIT_X64
            emitByte(imm8Bit ? 0x83 : 0x81);
            emitOp(o, *dst);
            if (imm8Bit)
              emitByteImmediate(*src);
            else
              emitDWordImmediate(*src);
            return;
#if defined(ASMJIT_X64)
          case 8:
            emitRex(1, o, *dst); // REX.W
            emitByte(imm8Bit ? 0x83 : 0x81);
            emitOp(o, *dst);
            if (imm8Bit)
              emitByteImmediate(*src);
            else
              emitDWordImmediate(*src);
            return;
#endif // ASMJIT_X64
        }

        break;
      }
    }

    ASMJIT_ILLEGAL();
  }

  void emitArithFP(int opcode1, int opcode2, int i)
  {
    // wrong opcode
    ASMJIT_ASSERT(isUInt8(opcode1) && isUInt8(opcode2));
    // illegal stack offset
    ASMJIT_ASSERT(0 <= i && i < 8);

    emitByte(opcode1);
    emitByte(opcode2 + i);
  }

  //! @brief Emits MMX/SSE instruction in form: inst dst, src or [src...]
  void emitMM(UInt8 prefix1, UInt8 prefix2, UInt8 opcode1, UInt8 opcode2, int dstCode, const Op& src)
  {
    if (!ensureSpace()) return;

    // Prefixes, we are using prefix1 and prefix2 as prefixes, but in fact
    // MM-X86 instruction has only one prefix. In documentation is also that
    // instruction has one or two opcodes, but never instructions has more.
#if defined(ASMJIT_X86)
    // no rex in 32 bit mode
    if (prefix1) emitByte(prefix1);
    if (prefix2) emitByte(prefix2);
#endif // ASMJIT_X86

    // TODO: Verify!
#if defined(ASMJIT_X64)
    if (prefix1) emitByte(prefix1);
    if (prefix2) emitByte(prefix2);
    emitRex(1, dstCode, src);
#endif // ASMJIT_X64

    emitByte(opcode1);
    emitByte(opcode2);
    emitOp(dstCode, src);
  }

  //! @brief Emits MMX/SSE instruction in form: inst dst, src or [src], imm8
  void emitMM(UInt8 prefix1, UInt8 prefix2, UInt8 opcode1, UInt8 opcode2, int dstCode, const Op& src, int imm8)
  {
    if (!ensureSpace()) return;
    emitMM(prefix1, prefix2, opcode1, opcode2, dstCode, src);
    emitByte(imm8 & 0xFF);
  }

  void emitDisp(Label* L, Displacement::Type type)
  {
    Displacement disp(L, type);
    L->linkTo(offset());
    emitDWord(static_cast<SysInt>(disp.data()));
  }

  // -------------------------------------------------------------------------
  // [Helpers]
  // -------------------------------------------------------------------------

  //! @brief Align buffer to @a m bytes.
  //!
  //! Typical usage of this is to align labels at start of inner loops.
  //!
  //! Inserts @c nop() instructions.
  void align(int m)
  {
    if (!ensureSpace()) return;
    ASMJIT_ASSERT(m == 1 || m == 2 || m == 4 || m == 8 || m == 16);
    while ((offset() & (m - 1)) != 0) nop();
  }

  // -------------------------------------------------------------------------
  // [Instructions]
  // -------------------------------------------------------------------------

  //! @brief Add with Carry.
  //!
  //! This instruction adds the destination operand (first operand), the source 
  //! operand (second operand), and the carry (CF) flag and stores the result in 
  //! the destination operand. The destination operand can be a register or a 
  //! memory location; the source operand can be an immediate, a register, or a 
  //! memory location. (However, two memory operands cannot be used in one 
  //! instruction.) The state of the CF flag represents a carry from a previous 
  //! addition. When an immediate value is used as an operand, it is 
  //! sign-extended to the length of the destination operand format.
  //!
  //! Processor evaluates the result for both data types and sets the OF and CF 
  //! flags to indicate a carry in the signed or unsigned result, respectively.
  //! The SF flag indicates the sign of the signed result. The ADC instruction 
  //! is usually executed as part of a multibyte or multiword addition in which
  //! an ADD instruction is followed by an ADC instruction.
  void adc(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x0A, 2);
  }

  //! @brief Add.
  //!
  //! This instruction adds the first operand (destination operand) and the 
  //! second operand (source operand) and stores the result in the destination 
  //! operand. The destination operand can be a register or a memory location. 
  //! The source operand can be an immediate, a register, or a memory location.
  //! (However, two memory operands cannot be used in one instruction.) When an 
  //! immediate value is used as an operand, it is sign-extended to the length 
  //! of the destination operand format.
  //!
  //! The ADD instruction does not distinguish between signed or unsigned 
  //! operands. Instead, the processor evaluates the result for both data 
  //! types and sets the OF and CF flags to indicate a carry in the signed 
  //! or unsigned result, respectively. The SF flag indicates the sign of 
  //! the signed result.
  void add(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x00, 0);
  }

  //! @brief Logical AND.
  //!
  //! This instruction performs a bitwise AND operation on the destination 
  //! (first) and source (second) operands and stores the result in the 
  //! destination operand location. The source operand can be an immediate, 
  //! a register, or a memory location; the destination operand can be a 
  //! register or a memory location. Two memory operands cannot, however, 
  //! be used in one instruction. Each bit of the instruction result is a 
  //! 1 if both corresponding bits of the operands are 1; otherwise, it 
  //! becomes a 0.
  void and_(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x20, 4);
  }

  //! @brief Byte swap (in 4 byte register) (i486).
  //!
  //! This instruction reverses the byte order of a 32-bit (destination) 
  //! register: bits 0 through 7 are swapped with bits 24 through 31, 
  //! and bits 8 through 15 are swapped with bits 16 through 23. This 
  //! instruction is provided for converting little-endian values to 
  //! big-endian format and vice versa.
  //! 
  //! To swap bytes in a word value (16-bit register), use the XCHG 
  //! instruction. When the BSWAP instruction references a 16-bit register,
  //! the result is undefined.
  void bswap(const Register& reg)
  {
    ASMJIT_ASSERT(reg.regType() == REG_GPD || reg.regType() == REG_GPQ);
    if (!ensureSpace()) return;

#if defined ASMJIT_X64
    emitRex(reg.regType() == REG_GPQ, 1, reg.regCode());
#endif // ASMJIT_X64
    emitByte(0x0F);
    emitByte(0xC8 | (reg.regCode() & 0x7));
  }

  //! @brief Bit test and set.
  void bts(const Op& dst, Register src)
  {
    if (!ensureSpace()) return;
#if defined ASMJIT_X64
    emitRex(src.regType() == REG_GPQ, src.regCode(), dst);
#endif // ASMJIT_X64
    emitByte(0x0F);
    emitByte(0xAB);
    emitOp(src.regCode(), dst);
  }

  //! @brief Call Procedure.
  //!
  //! This instruction saves procedure linking information on the stack 
  //! and branches to the procedure (called procedure) specified with 
  //! the destination (target) operand. The target operand specifies
  //! the address of the first instruction in the called procedure. 
  //! This operand can be an immediate value, a general-purpose register, 
  //! or a memory location.
  void call(const Op& adr)
  {
    ASMJIT_ASSERT(adr.op() != OP_IMM);
    if (!ensureSpace()) return;

    emitByte(0xFF);
    emitOp(2, adr);
  }

  //! @brief Call Procedure.
  //! @overload
  void call(Label* L)
  {
    if (!ensureSpace()) return;
    if (L->isBound())
    {
      const int long_size = 5;
      SysInt offs = L->pos() - offset();
      ASMJIT_ASSERT(offs <= 0);
      // 1110 1000 #32-bit disp
      emitByte(0xE8);
      emitDWord(offs - long_size);
    }
    else
    {
      // 1110 1000 #32-bit disp
      emitByte(0xE8);
      emitDisp(L, Displacement::OTHER);
    }
  }

  //! @brief Convert Byte to Word (Sign Extend).
  //!
  //! AX <- Sign Extend AL
  void cbw() 
  {
    if (!ensureSpace()) return;
    emitByte(0x66);
    emitByte(0x99); 
  }

  //! @brief Convert Word to DWord (Sign Extend).
  //!
  //! EAX <- Sign Extend AX
  void cwde() 
  {
    if (!ensureSpace()) return;
    emitByte(0x99); 
  }

#if defined(ASMJIT_X64)
  //! @brief Convert DWord to QWord (Sign Extend).
  void cdqe() 
  {
    if (!ensureSpace()) return;
    emitByte(0x48);
    emitByte(0x99); 
  }
#endif // ASMJIT_X64

  //! @brief Clear CARRY flag
  //!
  //! This instruction clears the CF flag in the EFLAGS register.
  void clc()
  {
    if (!ensureSpace()) return;
    emitByte(0xF8);
  }

  //! @brief Clear Direction flag
  //!
  //! This instruction clears the DF flag in the EFLAGS register.
  void cld()
  {
    if (!ensureSpace()) return;
    emitByte(0xFC);
  }

  //! @brief Complement Carry Flag.
  //!
  //! This instruction complements the CF flag in the EFLAGS register.
  //! (CF = NOT CF)
  void cmc()
  {
    if (!ensureSpace()) return;
    emitByte(0xF5); 
  }

  //! @brief Conditional Move.
  //!
  //! The CMOVcc instructions check the state of one or more of the status 
  //! flags in the EFLAGS register (CF, OF, PF, SF, and ZF) and perform 
  //! a move operation if the flags are in a specified state (or condition). 
  //! A condition code (cc) is associated with each instruction to indicate the
  //! condition being tested for. If the condition is not satisfied, a move is 
  //! not performed and execution continues with the instruction following the 
  //! CMOVcc instruction. 
  //!
  //! These instructions can move a 16- or 32-bit value from memory to a 
  //! general-purpose register or from one general-purpose register to 
  //! another. Conditional moves of 8-bit register operands are not supported.
  void cmov(CONDITION cc, const Register& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(dst.regType() == REG_GPQ, dst.regCode(), src);
#endif // ASMJIT_X64
    emitByte(0x0F);
    emitByte(0x40 | cc);
    emitOp(dst.regCode(), src);
  }

  //! @brief Compare Two Operands.
  //!
  //! This instruction compares the first source operand with the second source
  //! operand and sets the status flags in the EFLAGS register according to the 
  //! results. The comparison is performed by subtracting the second operand 
  //! from the first operand and then setting the status flags in the same 
  //! manner as the SUB instruction. When an immediate value is used as an 
  //! operand, it is signextended to the length of the first operand.
  //!
  //! The CMP instruction is typically used in conjunction with a conditional 
  //! jump (Jcc), condition move (CMOVcc), or SETcc instruction. The condition 
  //! codes used by the Jcc, CMOVcc, and SETcc instructions are based on the 
  //! results of a CMP instruction. Appendix B, EFLAGS Condition Codes, in the 
  //! Intel Architecture Software Developer’s Manual, Volume 1, shows the 
  //! relationship of the status flags and the condition codes.
  void cmp(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x38, 7);
  }

  //! @brief Compare and Exchange (i486).
  //!
  //! This instruction compares the value in the AL, AX, or EAX register 
  //! (depending on the size of the operand) with the first operand 
  //! (destination operand). If the two values are equal, the second operand 
  //! (source operand) is loaded into the destination operand. Otherwise, the 
  //! destination operand is loaded into the AL, AX, or EAX register. 
  //!
  //! This instruction can be used with a LOCK prefix to allow the instruction 
  //! to be executed atomically. To simplify the interface to the processor’s
  //! bus,the destination operand receives a write cycle without regard to the 
  //! result of the comparison. The destination operand is written back if the
  //! comparison fails; otherwise, the source operand is written into the 
  //! destination. (The processor never produces a locked read without also 
  //! producing a locked write.)
  void cmpxchg(const Op& dst, const Register& src)
  {
    ASMJIT_ASSERT((dst.op() == OP_REG || dst.op() == OP_MEM) && dst.size() == src.regSize());
    if (!ensureSpace()) return;

    if (src.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(src.regType() == REG_GPQ, src.regCode(), dst);
#endif // ASMJIT_X64
    emitByte(0x0F);
    emitByte(0xB0 + (src.regType() != REG_GPB));
    emitOp(src.regCode(), dst);
  }

#if defined(ASMJIT_X86)
  //! @brief Compares the 64-bit value in EDX:EAX with the memory operand (Pentium).
  //!
  //! If the values are equal, then this instruction stores the 64-bit value 
  //! in ECX:EBX into the memory operand and sets the zero flag. Otherwise,
  //! this instruction copies the 64-bit memory operand into the EDX:EAX 
  //! registers and clears the zero flag.
  void cmpxchg8b(const Op& dst)
  {
    ASMJIT_ASSERT(dst.op() == OP_MEM);
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0xC7);
    emitMem(1, dst);
  }
#endif // ASMJIT_X86

#if defined(ASMJIT_X64)
  //! @brief Compares the 128-bit value in RDX:RAX with the memory operand.
  //!
  //! If the values are equal, then this instruction stores the 128-bit value 
  //! in RCX:RBX into the memory operand and sets the zero flag. Otherwise,
  //! this instruction copies the 128-bit memory operand into the RDX:RAX 
  //! registers and clears the zero flag.
  void cmpxchg16b(const Op& dst)
  {
    ASMJIT_ASSERT(dst.op() == OP_MEM);
    if (!ensureSpace()) return;
    emitRex(1, 1, dst);
    emitByte(0x0F);
    emitByte(0xC7);
    emitMem(1, dst);
  }
#endif // ASMJIT_X64

  //! @brief CPU Identification (i486).
  void cpuid()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0xA2);
  }

#if defined(ASMJIT_X86)
  //! @brief Decimal adjust AL after addition
  //!
  //! This instruction adjusts the sum of two packed BCD values to create
  //! a packed BCD result.
  //!
  //! @note This instruction is only available in 32 bit mode.
  void daa()
  {
    if (!ensureSpace()) return;
    emitByte(0x27);
  }
#endif // ASMJIT_X86

#if defined(ASMJIT_X86)
  //! @brief Decimal adjust AL after subtraction
  //!
  //! This instruction adjusts the result of the subtraction of two packed 
  //! BCD values to create a packed BCD result.
  //!
  //! @note This instruction is only available in 32 bit mode.
  void das()
  {
    if (!ensureSpace()) return;
    emitByte(0x2F);
  }
#endif // ASMJIT_X86

  //! @brief Decrement by 1.
  //!
  //! @note This instruction can be slower than sub(dst, 1)
  void dec(const Op& dst)
  {
    if (!ensureSpace()) return;

    // DEC [r16|r32] in 64 bit mode is not encodable.
#if defined(ASMJIT_X86)
    if ((dst.op() == OP_REG) && 
        (dst.regType() == REG_GPW || dst.regType() == REG_GPD))
    {
      if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
      emitByte(0x48 | (dst.regCode() & 7));
      return;
    }
#endif // ASMJIT_X86

    emitRex(dst.size() == 8, 1, dst);
    emitByte(0xFE + (dst.size() != 1));
    emitOp(1, dst);
  }

  //! @brief Unsigned divide.
  //!
  //! This instruction divides (unsigned) the value in the AL, AX, or EAX 
  //! register by the source operand and stores the result in the AX, 
  //! DX:AX, or EDX:EAX registers.
  void div(const Op& dst)
  {
    if (!ensureSpace()) return;
    if (dst.size() == 2) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(dst.size() == 8, 6, dst);
#endif // ASMJIT_X64
    emitByte(0xF6 + (dst.size() != 1));
    emitOp(6, dst);
  }

  //! @brief Compute 2^x - 1.
  //!
  //! This instruction calculates the exponential value of 2 to the power 
  //! of the source operand minus 1. The source operand is located in 
  //! register ST(0) and the result is also stored in ST(0). The value of the 
  //! source operand must lie in the range –1.0 to +1.0. If the source value is 
  //! outside this range, the result is undefined.
  void f2xm1()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xF0);
  }

  //! @brief Absolute Value.
  //!
  //! This instruction clears the sign bit of ST(0) to create the absolute 
  //! value of the operand.
  void fabs()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xE1);
  }

  //! @brief Add.
  void fadd(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDC, 0xC0, i);
  }

  //! @brief Add and POP register stack.
  void faddp(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDE, 0xC0, i);
  }

  //! @brief Load Binary Coded Decimal.
  //!
  //! This instruction converts the BCD source operand into extended-real 
  //! format and pushes the value onto the FPU stack. The source operand 
  //! is loaded without rounding errors. The sign of the source operand is 
  //! preserved, including that of -0. The packed BCD digits are assumed to 
  //! be in the range 0 through 9; the instruction does not check for invalid 
  //! digits (AH through FH). Attempting to load an invalid encoding produces an
  //! undefined result.
  void fbld(const Op& src)
  {
    if (!ensureSpace()) return;

    emitByte(0xDF);
    emitMem(4, src);
  }

  //! @brief Store BCD Integer and Pop.
  //!
  //! This instruction converts the value in the ST(0) register to an 18-digit 
  //! packed BCD integer, stores the result in the destination operand, and 
  //! pops the register stack. If the source value is a non-integral value, it 
  //! is rounded to an integer value, according to rounding mode specified by
  //! the RC field of the FPU control word. To pop the register stack, the 
  //! processor marks the ST(0) register as empty and increments the stack 
  //! pointer (TOP) by 1.
  void fbstp(const Op& dst)
  {
    if (!ensureSpace()) return;

    emitByte(0xDF);
    emitMem(6, dst);
  }

  //! @brief Change Sign.
  //!
  //! This instruction complements the sign bit of ST(0). This operation 
  //! changes a positive value into a negative value of equal magnitude 
  //! or vice versa.
  void fchs()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xE0);
  }

  //! @brief FP Conditional Move.
  //!
  //! This instruction tests the status flags in the EFLAGS register and 
  //! moves the source operand (second operand) to the destination operand 
  //! (first operand) if the given test condition is true.
  void fcmov(CONDITION cc, int i)
  {
    if (!ensureSpace()) return;

    switch (cc)
    {
      // if below
      case C_BELOW:
        emitByte(0xDA);
        emitByte(0xC0 + i);
        return;
      // if equal
      case C_EQUAL:
        emitByte(0xDA);
        emitByte(0xC8 + i);
        return;
      // if below or equal
      case C_BELOW_EQUAL:
        emitByte(0xDA);
        emitByte(0xD0 + i);
        return;
      // if unordered
      case C_FP_UNORDERED:
        emitByte(0xDA);
        emitByte(0xD8 + i);
        return;
      // if not below
      case C_ABOVE_EQUAL:
        emitByte(0xDB);
        emitByte(0xC0 + i);
        return;
      // if not equal
      case C_NOT_EQUAL:
        emitByte(0xDB);
        emitByte(0xC8 + i);
        return;
      // if not below or equal
      case C_ABOVE:
        emitByte(0xDB);
        emitByte(0xD0 + i);
        return;
      // if not unordered
      case C_FP_NOT_UNORDERED:
        emitByte(0xDB);
        emitByte(0xD8 + i);
        return;
      default:
        ASMJIT_ILLEGAL();
    }
  }

  void fcomp()
  {
    if (!ensureSpace()) return;

    emitByte(0xD8);
    emitByte(0xD9);
  }

  void fcompp()
  {
    if (!ensureSpace()) return;

    emitByte(0xDE);
    emitByte(0xD9);
  }

  //! @brief Cosine.
  //!
  //! This instruction calculates the cosine of the source operand in 
  //! register ST(0) and stores the result in ST(0).
  void fcos()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xFF);
  }

  void fdiv(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDC, 0xF8, i);
  }

  void fdivp(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDE, 0xF8, i);
  }

  void ffree(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDD, 0xC0, i);
  }

  //! @brief Add 16 bit or 32 bit integer.
  void fiadd(const Op& adr)
  {
    if (!ensureSpace()) return;

    if (adr.size() == 2)
    {
      emitByte(0xDE);
      emitMem(0, adr);
    }
    else if (adr.size() == 4)
    {
      emitByte(0xDA);
      emitMem(0, adr);
    }
    else
    {
      ASMJIT_ILLEGAL();
    }
  }

  void fild_s(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDB);
    emitMem(0, adr);
  }

  void fild_d(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDF);
    emitMem(5, adr);
  }

  void fistp_s(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDB);
    emitMem(3, adr);
  }

  void fist_s(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDB);
    emitMem(2, adr);
  }

  void fistp_d(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDF);
    emitMem(7, adr);
  }

  void fincstp()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xF7);
  }

  void fisub_s(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDA);
    emitMem(4, adr);
  }

  void fld(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xD9, 0xC0, i);
  }

  void fld1()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xE8);
  }

  void fldz()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xEE);
  }

  void fld_s(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitMem(0, adr);
  }

  void fld_d(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDD);
    emitMem(0, adr);
  }

  void fldpi()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xEB);
  }

  void fnstsw_ax()
  {
    if (!ensureSpace()) return;
    emitByte(0xDF);
    emitByte(0xE0);
  }

  void fmul(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDC, 0xC8, i);
  }

  void fmulp(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDE, 0xC8, i);
  }

  void fprem()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xF8);
  }

  void fprem1()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xF5);
  }

  //! @brief Partial Tangent.
  void fptan()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xF2);
  }

  void frndint()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xFC);
  }

  //! @brief Restore FPU State.
  //!
  //! Load FPU state from src (94 bytes or 108 bytes).
  void frstor(const Op& src)
  {
    if (!ensureSpace()) return;

    emitByte(0xDD);
    emitMem(4, src);
  }

  //! @brief Save FPU State.
  //!
  //! Store FPU state to m94byte or m108byte after checking for
  //! pending unmasked FP exceptions. Then reinitialize
  //! the FPU.
  void fsave(const Op& dst)
  {
    if (!ensureSpace()) return;

    emitByte(0x9B);
    emitByte(0xDD);
    emitMem(6, dst);
  }

  //! @brief Save FPU State.
  //! Store FPU environment to m94byte or m108byte without
  //! checking for pending unmasked FP exceptions.
  //! Then re-initialize the FPU.
  void fnsave(const Op& dst)
  {
    if (!ensureSpace()) return;

    emitByte(0xDD);
    emitMem(6, dst);
  }

  //! @brief Scale.
  //!
  //! Scale ST(0) by ST(1).
  void fscale()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xFD);
  }

  //! @brief Sine.
  //!
  //! This instruction calculates the sine of the source operand in 
  //! register ST(0) and stores the result in ST(0).
  void fsin()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xFE);
  }

  //! @brief Sine and Cosine.
  //!
  //! Compute the sine and cosine of ST(0); replace ST(0) with
  //! the sine, and push the cosine onto the register stack.
  void fsincos()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xFB);
  }

  //! @brief Square Root.
  //!
  //! Calculates square root of ST(0) and stores the result in ST(0).
  void fsqrt()
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xFA);
  }

  void fstp_s(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitMem(3, adr);
  }

  void fstp_d(const Op& adr)
  {
    if (!ensureSpace()) return;
    emitByte(0xDD);
    emitMem(3, adr);
  }

  void fsub(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDC, 0xE8, i);
  }

  void fsubp(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDE, 0xE8, i);
  }

  void fsubrp(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDE, 0xE0, i);
  }

  void ftst() 
  {
    if (!ensureSpace()) return;
    emitByte(0xD9);
    emitByte(0xE4);
  }

  void fucomp(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xDD, 0xE8, i);
  }

  void fucompp()
  {
    if (!ensureSpace()) return;
    emitByte(0xDA);
    emitByte(0xE9);
  }

  void fwait()
  {
    if (!ensureSpace()) return;
    emitByte(0x9B);
  }

  void fxch(int i)
  {
    if (!ensureSpace()) return;
    emitArithFP(0xD9, 0xC8, i);
  }

  //! @brief Restore FP And MMX(tm) State And Streaming SIMD Extension State.
  //!
  //! Load FP and MMX(tm) technology and Streaming SIMD Extension state from 
  //! src512
  void fxrstor(const Op& src512)
  {
    if (!ensureSpace()) return;

    emitByte(0x0F);
    emitByte(0xAE);
    emitMem(1, src512);
  }

  //! @brief Store FP and MMX(tm) State and Streaming SIMD Extension State.
  //!
  //! Store FP and MMX(tm) technology state and Streaming SIMD Extension state 
  //! to dst512.
  void fxsave(const Op& dst512)
  {
    if (!ensureSpace()) return;

    emitByte(0x0F);
    emitByte(0xAE);
    emitMem(0, dst512);
  }

  //! @brief Extract Exponent and Significand.
  //!
  //! Separate value in ST(0) into exponent and significand, store exponent 
  //! in ST(0), and push the significand onto the register stack.
  void fxtract()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xF4);
  }

  //! @brief Compute y * log_2(x).
  //!
  //! Replace ST(1) with (ST(1) * log2ST(0)) and pop the register stack.
  void fyl2x()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xF1);
  }

  //! @brief Compute y * log_2(x+1).
  //!
  //! Replace ST(1) with (ST(1) * (log2ST(0) + 1.0)) and pop the register stack.
  void fyl2xp1()
  {
    if (!ensureSpace()) return;

    emitByte(0xD9);
    emitByte(0xF9);
  }

  //! @brief Signed divide.
  //!
  //! This instruction divides (signed) the value in the AL, AX, or EAX 
  //! register by the source operand and stores the result in the AX, 
  //! DX:AX, or EDX:EAX registers.
  void idiv(const Op& by)
  { 
    if (!ensureSpace()) return;
    if (by.size() == 2) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(by.size() == 8, 7, by);
#endif // ASMJIT_X64
    emitByte(0xF6 + (by.size() != 1));
    emitOp(7, by);
  }

  //! @brief Signed multiply.
  //!
  //! Source operand (in a general-purpose register or memory location) 
  //! is multiplied by the value in the AL, AX, or EAX register (depending
  //! on the operand size) and the product is stored in the AX, DX:AX, or 
  //! EDX:EAX registers, respectively.
  void imul(const Op& src)
  {
    if (!ensureSpace()) return;
    if (src.size() == 2) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(src.size() == 8, 5, by);
#endif // ASMJIT_X64
    emitByte(0xF6 + (src.size() != 1));
    emitOp(5, src);
  }

  //! @brief Signed multiply.
  //!
  //! Destination operand (the first operand) is multiplied by the source 
  //! operand (second operand). The destination operand is a generalpurpose
  //! register and the source operand is an immediate value, a general-purpose 
  //! register, or a memory location. The product is then stored in the 
  //! destination operand location.
  void imul(const Register& dst, const Op& src) 
  {
    ASMJIT_ASSERT(dst.regType() != REG_GPB);
    if (!ensureSpace()) return;

    if (src.op() == OP_REG || src.op() == OP_MEM)
    {
      if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
      emitRex(dst.regType() == REG_GPQ, dst.regCode(), src);
      emitByte(0x0F);
      emitByte(0xAF);
      emitOp(dst.regCode(), src);
      return;
    }
    else
    {
      imul(dst, dst, src);
      return;
    }
  }

  //! @brief Signed multiply.
  //!
  //! source operand (which can be a general-purpose register or a memory 
  //! location) is multiplied by the second source operand (an immediate 
  //! value). The product is then stored in the destination operand 
  //! (a general-purpose register).
  void imul(const Register& dst, const Op& src, const Op& imm)
  {
    ASMJIT_ASSERT(src.op() != OP_IMM);
    ASMJIT_ASSERT(imm.op() == OP_IMM);
    if (!ensureSpace()) return;

    if (isInt8(imm.imm()))
    {
      if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
      emitRex(dst.regType() == REG_GPQ, dst.regCode(), src);
#endif // ASMJIT_X64
      emitByte(0x6B);
      emitOp(dst.regCode(), src);
      emitByteImmediate(imm);
      return;
    }
    else
    {
      if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
      emitRex(dst.regType() == REG_GPQ, dst.regCode(), src);
#endif // ASMJIT_X64
      emitByte(0x69);
      emitOp(dst.regCode(), src);
      if (dst.regType() == REG_GPW)
        emitWordImmediate(imm);
      else
        emitDWordImmediate(imm);
      return;
    }
  }

  //! @brief Increment by 1.
  //!
  //! This instruction adds one to the destination operand, while preserving 
  //! the state of the CF flag. The destination operand can be a register or
  //! a memory location. This instruction allows a loop counter to be updated
  //! without disturbing the CF flag. (Use a ADD instruction with an immediate
  //! operand of 1 to perform an increment operation that does updates the CF flag.)
  //!
  //! @note This instruction can be slower than add(dst, 1)
  void inc(const Op& dst)
  {
    if (!ensureSpace()) return;

    // INC [r16|r32] in 64 bit mode is not encodable.
#if defined(ASMJIT_X86)
    if ((dst.op() == OP_REG) && 
        (dst.regType() == REG_GPW || dst.regType() == REG_GPD))
    {
      if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
      emitByte(0x40 | (dst.regCode() & 7));
      return;
    }
#endif // ASMJIT_X86

    emitRex(dst.size() == 8, 0, dst);
    emitByte(0xFE + (dst.size() != 1));
    emitOp(0, dst);
  }

  //! @brief Interrupt 3 — trap to debugger.
  void int3()
  {
    if (!ensureSpace()) return;
    emitByte(0xCC);
  }

  //! @brief Jump to label @a L if condition @a cc is met.
  //!
  //! This instruction checks the state of one or more of the status flags in 
  //! the EFLAGS register (CF, OF, PF, SF, and ZF) and, if the flags are in the 
  //! specified state (condition), performs a jump to the target instruction 
  //! specified by the destination operand. A condition code (cc) is associated
  //! with each instruction to indicate the condition being tested for. If the 
  //! condition is not satisfied, the jump is not performed and execution 
  //! continues with the instruction following the Jcc instruction.
  void j(CONDITION cc, Label* L, HINT hint = HINT_NONE)
  {
    ASMJIT_ASSERT(0 <= cc && cc < 16);
    if (!ensureSpace()) return;

    if (hint != HINT_NONE) emitByte(static_cast<UInt8>(hint));

    if (L->isBound())
    {
      const int short_size = 2;
      const int long_size  = 6;
      SysInt offs = L->pos() - offset();

      ASMJIT_ASSERT(offs <= 0);

      if (isInt8(offs - short_size))
      {
        // 0111 tttn #8-bit disp
        emitByte(0x70 | cc);
        emitByte((offs - short_size) & 0xFF);
      }
      else
      {
        // 0000 1111 1000 tttn #32-bit disp
        emitByte(0x0F);
        emitByte(0x80 | cc);
        emitDWord(offs - long_size);
      }
    }
    else
    {
      // 0000 1111 1000 tttn #32-bit disp
      // Note: could eliminate cond. jumps to this jump if condition
      //       is the same however, seems to be rather unlikely case.
      emitByte(0x0F);
      emitByte(0x80 | cc);
      emitDisp(L, Displacement::OTHER);
    }
  }

  //! @brief Jump
  //!
  //! This instruction transfers program control to a different point
  //! in the instruction stream without recording return information. 
  //! The destination (target) operand specifies the address of the
  //! instruction being jumped to.
  void jmp(Label* L)
  {
    if (!ensureSpace()) return;

    if (L->isBound())
    {
      const int short_size = 2;
      const int long_size  = 5;
      int offs = L->pos() - offset();

      ASMJIT_ASSERT(offs <= 0);

      if (isInt8(offs - short_size))
      {
        // 1110 1011 #8-bit disp
        emitByte(0xEB);
        emitByte((offs - short_size) & 0xFF);
      }
      else
      {
        // 1110 1001 #32-bit disp
        emitByte(0xE9);
        emitDWord(offs - long_size);
      }
    }
    else
    {
      // 1110 1001 #32-bit disp
      emitByte(0xE9);
      emitDisp(L, Displacement::UNCONDITIONAL_JUMP);
    }
  }

  //! @brief Load Effective Address
  //!
  //! This instruction computes the effective address of the second 
  //! operand (the source operand) and stores it in the first operand 
  //! (destination operand). The source operand is a memory address
  //! (offset part) specified with one of the processors addressing modes.
  //! The destination operand is a general-purpose register.
  void lea(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT(src.op() == OP_MEM);
    if (!ensureSpace()) return;

#if defined(ASMJIT_X64)
    emitRex(dst.regType() == REG_GPQ, dst.regCode(), src);
#endif // ASMJIT_X64
    emitByte(0x8D);
    emitMem(dst.regCode(), src);
  }

  //! @brief Assert LOCK# Signal Prefix.
  //!
  //! This instruction causes the processor’s LOCK# signal to be asserted
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
  void lock()
  {
    if (!ensureSpace()) return;
    emitByte(0xF0);
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
  void mov(const Op& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    switch (dst.op() << 4 | src.op())
    {
      // Reg <- Reg/Mem
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_GPB || src.regType() == REG_GPW ||
                      src.regType() == REG_GPD || src.regType() == REG_GPQ);
        // ... fall through ...
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_GPB || dst.regType() == REG_GPW ||
                      dst.regType() == REG_GPD || dst.regType() == REG_GPQ);

        if (dst.regType() == REG_GPW) emitByte(0x66);
#if defined(ASMJIT_X64)
        emitRex(dst.regType() == REG_GPQ, dst.regCode(), src);
#endif // ASMJIT_X64
        emitByte(0x8A + (dst.regType() != REG_GPB));
        emitOp(dst.regCode(), src);
        return;

      // Reg <- Imm
      case (OP_REG << 4) | OP_IMM:
      {
        switch (dst.regType())
        {
          case REG_GPB:
#if defined(ASMJIT_X64)
            if (dst.regCode() >= 8) _emitRex(0, 1, 0, 0);
#endif // ASMJIT_X64
            emitByte(0xB0 | (dst.regCode() & 0x7));
            emitByte(src.imm());
            return;
          case REG_GPW:
            emitByte(0x66); // 16 bit
          case REG_GPD:
#if defined(ASMJIT_X64)
            if (dst.regCode() >= 8) _emitRex(0, 1, 0, 0);
#endif // ASMJIT_X64
            emitByte(0xB8 | (dst.regCode() & 0x7));
            if (dst.regType() == REG_GPW)
              emitWord(src.imm());
            else
              emitDWord(src.imm());
            return;
#if defined(ASMJIT_X64)
          case REG_GPQ:
            _emitRex(1, dst.regCode() >= 8, 0, 0);
            emitByte(0xB8 | (dst.regCode() & 0x7));
            emitQWord(src.imm());
            return;
#endif // ASMJIT_X64
        }
        break;
      }

      // Mem <- Reg
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_GPB || src.regType() == REG_GPW ||
                      src.regType() == REG_GPD || src.regType() == REG_GPQ);

        if (src.regType() == REG_GPW) emitByte(0x66);
#if defined(ASMJIT_X64)
        emitRex(src.regType() == REG_GPQ, src.regCode(), dst);
#endif // ASMJIT_X86
        emitByte(0x88 + (src.regType() != REG_GPB));
        emitOp(src.regCode(), dst);
        return;

      // Mem <- Imm
      case (OP_MEM << 4) | OP_IMM:
        switch (dst.size())
        {
          case 1:
#if defined(ASMJIT_X64)
            emitRex(0, 0, dst);
#endif // ASMJIT_X64
            emitByte(0xC6);
            emitMem(0, dst);
            emitByte(src.imm());
            return;
          case 2:
            emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
            emitRex(0, 0, dst);
#endif // ASMJIT_X64
            emitByte(0xC7);
            emitMem(0, dst);
            emitWordImmediate(src.imm());
            return;
          case 4:
#if defined(ASMJIT_X64)
            emitRex(0, 0, dst);
#endif // ASMJIT_X64
            emitByte(0xC7);
            emitMem(0, dst);
            emitDWordImmediate(src.imm());
            return;
#if defined(ASMJIT_X64)
          case 8:
            emitRex(1, 0, dst);
            emitByte(0xC7);
            emitMem(0, dst);
            emitDWordImmediate(src.imm());
            return;
#endif // ASMJIT_X64
        }
        break;
    }

    ASMJIT_ILLEGAL();
  }

  //! @brief Move with Sign-Extension.
  //!
  //! This instruction copies the contents of the source operand (register 
  //! or memory location) to the destination operand (register) and sign 
  //! extends the value to 16, 32 or 64 bits.
  //!
  //! @sa movsxd().
  void movsx(const Register& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    switch (src.size())
    {
      case 1:
        if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
        emitRex((dst.regType() == REG_GPQ), dst.regCode(), src);
#endif // ASMJIT_X64
        emitByte(0x0F);
        emitByte(0xBE);
        emitOp(dst.regCode(), src);
        return;
      case 2:
#if defined(ASMJIT_X64)
        emitRex((dst.regType() == REG_GPQ), dst.regCode(), src);
#endif // ASMJIT_X64
        emitByte(0x0F);
        emitByte(0xBF);
        emitOp(dst.regCode(), src);
        return;
    }
    ASMJIT_ILLEGAL();
  }

#if defined(ASMJIT_X64)
  //! @brief Move DWord to QWord with sign-extension.
  void movsxd(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT(dst.regType() == REG_GPQ);
    emitRex(1, dst.regCode(), src);
    emitByte(0x63);
    emitOp(dst.regCode(), src);
    return;
  }
#endif // ASMJIT_X64

  //! @brief Move with Zero-Extend.
  //!
  //! This instruction copies the contents of the source operand (register 
  //! or memory location) to the destination operand (register) and zero 
  //! extends the value to 16 or 32 bits. The size of the converted value 
  //! depends on the operand-size attribute.
  void movzx(const Register& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    switch (src.size())
    {
      case 1:
        if (dst.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
        emitRex((dst.regType() == REG_GPQ), dst.regCode(), src);
#endif // ASMJIT_X64
        emitByte(0x0F);
        emitByte(0xB6);
        emitOp(dst.regCode(), src);
        return;
      case 2:
#if defined(ASMJIT_X64)
        emitRex((dst.regType() == REG_GPQ), dst.regCode(), src);
#endif // ASMJIT_X64
        emitByte(0x0F);
        emitByte(0xB7);
        emitOp(dst.regCode(), src);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Unsigned multiply.
  //!
  //! Source operand (in a general-purpose register or memory location) 
  //! is multiplied by the value in the AL, AX, or EAX register (depending
  //! on the operand size) and the product is stored in the AX, DX:AX, or 
  //! EDX:EAX registers, respectively.
  void mul(const Op& by)
  {
    if (!ensureSpace()) return;
#if defined(ASMJIT_X64)
    emitRex(by.size() == 8, 4, by);
#endif // ASMJIT_X64
    emitByte(0xF6 + (by.size() != 1));
    emitOp(4, by);
  }

  //! @brief Two's Complement Negation.
  //!
  //! This instruction replaces the value of operand (the destination operand)
  //! with its two's complement. (This operation is equivalent to subtracting
  //! the operand from 0.) The destination operand is located in a 
  //! general-purpose register or a memory location.
  void neg(const Op& dst)
  {
    if (!ensureSpace()) return;
    if (dst.size() == 3) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(dst.size() == 8, 2, dst);
#endif // ASMJIT_X64
    emitByte(0xF6 + (dst.size() != 1));
    emitOp(3, dst);
  }

  //! @brief No Operation.
  //!
  //! This instruction performs no operation. This instruction is a one-byte 
  //! instruction that takes up space in the instruction stream but does not 
  //! affect the machine context, except the EIP register. The NOP instruction 
  //! is an alias mnemonic for the XCHG (E)AX, (E)AX instruction.
  void nop()
  {
    if (!ensureSpace()) return;
    emitByte(0x90);
  }

  //! @brief One's Complement Negation.
  //!
  //! This instruction performs a bitwise NOT operation (each 1 is cleared to 
  //! 0, and each 0 is set to 1) on the destination operand and stores the 
  //! result in the destination operand location. The destination operand 
  //! can be a register or a memory location.
  void not_(const Op& dst)
  {
    if (!ensureSpace()) return;
    if (dst.size() == 2) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(dst.size() == 8, 2, dst);
#endif // ASMJIT_X64
    emitByte(0xF6 + (dst.size() != 1));
    emitOp(2, dst);
  }

  //! @brief Logical Inclusive OR.
  //!
  //! This instruction performs a bitwise inclusive OR operation between the 
  //! destination (first) and source (second) operands and stores the result
  //! in the destination operand location. The source operand can be an 
  //! immediate, a register, or a memory location; the destination operand can
  //! be a register or a memory location. (However, two memory operands cannot
  //! be used in one instruction.) Each bit of the result of the OR instruction
  //! is 0 if both corresponding bits of the operands are 0; otherwise, each bit 
  //! is 1.
  void or_(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x08, 1);
  }

  //! @brief Pop a Value from the Stack.
  //!
  //! This instruction loads the value from the top of the stack to the location 
  //! specified with the destination operand and then increments the stack pointer.
  //! The destination operand can be a general purpose register, memory location, 
  //! or segment register.
  void pop(const Op& op)
  {
    if (!ensureSpace()) return;

    switch (op.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(op.regType() == REG_GPW || op.regType() == REG_GPN);
        if (op.regType() == REG_GPW) emitByte(0x66);
        if (op.regCode() >= 8) emitByte(0x41);
        emitByte(0x58 | op.regCode());
        break;
      case OP_MEM:
        emitRex(1, 0, op);
        emitByte(0x8F);
        emitMem(0, op);
        break;
      default:
        ASMJIT_ILLEGAL();
    }
  }

#if defined(ASMJIT_X86)
  //! @brief Pop All General-Purpose Registers.
  //!
  //! Pop EDI, ESI, EBP, EBX, EDX, ECX, and EAX.
  void popad()
  {
    if (!ensureSpace()) return;
    emitByte(0x61);
  }

  //! @brief Pop Stack into EFLAGS Register.
  //!
  //! Pop top of stack into EFLAGS.
  void popfd()
  {
    if (!ensureSpace()) return;
    emitByte(0x9D);
  }
#endif // ASMJIT_X86

  //! @brief Push DWORD/QWORD Onto the Stack.
  //!
  //! @note 32 bit architecture pushed DWORD while 64 bit 
  //! pushes QWORD. 64 bit mode not provides instruction to
  //! push 32 bit register/memory/immediate.
  void push(const Op& op)
  {
    if (!ensureSpace()) return;

    switch (op.op())
    {
      case OP_MEM:
        emitRex(1, 6, op);
        emitByte(0xFF);
        emitMem(6, op);
        break;
      case OP_REG:
        ASMJIT_ASSERT(op.regType() == REG_GPW || op.regType() == REG_GPN);
        if (op.regType() == REG_GPW) emitByte(0x66);
        if (op.regCode() >= 8) emitByte(0x41);
        emitByte(0x50 | op.regCode());
        break;
      case OP_IMM:
        if (isInt8(op.imm()))
        {
          emitByte(0x6A);
          emitByte(op.imm());
        }
        else
        {
          emitByte(0x68);
          emitDWord(op.imm());
        }
        break;
      default:
        ASMJIT_ILLEGAL();
        break;
    }
  }

#if defined(ASMJIT_X86)
  //! @brief Push All General-Purpose Registers.
  //!
  //! Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI.
  void pushad()
  {
    if (!ensureSpace()) return;
    emitByte(0x60);
  }

  //! @brief Push EFLAGS Register onto the Stack.
  //!
  //! Push EFLAGS.
  void pushfd()
  {
    if (!ensureSpace()) return;
    emitByte(0x9C);
  }
#endif // ASMJIT_X86

  //! @brief Rotate Bits Left.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void rcl(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 2);
  }

  //! @brief Rotate Bits Right.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void rcr(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 3);
  }

  //! @brief Read Time-Stamp Counter (Pentium).
  void rdtsc()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x31);
  }

  //! @brief Read Time-Stamp Counter and Processor ID (New).
  void rdtscp()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x01);
    emitByte(0xf9);
  }

  //! @brief Return from Procedure.
  void ret()
  {
    if (!ensureSpace()) return;
    emitByte(0xC3);
  }

  //! @brief Return from Procedure.
  void ret(int imm16)
  {
    ASMJIT_ASSERT(isUInt16(imm16));
    if (!ensureSpace()) return;

    if (imm16 == 0)
    {
      emitByte(0xC3);
    }
    else
    {
      emitByte(0xC2);
      emitWord((UInt16)imm16);
    }
  }

  //! @brief Rotate Bits Left.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void rol(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 0);
  }

  //! @brief Rotate Bits Right.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void ror(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 1);
  }

#if defined(ASMJIT_X86)
  //! @brief Store AH into Flags.
  void sahf()
  {
    if (!ensureSpace()) return;
    emitByte(0x9E);
  }
#endif // ASMJIT_X86

  //! @brief Integer subtraction with borrow.
  void sbb(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x18, 3);
  }

  //! @brief Shift Bits Left.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void sal(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 4);
  }

  //! @brief Shift Bits Right.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void sar(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 7);
  }

  //! @brief Shift Bits Left.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void shl(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 4);
  }

  //! @brief Shift Bits Right.
  //!
  //! @note if @a src is register, it can be only @c cl.
  void shr(const Op& dst, const Op& src)
  {
    _emitBitArith(dst, src, 5);
  }

  //! @brief Double Precision Shift Left.
  //!
  //! @note src2 operand can be only @c cl register or 8 bit immediate.
  void shld(const Op& dst, const Register& src1, const Op& src2)
  {
    _emitShldShrd(dst, src1, src2, 0xA4);
  }

  //! @brief Double Precision Shift Right.
  //!
  //! @note src2 operand can be only @c cl register or 8 bit immediate.
  void shrd(const Op& dst, const Register& src1, const Op& src2)
  {
    _emitShldShrd(dst, src1, src2, 0xAC);
  }

  //! @brief Set Carry Flag to 1.
  void stc()
  {
    if (!ensureSpace()) return;
    emitByte(0xF9);
  }

  //! @brief Set Direction Flag to 1.
  void std()
  {
    if (!ensureSpace()) return;
    emitByte(0xFD);
  }

  void sub(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x28, 5);
  }

  //! @brief Logical Compare.
  void test(const Op& op1, const Op& op2)
  {
    switch (op1.op() << 4 | op2.op())
    {
      // Reg + Reg
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(op1.size() == op2.size());
        switch (op1.size())
        {
          case 1:
            emitByte(0x84);
            emitReg(op1.regCode(), op2.regCode());
            return;
          case 4:
            emitByte(0x85);
            emitReg(op1.regCode(), op2.regCode());
            return;
        }
        break;

      // Mem + Reg
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(op1.size() == op2.size());
        switch (op1.size())
        {
          case 1:
            emitByte(0x84);
            emitMem(op2.regCode(), op1);
            return;
          case 4:
            emitByte(0x85);
            emitMem(op2.regCode(), op1);
            return;
        }
        break;

      // Reg + Imm
      case (OP_REG << 4) | OP_IMM:
        if (op1.reg() == REG_AL)
        {
          emitByte(0xA8);
          emitByte(op2.imm() & 0xFF);
          return;
        }
        else if (op1.reg() == REG_EAX)
        {
          emitByte(0xA9);
          emitDWordImmediate(op2);
          return;
        }
        // ... fall through ...
      // Mem + Imm
      case (OP_MEM << 4) | OP_IMM:
        if (op1.size() == 1)
        {
          emitByte(0xF6);
          emitOp(0, op1);
          emitByte(op2.imm() & 0xFF);
          return;
        }
        if (op1.size() == 4)
        {
          emitByte(0xF7);
          emitOp(0, op1);
          emitDWordImmediate(op2);
          return;
        }
        break;
    }

    ASMJIT_ILLEGAL();
  }

  //! @brief Undefined instruction.
  //!
  //! Raise invalid opcode exception.
  void ud2()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x0B);
  }

  //! @brief Exchange and Add.
  void xadd(const Op& dst, const Register& src)
  {
    if (src.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(src.regType() == REG_GPQ, src.regCode(), src);
#endif // ASMJIT_X64
    emitByte(0x0F);
    emitByte(0xC0 + (src.regType() != REG_GPB));
    emitOp(src.regCode(), dst);
  }

  //! @brief Exchange Register/Memory with Register.
  void xchg(const Op& dst, const Register& src)
  {
    if (src.regType() == REG_GPW) emitByte(0x66); // 16 bit
#if defined(ASMJIT_X64)
    emitRex(src.regType() == REG_GPQ, src.regCode(), src);
#endif // ASMJIT_X64

    // Special opcode for index 0 registers (AX, EAX, RAX vs register)
    if ((dst.op() == OP_REG && dst.size() > 1) && 
        (dst.regCode() == 0 || src.regCode() == 0))
    {
      int index = dst.regCode() | src.regCode();
      emitByte(0x90 + index);
      return;
    }

    emitByte(0x86 + (src.regType() != REG_GPB));
    emitOp(src.regCode(), dst);
  }

  //! @brief Exchange Register with Register/Memory.
  void xchg(const Register& dst, const Op& src)
  {
    xchg(src, dst);
  }

  //! @brief Logical Exclusive OR.
  void xor_(const Op& dst, const Op& src)
  {
    _emitArithOp(dst, src, 0x30, 6);
  }

  // -------------------------------------------------------------------------
  // [MMX]
  // -------------------------------------------------------------------------

  //! @brief Clear MMX state (MMX).
  void emms()
  {
    if (!ensureSpace()) return;

    emitByte(0x0F);
    emitByte(0x77);
  }

  //! @brief 32 bit move targetted to MMX or SSE2 registers (between mem32 or GPD registers) (MMX/SSE2).
  //!
  //! @note This function requires SSE2 for SSE (XMM)registers and MMX 
  //! for MMX (MM) registers to work.
  //!
  //! @note You cannot use this function to move from one MMX/SSE register to 
  //! another one or from one memory location to another one. To move from
  //! SSE register to MMX (or vice versa), use @c movq() instruction,
  //! @c movdq2q() or movq2dq().
  void movd(const Op& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    switch (dst.op() << 4 | src.op())
    {
      // Reg <- Reg
      case (OP_REG << 4) | OP_REG:
        // MMX
        if (dst.regType() == REG_MMX && src.regType() == REG_GPD)
        {
          emitByte(0x0F);
          emitByte(0x6E);
          emitReg(dst.regCode(), src.regCode());
          return;
        }
        if (dst.regType() == REG_GPD && src.regType() == REG_MMX)
        {
          emitByte(0x0F);
          emitByte(0x7E);
          emitReg(src.regCode(), dst.regCode());
          return;
        }
        // SSE
        if (dst.regType() == REG_SSE && src.regType() == REG_GPD)
        {
          emitByte(0x66);
          emitByte(0x0F);
          emitByte(0x6E);
          emitReg(dst.regCode(), src.regCode());
          return;
        }
        if (dst.regType() == REG_GPD && src.regType() == REG_SSE)
        {
          emitByte(0x66);
          emitByte(0x0F);
          emitByte(0x7E);
          emitReg(src.regCode(), dst.regCode());
          return;
        }
        break;

      // Reg <- Mem
      case (OP_REG << 4) | OP_MEM:
        // MMX
        if (dst.regType() == REG_MMX)
        {
          if (src.size() == 4)
          {
            emitByte(0x0F);
            emitByte(0x6E);
            emitMem(dst.regCode(), src);
            return;
          }
        }
        // SSE
        if (dst.regType() == REG_SSE)
        {
          if (src.size() == 4)
          {
            emitByte(0x66);
            emitByte(0x0F);
            emitByte(0x6E);
            emitMem(dst.regCode(), src);
            return;
          }
        }
        break;

      // Mem <- Reg
      case (OP_MEM << 4) | OP_REG:
        // MMX
        if (src.regType() == REG_MMX)
        {
          if (dst.size() == 4)
          {
            emitByte(0x0F);
            emitByte(0x7E);
            emitMem(src.regCode(), dst);
            return;
          }
        }
        // SSE
        if (src.regType() == REG_SSE)
        {
          if (dst.size() == 4)
          {
            emitByte(0x66);
            emitByte(0x0F);
            emitByte(0x7E);
            emitMem(src.regCode(), dst);
            return;
          }
        }
        break;
    }

    ASMJIT_ILLEGAL();
  }
  
  //! @brief Move 64 bits (MMX).
  void movq(const Op& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_REG:
        if (dst.regType() == REG_MMX && src.regType() == REG_MMX)
        {
          emitByte(0x0F);
          emitByte(0x6F);
          emitReg(dst.regCode(), src.regCode());
          return;
        }
        if (dst.regType() == REG_SSE && src.regType() == REG_SSE)
        {
          emitByte(0xF3);
          emitByte(0x0F);
          emitByte(0x7E);
          emitReg(dst.regCode(), src.regCode());
          return;
        }

        // Convenience - movq2dq
        if (dst.regType() == REG_SSE && src.regType() == REG_MMX)
        {
          emitByte(0xF3);
          emitByte(0x0F);
          emitByte(0xD6);
          emitReg(dst.regCode(), src.regCode());
          return;
        }

        // Convenience - movdq2q
        if (dst.regType() == REG_MMX && src.regType() == REG_SSE)
        {
          emitByte(0xF2);
          emitByte(0x0F);
          emitByte(0xD6);
          emitReg(dst.regCode(), src.regCode());
          return;
        }
        break;

      case (OP_REG << 4) | OP_MEM:
        if (dst.regType() == REG_MMX)
        {
          emitByte(0x0F);
          emitByte(0x6F);
          emitMem(dst.regCode(), src);
          return;
        }
        if (dst.regType() == REG_SSE)
        {
          emitByte(0xF3);
          emitByte(0x0F);
          emitByte(0x7E);
          emitMem(dst.regCode(), src);
          return;
        }
        break;

      case (OP_MEM << 4) | OP_REG:
        if (src.regType() == REG_MMX)
        {
          emitByte(0x0F);
          emitByte(0x7F);
          emitMem(src.regCode(), dst);
          return;
        }
        if (src.regType() == REG_SSE)
        {
          emitByte(0xF3);
          emitByte(0x0F);
          emitByte(0xD6);
          emitMem(src.regCode(), dst);
          return;
        }
        break;
    }

    ASMJIT_ILLEGAL();
  }

  //! @brief Pack with Signed Saturation (MMX).
  void packsswb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x63, dst.regCode(), src);
  }
  
  //! @brief Pack with Signed Saturation (MMX).
  void packssdw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x6B, dst.regCode(), src);
  }
  
  //! @brief Pack with Unsigned Saturation (MMX).
  void packuswb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x67, dst.regCode(), src);
  }

  //! @brief Packed BYTE Add (MMX).
  void paddb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xFC, dst.regCode(), src);
  }
  
  //! @brief Packed WORD Add (MMX).
  void paddw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xFD, dst.regCode(), src);
  }
  
  //! @brief Packed DWORD Add (MMX).
  void paddd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xFE, dst.regCode(), src);
  }
  
  //! @brief Packed Add with Saturation (MMX).
  void paddsb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xEC, dst.regCode(), src);
  }

  //! @brief Packed Add with Saturation (MMX).
  void paddsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xED, dst.regCode(), src);
  }
  
  //! @brief Packed Add Unsigned with Saturation (MMX).
  void paddusb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xDC, dst.regCode(), src);
  }

  //! @brief Packed Add Unsigned with Saturation (MMX).
  void paddusw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xDD, dst.regCode(), src);
  }

  //! @brief Logical AND (MMX).
  void pand(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xDB, dst.regCode(), src);
  }

  //! @brief Logical AND Not (MMX).
  void pandn(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xDF, dst.regCode(), src);
  }

  //! @brief Packed Compare for Equal (BYTES) (MMX).
  void pcmpeqb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x74, dst.regCode(), src);
  }

  //! @brief Packed Compare for Equal (WORDS) (MMX).
  void pcmpeqw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x75, dst.regCode(), src);
  }

  //! @brief Packed Compare for Equal (DWORDS) (MMX).
  void pcmpeqd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x76, dst.regCode(), src);
  }

  //! @brief Packed Compare for Greater Than (BYTES) (MMX).
  void pcmpgtb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x64, dst.regCode(), src);
  }

  //! @brief Packed Compare for Greater Than (WORDS) (MMX).
  void pcmpgtw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x65, dst.regCode(), src);
  }

  //! @brief Packed Compare for Greater Than (DWORDS) (MMX).
  void pcmpgtd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x66, dst.regCode(), src);
  }
  
  //! @brief Packed Multiply High (MMX).
  void pmulhw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE5, dst.regCode(), src);
  }
  
  //! @brief Packed Multiply Low (MMX).
  void pmullw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xD5, dst.regCode(), src);
  }

  //! @brief Bitwise Logical OR (MMX).
  void por(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xEB, dst.regCode(), src);
  }

  //! @brief Packed Multiply and Add (MMX).
  void pmaddwd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xF5, dst.regCode(), src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  void psllw(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xF1, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x71, 6, dst.regCode(), src.imm());
        return;
    }
  }

  //! @brief Packed Shift Left Logical (MMX).
  void pslld(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xF2, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x72, 6, dst.regCode(), src.imm());
        return;
    }
  }
  
  //! @brief Packed Shift Left Logical (MMX).
  void psllq(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xF3, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x73, 6, dst.regCode(), src.imm());
        return;
    }
  }

  //! @brief Packed Shift Right Arithmetic (MMX).
  void psraw(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xE1, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x71, 4, dst.regCode(), src.imm());
        return;
    }
  }
  
  //! @brief Packed Shift Right Arithmetic (MMX).
  void psrad(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xE2, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x72, 4, dst.regCode(), src.imm());
        return;
    }
  }

  //! @brief Packed Shift Right Logical (MMX).
  void psrlw(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xD1, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x71, 2, dst, src.imm());
        return;
    }
  }

  //! @brief Packed Shift Right Logical (MMX).
  void psrld(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xD2, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x72, 2, dst, src.imm());
        return;
    }
  }

  //! @brief Packed Shift Right Logical (MMX).
  void psrlq(const MMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_MMX);
        // ... fall through ...
      case OP_MEM:
        emitMM(0x00, 0x00, 0x0F, 0xD3, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x00, 0x0F, 0x73, 2, dst, src.imm());
        return;
    }
  }

  //! @brief Packed Subtract (MMX).
  void psubb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xF8, dst.regCode(), src);
  }

  //! @brief Packed Subtract (MMX).
  void psubw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xF9, dst.regCode(), src);
  }

  //! @brief Packed Subtract (MMX).
  void psubd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xFA, dst.regCode(), src);
  }
  
  //! @brief Packed Subtract with Saturation (MMX).
  void psubsb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE8, dst.regCode(), src);
  }
  
  //! @brief Packed Subtract with Saturation (MMX).
  void psubsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE9, dst.regCode(), src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  void psubusb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xD8, dst.regCode(), src);
  }
  
  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  void psubusw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xD9, dst.regCode(), src);
  }
  
  //! @brief Unpack High Packed Data (MMX).
  void punpckhbw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x68, dst.regCode(), src);
  }
  
  //! @brief Unpack High Packed Data (MMX).
  void punpckhwd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x69, dst.regCode(), src);
  }

  //! @brief Unpack High Packed Data (MMX).
  void punpckhdq(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x6A, dst.regCode(), src);
  }

  //! @brief Unpack High Packed Data (MMX).
  void punpcklbw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x60, dst.regCode(), src);
  }
  
  //! @brief Unpack High Packed Data (MMX).
  void punpcklwd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x61, dst.regCode(), src);
  }

  //! @brief Unpack High Packed Data (MMX).
  void punpckldq(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x62, dst.regCode(), src);
  }

  //! @brief Bitwise Exclusive OR (MMX).
  void pxor(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xEF, dst.regCode(), src);
  }
  
  // -------------------------------------------------------------------------
  // [SSE]
  // -------------------------------------------------------------------------

  //! @brief Packed SP-FP Add (SSE).
  void addps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x58, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP Add (SSE).
  void addss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x58, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical And Not For SP-FP (SSE).
  void andnps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x55, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical And For SP-FP (SSE).
  void andps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x54, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Compare (SSE).
  void cmpps(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xC2, dst.regCode(), src, imm8);
  }

  //! @brief Compare Scalar SP-FP Values (SSE).
  void cmpss(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0xC2, dst.regCode(), src, imm8);
  }

  //! @brief Scalar Ordered SP-FP Compare and Set EFLAGS (SSE).
  void comiss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x2F, dst.regCode(), src);
  }

  //! @brief Packed Signed INT32 to Packed SP-FP Conversion (SSE).
  void cvtpi2ps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x2A, dst.regCode(), src);
  }

  //! @brief Packed SP-FP to Packed INT32 Conversion (SSE).
  void cvtps2pi(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x2D, dst.regCode(), src);
  }

  //! @brief Scalar Signed INT32 to SP-FP Conversion (SSE).
  void cvtsi2ss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_GPD) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x2A, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP to Signed INT32 Conversion (SSE).
  void cvtss2si(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_GPD) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x2D, dst.regCode(), src);
  }

  //! @brief Packed SP-FP to Packed INT32 Conversion (truncate) (SSE).
  void cvttps2pi(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x2C, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP to Signed INT32 Conversion (truncate) (SSE).
  void cvttss2si(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x2C, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Divide (SSE).
  void divps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x5E, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP Divide (SSE).
  void divss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x5E, dst.regCode(), src);
  }

  //! @brief Load Streaming SIMD Extension Control/Status (SSE).
  void ldmxcsr(const Op& src_m32)
  {
    ASMJIT_ASSERT(src_m32.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xAE, 2, src_m32);
  }

  //! @brief Byte Mask Write (SSE).
  //!
  //! @note The default memory location is specified by DS:EDI.
  void maskmovq(const MMRegister& data, const MMRegister& mask)
  {
    emitMM(0x00, 0x00, 0x0F, 0xF7, data.regCode(), mask);
  }

  //! @brief Packed SP-FP Maximum (SSE).
  void maxps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x5F, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP Maximum (SSE).
  void maxss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x5F, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Minimum (SSE).
  void minps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x5D, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP Minimum (SSE).
  void minss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x5D, dst.regCode(), src);
  }

  //! @brief Move Aligned Packed SP-FP Values (SSE).
  void movaps(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        // ... fall through ...
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x28, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x29, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Move 64 Bits Non Temporal (SSE).
  void movntq(const Op& dst_mem, const MMRegister& src)
  {
    ASMJIT_ASSERT(dst_mem.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE7, src.regCode(), dst_mem);
  }

  //! @brief High to Low Packed SP-FP (SSE).
  void movhlps(const XMMRegister& dst, const XMMRegister& src)
  {
    emitMM(0x00, 0x00, 0x0F, 0x12, dst.regCode(), src);
  }

  //! @brief Move High Packed SP-FP (SSE).
  void movhps(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x16, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x17, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Move Low to High Packed SP-FP (SSE).
  void movlhps(const XMMRegister& dst, const XMMRegister& src)
  {
    emitMM(0x00, 0x00, 0x0F, 0x16, dst.regCode(), src);
  }

  //! @brief Move Low Packed SP-FP (SSE).
  void movlps(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x12, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x13, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }
  
  //! @brief Move Aligned Four Packed SP-FP Non Temporal (SSE).
  void movntps(const Op& dst_mem, const XMMRegister& src)
  {
    ASMJIT_ASSERT(dst_mem.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x2B, src.regCode(), dst_mem);
  }

  //! @brief Move Scalar SP-FP (SSE).
  void movss(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        // ... fall through ...
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0xF3, 0x0F, 0x10, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0xF3, 0x0F, 0x11, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  void movups(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        // ... fall through ...
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x10, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x00, 0x0F, 0x11, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed SP-FP Multiply (SSE).
  void mulps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x59, dst.regCode(), src);
  }
   
  //! @brief Scalar SP-FP Multiply (SSE).
  void mulss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x59, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical OR for SP-FP Data (SSE).
  void orps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x56, dst.regCode(), src);
  }

  //! @brief Packed Average (SSE).
  void pavgb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE0, dst.regCode(), src);
  }
  
  //! @brief Packed Average (SSE).
  void pavgw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE3, dst.regCode(), src);
  }

  //! @brief Extract Word (SSE).
  void pextrw(const Register& dst, const MMRegister& src, int imm8)
  {
    emitMM(0x00, 0x00, 0x0F, 0xC5, dst.regCode(), src, imm8);
  }
  
  //! @brief Insert Word (SSE).
  void pinsrw(const MMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xC4, dst.regCode(), src, imm8);
  }
  
  //! @brief Packed Signed Integer Word Maximum (SSE).
  void pmaxsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xEE, dst.regCode(), src);
  }
  
  //! @brief Packed Unsigned Integer Byte Maximum (SSE).
  void pmaxub(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xDE, dst.regCode(), src);
  }
  
  //! @brief Packed Signed Integer Word Minimum (SSE).
  void pminsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xEA, dst.regCode(), src);
  }

  //! @brief Packed Unsigned Integer Byte Minimum (SSE).
  void pminub(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xDA, dst.regCode(), src);
  }

  //! @brief Move Byte Mask To Integer (SSE).
  void pmovmskb(const Register& dst, const MMRegister& src)
  {
    emitMM(0x00, 0x00, 0x0F, 0xD7, dst.regCode(), src);
  }

  //! @brief Packed Multiply High Unsigned (SSE).
  void pmulhuw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xE4, dst.regCode(), src);
  }

  //! @brief Packed Sum of Absolute Differences (SSE).
  void psadbw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xF6, dst.regCode(), src);
  }
  
  //! @brief Packed Shuffle word (SSE).
  void pshufw(const MMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x70, dst.regCode(), src, imm8);
  }

  //! @brief Packed SP-FP Reciprocal (SSE).
  void rcpps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x53, dst.regCode(), src);
  }
  
  //! @brief Scalar SP-FP Reciprocal (SSE).
  void rcpss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x53, dst.regCode(), src);
  }

  //! @brief Prefetch (SSE).
  void prefetch(const Op& mem, int hint)
  {
    ASMJIT_ASSERT(mem.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x18, hint, mem);
  }

  //! @brief Compute Sum of Absolute Differences (SSE).
  void psadbw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xF6, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Square Root Reciprocal (SSE).
  void rsqrtps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0x52, dst.regCode(), src);
  }
  
  //! @brief Scalar SP-FP Square Root Reciprocal (SSE).
  void rsqrtss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x52, dst.regCode(), src);
  }
  
  //! @brief Store fence (SSE).
  void sfence(const Op& mem)
  {
    ASMJIT_ASSERT(mem.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xAE, 7, mem);
  }

  //! @brief Shuffle SP-FP (SSE).
  void shufps(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0xC6, dst.regCode(), src, imm8);
  }
  
  //! @brief Packed SP-FP Square Root (SSE).
  void sqrtps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x51, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP Square Root (SSE).
  void sqrtss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x51, dst.regCode(), src);
  }

  //! @brief Store Streaming SIMD Extension Control/Status (SSE).
  void stmxcsr(const Op& dst_m32)
  {
    emitMM(0x00, 0x00, 0x0F, 0xAE, 3, dst_m32);
  }
  
  //! @brief Packed SP-FP Subtract (SSE).
  void subps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x5C, dst.regCode(), src);
  }

  //! @brief Scalar SP-FP Subtract (SSE).
  void subss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x51, dst.regCode(), src);
  }
  
  //! @brief Unordered Scalar SP-FP compare and set EFLAGS (SSE).
  void ucomiss(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x2E, dst.regCode(), src);
  }
  
  //! @brief Unpack High Packed SP-FP Data (SSE).
  void unpckhps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x15, dst.regCode(), src);
  }
  
  //! @brief Unpack Low Packed SP-FP Data (SSE).
  void unpcklps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x14, dst.regCode(), src);
  }
  
  //! @brief Bit-wise Logical Xor for SP-FP Data (SSE).
  void xorps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x57, dst.regCode(), src);
  }

  // -------------------------------------------------------------------------
  // [SSE2]
  // -------------------------------------------------------------------------

  //! @brief Packed DP-FP Add (SSE2).
  void addpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x58, dst.regCode(), src);
  }

  //! @brief Scalar DP-FP Add (SSE2).
  void addsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x58, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical And Not For DP-FP (SSE2).
  void andnpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x55, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical And For DP-FP (SSE2).
  void andpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x54, dst.regCode(), src);
  }

  //! @brief Flush Cache Line (SSE2).
  void clflush(const Op& mem)
  {
    ASMJIT_ASSERT(mem.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xAE, 7, mem);
  }

  //! @brief Packed DP-FP Compare (SSE2).
  void cmppd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0xC2, dst.regCode(), src, imm8);
  }

  //! @brief Compare Scalar SP-FP Values (SSE2).
  void cmpsd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0xC2, dst.regCode(), src, imm8);
  }

  //! @brief Scalar Ordered DP-FP Compare and Set EFLAGS (SSE2).
  void comisd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x2F, dst.regCode(), src);
  }

  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  void cvtdq2pd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0xE6, dst.regCode(), src);
  }

  //! @brief Convert Packed Dword Integers to Packed SP-FP Values (SSE2).
  void cvtdq2ps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x5B, dst.regCode(), src);
  }

  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  void cvtpd2dq(const XMMRegister& dst, const Op& src)
  {
    emitMM(0x00, 0xF2, 0x0F, 0xE6, dst.regCode(), src);
  }

  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  void cvtpd2pi(const MMRegister& dst, const Op& src)
  {
    emitMM(0x00, 0x66, 0x0F, 0x2D, dst.regCode(), src);
  }

  //! @brief Convert Packed DP-FP Values to Packed SP-FP Values (SSE2).
  void cvtpd2ps(const XMMRegister& dst, const Op& src)
  {
    emitMM(0x00, 0x66, 0x0F, 0x5A, dst.regCode(), src);
  }

  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  void cvtpi2pd(const XMMRegister& dst, const Op& src64)
  {
    emitMM(0x00, 0x66, 0x0F, 0x2A, dst.regCode(), src64);
  }

  //! @brief Convert Packed SP-FP Values to Packed Dword Integers (SSE2).
  void cvtps2dq(const XMMRegister& dst, const Op& src)
  {
    emitMM(0x00, 0x66, 0x0F, 0x5B, dst.regCode(), src);
  }

  //! @brief Convert Packed SP-FP Values to Packed DP-FP Values (SSE2).
  void cvtps2pd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x00, 0x0F, 0x5A, dst.regCode(), src);
  }

  //! @brief Convert Scalar DP-FP Value to Dword Integer (SSE2).
  void cvtsd2si(const Register& dst, const Op& src)
  {
    emitMM(0x00, 0xF2, 0x0F, 0x2D, dst.regCode(), src);
  }

  //! @brief Convert Scalar DP-FP Value to Scalar SP-FP Value (SSE2).
  void cvtsd2ss(const XMMRegister& dst, const Op& src)
  {
    emitMM(0x00, 0xF2, 0x0F, 0x5A, dst.regCode(), src);
  }

  //! @brief Convert Dword Integer to Scalar DP-FP Value (SSE2).
  void cvtsi2sd(const XMMRegister& dst, const Op& src32)
  {
    ASMJIT_ASSERT((src32.op() == OP_REG && src32.regType() == REG_GPD) || (src32.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x2A, dst.regCode(), src32);
  }

  //! @brief Convert Scalar SP-FP Value to Scalar DP-FP Value (SSE2).
  void cvtss2sd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x5A, dst.regCode(), src);
  }

  //! @brief Convert Scalar SP-FP Value to Dword Integer (SSE2).
  void cvtss2si(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x2D, dst.regCode(), src);
  }

  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  void cvttpd2pi(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x2C, dst.regCode(), src);
  }

  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  void cvttpd2dq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0xE6, dst.regCode(), src);
  }

  //! @brief Convert with Truncation Packed SP-FP Values to Packed Dword Integers (SSE2).
  void cvttps2dq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF3, 0x0F, 0x5B, dst.regCode(), src);
  }

  //! @brief Convert with Truncation Scalar DP-FP Value to Signed Dword Integer (SSE2).
  void cvttsd2si(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x2C, dst.regCode(), src);
  }

  //! @brief Packed DP-FP Divide (SSE2).
  void divpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x5E, dst.regCode(), src);
  }

  //! @brief Scalar DP-FP Divide (SSE2).
  void divsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x5E, dst.regCode(), src);
  }

  //! @brief Load Fence (SSE2).
  void lfence()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0xAE);
    emitReg(5, 0);
  }

  //! @brief Store Selected Bytes of Double Quadword (SSE2).
  //!
  //! @note Target is DS:EDI.
  void maskmovdqu(const XMMRegister& src, const XMMRegister& mask)
  {
    emitMM(0x00, 0x66, 0x0F, 0x57, src.regCode(), mask);
  }

  //! @brief Return Maximum Packed Double-Precision FP Values (SSE2).
  void maxpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x5F, dst.regCode(), src);
  }

  //! @brief Return Maximum Scalar Double-Precision FP Value (SSE2).
  void maxsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x5F, dst.regCode(), src);
  }

  //! @brief Memory Fence (SSE2).
  void mfence()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0xAE);
    emitReg(6, 0);
  }

  //! @brief Return Minimum Packed Double-Precision FP Values (SSE2).
  void minpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x5D, dst.regCode(), src);
  }

  //! @brief Return Minimum Scalar Double-Precision FP Value (SSE2).
  void minsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x5D, dst.regCode(), src);
  }

  //! @brief Move Aligned Double Quadword (SSE2).
  void movdqa(const Op& dst, const Op& src)
  {
    switch (dst.op() << 4 | src.op())
    {
      // Reg <- Reg
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        // ... fall through ...
      // Reg <- Mem
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x6F, dst.regCode(), src);
        return;

      // Mem <- Reg
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x7F, dst.regCode(), src);
        return;
    }

    ASMJIT_ILLEGAL();
  }

  //! @brief Move Unaligned Double Quadword (SSE2).
  void movdqu(const Op& dst, const Op& src)
  {
    switch (dst.op() << 4 | src.op())
    {
      // Reg <- Reg
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        // ... fall through ...
      // Reg <- Mem
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0xF3, 0x0F, 0x6F, dst.regCode(), src);
        return;

      // Mem <- Reg
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0xF3, 0x0F, 0x7F, dst.regCode(), src);
        return;
    }

    ASMJIT_ILLEGAL();
  }

  //! @brief Extract Packed SP-FP Sign Mask (SSE2).
  void movmskps(const Register& dst, const XMMRegister& src)
  {
    emitMM(0x00, 0x00, 0x0F, 0x50, dst.regCode(), src);
  }

  //! @brief Extract Packed DP-FP Sign Mask (SSE2).
  void movmskpd(const Register& dst, const XMMRegister& src)
  {
    emitMM(0x00, 0x66, 0x0F, 0x50, dst.regCode(), src);
  }

  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  void movsd(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        // ... fall through ...
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0xF2, 0x0F, 0x10, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0xF2, 0x0F, 0x11, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Move Aligned Packed Double-Precision FP Values (SSE2).
  void movapd(const Op& dst, const Op& src)
  {
    if (!ensureSpace()) return;

    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x28, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x29, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Move Quadword from XMM to MMX Technology Register (SSE2).
  void movdq2q(const MMRegister& dst, const XMMRegister& src)
  {
    emitMM(0x00, 0xF2, 0x0F, 0xD6, dst.regCode(), src);
  }

  //! @brief Move Quadword from MMX Technology to XMM Register (SSE2).
  void movq2dq(const XMMRegister& dst, const MMRegister& src)
  {
    emitMM(0x00, 0xF3, 0x0F, 0xD6, dst.regCode(), src);
  }

  //! @brief Move High Packed Double-Precision FP Value (SSE2).
  void movhpd(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x16, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x17, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Move Low Packed Double-Precision FP Value (SSE2).
  void movlpd(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x12, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x13, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Store Double Quadword Using Non-Temporal Hint (SSE2).
  void movntdq(const Op& dst, const XMMRegister& src)
  {
    ASMJIT_ASSERT(dst.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xE7, src.regCode(), dst);
  }

  //! @brief Store Store DWORD Using Non-Temporal Hint (SSE2).
  void movnti(const Op& dst, const Register& src)
  {
    ASMJIT_ASSERT(dst.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xC3, src.regCode(), dst);
  }

  //! @brief Store Packed Double-Precision FP Values Using Non-Temporal Hint (SSE2).
  void movntpd(const Op& dst, const XMMRegister& src)
  {
    ASMJIT_ASSERT(dst.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x2B, src.regCode(), dst);
  }

  //! @brief Move Unaligned Packed Double-Precision FP Values (SSE2).
  void movupd(const Op& dst, const Op& src)
  {
    switch ((dst.op() << 4) | src.op())
    {
      case (OP_REG << 4) | OP_MEM:
        ASMJIT_ASSERT(dst.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x10, dst.regCode(), src);
        return;
      case (OP_MEM << 4) | OP_REG:
        ASMJIT_ASSERT(src.regType() == REG_SSE);
        emitMM(0x00, 0x66, 0x0F, 0x11, src.regCode(), dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed DP-FP Multiply (SSE2).
  void mulpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x59, dst.regCode(), src);
  }

  //! @brief Scalar DP-FP Multiply (SSE2).
  void mulsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0xF2, 0x0F, 0x59, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  void orpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x00, 0x66, 0x0F, 0x56, dst.regCode(), src);
  }

  //! @brief Pack with Signed Saturation (SSE2).
  void packsswb(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x63, dst.regCode(), src, imm8);
  }

  //! @brief Pack with Signed Saturation (SSE2).
  void packssdw(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x6B, dst.regCode(), src, imm8);
  }

  //! @brief Pack with Unsigned Saturation (SSE2).
  void packuswb(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x67, dst.regCode(), src, imm8);
  }

  //! @brief Packed BYTE Add (SSE2).
  void paddb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xFC, dst.regCode(), src);
  }
  
  //! @brief Packed WORD Add (SSE2).
  void paddw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xFD, dst.regCode(), src);
  }
  
  //! @brief Packed DWORD Add (SSE2).
  void paddd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xFE, dst.regCode(), src);
  }

  //! @brief Packed QWORD Add (SSE2).
  void paddq(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xD4, dst.regCode(), src);
  }

  //! @brief Packed QWORD Add (SSE2).
  void paddq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xD4, dst.regCode(), src);
  }

  //! @brief Packed Add with Saturation (SSE2).
  void paddsb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xEC, dst.regCode(), src);
  }

  //! @brief Packed Add with Saturation (SSE2).
  void paddsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xED, dst.regCode(), src);
  }
  
  //! @brief Packed Add Unsigned with Saturation (SSE2).
  void paddusb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xDC, dst.regCode(), src);
  }

  //! @brief Packed Add Unsigned with Saturation (SSE2).
  void paddusw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xDD, dst.regCode(), src);
  }

  //! @brief Logical AND (SSE2).
  void pand(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xDB, dst.regCode(), src);
  }

  //! @brief Logical AND Not (SSE2).
  void pandn(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xDF, dst.regCode(), src);
  }

  //! @brief Spin Loop Hint (SSE2).
  void pause()
  {
    if (!ensureSpace()) return;
    emitByte(0xF3);
    emitByte(0x90);
  }

  //! @brief Packed Compare for Equal (BYTES) (SSE2).
  void pcmpeqb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x74, dst.regCode(), src);
  }

  //! @brief Packed Compare for Equal (WORDS) (SSE2).
  void pcmpeqw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x75, dst.regCode(), src);
  }

  //! @brief Packed Compare for Equal (DWORDS) (SSE2).
  void pcmpeqd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x76, dst.regCode(), src);
  }

  //! @brief Packed Compare for Greater Than (BYTES) (SSE2).
  void pcmpgtb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x64, dst.regCode(), src);
  }

  //! @brief Packed Compare for Greater Than (WORDS) (SSE2).
  void pcmpgtw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x65, dst.regCode(), src);
  }

  //! @brief Packed Compare for Greater Than (DWORDS) (SSE2).
  void pcmpgtd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x66, dst.regCode(), src);
  }

  //! @brief Move Byte Mask (SSE2).
  void pmovmskb(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT(src.op() == OP_REG && (src.regType() == REG_MMX || src.regType() == REG_SSE));

    if (src.regType() == REG_MMX)
      emitMM(0x00, 0x00, 0x0F, 0xD7, dst.regCode(), src);
    else
      emitMM(0x00, 0x66, 0x0F, 0xD7, dst.regCode(), src);
  }

  //! @brief Packed Multiply High (SSE2).
  void pmulhw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xE5, dst.regCode(), src);
  }
  
  //! @brief Packed Multiply Low (SSE2).
  void pmullw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xD5, dst.regCode(), src);
  }

  //! @brief Packed Multiply to QWORD (SSE2).
  void pmuludq(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xF4, dst.regCode(), src);
  }

  //! @brief Packed Multiply to QWORD (SSE2).
  void pmuludq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xF4, dst.regCode(), src);
  }

  //! @brief Bitwise Logical OR (SSE2).
  void por(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xEB, dst.regCode(), src);
  }

  //! @brief Packed Shift Right Arithmetic (SSE2).
  void psraw(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xE1, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x71, 4, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }
  
  //! @brief Packed Shift Right Arithmetic (SSE2).
  void psrad(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xE2, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x72, 4, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }


  //! @brief Packed Shift Left Logical (SSE2).
  void psllw(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xF1, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x71, 6, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Shift Left Logical (SSE2).
  void pslld(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xF2, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x72, 6, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }
  
  //! @brief Packed Shift Left Logical (SSE2).
  void psllq(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xF3, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x73, 6, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Shift Left Logical (SSE2).
  void pslldq(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x73, 7, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Subtract (SSE2).
  void psubb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xF8, dst.regCode(), src);
  }

  //! @brief Packed Subtract (SSE2).
  void psubw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xF9, dst.regCode(), src);
  }

  //! @brief Packed Subtract (SSE2).
  void psubd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xFA, dst.regCode(), src);
  }

  //! @brief Packed Subtract (SSE2).
  void psubq(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || src.op() == OP_MEM);
    emitMM(0x00, 0x00, 0x0F, 0xFB, dst.regCode(), src);
  }

  //! @brief Packed Subtract (SSE2).
  void psubq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xFB, dst.regCode(), src);
  }

  //! @brief Packed Multiply and Add (SSE2).
  void pmaddwd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xF5, dst.regCode(), src);
  }

  //! @brief Shuffle Packed DWORDs (SSE2).
  void pshufd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x70, dst.regCode(), src, imm8);
  }

  //! @brief Shuffle Packed High Words (SSE2).
  void pshuhw(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x70, dst.regCode(), src, imm8);
  }

  //! @brief Shuffle Packed Low Words (SSE2).
  void pshulw(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0x70, dst.regCode(), src, imm8);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  void psrlw(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xD1, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x71, 2, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Shift Right Logical (SSE2).
  void psrld(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xD2, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x72, 2, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Shift Right Logical (SSE2).
  void psrlq(const XMMRegister& dst, const Op& src)
  {
    switch (src.op())
    {
      case OP_REG:
      case OP_MEM:
        emitMM(0x00, 0x66, 0x0F, 0xD3, dst.regCode(), src);
        return;
      case OP_IMM:
        emitMM(0x00, 0x66, 0x0F, 0x73, 2, dst, src.imm());
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Subtract with Saturation (SSE2).
  void psubsb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xE8, dst.regCode(), src);
  }
  
  //! @brief Packed Subtract with Saturation (SSE2).
  void psubsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xE9, dst.regCode(), src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  void psubusb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xD8, dst.regCode(), src);
  }
  
  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  void psubusw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xD9, dst.regCode(), src);
  }

  //! @brief Unpack High Data (SSE2).
  void punpckhbw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x68, dst.regCode(), src);
  }

  //! @brief Unpack High Data (SSE2).
  void punpckhwd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x69, dst.regCode(), src);
  }

  //! @brief Unpack High Data (SSE2).
  void punpckhdq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x6A, dst.regCode(), src);
  }

  //! @brief Unpack High Data (SSE2).
  void punpckhqdq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x6D, dst.regCode(), src);
  }

  //! @brief Unpack Low Data (SSE2).
  void punpcklbw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x60, dst.regCode(), src);
  }

  //! @brief Unpack Low Data (SSE2).
  void punpcklwd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x61, dst.regCode(), src);
  }

  //! @brief Unpack Low Data (SSE2).
  void punpckldq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x62, dst.regCode(), src);
  }

  //! @brief Unpack Low Data (SSE2).
  void punpcklqdq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x6C, dst.regCode(), src);
  }

  //! @brief Bitwise Exclusive OR (SSE2).
  void pxor(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xEF, dst.regCode(), src);
  }

  //! @brief Compute Square Roots of Packed DP-FP Values (SSE2).
  void sqrtpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x51, dst.regCode(), src);
  }

  //! @brief Compute Square Root of Scalar DP-FP Value (SSE2).
  void sqrtsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0x51, dst.regCode(), src);
  }

  //! @brief Packed DP-FP Subtract (SSE2).
  void subpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x5C, dst.regCode(), src);
  }

  //! @brief Scalar DP-FP Subtract (SSE2).
  void subsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0x5C, dst.regCode(), src);
  }

  //! @brief Scalar Unordered DP-FP Compare and Set EFLAGS (SSE2).
  void ucomisd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x2E, dst.regCode(), src);
  }

  //! @brief Unpack and Interleave High Packed Double-Precision FP Values (SSE2).
  void unpckhpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x15, dst.regCode(), src);
  }

  //! @brief Unpack and Interleave Low Packed Double-Precision FP Values (SSE2).
  void unpcklpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x14, dst.regCode(), src);
  }

  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  void xorpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x57, dst.regCode(), src);
  }

  // -------------------------------------------------------------------------
  // [SSE3]
  // -------------------------------------------------------------------------

  //! @brief Packed DP-FP Add/Subtract (SSE3).
  void addsubpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0xD0, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Add/Subtract (SSE3).
  void addsubps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0xD0, dst.regCode(), src);
  }

  //! @brief Store Integer with Truncation (SSE3).
  void fisttp(const Op& dst)
  {
    ASMJIT_ASSERT(dst.op() == OP_MEM);
    if (!ensureSpace()) return;

    switch (dst.size())
    {
      case 2:
        emitRex(0, 1, dst);
        emitByte(0xDF);
        emitMem(1, dst);
        return;
      case 4:
        emitRex(0, 1, dst);
        emitByte(0xDB);
        emitMem(1, dst);
        return;
      case 8:
        emitRex(0, 1, dst);
        emitByte(0xDD);
        emitMem(1, dst);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed DP-FP Horizontal Add (SSE3).
  void haddpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x7C, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Horizontal Add (SSE3).
  void haddps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0x7C, dst.regCode(), src);
  }

  //! @brief Packed DP-FP Horizontal Subtract (SSE3).
  void hsubpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0x66, 0x0F, 0x7D, dst.regCode(), src);
  }

  //! @brief Packed SP-FP Horizontal Subtract (SSE3).
  void hsubps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0x7D, dst.regCode(), src);
  }

  //! @brief Load Unaligned Integer 128 Bits (SSE3).
  void lddqu(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT(src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0xF0, dst.regCode(), src);
  }

  //! @brief Set Up Monitor Address (SSE3).
  void monitor()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x01);
    emitByte(0xC8);
  }

  //! @brief Move One DP-FP and Duplicate (SSE3).
  void movddup(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF2, 0x0F, 0x12, dst.regCode(), src);
  }

  //! @brief Move Packed SP-FP High and Duplicate (SSE3).
  void movshdup(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x16, dst.regCode(), src);
  }

  //! @brief Move Packed SP-FP Low and Duplicate (SSE3).
  void movsldup(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x00, 0xF3, 0x0F, 0x12, dst.regCode(), src);
  }

  //! @brief Monitor Wait (SSE3).
  void mwait()
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x01);
    emitByte(0xC9);
  }

  // -------------------------------------------------------------------------
  // [SSSE3]
  // -------------------------------------------------------------------------

  //! @brief Packed SIGN (SSSE3).
  void psignb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x08, dst.regCode(), src);
  }

  //! @brief Packed SIGN (SSSE3).
  void psignb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x08, dst.regCode(), src);
  }

  //! @brief Packed SIGN (SSSE3).
  void psignw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x09, dst.regCode(), src);
  }

  //! @brief Packed SIGN (SSSE3).
  void psignw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x09, dst.regCode(), src);
  }

  //! @brief Packed SIGN (SSSE3).
  void psignd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x0A, dst.regCode(), src);
  }

  //! @brief Packed SIGN (SSSE3).
  void psignd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x0A, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  void phaddw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x01, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  void phaddw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x01, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  void phaddd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x02, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  void phaddd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x02, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  void phaddsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x03, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  void phaddsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x03, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  void phsubw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x05, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  void phsubw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x05, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  void phsubd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x06, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  void phsubd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x06, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  void phsubsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x07, dst.regCode(), src);
  }

  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  void phsubsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x07, dst.regCode(), src);
  }

  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  void pmaddubsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x04, dst.regCode(), src);
  }

  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  void pmaddubsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x04, dst.regCode(), src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  void pabsb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x1C, dst.regCode(), src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  void pabsb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x1C, dst.regCode(), src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  void pabsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x1D, dst.regCode(), src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  void pabsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x1D, dst.regCode(), src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  void pabsd(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x1E, dst.regCode(), src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  void pabsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x1E, dst.regCode(), src);
  }

  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  void pmulhrsw(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x0B, dst.regCode(), src);
  }

  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  void pmulhrsw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x0B, dst.regCode(), src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  void pshufb(const MMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x38, 0x00, dst.regCode(), src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  void pshufb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x00, dst.regCode(), src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  void palignr(const MMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_MMX) || (src.op() == OP_MEM));
    emitMM(0x00, 0x0F, 0x3A, 0x0F, dst.regCode(), src, imm8);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  void palignr(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x3A, 0x0F, dst.regCode(), src, imm8);
  }

  // -------------------------------------------------------------------------
  // [SSE4.1]
  // -------------------------------------------------------------------------

  //! @brief Blend Packed DP-FP Values (SSE4.1).
  void blendpd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x3A, 0x0D, dst.regCode(), src, imm8);
  }

  //! @brief Blend Packed SP-FP Values (SSE4.1).
  void blendps(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x3A, 0x0C, dst.regCode(), src, imm8);
  }

  //! @brief Variable Blend Packed DP-FP Values (SSE4.1).
  void blendvpd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x15, dst.regCode(), src);
  }

  //! @brief Variable Blend Packed SP-FP Values (SSE4.1).
  void blendvps(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || (src.op() == OP_MEM));
    emitMM(0x66, 0x0F, 0x38, 0x14, dst.regCode(), src);
  }

  //! @brief Dot Product of Packed DP-FP Values (SSE4.1).
  void dppd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x41, dst.regCode(), src, imm8);
  } 

  //! @brief Dot Product of Packed SP-FP Values (SSE4.1).
  void dpps(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x40, dst.regCode(), src, imm8);
  } 

  //! @brief Extract Packed SP-FP Value @brief (SSE4.1).
  void extractps(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x17, dst.regCode(), src, imm8);
   }

  //! @brief Load Double Quadword Non-Temporal Aligned Hint (SSE4.1).
  void movntdqa(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT(src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x2A, dst.regCode(), src);
  }

  //! @brief Compute Multiple Packed Sums of Absolute Difference (SSE4.1).
  void mpsadbw(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x42, dst.regCode(), src, imm8);
  }

  //! @brief Pack with Unsigned Saturation (SSE4.1).
  void packusdw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x2B, dst.regCode(), src);
  }

  //! @brief Variable Blend Packed Bytes (SSE4.1).
  void pblendvb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x10, dst.regCode(), src);
  } 

  //! @brief Blend Packed Words (SSE4.1).
  void pblendw(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x0E, dst.regCode(), src, imm8);
  }

  //! @brief Compare Packed Qword Data for Equal (SSE4.1).
  void pcmpeqq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x29, dst.regCode(), src);
  }

  //! @brief Extract Byte (SSE4.1).
  void pextrb(const Op& dst, const XMMRegister& src, int imm8)
  {
    ASMJIT_ASSERT((dst.op() == OP_REG && dst.regType() == REG_GPD) ||
                  (dst.op() == OP_MEM && dst.size() == 1));
    emitMM(0x66, 0x0F, 0x3A, 0x14, src.regCode(), dst, imm8);
  }

  //! @brief Extract Word (SSE4.1).
  void pextrw(const Op& dst, const XMMRegister& src, int imm8)
  {
    ASMJIT_ASSERT((dst.op() == OP_REG && dst.regType() == REG_GPD) ||
                  (dst.op() == OP_MEM && dst.size() == 2));
    emitMM(0x66, 0x0F, 0x3A, 0x15, src.regCode(), dst, imm8);
  }

  //! @brief Extract Dword (SSE4.1).
  void pextrd(const Op& dst, const XMMRegister& src, int imm8)
  {
    ASMJIT_ASSERT((dst.op() == OP_REG && dst.regType() == REG_GPD) ||
                  (dst.op() == OP_MEM && dst.size() == 4));
    emitMM(0x66, 0x0F, 0x3A, 0x16, src.regCode(), dst, imm8);
  }

  //! @brief Packed Horizontal Word Minimum (SSE4.1).
  void phminposuw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x41, dst.regCode(), src);
  } 

  //! @brief Insert Byte (SSE4.1).
  void pinsrb(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x20, dst.regCode(), src, imm8);
  }

  //! @brief Insert Dword (SSE4.1).
  void pinsrd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x22, dst.regCode(), src, imm8);
  }

  //! @brief Maximum of Packed Word Integers (SSE4.1).
  void pmaxuw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x3E, dst.regCode(), src);
  }

  //! @brief Maximum of Packed Signed Byte Integers (SSE4.1).
  void pmaxsb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x3C, dst.regCode(), src);
  }

  //! @brief Maximum of Packed Signed Dword Integers (SSE4.1).
  void pmaxsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x3D, dst.regCode(), src);
  }

  //! @brief Maximum of Packed Unsigned Dword Integers (SSE4.1).
  void pmaxud(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x3F, dst.regCode(), src);
  }

  //! @brief Minimum of Packed Signed Byte Integers (SSE4.1).
  void pminsb(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x38, dst.regCode(), src);
  }

  //! @brief Minimum of Packed Word Integers (SSE4.1).
  void pminuw(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x3A, dst.regCode(), src);
  }

  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  void pminud(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x3B, dst.regCode(), src);
  }

  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  void pminsd(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x39, dst.regCode(), src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  void pmovsxbw(const XMMRegister& dst, const Op& xmm_m64)
  {
    ASMJIT_ASSERT((xmm_m64.op() == OP_REG && xmm_m64.regType() == REG_SSE) || xmm_m64.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x20, dst.regCode(), xmm_m64);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  void pmovsxbd(const XMMRegister& dst, const Op& xmm_m32)
  {
    ASMJIT_ASSERT((xmm_m32.op() == OP_REG && xmm_m32.regType() == REG_SSE) || xmm_m32.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x21, dst.regCode(), xmm_m32);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  void pmovsxbq(const XMMRegister& dst, const Op& xmm_m16)
  {
    ASMJIT_ASSERT((xmm_m16.op() == OP_REG && xmm_m16.regType() == REG_SSE) || xmm_m16.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x22, dst.regCode(), xmm_m16);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  void pmovsxwd(const XMMRegister& dst, const Op& xmm_m64)
  {
    ASMJIT_ASSERT((xmm_m64.op() == OP_REG && xmm_m64.regType() == REG_SSE) || xmm_m64.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x23, dst.regCode(), xmm_m64);
  }

  //! @brief (SSE4.1).
  void pmovsxwq(const XMMRegister& dst, const Op& xmm_m32)
  {
    ASMJIT_ASSERT((xmm_m32.op() == OP_REG && xmm_m32.regType() == REG_SSE) || xmm_m32.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x24, dst.regCode(), xmm_m32);
  }

  //! @brief (SSE4.1).
  void pmovsxdq(const XMMRegister& dst, const Op& xmm_m64)
  {
    ASMJIT_ASSERT((xmm_m64.op() == OP_REG && xmm_m64.regType() == REG_SSE) || xmm_m64.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x25, dst.regCode(), xmm_m64);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  void pmovzxbw(const XMMRegister& dst, const Op& xmm_m64)
  {
    ASMJIT_ASSERT((xmm_m64.op() == OP_REG && xmm_m64.regType() == REG_SSE) || xmm_m64.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x30, dst.regCode(), xmm_m64);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  void pmovzxbd(const XMMRegister& dst, const Op& xmm_m32)
  {
    ASMJIT_ASSERT((xmm_m32.op() == OP_REG && xmm_m32.regType() == REG_SSE) || xmm_m32.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x31, dst.regCode(), xmm_m32);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  void pmovzxbq(const XMMRegister& dst, const Op& xmm_m16)
  {
    ASMJIT_ASSERT((xmm_m16.op() == OP_REG && xmm_m16.regType() == REG_SSE) || xmm_m16.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x32, dst.regCode(), xmm_m16);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  void pmovzxwd(const XMMRegister& dst, const Op& xmm_m64)
  {
    ASMJIT_ASSERT((xmm_m64.op() == OP_REG && xmm_m64.regType() == REG_SSE) || xmm_m64.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x33, dst.regCode(), xmm_m64);
  }

  //! @brief (SSE4.1).
  void pmovzxwq(const XMMRegister& dst, const Op& xmm_m32)
  {
    ASMJIT_ASSERT((xmm_m32.op() == OP_REG && xmm_m32.regType() == REG_SSE) || xmm_m32.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x34, dst.regCode(), xmm_m32);
  }

  //! @brief (SSE4.1).
  void pmovzxdq(const XMMRegister& dst, const Op& xmm_m64)
  {
    ASMJIT_ASSERT((xmm_m64.op() == OP_REG && xmm_m64.regType() == REG_SSE) || xmm_m64.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x35, dst.regCode(), xmm_m64);
  }

  //! @brief Multiply Packed Signed Dword Integers (SSE4.1).
  void pmuldq(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x28, dst.regCode(), src);
  } 

  //! @brief Multiply Packed Signed Integers and Store Low Result (SSE4.1).
  void pmulld(const XMMRegister& dst, const Op& src)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x40, dst.regCode(), src);
  } 

  //! @brief Logical Compare (SSE4.1).
  void ptest(const XMMRegister& op1, const Op& op2)
  {
    ASMJIT_ASSERT((op2.op() == OP_REG && op2.regType() == REG_SSE) || op2.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x38, 0x17, op1.regCode(), op2);
  }

  //! Round Packed SP-FP Values @brief (SSE4.1).
  void roundps(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x08, dst.regCode(), src, imm8);
  }

  //! @brief Round Scalar SP-FP Values (SSE4.1).
  void roundss(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x0A, dst.regCode(), src, imm8);
  }

  //! @brief Round Packed DP-FP Values (SSE4.1).
  void roundpd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x09, dst.regCode(), src, imm8);
  }

  //! @brief Round Scalar DP-FP Values (SSE4.1).
  void roundsd(const XMMRegister& dst, const Op& src, int imm8)
  {
    ASMJIT_ASSERT((src.op() == OP_REG && src.regType() == REG_SSE) || src.op() == OP_MEM);
    emitMM(0x66, 0x0F, 0x3A, 0x0B, dst.regCode(), src, imm8);
  }

  // -------------------------------------------------------------------------
  // [SSE4.2]
  // -------------------------------------------------------------------------

  //! @brief Accumulate CRC32 Value (polynomial 0x11EDC6F41) (SSE4.2).
  void crc32(const Register& dst, const Op& src)
  {
    ASMJIT_ASSERT(dst.regType() == REG_GPD);

    switch (src.size())
    {
      case 1:
        emitMM(0xF2, 0x0F, 0x38, 0xF0, dst.regCode(), src);
        return;
      case 4:
        emitMM(0xF2, 0x0F, 0x38, 0xF1, dst.regCode(), src);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  //! @brief Packed Compare Explicit Length Strings, Return Index (SSE4.2).
  void pcmpestri(const XMMRegister& dst, const Op& src, int imm8)
  {
    emitMM(0x66, 0x0F, 0x3A, 0x61, dst.regCode(), src, imm8);
  }

  //! @brief Packed Compare Explicit Length Strings, Return Mask (SSE4.2).
  void pcmpestrm(const XMMRegister& dst, const Op& src, int imm8)
  {
    emitMM(0x66, 0x0F, 0x3A, 0x60, dst.regCode(), src, imm8);
  }

  //! @brief Packed Compare Implicit Length Strings, Return Index (SSE4.2).
  void pcmpistri(const XMMRegister& dst, const Op& src, int imm8)
  {
    emitMM(0x66, 0x0F, 0x3A, 0x63, dst.regCode(), src, imm8);
  }

  //! @brief Packed Compare Implicit Length Strings, Return Mask (SSE4.2).
  void pcmpistrm(const XMMRegister& dst, const Op& src, int imm8)
  {
    emitMM(0x66, 0x0F, 0x3A, 0x62, dst.regCode(), src, imm8);
  }

  //! @brief Compare Packed Data for Greater Than (SSE4.2).
  void pcmpgtq(const XMMRegister& dst, const Op& src)
  {
    emitMM(0x66, 0x0F, 0x38, 0x37, dst.regCode(), src);
  }

  //! @brief Return the Count of Number of Bits Set to 1 (SSE4.2).
  void popcnt(const Register& dst, const Op& src)
  {
    emitMM(0x00, 0xF3, 0x0F, 0xB8, dst.regCode(), src);
  }

  // -------------------------------------------------------------------------
  // [AMD only]
  // -------------------------------------------------------------------------

  //! @brief Prefetch (3dNow - Amd).
  //!
  //! Loads the entire 64-byte aligned memory sequence containing the 
  //! specified memory address into the L1 data cache. The position of 
  //! the specified memory address within the 64-byte cache line is
  //! irrelevant. If a cache hit occurs, or if a memory fault is detected, 
  //! no bus cycle is initiated and the instruction is treated as a NOP.
  void amd_prefetch(const Op& mem)
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x0D);
    emitMem(0, mem);
  }

  //! @brief Prefetch and set cache to modified (3dNow - Amd).
  //!
  //! The PREFETCHW instruction loads the prefetched line and sets the 
  //! cache-line state to Modified, in anticipation of subsequent data 
  //! writes to the line. The PREFETCH instruction, by contrast, typically
  //! sets the cache-line state to Exclusive (depending on the hardware 
  //! implementation).
  void amd_prefetchw(const Op& mem)
  {
    if (!ensureSpace()) return;
    emitByte(0x0F);
    emitByte(0x0D);
    emitMem(1, mem);
  }

  // -------------------------------------------------------------------------
  // [Intel only]
  // -------------------------------------------------------------------------

  //! @brief Move Data After Swapping Bytes (SSE3 - Intel Atom).
  void movbe(const Op& _dst, const Op& _src)
  {
    if (!ensureSpace()) return;

    const Op* dst = &_dst;
    const Op* src = &_src;
    UInt8 opCode = 0xF0;

    switch (dst->op() << 4 | src->op())
    {
      case (OP_MEM << 4) | OP_REG: // Mem <- Reg
        dst = &_src;
        src = &_dst;
        opCode++;
        // ... fall through ...
      case (OP_REG << 4) | OP_MEM: // Reg <- Mem
        ASMJIT_ASSERT(dst->regType() == REG_GPW || 
                      dst->regType() == REG_GPD ||
                      dst->regType() == REG_GPQ);

        if (dst->regType() == REG_GPW) emitByte(0x66); // 16 bit
        emitRex(dst->regType() == REG_GPQ, dst->regCode(), *src);
        emitByte(0x0F);
        emitByte(0x38);
        emitByte(opCode);
        emitMem(dst->regCode(), *src);
        return;
    }
    ASMJIT_ILLEGAL();
  }

  // -------------------------------------------------------------------------
  // [Bind and Labels Declaration]
  // -------------------------------------------------------------------------

  //! @brief Bind label to the current offset.
  //!
  //! @note Label can be bound only once!
  void bind(Label* L)
  {
    // label can only be bound once
    ASMJIT_ASSERT(!L->isBound());
    bindTo(L, offset());
  }

  //! @brief Bind label to pos - called from bind(Label*L).
  void bindTo(Label* L, SysInt pos)
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
      SysInt immx = pos - (fixup_pos + sizeof(SysInt));
      setIntAt(fixup_pos, immx);
      disp.next(L);
    }
    L->bindTo(pos);
  }

  //! @brief link label, called internally from jumpers.
  void linkTo(Label* L, Label* appendix)
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

  // -------------------------------------------------------------------------
  // [Internal buffer]
  // -------------------------------------------------------------------------

  //! @brief Beginning position of internal buffer.
  UInt8* pData;
  //! @brief Current position in internal buffer.
  UInt8* pCur;
  //! @brief Maximum position in internal buffer for realloc.
  UInt8* pMax;

  //! @brief Size of internal buffer.
  SysInt _capacity;
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJIT_X86_H
