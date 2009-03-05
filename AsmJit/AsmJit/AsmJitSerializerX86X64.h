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
#ifndef _ASMJITSERIALIZERX86X64_H
#define _ASMJITSERIALIZERX86X64_H

// [Dependencies]
#include "AsmJitBuild.h"
#include "AsmJitDefs.h"
#include "AsmJitUtil.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127) // conditional expression is constant
#endif // _MSC_VER

namespace AsmJit {

// ============================================================================
// [AsmJit::_Serializer]
// ============================================================================

//! @brief Assembler instruction seralizer base.
//!
//! @note Use always @c Serializer class, this class is only designed to 
//! decrease code size when exporting AsmJit library symbols. Some compilers
//! (for example MSVC) are exporting inline symbols when class is declared 
//! to export them and @c Serializer class contains really huge count of 
//! them.
struct ASMJIT_API _Serializer
{
  // -------------------------------------------------------------------------
  // [Construction / Destruction]
  // -------------------------------------------------------------------------

  //! @brief Create _Serializer instance.
  _Serializer();
  //! @brief Destroy _Serializer instance.
  virtual ~_Serializer();

  // -------------------------------------------------------------------------
  // [Abstract Emitters]
  // -------------------------------------------------------------------------

  //! @brief Emits X86/FPU or MM instruction.
  //!
  //! Operands @a o1, @a o2 or @a o3 can't be @c NULL.
  //!
  //! Use @c __emitX86() helpers to emit instructions.
  virtual void _emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3) = 0;

  // -------------------------------------------------------------------------
  // [Align]
  // -------------------------------------------------------------------------

  //! @brief Align buffer to @a m bytes.
  //!
  //! Typical usage of this is to align labels at start of inner loops.
  //!
  //! Inserts @c nop() instructions.
  virtual void align(SysInt m) = 0;

  // -------------------------------------------------------------------------
  // [Bind]
  // -------------------------------------------------------------------------

  //! @brief Bind label to the current offset.
  //!
  //! @note Label can be bound only once!
  virtual void bind(Label* label) = 0;

protected:
  // helpers to decrease binary code size
  void __emitX86(UInt32 code);
  void __emitX86(UInt32 code, const Operand* o1);
  void __emitX86(UInt32 code, const Operand* o1, const Operand* o2);
  void __emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);

  //! @brief Private method for emitting jcc.
  void _emitJcc(CONDITION cc, Label* label, UInt32 hint);
  //! @brief Private method for emitting jcc.
  void _emitJ(UInt32 code, Label* label, UInt32 hint);

  static const UInt32 _jcctable[16];
  static const UInt32 _cmovcctable[16];

private:
  // disable copy
  inline _Serializer(const _Serializer& other);
  inline _Serializer& operator=(const _Serializer& other);
};

// ============================================================================
// [AsmJit::Serializer]
// ============================================================================

//! @brief Assembler instruction serializer.
struct Serializer : public _Serializer
{
  // -------------------------------------------------------------------------
  // [X86 Instructions]
  // -------------------------------------------------------------------------

  //! @brief Add with Carry.
  inline void adc(const Register& dst, const Register& src)
  {
    __emitX86(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const Register& dst, const Mem& src)
  {
    __emitX86(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const Mem& dst, const Register& src)
  {
    __emitX86(INST_ADC, &dst, &src);
  }
  //! @brief Add with Carry.
  inline void adc(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_ADC, &dst, &src);
  }

  //! @brief Add.
  inline void add(const Register& dst, const Register& src)
  {
    __emitX86(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const Register& dst, const Mem& src)
  {
    __emitX86(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const Mem& dst, const Register& src)
  {
    __emitX86(INST_ADD, &dst, &src);
  }
  //! @brief Add.
  inline void add(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_ADD, &dst, &src);
  }

  //! @brief Logical And.
  inline void and_(const Register& dst, const Register& src)
  {
    __emitX86(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const Register& dst, const Mem& src)
  {
    __emitX86(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const Mem& dst, const Register& src)
  {
    __emitX86(INST_AND, &dst, &src);
  }
  //! @brief Logical And.
  inline void and_(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_AND, &dst, &src);
  }

  //! @brief Bit Scan Forward.
  inline void bsf(const Register& dst, const Register& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    __emitX86(INST_BSF, &dst, &src);
  }
  //! @brief Bit Scan Forward.
  inline void bsf(const Register& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    __emitX86(INST_BSF, &dst, &src);
  }

  //! @brief Bit Scan Reverse.
  inline void bsr(const Register& dst, const Register& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    __emitX86(INST_BSR, &dst, &src);
  }
  //! @brief Bit Scan Reverse.
  inline void bsr(const Register& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    __emitX86(INST_BSR, &dst, &src);
  }

  //! @brief Byte swap (32 bit or 64 bit registers only) (i486).
  inline void bswap(const Register& dst)
  {
    ASMJIT_ASSERT(dst.type() == REG_GPD || dst.type() == REG_GPQ);
    __emitX86(INST_BSWAP, &dst);
  }

  //! @brief Bit test.
  inline void bt(const Register& dst, const Register& src)
  {
    __emitX86(INST_BT, &dst, &src);
  }
  //! @brief Bit test.
  inline void bt(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_BT, &dst, &src);
  }
  //! @brief Bit test.
  inline void bt(const Mem& dst, const Register& src)
  {
    __emitX86(INST_BT, &dst, &src);
  }
  //! @brief Bit test.
  inline void bt(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_BT, &dst, &src);
  }

  //! @brief Bit test and complement.
  inline void btc(const Register& dst, const Register& src)
  {
    __emitX86(INST_BTC, &dst, &src);
  }
  //! @brief Bit test and complement.
  inline void btc(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_BTC, &dst, &src);
  }
  //! @brief Bit test and complement.
  inline void btc(const Mem& dst, const Register& src)
  {
    __emitX86(INST_BTC, &dst, &src);
  }
  //! @brief Bit test and complement.
  inline void btc(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_BTC, &dst, &src);
  }

  //! @brief Bit test and reset.
  inline void btr(const Register& dst, const Register& src)
  {
    __emitX86(INST_BTR, &dst, &src);
  }
  //! @brief Bit test and reset.
  inline void btr(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_BTR, &dst, &src);
  }
  //! @brief Bit test and reset.
  inline void btr(const Mem& dst, const Register& src)
  {
    __emitX86(INST_BTR, &dst, &src);
  }
  //! @brief Bit test and reset.
  inline void btr(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_BTR, &dst, &src);
  }

  //! @brief Bit test and set.
  inline void bts(const Register& dst, const Register& src)
  {
    __emitX86(INST_BTS, &dst, &src);
  }
  //! @brief Bit test and set.
  inline void bts(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_BTS, &dst, &src);
  }
  //! @brief Bit test and set.
  inline void bts(const Mem& dst, const Register& src)
  {
    __emitX86(INST_BTS, &dst, &src);
  }
  //! @brief Bit test and set.
  inline void bts(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_BTS, &dst, &src);
  }

  //! @brief Call Procedure.
  inline void call(const Register& dst)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_GPN));
    __emitX86(INST_CALL, &dst);
  }
  //! @brief Call Procedure.
  inline void call(const Mem& dst)
  {
    __emitX86(INST_CALL, &dst);
  }
  //! @brief Call Procedure.
  inline void call(Label* label)
  {
    __emitX86(INST_CALL, label);
  }

  //! @brief Convert Byte to Word (Sign Extend).
  //!
  //! AX <- Sign Extend AL
  inline void cbw() 
  {
    __emitX86(INST_CBW);
  }

  //! @brief Convert Word to DWord (Sign Extend).
  //!
  //! EAX <- Sign Extend AX
  inline void cwde() 
  {
    __emitX86(INST_CWDE);
  }

#if defined(ASMJIT_X64)
  //! @brief Convert DWord to QWord (Sign Extend).
  //!
  //! RAX <- Sign Extend EAX
  inline void cdqe() 
  {
    __emitX86(INST_CDQE);
  }
#endif // ASMJIT_X64

  //! @brief Clear CARRY flag
  //!
  //! This instruction clears the CF flag in the EFLAGS register.
  inline void clc()
  {
    __emitX86(INST_CLC);
  }

  //! @brief Clear Direction flag
  //!
  //! This instruction clears the DF flag in the EFLAGS register.
  inline void cld()
  {
    __emitX86(INST_CLD);
  }

  //! @brief Complement Carry Flag.
  //!
  //! This instruction complements the CF flag in the EFLAGS register.
  //! (CF = NOT CF)
  inline void cmc()
  {
    __emitX86(INST_CMC);
  }

  //! @brief Conditional Move.
  inline void cmov(CONDITION cc, const Register& dst, const BaseRegMem& src)
  {
    ASMJIT_ASSERT(static_cast<UInt32>(cc) <= 0xF);
    __emitX86(_cmovcctable[cc], &dst, &src);
  }

  //! @brief Conditional Move.
  inline void cmova  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVA  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovae (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVAE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovb  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVB  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovbe (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVBE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovc  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVC  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmove  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVE  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovg  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVG  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovge (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVGE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovl  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVL  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovle (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVLE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovna (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNA , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnae(const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNAE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnb (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNB , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnbe(const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNBE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnc (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNC , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovne (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovng (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNG , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnge(const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNGE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnl (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNL , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnle(const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNLE, &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovno (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNO , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnp (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNP , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovns (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNS , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovnz (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVNZ , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovo  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVO  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovp  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVP  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovpe (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVPE , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovpo (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVPO , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovs  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVS  , &dst, &src); }
  //! @brief Conditional Move.
  inline void cmovz  (const Register& dst, const BaseRegMem& src) { __emitX86(INST_CMOVZ  , &dst, &src); }

  //! @brief Compare Two Operands.
  inline void cmp(const Register& dst, const Register& src)
  {
    __emitX86(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const Register& dst, const Mem& src)
  {
    __emitX86(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const Mem& dst, const Register& src)
  {
    __emitX86(INST_CMP, &dst, &src);
  }
  //! @brief Compare Two Operands.
  inline void cmp(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_CMP, &dst, &src);
  }

  //! @brief Compare and Exchange (i486).
  inline void cmpxchg(const Register& dst, const Register& src)
  {
    __emitX86(INST_CMPXCHG, &dst, &src);
  }
  //! @brief Compare and Exchange (i486).
  inline void cmpxchg(const Mem& dst, const Register& src)
  {
    __emitX86(INST_CMPXCHG, &dst, &src);
  }

  //! @brief Compares the 64-bit value in EDX:EAX with the memory operand (Pentium).
  //!
  //! If the values are equal, then this instruction stores the 64-bit value 
  //! in ECX:EBX into the memory operand and sets the zero flag. Otherwise,
  //! this instruction copies the 64-bit memory operand into the EDX:EAX 
  //! registers and clears the zero flag.
  inline void cmpxchg8b(const Mem& dst)
  {
    __emitX86(INST_CMPXCHG8B, &dst);
  }

#if defined(ASMJIT_X64)
  //! @brief Compares the 128-bit value in RDX:RAX with the memory operand.
  //!
  //! If the values are equal, then this instruction stores the 128-bit value 
  //! in RCX:RBX into the memory operand and sets the zero flag. Otherwise,
  //! this instruction copies the 128-bit memory operand into the RDX:RAX 
  //! registers and clears the zero flag.
  inline void cmpxchg16b(const Mem& dst)
  {
    __emitX86(INST_CMPXCHG16B, &dst);
  }
#endif // ASMJIT_X64

  //! @brief CPU Identification (i486).
  inline void cpuid()
  {
    __emitX86(INST_CPUID);
  }

#if defined(ASMJIT_X86)
  //! @brief Decimal adjust AL after addition
  //!
  //! This instruction adjusts the sum of two packed BCD values to create
  //! a packed BCD result.
  //!
  //! @note This instruction is only available in 32 bit mode.
  inline void daa()
  {
    __emitX86(INST_DAA);
  }
#endif // ASMJIT_X86

#if defined(ASMJIT_X86)
  //! @brief Decimal adjust AL after subtraction
  //!
  //! This instruction adjusts the result of the subtraction of two packed 
  //! BCD values to create a packed BCD result.
  //!
  //! @note This instruction is only available in 32 bit mode.
  inline void das()
  {
    __emitX86(INST_DAS);
  }
#endif // ASMJIT_X86

  //! @brief Decrement by 1.
  //! @note This instruction can be slower than sub(dst, 1)
  inline void dec(const Register& dst)
  {
    __emitX86(INST_DEC, &dst);
  }
  //! @brief Decrement by 1.
  //! @note This instruction can be slower than sub(dst, 1)
  inline void dec(const Mem& dst)
  {
    __emitX86(INST_DEC, &dst);
  }

  //! @brief Unsigned divide.
  //!
  //! This instruction divides (unsigned) the value in the AL, AX, or EAX 
  //! register by the source operand and stores the result in the AX, 
  //! DX:AX, or EDX:EAX registers.
  inline void div(const Register& src)
  {
    __emitX86(INST_DIV, &src);
  }
  //! @brief Unsigned divide.
  //! @overload
  inline void div(const Mem& src)
  {
    __emitX86(INST_DIV, &src);
  }

  //! @brief Signed divide.
  //!
  //! This instruction divides (signed) the value in the AL, AX, or EAX 
  //! register by the source operand and stores the result in the AX, 
  //! DX:AX, or EDX:EAX registers.
  inline void idiv(const Register& src)
  {
    __emitX86(INST_IDIV, &src);
  }
  //! @brief Signed divide.
  //! @overload
  inline void idiv(const Mem& src)
  {
    __emitX86(INST_IDIV, &src);
  }

  //! @brief Signed multiply.
  //!
  //! Source operand (in a general-purpose register or memory location) 
  //! is multiplied by the value in the AL, AX, or EAX register (depending
  //! on the operand size) and the product is stored in the AX, DX:AX, or 
  //! EDX:EAX registers, respectively.
  inline void imul(const Register& src)
  {
    __emitX86(INST_IMUL, &src);
  }
  //! @overload
  inline void imul(const Mem& src)
  {
    __emitX86(INST_IMUL, &src);
  }

  //! @brief Signed multiply.
  //!
  //! Destination operand (the first operand) is multiplied by the source 
  //! operand (second operand). The destination operand is a generalpurpose
  //! register and the source operand is an immediate value, a general-purpose 
  //! register, or a memory location. The product is then stored in the 
  //! destination operand location.
  inline void imul(const Register& dst, const Register& src) 
  {
    __emitX86(INST_IMUL, &dst, &src);
  }
  //! @brief Signed multiply.
  //! @overload
  inline void imul(const Register& dst, const Mem& src) 
  {
    __emitX86(INST_IMUL, &dst, &src);
  }
  //! @brief Signed multiply.
  //! @overload
  inline void imul(const Register& dst, const Immediate& src) 
  {
    __emitX86(INST_IMUL, &dst, &src);
  }

  //! @brief Signed multiply.
  //!
  //! source operand (which can be a general-purpose register or a memory 
  //! location) is multiplied by the second source operand (an immediate 
  //! value). The product is then stored in the destination operand 
  //! (a general-purpose register).
  inline void imul(const Register& dst, const Register& src, const Immediate& imm)
  {
    __emitX86(INST_IMUL, &dst, &src, &imm);
  }
  //! @overload
  inline void imul(const Register& dst, const Mem& src, const Immediate& imm)
  {
    __emitX86(INST_IMUL, &dst, &src, &imm);
  }

  //! @brief Increment by 1.
  //! @note This instruction can be slower than add(dst, 1)
  inline void inc(const Register& dst)
  {
    __emitX86(INST_INC, &dst);
  }
  //! @brief Increment by 1.
  //! @note This instruction can be slower than add(dst, 1)
  inline void inc(const Mem& dst)
  {
    __emitX86(INST_INC, &dst);
  }

  //! @brief Interrupt 3 — trap to debugger.
  inline void int3()
  {
    __emitX86(INST_INT3);
  }

  //! @brief Jump to label @a label if condition @a cc is met.
  //!
  //! This instruction checks the state of one or more of the status flags in 
  //! the EFLAGS register (CF, OF, PF, SF, and ZF) and, if the flags are in the 
  //! specified state (condition), performs a jump to the target instruction 
  //! specified by the destination operand. A condition code (cc) is associated
  //! with each instruction to indicate the condition being tested for. If the 
  //! condition is not satisfied, the jump is not performed and execution 
  //! continues with the instruction following the Jcc instruction.
  inline void j(CONDITION cc, Label* label, UInt32 hint = HINT_NONE)
  {
    ASMJIT_ASSERT(static_cast<UInt32>(cc) <= 0xF);
    _emitJ(_jcctable[cc], label, hint);
  }

  //! @brief Jump to label @a label if condition is met.
  inline void ja  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JA  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jae (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JAE , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jb  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JB  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jbe (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JBE , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jc  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JC  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void je  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JE  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jg  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JG  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jge (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JGE , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jl  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JL  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jle (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JLE , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jna (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNA , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnae(Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNAE, label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnb (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNB , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnbe(Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNBE, label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnc (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNC , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jne (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNE , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jng (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNG , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnge(Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNGE, label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnl (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNL , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnle(Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNLE, label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jno (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNO , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnp (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNP , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jns (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNS , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jnz (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JNZ , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jo  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JO  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jp  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JP  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jpe (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JPE , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jpo (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JPO , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void js  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JS  , label, hint); }
  //! @brief Jump to label @a label if condition is met.
  inline void jz  (Label* label, UInt32 hint = HINT_NONE) { _emitJ(INST_JZ  , label, hint); }

  //! @brief Jump.
  //!
  //! This instruction transfers program control to a different point
  //! in the instruction stream without recording return information. 
  //! The destination (target) operand specifies the address of the
  //! instruction being jumped to.
  inline void jmp(Label* label)
  {
    __emitX86(INST_JMP, label);
  }

  //! @overload
  inline void jmp(const BaseRegMem& dst)
  {
    __emitX86(INST_JMP, &dst);
  }

  //! @brief Same instruction as @c jmp(), but target is given as a pointer
  //! to memory location that will be patched by relocCode() to real relative
  //! address.
  //!
  //! This function are introduced here for performance, but it can be danger
  //! to use it, because relative address is always 32 bit displacement (on
  //! 64 bit platforms too!). So if the difference between target address and
  //! current address is larger than 31 bits, jmp() can't be created. For that
  //! reasons, there is second parameter (temporary register) that can be used
  //! if relative addressing fails.
  //!
  //! So, jmp_rel will always reserve some space for failure cases and can
  //! overwrite @a temporary register.
  inline void jmp_ptr(void* ptr, const Register& temporary)
  {
    Immediate i((SysInt)ptr);
    // FIXME: I don't know why I need cast to Operand*
    __emitX86(INST_JMP_PTR, (const Operand*)(&i), &temporary);
  }

  //! @brief Load Effective Address
  //!
  //! This instruction computes the effective address of the second 
  //! operand (the source operand) and stores it in the first operand 
  //! (destination operand). The source operand is a memory address
  //! (offset part) specified with one of the processors addressing modes.
  //! The destination operand is a general-purpose register.
  inline void lea(const Register& dst, const Mem& src)
  {
    __emitX86(INST_LEA, &dst, &src);
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
  inline void lock()
  {
    __emitX86(INST_LOCK);
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
  inline void mov(const Register& dst, const Register& src)
  {
    __emitX86(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const Register& dst, const Mem& src)
  {
    __emitX86(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const Mem& dst, const Register& src)
  {
    __emitX86(INST_MOV, &dst, &src);
  }
  //! @brief Move.
  //! @overload
  inline void mov(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_MOV, &dst, &src);
  }

  //! @brief Move byte, word, dword or qword from absolute address @a src to
  //! AL, AX, EAX or RAX register.
  inline void mov_ptr(const Register& dst, void* src)
  {
    ASMJIT_ASSERT(dst.code() == 0);
    Immediate imm((SysInt)src);
    __emitX86(INST_MOV_PTR, &dst, &imm);
  }

  //! @brief Move byte, word, dword or qword from AL, AX, EAX or RAX register
  //! to absolute address @a dst.
  inline void mov_ptr(void* dst, const Register& src)
  {
    ASMJIT_ASSERT(src.code() == 0);
    Immediate imm((SysInt)dst);
    __emitX86(INST_MOV_PTR, &imm, &src);
  }

  //! @brief Move with Sign-Extension.
  //!
  //! This instruction copies the contents of the source operand (register 
  //! or memory location) to the destination operand (register) and sign 
  //! extends the value to 16, 32 or 64 bits.
  //!
  //! @sa movsxd().
  void movsx(const Register& dst, const Register& src)
  {
    __emitX86(INST_MOVSX, &dst, &src);
  }
  //! @brief Move with Sign-Extension.
  //! @overload
  void movsx(const Register& dst, const Mem& src)
  {
    __emitX86(INST_MOVSX, &dst, &src);
  }

#if defined(ASMJIT_X64)
  //! @brief Move DWord to QWord with sign-extension.
  inline void movsxd(const Register& dst, const Register& src)
  {
    __emitX86(INST_MOVSXD, &dst, &src);
  }
  //! @brief Move DWord to QWord with sign-extension.
  //! @overload
  inline void movsxd(const Register& dst, const Mem& src)
  {
    __emitX86(INST_MOVSXD, &dst, &src);
  }
#endif // ASMJIT_X64

  //! @brief Move with Zero-Extend.
  //!
  //! This instruction copies the contents of the source operand (register 
  //! or memory location) to the destination operand (register) and zero 
  //! extends the value to 16 or 32 bits. The size of the converted value 
  //! depends on the operand-size attribute.
  inline void movzx(const Register& dst, const Register& src)
  {
    __emitX86(INST_MOVZX, &dst, &src);
  }
  //! @brief Move with Zero-Extend.
  //! @brief Overload
  inline void movzx(const Register& dst, const Mem& src)
  {
    __emitX86(INST_MOVZX, &dst, &src);
  }

  //! @brief Unsigned multiply.
  //!
  //! Source operand (in a general-purpose register or memory location) 
  //! is multiplied by the value in the AL, AX, or EAX register (depending
  //! on the operand size) and the product is stored in the AX, DX:AX, or 
  //! EDX:EAX registers, respectively.
  inline void mul(const Register& src)
  {
    __emitX86(INST_MUL, &src);
  }
  //! @brief Unsigned multiply.
  //! @overload
  inline void mul(const Mem& src)
  {
    __emitX86(INST_MUL, &src);
  }

  //! @brief Two's Complement Negation.
  inline void neg(const Register& dst)
  {
    __emitX86(INST_NEG, &dst);
  }
  //! @brief Two's Complement Negation.
  inline void neg(const Mem& dst)
  {
    __emitX86(INST_NEG, &dst);
  }

  //! @brief No Operation.
  //!
  //! This instruction performs no operation. This instruction is a one-byte 
  //! instruction that takes up space in the instruction stream but does not 
  //! affect the machine context, except the EIP register. The NOP instruction 
  //! is an alias mnemonic for the XCHG (E)AX, (E)AX instruction.
  inline void nop()
  {
    __emitX86(INST_NOP);
  }

  //! @brief One's Complement Negation.
  inline void not_(const Register& dst)
  {
    __emitX86(INST_NOT, &dst);
  }
  //! @brief One's Complement Negation.
  inline void not_(const Mem& dst)
  {
    __emitX86(INST_NOT, &dst);
  }

  //! @brief Logical Inclusive OR.
  inline void or_(const Register& dst, const Register& src)
  {
    __emitX86(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const Register& dst, const Mem& src)
  {
    __emitX86(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const Mem& dst, const Register& src)
  {
    __emitX86(INST_OR, &dst, &src);
  }
  //! @brief Logical Inclusive OR.
  inline void or_(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_OR, &dst, &src);
  }

  //! @brief Pop a Value from the Stack.
  //!
  //! This instruction loads the value from the top of the stack to the location 
  //! specified with the destination operand and then increments the stack pointer.
  //! The destination operand can be a general purpose register, memory location, 
  //! or segment register.
  inline void pop(const Register& dst)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_GPW) || dst.isRegType(REG_GPN));
    __emitX86(INST_POP, &dst);
  }

  inline void pop(const Mem& dst)
  {
    ASMJIT_ASSERT(dst.size() == 2 || dst.size() == sizeof(SysInt));
    __emitX86(INST_POP, &dst);
  }

#if defined(ASMJIT_X86)
  //! @brief Pop All General-Purpose Registers.
  //!
  //! Pop EDI, ESI, EBP, EBX, EDX, ECX, and EAX.
  inline void popad()
  {
    __emitX86(INST_POPAD);
  }
#endif // ASMJIT_X86

  //! @brief Pop Stack into EFLAGS Register (32 bit or 64 bit).
  inline void popf()
  {
#if defined(ASMJIT_X86)
    popfd();
#else
    popfq();
#endif
  }

#if defined(ASMJIT_X86)
  //! @brief Pop Stack into EFLAGS Register (32 bit).
  inline void popfd() { __emitX86(INST_POPFD); }
#else
  //! @brief Pop Stack into EFLAGS Register (64 bit).
  inline void popfq() { __emitX86(INST_POPFQ); }
#endif

  //! @brief Push WORD/DWORD/QWORD Onto the Stack.
  //!
  //! @note 32 bit architecture pushed DWORD while 64 bit 
  //! pushes QWORD. 64 bit mode not provides instruction to
  //! push 32 bit register/memory.
  inline void push(const Register& src)
  {
    ASMJIT_ASSERT(src.isRegType(REG_GPW) || src.isRegType(REG_GPN));
    __emitX86(INST_PUSH, &src);
  }
  //! @brief Push WORD/DWORD/QWORD Onto the Stack.
  inline void push(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == sizeof(SysInt));
    __emitX86(INST_PUSH, &src);
  }
  //! @brief Push WORD/DWORD/QWORD Onto the Stack.
  inline void push(const Immediate& src)
  {
    __emitX86(INST_PUSH, &src);
  }

#if defined(ASMJIT_X86)
  //! @brief Push All General-Purpose Registers.
  //!
  //! Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI.
  inline void pushad()
  {
    __emitX86(INST_PUSHAD);
  }
#endif // ASMJIT_X86

  //! @brief Push EFLAGS Register (32 bit or 64 bit) onto the Stack.
  inline void pushf()
  {
#if defined(ASMJIT_X86)
    pushfd();
#else
    pushfq();
#endif
  }

#if defined(ASMJIT_X86)
  //! @brief Push EFLAGS Register (32 bit) onto the Stack.
  inline void pushfd() { __emitX86(INST_PUSHFD); }
#else
  //! @brief Push EFLAGS Register (64 bit) onto the Stack.
  inline void pushfq() { __emitX86(INST_PUSHFQ); }
#endif // ASMJIT_X86

  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rcl(const Register& dst, const Register& src)
  {
    __emitX86(INST_RCL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rcl(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_RCL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rcl(const Mem& dst, const Register& src)
  {
    __emitX86(INST_RCL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rcl(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_RCL, &dst, &src);
  }

  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void rcr(const Register& dst, const Register& src)
  {
    __emitX86(INST_RCR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void rcr(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_RCR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void rcr(const Mem& dst, const Register& src)
  {
    __emitX86(INST_RCR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void rcr(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_RCR, &dst, &src);
  }

  //! @brief Read Time-Stamp Counter (Pentium).
  inline void rdtsc()
  {
    __emitX86(INST_RDTSC);
  }

  //! @brief Read Time-Stamp Counter and Processor ID (New).
  inline void rdtscp()
  {
    __emitX86(INST_RDTSCP);
  }

  //! @brief Return from Procedure.
  inline void ret()
  {
    __emitX86(INST_RET);
  }

  //! @brief Return from Procedure.
  inline void ret(const Immediate& imm16)
  {
    __emitX86(INST_RET, &imm16);
  }

  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rol(const Register& dst, const Register& src)
  {
    __emitX86(INST_ROL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rol(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_ROL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  //! @note @a src register can be only @c cl.
  inline void rol(const Mem& dst, const Register& src)
  {
    __emitX86(INST_ROL, &dst, &src);
  }
  //! @brief Rotate Bits Left.
  inline void rol(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_ROL, &dst, &src);
  }

  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void ror(const Register& dst, const Register& src)
  {
    __emitX86(INST_ROR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void ror(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_ROR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  //! @note @a src register can be only @c cl.
  inline void ror(const Mem& dst, const Register& src)
  {
    __emitX86(INST_ROR, &dst, &src);
  }
  //! @brief Rotate Bits Right.
  inline void ror(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_ROR, &dst, &src);
  }

#if defined(ASMJIT_X86)
  //! @brief Store AH into Flags.
  inline void sahf()
  {
    __emitX86(INST_SAHF);
  }
#endif // ASMJIT_X86

  //! @brief Integer subtraction with borrow.
  inline void sbb(const Register& dst, const Register& src)
  {
    __emitX86(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const Register& dst, const Mem& src)
  {
    __emitX86(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const Mem& dst, const Register& src)
  {
    __emitX86(INST_SBB, &dst, &src);
  }
  //! @brief Integer subtraction with borrow.
  inline void sbb(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_SBB, &dst, &src);
  }

  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void sal(const Register& dst, const Register& src)
  {
    __emitX86(INST_SAL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void sal(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_SAL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void sal(const Mem& dst, const Register& src)
  {
    __emitX86(INST_SAL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void sal(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_SAL, &dst, &src);
  }

  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void sar(const Register& dst, const Register& src)
  {
    __emitX86(INST_SAR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void sar(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_SAR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void sar(const Mem& dst, const Register& src)
  {
    __emitX86(INST_SAR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void sar(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_SAR, &dst, &src);
  }

  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void shl(const Register& dst, const Register& src)
  {
    __emitX86(INST_SHL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void shl(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_SHL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  //! @note @a src register can be only @c cl.
  inline void shl(const Mem& dst, const Register& src)
  {
    __emitX86(INST_SHL, &dst, &src);
  }
  //! @brief Shift Bits Left.
  inline void shl(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_SHL, &dst, &src);
  }

  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void shr(const Register& dst, const Register& src)
  {
    __emitX86(INST_SHR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void shr(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_SHR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  //! @note @a src register can be only @c cl.
  inline void shr(const Mem& dst, const Register& src)
  {
    __emitX86(INST_SHR, &dst, &src);
  }
  //! @brief Shift Bits Right.
  inline void shr(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_SHR, &dst, &src);
  }

  //! @brief Double Precision Shift Left.
  //! @note src2 register can be only @c cl register.
  inline void shld(const Register& dst, const Register& src1, const Register& src2)
  {
    __emitX86(INST_SHLD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Left.
  inline void shld(const Register& dst, const Register& src1, const Immediate& src2)
  {
    __emitX86(INST_SHLD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Left.
  //! @note src2 register can be only @c cl register.
  inline void shld(const Mem& dst, const Register& src1, const Register& src2)
  {
    __emitX86(INST_SHLD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Left.
  inline void shld(const Mem& dst, const Register& src1, const Immediate& src2)
  {
    __emitX86(INST_SHLD, &dst, &src1, &src2);
  }

  //! @brief Double Precision Shift Right.
  //! @note src2 register can be only @c cl register.
  inline void shrd(const Register& dst, const Register& src1, const Register& src2)
  {
    __emitX86(INST_SHRD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Right.
  inline void shrd(const Register& dst, const Register& src1, const Immediate& src2)
  {
    __emitX86(INST_SHRD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Right.
  //! @note src2 register can be only @c cl register.
  inline void shrd(const Mem& dst, const Register& src1, const Register& src2)
  {
    __emitX86(INST_SHRD, &dst, &src1, &src2);
  }
  //! @brief Double Precision Shift Right.
  inline void shrd(const Mem& dst, const Register& src1, const Immediate& src2)
  {
    __emitX86(INST_SHRD, &dst, &src1, &src2);
  }

  //! @brief Set Carry Flag to 1.
  inline void stc()
  {
    __emitX86(INST_STC);
  }

  //! @brief Set Direction Flag to 1.
  inline void std()
  {
    __emitX86(INST_STD);
  }

  //! @brief Subtract.
  inline void sub(const Register& dst, const Register& src)
  {
    __emitX86(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const Register& dst, const Mem& src)
  {
    __emitX86(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const Mem& dst, const Register& src)
  {
    __emitX86(INST_SUB, &dst, &src);
  }
  //! @brief Subtract.
  inline void sub(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_SUB, &dst, &src);
  }

  //! @brief Logical Compare.
  inline void test(const Register& op1, const Register& op2)
  {
    __emitX86(INST_TEST, &op1, &op2);
  }
  //! @brief Logical Compare.
  inline void test(const Register& op1, const Immediate& op2)
  {
    __emitX86(INST_TEST, &op1, &op2);
  }
  //! @brief Logical Compare.
  inline void test(const Mem& op1, const Register& op2)
  {
    __emitX86(INST_TEST, &op1, &op2);
  }
  //! @brief Logical Compare.
  inline void test(const Mem& op1, const Immediate& op2)
  {
    __emitX86(INST_TEST, &op1, &op2);
  }

  //! @brief Undefined instruction - Raise invalid opcode exception.
  inline void ud2()
  {
    __emitX86(INST_UD2);
  }

  //! @brief Exchange and Add.
  inline void xadd(const Register& dst, const Register& src)
  {
    __emitX86(INST_XADD, &dst, &src);
  }
  //! @brief Exchange and Add.
  inline void xadd(const Mem& dst, const Register& src)
  {
    __emitX86(INST_XADD, &dst, &src);
  }

  //! @brief Exchange Register/Memory with Register.
  inline void xchg(const Register& dst, const Register& src)
  {
    __emitX86(INST_XCHG, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xchg(const Mem& dst, const Register& src)
  {
    __emitX86(INST_XCHG, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xchg(const Register& dst, const Mem& src)
  {
    __emitX86(INST_XCHG, &src, &dst);
  }

  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Register& dst, const Register& src)
  {
    __emitX86(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Register& dst, const Mem& src)
  {
    __emitX86(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Register& dst, const Immediate& src)
  {
    __emitX86(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Mem& dst, const Register& src)
  {
    __emitX86(INST_XOR, &dst, &src);
  }
  //! @brief Exchange Register/Memory with Register.
  inline void xor_(const Mem& dst, const Immediate& src)
  {
    __emitX86(INST_XOR, &dst, &src);
  }

  // -------------------------------------------------------------------------
  // [X87 Instructions (FPU)]
  // -------------------------------------------------------------------------

  //! @brief Compute 2^x - 1 (FPU).
  inline void f2xm1()
  {
    __emitX86(INST_F2XM1);
  }

  //! @brief Absolute Value of st(0) (FPU).
  inline void fabs()
  {
    __emitX86(INST_FABS);
  }

  //! @brief Add @a src to @a dst and store result in @a dst (FPU).
  //!
  //! @note One of dst or src must be st(0).
  inline void fadd(const X87Register& dst, const X87Register& src)
  {
    ASMJIT_ASSERT(dst.index() == 0 || src.index() == 0);
    __emitX86(INST_FADD, &dst, &src);
  }

  //! @brief Add @a src to st(0) and store result in st(0) (FPU).
  //!
  //! @note SP-FP or DP-FP determined by @a adr size.
  inline void fadd(const Mem& src)
  {
    __emitX86(INST_FADD, &src);
  }

  //! @brief Add st(0) to @a dst and POP register stack (FPU).
  inline void faddp(const X87Register& dst = st(1))
  {
    __emitX86(INST_FADDP, &dst);
  }

  //! @brief Load Binary Coded Decimal (FPU).
  inline void fbld(const Mem& src)
  {
    __emitX86(INST_FBLD, &src);
  }

  //! @brief Store BCD Integer and Pop (FPU).
  inline void fbstp(const Mem& dst)
  {
    __emitX86(INST_FBSTP, &dst);
  }

  //! @brief Change st(0) Sign (FPU).
  inline void fchs()
  {
    __emitX86(INST_FCHS);
  }

  //! @brief Clear Exceptions (FPU).
  //!
  //! Clear floating-point exception flags after checking for pending unmasked 
  //! floatingpoint exceptions.
  //!
  //! Clears the floating-point exception flags (PE, UE, OE, ZE, DE, and IE), 
  //! the exception summary status flag (ES), the stack fault flag (SF), and
  //! the busy flag (B) in the FPU status word. The FCLEX instruction checks 
  //! for and handles any pending unmasked floating-point exceptions before 
  //! clearing the exception flags.
  inline void fclex()
  {
    __emitX86(INST_FCLEX);
  }

  //! @brief FP Conditional Move (FPU).
  inline void fcmovb(const X87Register& src)
  {
    __emitX86(INST_FCMOVB, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmovbe(const X87Register& src)
  {
    __emitX86(INST_FCMOVBE, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmove(const X87Register& src)
  {
    __emitX86(INST_FCMOVE, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmovnb(const X87Register& src)
  {
    __emitX86(INST_FCMOVNB, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmovnbe(const X87Register& src)
  {
    __emitX86(INST_FCMOVNBE, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmovne(const X87Register& src)
  {
    __emitX86(INST_FCMOVNE, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmovnu(const X87Register& src)
  {
    __emitX86(INST_FCMOVNU, &src);
  }
  //! @brief FP Conditional Move (FPU).
  inline void fcmovu(const X87Register& src)
  {
    __emitX86(INST_FCMOVU, &src);
  }

  //! @brief Compare st(0) with @a reg (FPU).
  inline void fcom(const X87Register& reg = st(1))
  {
    __emitX86(INST_FCOM, &reg);
  }
  //! @brief Compare st(0) with 4 byte or 8 byte FP at @a src (FPU).
  inline void fcom(const Mem& src)
  {
    __emitX86(INST_FCOM, &src);
  }

  //! @brief Compare st(0) with @a reg and pop the stack (FPU).
  inline void fcomp(const X87Register& reg = st(1))
  {
    __emitX86(INST_FCOMP, &reg);
  }
  //! @brief Compare st(0) with 4 byte or 8 byte FP at @a adr and pop the 
  //! stack (FPU).
  inline void fcomp(const Mem& mem)
  {
    __emitX86(INST_FCOMP, &mem);
  }

  //! @brief Compare st(0) with st(1) and pop register stack twice (FPU).
  inline void fcompp()
  {
    __emitX86(INST_FCOMPP);
  }

  //! @brief Compare st(0) and @a reg and Set EFLAGS (FPU).
  inline void fcomi(const X87Register& reg)
  {
    __emitX86(INST_FCOMI, &reg);
  }

  //! @brief Compare st(0) and @a reg and Set EFLAGS and pop the stack (FPU).
  inline void fcomip(const X87Register& reg)
  {
    __emitX86(INST_FCOMIP, &reg);
  }

  //! @brief Cosine (FPU).
  //!
  //! This instruction calculates the cosine of the source operand in 
  //! register st(0) and stores the result in st(0).
  inline void fcos()
  {
    __emitX86(INST_FCOS);
  }

  //! @brief Decrement Stack-Top Pointer (FPU).
  //!
  //! Subtracts one from the TOP field of the FPU status word (decrements 
  //! the top-ofstack pointer). If the TOP field contains a 0, it is set 
  //! to 7. The effect of this instruction is to rotate the stack by one 
  //! position. The contents of the FPU data registers and tag register 
  //! are not affected.
  inline void fdecstp()
  {
    __emitX86(INST_FDECSTP);
  }

  //! @brief Divide @a dst by @a src (FPU).
  //!
  //! @note One of @a dst or @a src register must be st(0).
  inline void fdiv(const X87Register& dst, const X87Register& src)
  {
    ASMJIT_ASSERT(dst.index() == 0 || src.index() == 0);
    __emitX86(INST_FDIV, &dst, &src);
  }
  //! @brief Divide st(0) by 32 bit or 64 bit FP value (FPU).
  inline void fdiv(const Mem& src)
  {
    __emitX86(INST_FDIV, &src);
  }

  //! @brief Divide @a reg by st(0) (FPU).
  inline void fdivp(const X87Register& reg = st(1))
  {
    __emitX86(INST_FDIVP, &reg);
  }

  //! @brief Reverse Divide @a dst by @a src (FPU).
  //!
  //! @note One of @a dst or @a src register must be st(0).
  inline void fdivr(const X87Register& dst, const X87Register& src)
  {
    ASMJIT_ASSERT(dst.index() == 0 || src.index() == 0);
    __emitX86(INST_FDIVR, &dst, &src);
  }
  //! @brief Reverse Divide st(0) by 32 bit or 64 bit FP value (FPU).
  inline void fdivr(const Mem& src)
  {
    __emitX86(INST_FDIVR, &src);
  }

  //! @brief Reverse Divide @a reg by st(0) (FPU).
  inline void fdivrp(const X87Register& reg = st(1))
  {
    __emitX86(INST_FDIVRP, &reg);
  }

  //! @brief Free Floating-Point Register (FPU).
  //!
  //! Sets the tag in the FPU tag register associated with register @a reg
  //! to empty (11B). The contents of @a reg and the FPU stack-top pointer 
  //! (TOP) are not affected.
  inline void ffree(const X87Register& reg)
  {
    __emitX86(INST_FFREE, &reg);
  }

  //! @brief Add 16 bit or 32 bit integer to st(0) (FPU).
  inline void fiadd(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FIADD, &src);
  }

  //! @brief Compare st(0) with 16 bit or 32 bit Integer (FPU).
  inline void ficom(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FICOM, &src);
  }

  //! @brief Compare st(0) with 16 bit or 32 bit Integer and pop the stack (FPU).
  inline void ficomp(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FICOMP, &src);
  }

  //! @brief Divide st(0) by 32 bit or 16 bit integer (@a src) (FPU).
  inline void fidiv(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FIDIV, &src);
  }

  //! @brief Reverse Divide st(0) by 32 bit or 16 bit integer (@a src) (FPU).
  inline void fidivr(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FIDIVR, &src);
  }

  //! @brief Load 16 bit, 32 bit or 64 bit Integer and push it to the stack (FPU).
  //!
  //! Converts the signed-integer source operand into double extended-precision 
  //! floating point format and pushes the value onto the FPU register stack. 
  //! The source operand can be a word, doubleword, or quadword integer. It is 
  //! loaded without rounding errors. The sign of the source operand is 
  //! preserved.
  inline void fild(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4 || src.size() == 8);
    __emitX86(INST_FILD, &src);
  }

  //! @brief Multiply st(0) by 16 bit or 32 bit integer and store it 
  //! to st(0) (FPU).
  inline void fimul(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FIMUL, &src);
  }

  //! @brief Increment Stack-Top Pointer (FPU).
  //!
  //! Adds one to the TOP field of the FPU status word (increments the 
  //! top-of-stack pointer). If the TOP field contains a 7, it is set to 0. 
  //! The effect of this instruction is to rotate the stack by one position. 
  //! The contents of the FPU data registers and tag register are not affected.
  //! This operation is not equivalent to popping the stack, because the tag 
  //! for the previous top-of-stack register is not marked empty.
  inline void fincstp()
  {
    __emitX86(INST_FINCSTP);
  }

  //! @brief Initialize Floating-Point Unit (FPU).
  //!
  //! Initialize FPU after checking for pending unmasked floating-point 
  //! exceptions.
  inline void finit()
  {
    __emitX86(INST_FINIT);
  }

  //! @brief Subtract 16 bit or 32 bit integer from st(0) and store result to 
  //! st(0) (FPU).
  inline void fisub(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FISUB, &src);
  }

  //! @brief Reverse Subtract 16 bit or 32 bit integer from st(0) and 
  //! store result to  st(0) (FPU).
  inline void fisubr(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 2 || src.size() == 4);
    __emitX86(INST_FISUBR, &src);
  }

  //! @brief Initialize Floating-Point Unit (FPU).
  //!
  //! Initialize FPU without checking for pending unmasked floating-point
  //! exceptions.
  inline void fninit()
  {
    __emitX86(INST_FNINIT);
  }

  //! @brief Store st(0) as 16 bit or 32 bit Integer to @a dst (FPU).
  inline void fist(const Mem& dst)
  {
    ASMJIT_ASSERT(dst.size() == 2 || dst.size() == 4);
    __emitX86(INST_FIST, &dst);
  }

  //! @brief Store st(0) as 16 bit, 32 bit or 64 bit Integer to @a dst and pop
  //! stack (FPU).
  inline void fistp(const Mem& dst)
  {
    ASMJIT_ASSERT(dst.size() == 2 || dst.size() == 4 || dst.size() == 8);
    __emitX86(INST_FISTP, &dst);
  }

  //! @brief Push 32 bit, 64 bit or 80 bit Floating Point Value onto the FPU 
  //! register stack (FPU).
  inline void fld(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 4 || src.size() == 8 || src.size() == 10);
    __emitX86(INST_FLD, &src);
  }

  //! @brief Push @a reg onto the FPU register stack (FPU).
  inline void fld(const X87Register& reg)
  {
    __emitX86(INST_FLD, &reg);
  }

  //! @brief Push +1.0 onto the FPU register stack (FPU).
  inline void fld1()
  {
    __emitX86(INST_FLD1);
  }

  //! @brief Push log2(10) onto the FPU register stack (FPU).
  inline void fldl2t()
  {
    __emitX86(INST_FLDL2T);
  }

  //! @brief Push log2(e) onto the FPU register stack (FPU).
  inline void fldl2e()
  {
    __emitX86(INST_FLDL2E);
  }

  //! @brief Push pi onto the FPU register stack (FPU).
  inline void fldpi()
  {
    __emitX86(INST_FLDPI);
  }

  //! @brief Push log10(2) onto the FPU register stack (FPU).
  inline void fldlg2()
  {
    __emitX86(INST_FLDLG2);
  }

  //! @brief Push ln(2) onto the FPU register stack (FPU).
  inline void fldln2()
  {
    __emitX86(INST_FLDLN2);
  }

  //! @brief Push +0.0 onto the FPU register stack (FPU).
  inline void fldz()
  {
    __emitX86(INST_FLDZ);
  }
  
  //! @brief Load x87 FPU Control Word (2 bytes) (FPU).
  inline void fldcw(const Mem& src)
  {
    __emitX86(INST_FLDCW, &src);
  }

  //! @brief Load x87 FPU Environment (14 or 28 bytes) (FPU).
  inline void fldenv(const Mem& src)
  {
    __emitX86(INST_FLDENV, &src);
  }

  //! @brief Multiply @a dst by @a src and store result in @a dst (FPU).
  //!
  //! @note One of dst or src must be st(0).
  inline void fmul(const X87Register& dst, const X87Register& src)
  {
    ASMJIT_ASSERT(dst.index() == 0 || src.index() == 0);
    __emitX86(INST_FMUL, &dst, &src);
  }
  //! @brief Multiply st(0) by @a src and store result in st(0) (FPU).
  //!
  //! @note SP-FP or DP-FP determined by @a adr size.
  inline void fmul(const Mem& src)
  {
    __emitX86(INST_FMUL, &src);
  }

  //! @brief Multiply st(0) by @a dst and POP register stack (FPU).
  inline void fmulp(const X87Register& dst = st(1))
  {
    __emitX86(INST_FMULP, &dst);
  }

  //! @brief Clear Exceptions (FPU).
  //!
  //! Clear floating-point exception flags without checking for pending 
  //! unmasked floating-point exceptions.
  //!
  //! Clears the floating-point exception flags (PE, UE, OE, ZE, DE, and IE), 
  //! the exception summary status flag (ES), the stack fault flag (SF), and
  //! the busy flag (B) in the FPU status word. The FCLEX instruction does
  //! not checks for and handles any pending unmasked floating-point exceptions
  //! before clearing the exception flags.
  inline void fnclex()
  {
    __emitX86(INST_FNCLEX);
  }

  //! @brief No Operation (FPU).
  inline void fnop()
  {
    __emitX86(INST_FNOP);
  }

  //! @brief Save FPU State (FPU).
  //!
  //! Store FPU environment to m94byte or m108byte without
  //! checking for pending unmasked FP exceptions.
  //! Then re-initialize the FPU.
  inline void fnsave(const Mem& dst)
  {
    __emitX86(INST_FNSAVE, &dst);
  }

  //! @brief Store x87 FPU Environment (FPU).
  //!
  //! Store FPU environment to @a dst (14 or 28 Bytes) without checking for
  //! pending unmasked floating-point exceptions. Then mask all floating
  //! point exceptions.
  inline void fnstenv(const Mem& dst)
  {
    __emitX86(INST_FNSTENV, &dst);
  }

  //! @brief Store x87 FPU Control Word (FPU).
  //!
  //! Store FPU control word to @a dst (2 Bytes) without checking for pending 
  //! unmasked floating-point exceptions.
  inline void fnstcw(const Mem& dst)
  {
    __emitX86(INST_FNSTCW, &dst);
  }

  //! @brief Store x87 FPU Status Word (2 Bytes) (FPU).
  inline void fnstsw(const Register& dst)
  {
    ASMJIT_ASSERT(dst.isRegCode(REG_AX));
    __emitX86(INST_FNSTSW, &dst);
  }
  //! @brief Store x87 FPU Status Word (2 Bytes) (FPU).
  inline void fnstsw(const Mem& dst)
  {
    __emitX86(INST_FNSTSW, &dst);
  }

  //! @brief Partial Arctangent (FPU).
  //!
  //! Replace st(1) with arctan(st(1)/st(0)) and pop the register stack.
  inline void fpatan()
  {
    __emitX86(INST_FPATAN);
  }

  //! @brief Partial Remainder (FPU).
  //!
  //! Replace st(0) with the remainder obtained from dividing st(0) by st(1).
  inline void fprem()
  {
    __emitX86(INST_FPREM);
  }

  //! @brief Partial Remainder (FPU).
  //!
  //! Replace st(0) with the IEEE remainder obtained from dividing st(0) by 
  //! st(1).
  inline void fprem1()
  {
    __emitX86(INST_FPREM1);
  }

  //! @brief Partial Tangent (FPU).
  //! 
  //! Replace st(0) with its tangent and push 1 onto the FPU stack.
  inline void fptan()
  {
    __emitX86(INST_FPTAN);
  }

  //! @brief Round to Integer (FPU).
  //!
  //! Rount st(0) to an Integer.
  inline void frndint()
  {
    __emitX86(INST_FRNDINT);
  }

  //! @brief Restore FPU State (FPU).
  //!
  //! Load FPU state from src (94 bytes or 108 bytes).
  inline void frstor(const Mem& src)
  {
    __emitX86(INST_FRSTOR, &src);
  }

  //! @brief Save FPU State (FPU).
  //!
  //! Store FPU state to 94 or 108 bytes after checking for
  //! pending unmasked FP exceptions. Then reinitialize
  //! the FPU.
  inline void fsave(const Mem& dst)
  {
    __emitX86(INST_FSAVE, &dst);
  }

  //! @brief Scale (FPU).
  //!
  //! Scale st(0) by st(1).
  inline void fscale()
  {
    __emitX86(INST_FSCALE);
  }

  //! @brief Sine (FPU).
  //!
  //! This instruction calculates the sine of the source operand in 
  //! register st(0) and stores the result in st(0).
  inline void fsin()
  {
    __emitX86(INST_FSIN);
  }

  //! @brief Sine and Cosine (FPU).
  //!
  //! Compute the sine and cosine of st(0); replace st(0) with
  //! the sine, and push the cosine onto the register stack.
  inline void fsincos()
  {
    __emitX86(INST_FSINCOS);
  }

  //! @brief Square Root (FPU).
  //!
  //! Calculates square root of st(0) and stores the result in st(0).
  inline void fsqrt()
  {
    __emitX86(INST_FSQRT);
  }

  //! @brief Store Floating Point Value (FPU).
  //!
  //! Store st(0) as 32 bit or 64 bit floating point value to @a dst.
  inline void fst(const Mem& dst)
  {
    ASMJIT_ASSERT(dst.size() == 4 || dst.size() == 8);
    __emitX86(INST_FST, &dst);
  }

  //! @brief Store Floating Point Value (FPU).
  //!
  //! Store st(0) to !a reg.
  inline void fst(const X87Register& reg)
  {
    __emitX86(INST_FST, &reg);
  }

  //! @brief Store Floating Point Value and Pop Register Stack (FPU).
  //!
  //! Store st(0) as 32 bit or 64 bit floating point value to @a dst
  //! and pop register stack.
  inline void fstp(const Mem& dst)
  {
    ASMJIT_ASSERT(dst.size() == 4 || dst.size() == 8 || dst.size() == 10);
    __emitX86(INST_FSTP, &dst);
  }

  //! @brief Store Floating Point Value and Pop Register Stack  (FPU).
  //!
  //! Store st(0) to !a reg and pop register stack.
  inline void fstp(const X87Register& reg)
  {
    __emitX86(INST_FSTP, &reg);
  }

  //! @brief Store x87 FPU Control Word (FPU).
  //!
  //! Store FPU control word to @a dst (2 Bytes) after checking for pending 
  //! unmasked floating-point exceptions.
  inline void fstcw(const Mem& dst)
  {
    __emitX86(INST_FSTCW, &dst);
  }

  //! @brief Store x87 FPU Environment (FPU).
  //!
  //! Store FPU environment to @a dst (14 or 28 Bytes) after checking for
  //! pending unmasked floating-point exceptions. Then mask all floating
  //! point exceptions.
  inline void fstenv(const Mem& dst)
  {
    __emitX86(INST_FSTENV, &dst);
  }

  //! @brief Store x87 FPU Status Word (2 Bytes) (FPU).
  inline void fstsw(const Register& dst)
  {
    ASMJIT_ASSERT(dst.isRegCode(REG_AX));
    __emitX86(INST_FSTSW, &dst);
  }
  //! @brief Store x87 FPU Status Word (2 Bytes) (FPU).
  inline void fstsw(const Mem& dst)
  {
    __emitX86(INST_FSTSW, &dst);
  }

  //! @brief Subtract @a src from @a dst and store result in @a dst (FPU).
  //!
  //! @note One of dst or src must be st(0).
  inline void fsub(const X87Register& dst, const X87Register& src)
  {
    ASMJIT_ASSERT(dst.index() == 0 || src.index() == 0);
    __emitX86(INST_FSUB, &dst, &src);
  }
  //! @brief Subtract @a src from st(0) and store result in st(0) (FPU).
  //!
  //! @note SP-FP or DP-FP determined by @a adr size.
  inline void fsub(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 4 || src.size() == 8);
    __emitX86(INST_FSUB, &src);
  }

  //! @brief Subtract st(0) from @a dst and POP register stack (FPU).
  inline void fsubp(const X87Register& dst = st(1))
  {
    __emitX86(INST_FSUBP, &dst);
  }

  //! @brief Reverse Subtract @a src from @a dst and store result in @a dst (FPU).
  //!
  //! @note One of dst or src must be st(0).
  inline void fsubr(const X87Register& dst, const X87Register& src)
  {
    ASMJIT_ASSERT(dst.index() == 0 || src.index() == 0);
    __emitX86(INST_FSUBR, &dst, &src);
  }

  //! @brief Reverse Subtract @a src from st(0) and store result in st(0) (FPU).
  //!
  //! @note SP-FP or DP-FP determined by @a adr size.
  inline void fsubr(const Mem& src)
  {
    ASMJIT_ASSERT(src.size() == 4 || src.size() == 8);
    __emitX86(INST_FSUBR, &src);
  }

  //! @brief Reverse Subtract st(0) from @a dst and POP register stack (FPU).
  inline void fsubrp(const X87Register& dst = st(1))
  {
    __emitX86(INST_FSUBRP, &dst);
  }

  //! @brief Floating point test - Compare st(0) with 0.0. (FPU).
  inline void ftst()
  {
    __emitX86(INST_FTST);
  }

  //! @brief Unordered Compare st(0) with @a reg (FPU).
  inline void fucom(const X87Register& reg = st(1))
  {
    __emitX86(INST_FUCOM, &reg);
  }

  //! @brief Unordered Compare st(0) and @a reg, check for ordered values
  //! and Set EFLAGS (FPU).
  inline void fucomi(const X87Register& reg)
  {
    __emitX86(INST_FUCOMI, &reg);
  }

  //! @brief UnorderedCompare st(0) and @a reg, Check for ordered values
  //! and Set EFLAGS and pop the stack (FPU).
  inline void fucomip(const X87Register& reg = st(1))
  {
    __emitX86(INST_FUCOMIP, &reg);
  }

  //! @brief Unordered Compare st(0) with @a reg and pop register stack (FPU).
  inline void fucomp(const X87Register& reg = st(1))
  {
    __emitX86(INST_FUCOMP, &reg);
  }

  //! @brief Unordered compare st(0) with st(1) and pop register stack twice 
  //! (FPU).
  inline void fucompp()
  {
    __emitX86(INST_FUCOMPP);
  }

  inline void fwait()
  {
    __emitX86(INST_FWAIT);
  }

  //! @brief Examine st(0) (FPU).
  //!
  //! Examines the contents of the ST(0) register and sets the condition code 
  //! flags C0, C2, and C3 in the FPU status word to indicate the class of 
  //! value or number in the register.
  inline void fxam()
  {
    __emitX86(INST_FXAM);
  }

  //! @brief Exchange Register Contents (FPU).
  //!
  //! Exchange content of st(0) with @a reg.
  inline void fxch(const X87Register& reg = st(1))
  {
    __emitX86(INST_FXCH, &reg);
  }

  //! @brief Restore FP And MMX(tm) State And Streaming SIMD Extension State 
  //! (FPU, MMX, SSE).
  //!
  //! Load FP and MMX(tm) technology and Streaming SIMD Extension state from 
  //! src (512 bytes).
  inline void fxrstor(const Mem& src)
  {
    __emitX86(INST_FXRSTOR, &src);
  }

  //! @brief Store FP and MMX(tm) State and Streaming SIMD Extension State 
  //! (FPU, MMX, SSE).
  //!
  //! Store FP and MMX(tm) technology state and Streaming SIMD Extension state 
  //! to dst (512 bytes).
  inline void fxsave(const Mem& dst)
  {
    __emitX86(INST_FXSAVE, &dst);
  }

  //! @brief Extract Exponent and Significand (FPU).
  //!
  //! Separate value in st(0) into exponent and significand, store exponent 
  //! in st(0), and push the significand onto the register stack.
  inline void fxtract()
  {
    __emitX86(INST_FXTRACT);
  }

  //! @brief Compute y * log2(x).
  //!
  //! Replace st(1) with (st(1) * log2st(0)) and pop the register stack.
  inline void fyl2x()
  {
    __emitX86(INST_FYL2X);
  }

  //! @brief Compute y * log_2(x+1).
  //!
  //! Replace st(1) with (st(1) * (log2st(0) + 1.0)) and pop the register stack.
  inline void fyl2xp1()
  {
    __emitX86(INST_FYL2XP1);
  }

  // -------------------------------------------------------------------------
  // [MMX]
  // -------------------------------------------------------------------------

  //! @brief Empty MMX state.
  inline void emms()
  {
    __emitX86(INST_EMMS);
  }

  //! @brief Move DWord (MMX).
  inline void movd(const Mem& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord (MMX).
  inline void movd(const Register& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord (MMX).
  inline void movd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord (MMX).
  inline void movd(const MMRegister& dst, const Register& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }

  //! @brief Move QWord (MMX).
  inline void movq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
  //! @brief Move QWord (MMX).
  inline void movq(const Mem& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
#if defined(ASMJIT_X64)
  //! @brief Move QWord (MMX).
  inline void movq(const Register& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
#endif
  //! @brief Move QWord (MMX).
  inline void movq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
#if defined(ASMJIT_X64)
  //! @brief Move QWord (MMX).
  inline void movq(const MMRegister& dst, const Register& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
#endif

  //! @brief Pack with Unsigned Saturation (MMX).
  inline void packuswb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PACKUSWB, &dst, &src);
  }
  //! @brief Pack with Unsigned Saturation (MMX).
  inline void packuswb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PACKUSWB, &dst, &src);
  }

  //! @brief Packed BYTE Add (MMX).
  inline void paddb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDB, &dst, &src);
  }
  //! @brief Packed BYTE Add (MMX).
  inline void paddb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDB, &dst, &src);
  }

  //! @brief Packed WORD Add (MMX).
  inline void paddw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDW, &dst, &src);
  }
  //! @brief Packed WORD Add (MMX).
  inline void paddw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDW, &dst, &src);
  }

  //! @brief Packed DWORD Add (MMX).
  inline void paddd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDD, &dst, &src);
  }
  //! @brief Packed DWORD Add (MMX).
  inline void paddd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDD, &dst, &src);
  }

  //! @brief Packed Add with Saturation (MMX).
  inline void paddsb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDSB, &dst, &src);
  }
  //! @brief Packed Add with Saturation (MMX).
  inline void paddsb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDSB, &dst, &src);
  }

  //! @brief Packed Add with Saturation (MMX).
  inline void paddsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDSW, &dst, &src);
  }
  //! @brief Packed Add with Saturation (MMX).
  inline void paddsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDSW, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDUSB, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDUSB, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDUSW, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (MMX).
  inline void paddusw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDUSW, &dst, &src);
  }

  //! @brief Logical AND (MMX).
  inline void pand(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PAND, &dst, &src);
  }
  //! @brief Logical AND (MMX).
  inline void pand(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PAND, &dst, &src);
  }

  //! @brief Logical AND Not (MMX).
  inline void pandn(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PANDN, &dst, &src);
  }
  //! @brief Logical AND Not (MMX).
  inline void pandn(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PANDN, &dst, &src);
  }

  //! @brief Packed Compare for Equal (BYTES) (MMX).
  inline void pcmpeqb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PCMPEQB, &dst, &src);
  }
  //! @brief Packed Compare for Equal (BYTES) (MMX).
  inline void pcmpeqb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQB, &dst, &src);
  }

  //! @brief Packed Compare for Equal (WORDS) (MMX).
  inline void pcmpeqw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PCMPEQW, &dst, &src);
  }
  //! @brief Packed Compare for Equal (WORDS) (MMX).
  inline void pcmpeqw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQW, &dst, &src);
  }

  //! @brief Packed Compare for Equal (DWORDS) (MMX).
  inline void pcmpeqd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PCMPEQD, &dst, &src);
  }
  //! @brief Packed Compare for Equal (DWORDS) (MMX).
  inline void pcmpeqd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQD, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (BYTES) (MMX).
  inline void pcmpgtb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PCMPGTB, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (BYTES) (MMX).
  inline void pcmpgtb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTB, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (WORDS) (MMX).
  inline void pcmpgtw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PCMPGTW, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (WORDS) (MMX).
  inline void pcmpgtw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTW, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (DWORDS) (MMX).
  inline void pcmpgtd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PCMPGTD, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (DWORDS) (MMX).
  inline void pcmpgtd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTD, &dst, &src);
  }

  //! @brief Packed Multiply High (MMX).
  inline void pmulhw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMULHW, &dst, &src);
  }
  //! @brief Packed Multiply High (MMX).
  inline void pmulhw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULHW, &dst, &src);
  }

  //! @brief Packed Multiply Low (MMX).
  inline void pmullw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMULLW, &dst, &src);
  }
  //! @brief Packed Multiply Low (MMX).
  inline void pmullw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULLW, &dst, &src);
  }

  //! @brief Bitwise Logical OR (MMX).
  inline void por(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_POR, &dst, &src);
  }
  //! @brief Bitwise Logical OR (MMX).
  inline void por(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_POR, &dst, &src);
  }

  //! @brief Packed Multiply and Add (MMX).
  inline void pmaddwd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMADDWD, &dst, &src);
  }
  //! @brief Packed Multiply and Add (MMX).
  inline void pmaddwd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMADDWD, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  inline void pslld(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void pslld(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void pslld(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLD, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  inline void psllq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllq(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLQ, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (MMX).
  inline void psllw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (MMX).
  inline void psllw(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLW, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psrad(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psrad(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psrad(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRAD, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psraw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psraw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (MMX).
  inline void psraw(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRAW, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (MMX).
  inline void psrld(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrld(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrld(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLD, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlq(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLQ, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (MMX).
  inline void psrlw(const MMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLW, &dst, &src);
  }

  //! @brief Packed Subtract (MMX).
  inline void psubb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBB, &dst, &src);
  }
  //! @brief Packed Subtract (MMX).
  inline void psubb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBB, &dst, &src);
  }

  //! @brief Packed Subtract (MMX).
  inline void psubw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBW, &dst, &src);
  }
  //! @brief Packed Subtract (MMX).
  inline void psubw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBW, &dst, &src);
  }

  //! @brief Packed Subtract (MMX).
  inline void psubd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBD, &dst, &src);
  }
  //! @brief Packed Subtract (MMX).
  inline void psubd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBD, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBSB, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBSB, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBSW, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (MMX).
  inline void psubsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBSW, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBUSB, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBUSB, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBUSW, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (MMX).
  inline void psubusw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBUSW, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhbw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PUNPCKHBW, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhbw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHBW, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhwd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PUNPCKHWD, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhwd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHWD, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhdq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PUNPCKHDQ, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckhdq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHDQ, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklbw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PUNPCKLBW, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklbw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLBW, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklwd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PUNPCKLWD, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpcklwd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLWD, &dst, &src);
  }

  //! @brief Unpack High Packed Data (MMX).
  inline void punpckldq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PUNPCKLDQ, &dst, &src);
  }
  //! @brief Unpack High Packed Data (MMX).
  inline void punpckldq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLDQ, &dst, &src);
  }

  //! @brief Bitwise Exclusive OR (MMX).
  inline void pxor(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PXOR, &dst, &src);
  }
  //! @brief Bitwise Exclusive OR (MMX).
  inline void pxor(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PXOR, &dst, &src);
  }

  // -------------------------------------------------------------------------
  // [3dNow]
  // -------------------------------------------------------------------------

  //! @brief Faster EMMS (3dNow!).
  //!
  //! @note Use only for early AMD processors where is only 3dNow! or SSE. If
  //! CPU contains SSE2, it's better to use @c emms() ( @c femms() is mapped 
  //! to @c emms() ).
  inline void femms()
  {
    __emitX86(INST_FEMMS);
  }

  //! @brief Packed SP-FP to Integer Convert (3dNow!).
  inline void pf2id(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PF2ID, &dst, &src);
  }
  //! @brief Packed SP-FP to Integer Convert (3dNow!).
  inline void pf2id(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PF2ID, &dst, &src);
  }

  //! @brief  Packed SP-FP to Integer Word Convert (3dNow!).
  inline void pf2iw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PF2IW, &dst, &src);
  }
  //! @brief  Packed SP-FP to Integer Word Convert (3dNow!).
  inline void pf2iw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PF2IW, &dst, &src);
  }

  //! @brief Packed SP-FP Accumulate (3dNow!).
  inline void pfacc(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFACC, &dst, &src);
  }
  //! @brief Packed SP-FP Accumulate (3dNow!).
  inline void pfacc(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFACC, &dst, &src);
  }

  //! @brief Packed SP-FP Addition (3dNow!).
  inline void pfadd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFADD, &dst, &src);
  }
  //! @brief Packed SP-FP Addition (3dNow!).
  inline void pfadd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFADD, &dst, &src);
  }

  //! @brief Packed SP-FP Compare - dst == src (3dNow!).
  inline void pfcmpeq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFCMPEQ, &dst, &src);
  }
  //! @brief Packed SP-FP Compare - dst == src (3dNow!).
  inline void pfcmpeq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFCMPEQ, &dst, &src);
  }

  //! @brief Packed SP-FP Compare - dst >= src (3dNow!).
  inline void pfcmpge(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFCMPGE, &dst, &src);
  }
  //! @brief Packed SP-FP Compare - dst >= src (3dNow!).
  inline void pfcmpge(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFCMPGE, &dst, &src);
  }

  //! @brief Packed SP-FP Compare - dst > src (3dNow!).
  inline void pfcmpgt(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFCMPGT, &dst, &src);
  }
  //! @brief Packed SP-FP Compare - dst > src (3dNow!).
  inline void pfcmpgt(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFCMPGT, &dst, &src);
  }

  //! @brief Packed SP-FP Maximum (3dNow!).
  inline void pfmax(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFMAX, &dst, &src);
  }
  //! @brief Packed SP-FP Maximum (3dNow!).
  inline void pfmax(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFMAX, &dst, &src);
  }

  //! @brief Packed SP-FP Minimum (3dNow!).
  inline void pfmin(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFMIN, &dst, &src);
  }
  //! @brief Packed SP-FP Minimum (3dNow!).
  inline void pfmin(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFMIN, &dst, &src);
  }

  //! @brief Packed SP-FP Multiply (3dNow!).
  inline void pfmul(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFMUL, &dst, &src);
  }
  //! @brief Packed SP-FP Multiply (3dNow!).
  inline void pfmul(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFMUL, &dst, &src);
  }

  //! @brief Packed SP-FP Negative Accumulate (3dNow!).
  inline void pfnacc(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFNACC, &dst, &src);
  }
  //! @brief Packed SP-FP Negative Accumulate (3dNow!).
  inline void pfnacc(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFNACC, &dst, &src);
  }

  //! @brief Packed SP-FP Mixed Accumulate (3dNow!).
  inline void pfpnaxx(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFPNACC, &dst, &src);
  }
  //! @brief Packed SP-FP Mixed Accumulate (3dNow!).
  inline void pfpnacc(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFPNACC, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal Approximation (3dNow!).
  inline void pfrcp(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFRCP, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal Approximation (3dNow!).
  inline void pfrcp(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFRCP, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal, First Iteration Step (3dNow!).
  inline void pfrcpit1(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFRCPIT1, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal, First Iteration Step (3dNow!).
  inline void pfrcpit1(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFRCPIT1, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal, Second Iteration Step (3dNow!).
  inline void pfrcpit2(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFRCPIT2, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal, Second Iteration Step (3dNow!).
  inline void pfrcpit2(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFRCPIT2, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal Square Root, First Iteration Step (3dNow!).
  inline void pfrsqit1(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFRSQIT1, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal Square Root, First Iteration Step (3dNow!).
  inline void pfrsqit1(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFRSQIT1, &dst, &src);
  }

  //! @brief Packed SP-FP Reciprocal Square Root Approximation (3dNow!).
  inline void pfrsqrt(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFRSQRT, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal Square Root Approximation (3dNow!).
  inline void pfrsqrt(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFRSQRT, &dst, &src);
  }

  //! @brief Packed SP-FP Subtract (3dNow!).
  inline void pfsub(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFSUB, &dst, &src);
  }
  //! @brief Packed SP-FP Subtract (3dNow!).
  inline void pfsub(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFSUB, &dst, &src);
  }

  //! @brief Packed SP-FP Reverse Subtract (3dNow!).
  inline void pfsubr(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PFSUBR, &dst, &src);
  }
  //! @brief Packed SP-FP Reverse Subtract (3dNow!).
  inline void pfsubr(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PFSUBR, &dst, &src);
  }

  //! @brief Packed DWords to SP-FP (3dNow!).
  inline void pi2fd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PI2FD, &dst, &src);
  }
  //! @brief Packed DWords to SP-FP (3dNow!).
  inline void pi2fd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PI2FD, &dst, &src);
  }

  //! @brief Packed Words to SP-FP (3dNow!).
  inline void pi2fw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PI2FW, &dst, &src);
  }
  //! @brief Packed Words to SP-FP (3dNow!).
  inline void pi2fw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PI2FW, &dst, &src);
  }

  //! @brief Packed swap DWord (3dNow!)
  inline void pswapd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSWAPD, &dst, &src);
  }
  //! @brief Packed swap DWord (3dNow!)
  inline void pswapd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSWAPD, &dst, &src);
  }

  // -------------------------------------------------------------------------
  // [SSE]
  // -------------------------------------------------------------------------

  //! @brief Packed SP-FP Add (SSE).
  inline void addps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ADDPS, &dst, &src);
  }
  //! @brief Packed SP-FP Add (SSE).
  inline void addps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ADDPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Add (SSE).
  inline void addss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ADDSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Add (SSE).
  inline void addss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ADDSS, &dst, &src);
  }

  //! @brief Bit-wise Logical And Not For SP-FP (SSE).
  inline void andnps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ANDNPS, &dst, &src);
  }
  //! @brief Bit-wise Logical And Not For SP-FP (SSE).
  inline void andnps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ANDNPS, &dst, &src);
  }

  //! @brief Bit-wise Logical And For SP-FP (SSE).
  inline void andps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ANDPS, &dst, &src);
  }
  //! @brief Bit-wise Logical And For SP-FP (SSE).
  inline void andps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ANDPS, &dst, &src);
  }

  //! @brief Packed SP-FP Compare (SSE).
  inline void cmpps(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPPS, &dst, &src, &imm8);
  }
  //! @brief Packed SP-FP Compare (SSE).
  inline void cmpps(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPPS, &dst, &src, &imm8);
  }

  //! @brief Compare Scalar SP-FP Values (SSE).
  inline void cmpss(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPSS, &dst, &src, &imm8);
  }
  //! @brief Compare Scalar SP-FP Values (SSE).
  inline void cmpss(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPSS, &dst, &src, &imm8);
  }

  //! @brief Scalar Ordered SP-FP Compare and Set EFLAGS (SSE).
  inline void comiss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_COMISS, &dst, &src);
  }
  //! @brief Scalar Ordered SP-FP Compare and Set EFLAGS (SSE).
  inline void comiss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_COMISS, &dst, &src);
  }

  //! @brief Packed Signed INT32 to Packed SP-FP Conversion (SSE).
  inline void cvtpi2ps(const XMMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_CVTPI2PS, &dst, &src);
  }
  //! @brief Packed Signed INT32 to Packed SP-FP Conversion (SSE).
  inline void cvtpi2ps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPI2PS, &dst, &src);
  }

  //! @brief Packed SP-FP to Packed INT32 Conversion (SSE).
  inline void cvtps2pi(const MMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTPS2PI, &dst, &src);
  }
  //! @brief Packed SP-FP to Packed INT32 Conversion (SSE).
  inline void cvtps2pi(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPS2PI, &dst, &src);
  }

  //! @brief Scalar Signed INT32 to SP-FP Conversion (SSE).
  inline void cvtsi2ss(const XMMRegister& dst, const Register& src)
  {
    __emitX86(INST_CVTSI2SS, &dst, &src);
  }
  //! @brief Scalar Signed INT32 to SP-FP Conversion (SSE).
  inline void cvtsi2ss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTSI2SS, &dst, &src);
  }

  //! @brief Scalar SP-FP to Signed INT32 Conversion (SSE).
  inline void cvtss2si(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTSS2SI, &dst, &src);
  }
  //! @brief Scalar SP-FP to Signed INT32 Conversion (SSE).
  inline void cvtss2si(const Register& dst, const Mem& src)
  {
    __emitX86(INST_CVTSS2SI, &dst, &src);
  }

  //! @brief Packed SP-FP to Packed INT32 Conversion (truncate) (SSE).
  inline void cvttps2pi(const MMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTTPS2PI, &dst, &src);
  }
  //! @brief Packed SP-FP to Packed INT32 Conversion (truncate) (SSE).
  inline void cvttps2pi(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTTPS2PI, &dst, &src);
  }

  //! @brief Scalar SP-FP to Signed INT32 Conversion (truncate) (SSE).
  inline void cvttss2si(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTTSS2SI, &dst, &src);
  }
  //! @brief Scalar SP-FP to Signed INT32 Conversion (truncate) (SSE).
  inline void cvttss2si(const Register& dst, const Mem& src)
  {
    __emitX86(INST_CVTTSS2SI, &dst, &src);
  }

  //! @brief Packed SP-FP Divide (SSE).
  inline void divps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_DIVPS, &dst, &src);
  }
  //! @brief Packed SP-FP Divide (SSE).
  inline void divps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_DIVPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Divide (SSE).
  inline void divss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_DIVSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Divide (SSE).
  inline void divss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_DIVSS, &dst, &src);
  }

  //! @brief Load Streaming SIMD Extension Control/Status (SSE).
  inline void ldmxcsr(const Mem& src)
  {
    __emitX86(INST_LDMXCSR, &src);
  }

  //! @brief Byte Mask Write (SSE).
  //!
  //! @note The default memory location is specified by DS:EDI.
  inline void maskmovq(const MMRegister& data, const MMRegister& mask)
  {
    __emitX86(INST_MASKMOVQ, &data, &mask);
  }

  //! @brief Packed SP-FP Maximum (SSE).
  inline void maxps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MAXPS, &dst, &src);
  }
  //! @brief Packed SP-FP Maximum (SSE).
  inline void maxps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MAXPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Maximum (SSE).
  inline void maxss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MAXSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Maximum (SSE).
  inline void maxss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MAXSS, &dst, &src);
  }

  //! @brief Packed SP-FP Minimum (SSE).
  inline void minps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MINPS, &dst, &src);
  }
  //! @brief Packed SP-FP Minimum (SSE).
  inline void minps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MINPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Minimum (SSE).
  inline void minss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MINSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Minimum (SSE).
  inline void minss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MINSS, &dst, &src);
  }

  //! @brief Move Aligned Packed SP-FP Values (SSE).
  inline void movaps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVAPS, &dst, &src);
  }
  //! @brief Move Aligned Packed SP-FP Values (SSE).
  inline void movaps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVAPS, &dst, &src);
  }

  //! @brief Move Aligned Packed SP-FP Values (SSE).
  inline void movaps(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVAPS, &dst, &src);
  }

  //! @brief Move DWord.
  inline void movd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord.
  inline void movd(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord.
  inline void movd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }
  //! @brief Move DWord.
  inline void movd(const XMMRegister& dst, const Register& src)
  {
    __emitX86(INST_MOVD, &dst, &src);
  }

  //! @brief Move QWord (SSE).
  inline void movq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
  //! @brief Move QWord (SSE).
  inline void movq(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }
  //! @brief Move QWord (SSE).
  inline void movq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVQ, &dst, &src);
  }

  //! @brief Move 64 Bits Non Temporal (SSE).
  inline void movntq(const Mem& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVNTQ, &dst, &src);
  }

  //! @brief High to Low Packed SP-FP (SSE).
  inline void movhlps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVHLPS, &dst, &src);
  }

  //! @brief Move High Packed SP-FP (SSE).
  inline void movhps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVHPS, &dst, &src);
  }

  //! @brief Move High Packed SP-FP (SSE).
  inline void movhps(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVHPS, &dst, &src);
  }

  //! @brief Move Low to High Packed SP-FP (SSE).
  inline void movlhps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVLHPS, &dst, &src);
  }

  //! @brief Move Low Packed SP-FP (SSE).
  inline void movlps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVLPS, &dst, &src);
  }

  //! @brief Move Low Packed SP-FP (SSE).
  inline void movlps(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVLPS, &dst, &src);
  }

  //! @brief Move Aligned Four Packed SP-FP Non Temporal (SSE).
  inline void movntps(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVNTPS, &dst, &src);
  }

  //! @brief Move Scalar SP-FP (SSE).
  inline void movss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVSS, &dst, &src);
  }

  //! @brief Move Scalar SP-FP (SSE).
  inline void movss(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVSS, &dst, &src);
  }

  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  inline void movups(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVUPS, &dst, &src);
  }
  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  inline void movups(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVUPS, &dst, &src);
  }

  //! @brief Move Unaligned Packed SP-FP Values (SSE).
  inline void movups(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVUPS, &dst, &src);
  }

  //! @brief Packed SP-FP Multiply (SSE).
  inline void mulps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MULPS, &dst, &src);
  }
  //! @brief Packed SP-FP Multiply (SSE).
  inline void mulps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MULPS, &dst, &src);
  }
   
  //! @brief Scalar SP-FP Multiply (SSE).
  inline void mulss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MULSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Multiply (SSE).
  inline void mulss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MULSS, &dst, &src);
  }

  //! @brief Bit-wise Logical OR for SP-FP Data (SSE).
  inline void orps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ORPS, &dst, &src);
  }
  //! @brief Bit-wise Logical OR for SP-FP Data (SSE).
  inline void orps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ORPS, &dst, &src);
  }

  //! @brief Packed Average (SSE).
  inline void pavgb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PAVGB, &dst, &src);
  }
  //! @brief Packed Average (SSE).
  inline void pavgb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PAVGB, &dst, &src);
  }

  //! @brief Packed Average (SSE).
  inline void pavgw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PAVGW, &dst, &src);
  }
  //! @brief Packed Average (SSE).
  inline void pavgw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PAVGW, &dst, &src);
  }

  //! @brief Extract Word (SSE).
  inline void pextrw(const Register& dst, const MMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRW, &dst, &src, &imm8);
  }

  //! @brief Insert Word (SSE).
  inline void pinsrw(const MMRegister& dst, const MMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRW, &dst, &src, &imm8);
  }
  //! @brief Insert Word (SSE).
  inline void pinsrw(const MMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRW, &dst, &src, &imm8);
  }

  //! @brief Packed Signed Integer Word Maximum (SSE).
  inline void pmaxsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMAXSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Maximum (SSE).
  inline void pmaxsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Maximum (SSE).
  inline void pmaxub(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMAXUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Maximum (SSE).
  inline void pmaxub(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXUB, &dst, &src);
  }

  //! @brief Packed Signed Integer Word Minimum (SSE).
  inline void pminsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMINSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Minimum (SSE).
  inline void pminsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Minimum (SSE).
  inline void pminub(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMINUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Minimum (SSE).
  inline void pminub(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINUB, &dst, &src);
  }

  //! @brief Move Byte Mask To Integer (SSE).
  inline void pmovmskb(const Register& dst, const MMRegister& src)
  {
    __emitX86(INST_PMOVMSKB, &dst, &src);
  }

  //! @brief Packed Multiply High Unsigned (SSE).
  inline void pmulhuw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMULHUW, &dst, &src);
  }
  //! @brief Packed Multiply High Unsigned (SSE).
  inline void pmulhuw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULHUW, &dst, &src);
  }

  //! @brief Packed Sum of Absolute Differences (SSE).
  inline void psadbw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSADBW, &dst, &src);
  }
  //! @brief Packed Sum of Absolute Differences (SSE).
  inline void psadbw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSADBW, &dst, &src);
  }

  //! @brief Packed Shuffle word (SSE).
  inline void pshufw(const MMRegister& dst, const MMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFW, &dst, &src, &imm8);
  }
  //! @brief Packed Shuffle word (SSE).
  inline void pshufw(const MMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFW, &dst, &src, &imm8);
  }

  //! @brief Packed SP-FP Reciprocal (SSE).
  inline void rcpps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_RCPPS, &dst, &src);
  }
  //! @brief Packed SP-FP Reciprocal (SSE).
  inline void rcpps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_RCPPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Reciprocal (SSE).
  inline void rcpss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_RCPSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Reciprocal (SSE).
  inline void rcpss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_RCPSS, &dst, &src);
  }

  //! @brief Prefetch (SSE).
  inline void prefetch(const Mem& mem, const Immediate& hint)
  {
    __emitX86(INST_PREFETCH, &mem, &hint);
  }

  //! @brief Compute Sum of Absolute Differences (SSE).
  inline void psadbw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSADBW, &dst, &src);
  }
  //! @brief Compute Sum of Absolute Differences (SSE).
  inline void psadbw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSADBW, &dst, &src);
  }

  //! @brief Packed SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_RSQRTPS, &dst, &src);
  }
  //! @brief Packed SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_RSQRTPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_RSQRTSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Square Root Reciprocal (SSE).
  inline void rsqrtss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_RSQRTSS, &dst, &src);
  }

  //! @brief Store fence (SSE).
  inline void sfence()
  {
    __emitX86(INST_SFENCE);
  }

  //! @brief Shuffle SP-FP (SSE).
  inline void shufps(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_SHUFPS, &dst, &src, &imm8);
  }
  //! @brief Shuffle SP-FP (SSE).
  inline void shufps(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_SHUFPS, &dst, &src, &imm8);
  }

  //! @brief Packed SP-FP Square Root (SSE).
  inline void sqrtps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SQRTPS, &dst, &src);
  }
  //! @brief Packed SP-FP Square Root (SSE).
  inline void sqrtps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SQRTPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Square Root (SSE).
  inline void sqrtss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SQRTSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Square Root (SSE).
  inline void sqrtss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SQRTSS, &dst, &src);
  }

  //! @brief Store Streaming SIMD Extension Control/Status (SSE).
  inline void stmxcsr(const Mem& dst)
  {
    __emitX86(INST_STMXCSR, &dst);
  }

  //! @brief Packed SP-FP Subtract (SSE).
  inline void subps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SUBPS, &dst, &src);
  }
  //! @brief Packed SP-FP Subtract (SSE).
  inline void subps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SUBPS, &dst, &src);
  }

  //! @brief Scalar SP-FP Subtract (SSE).
  inline void subss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SUBSS, &dst, &src);
  }
  //! @brief Scalar SP-FP Subtract (SSE).
  inline void subss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SUBSS, &dst, &src);
  }

  //! @brief Unordered Scalar SP-FP compare and set EFLAGS (SSE).
  inline void ucomiss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_UCOMISS, &dst, &src);
  }
  //! @brief Unordered Scalar SP-FP compare and set EFLAGS (SSE).
  inline void ucomiss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_UCOMISS, &dst, &src);
  }

  //! @brief Unpack High Packed SP-FP Data (SSE).
  inline void unpckhps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_UNPCKHPS, &dst, &src);
  }
  //! @brief Unpack High Packed SP-FP Data (SSE).
  inline void unpckhps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_UNPCKHPS, &dst, &src);
  }

  //! @brief Unpack Low Packed SP-FP Data (SSE).
  inline void unpcklps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_UNPCKLPS, &dst, &src);
  }
  //! @brief Unpack Low Packed SP-FP Data (SSE).
  inline void unpcklps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_UNPCKLPS, &dst, &src);
  }

  //! @brief Bit-wise Logical Xor for SP-FP Data (SSE).
  inline void xorps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_XORPS, &dst, &src);
  }
  //! @brief Bit-wise Logical Xor for SP-FP Data (SSE).
  inline void xorps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_XORPS, &dst, &src);
  }

  // -------------------------------------------------------------------------
  // [SSE2]
  // -------------------------------------------------------------------------

  //! @brief Packed DP-FP Add (SSE2).
  inline void addpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ADDPD, &dst, &src);
  }
  //! @brief Packed DP-FP Add (SSE2).
  inline void addpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ADDPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Add (SSE2).
  inline void addsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ADDSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Add (SSE2).
  inline void addsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ADDSD, &dst, &src);
  }

  //! @brief Bit-wise Logical And Not For DP-FP (SSE2).
  inline void andnpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ANDNPD, &dst, &src);
  }
  //! @brief Bit-wise Logical And Not For DP-FP (SSE2).
  inline void andnpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ANDNPD, &dst, &src);
  }

  //! @brief Bit-wise Logical And For DP-FP (SSE2).
  inline void andpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ANDPD, &dst, &src);
  }
  //! @brief Bit-wise Logical And For DP-FP (SSE2).
  inline void andpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ANDPD, &dst, &src);
  }

  //! @brief Flush Cache Line (SSE2).
  inline void clflush(const Mem& mem)
  {
    __emitX86(INST_CLFLUSH, &mem);
  }

  //! @brief Packed DP-FP Compare (SSE2).
  inline void cmppd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPPD, &dst, &src, &imm8);
  }
  //! @brief Packed DP-FP Compare (SSE2).
  inline void cmppd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPPD, &dst, &src, &imm8);
  }

  //! @brief Compare Scalar SP-FP Values (SSE2).
  inline void cmpsd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPSD, &dst, &src, &imm8);
  }
  //! @brief Compare Scalar SP-FP Values (SSE2).
  inline void cmpsd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_CMPSD, &dst, &src, &imm8);
  }

  //! @brief Scalar Ordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void comisd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_COMISD, &dst, &src);
  }
  //! @brief Scalar Ordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void comisd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_COMISD, &dst, &src);
  }

  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtdq2pd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTDQ2PD, &dst, &src);
  }
  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtdq2pd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTDQ2PD, &dst, &src);
  }

  //! @brief Convert Packed Dword Integers to Packed SP-FP Values (SSE2).
  inline void cvtdq2ps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTDQ2PS, &dst, &src);
  }
  //! @brief Convert Packed Dword Integers to Packed SP-FP Values (SSE2).
  inline void cvtdq2ps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTDQ2PS, &dst, &src);
  }

  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2dq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTPD2DQ, &dst, &src);
  }
  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2dq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPD2DQ, &dst, &src);
  }

  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2pi(const MMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTPD2PI, &dst, &src);
  }
  //! @brief Convert Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtpd2pi(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPD2PI, &dst, &src);
  }

  //! @brief Convert Packed DP-FP Values to Packed SP-FP Values (SSE2).
  inline void cvtpd2ps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTPD2PS, &dst, &src);
  }
  //! @brief Convert Packed DP-FP Values to Packed SP-FP Values (SSE2).
  inline void cvtpd2ps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPD2PS, &dst, &src);
  }

  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtpi2pd(const XMMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_CVTPI2PD, &dst, &src);
  }
  //! @brief Convert Packed Dword Integers to Packed DP-FP Values (SSE2).
  inline void cvtpi2pd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPI2PD, &dst, &src);
  }

  //! @brief Convert Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtps2dq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTPS2DQ, &dst, &src);
  }
  //! @brief Convert Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvtps2dq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPS2DQ, &dst, &src);
  }

  //! @brief Convert Packed SP-FP Values to Packed DP-FP Values (SSE2).
  inline void cvtps2pd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTPS2PD, &dst, &src);
  }
  //! @brief Convert Packed SP-FP Values to Packed DP-FP Values (SSE2).
  inline void cvtps2pd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTPS2PD, &dst, &src);
  }

  //! @brief Convert Scalar DP-FP Value to Dword Integer (SSE2).
  inline void cvtsd2si(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTSD2SI, &dst, &src);
  }
  //! @brief Convert Scalar DP-FP Value to Dword Integer (SSE2).
  inline void cvtsd2si(const Register& dst, const Mem& src)
  {
    __emitX86(INST_CVTSD2SI, &dst, &src);
  }

  //! @brief Convert Scalar DP-FP Value to Scalar SP-FP Value (SSE2).
  inline void cvtsd2ss(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTSD2SS, &dst, &src);
  }
  //! @brief Convert Scalar DP-FP Value to Scalar SP-FP Value (SSE2).
  inline void cvtsd2ss(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTSD2SS, &dst, &src);
  }

  //! @brief Convert Dword Integer to Scalar DP-FP Value (SSE2).
  inline void cvtsi2sd(const XMMRegister& dst, const Register& src)
  {
    __emitX86(INST_CVTSI2SD, &dst, &src);
  }
  //! @brief Convert Dword Integer to Scalar DP-FP Value (SSE2).
  inline void cvtsi2sd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTSI2SD, &dst, &src);
  }

  //! @brief Convert Scalar SP-FP Value to Scalar DP-FP Value (SSE2).
  inline void cvtss2sd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTSS2SD, &dst, &src);
  }
  //! @brief Convert Scalar SP-FP Value to Scalar DP-FP Value (SSE2).
  inline void cvtss2sd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTSS2SD, &dst, &src);
  }

  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2pi(const MMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTTPD2PI, &dst, &src);
  }
  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2pi(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTTPD2PI, &dst, &src);
  }

  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2dq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTTPD2DQ, &dst, &src);
  }
  //! @brief Convert with Truncation Packed DP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttpd2dq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTTPD2DQ, &dst, &src);
  }

  //! @brief Convert with Truncation Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttps2dq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTTPS2DQ, &dst, &src);
  }
  //! @brief Convert with Truncation Packed SP-FP Values to Packed Dword Integers (SSE2).
  inline void cvttps2dq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_CVTTPS2DQ, &dst, &src);
  }

  //! @brief Convert with Truncation Scalar DP-FP Value to Signed Dword Integer (SSE2).
  inline void cvttsd2si(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_CVTTSD2SI, &dst, &src);
  }
  //! @brief Convert with Truncation Scalar DP-FP Value to Signed Dword Integer (SSE2).
  inline void cvttsd2si(const Register& dst, const Mem& src)
  {
    __emitX86(INST_CVTTSD2SI, &dst, &src);
  }

  //! @brief Packed DP-FP Divide (SSE2).
  inline void divpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_DIVPD, &dst, &src);
  }
  //! @brief Packed DP-FP Divide (SSE2).
  inline void divpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_DIVPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Divide (SSE2).
  inline void divsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_DIVSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Divide (SSE2).
  inline void divsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_DIVSD, &dst, &src);
  }

  //! @brief Load Fence (SSE2).
  inline void lfence()
  {
    __emitX86(INST_LFENCE);
  }

  //! @brief Store Selected Bytes of Double Quadword (SSE2).
  //!
  //! @note Target is DS:EDI.
  inline void maskmovdqu(const XMMRegister& src, const XMMRegister& mask)
  {
    __emitX86(INST_MASKMOVDQU, &src, &mask);
  }

  //! @brief Return Maximum Packed Double-Precision FP Values (SSE2).
  inline void maxpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MAXPD, &dst, &src);
  }
  //! @brief Return Maximum Packed Double-Precision FP Values (SSE2).
  inline void maxpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MAXPD, &dst, &src);
  }

  //! @brief Return Maximum Scalar Double-Precision FP Value (SSE2).
  inline void maxsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MAXSD, &dst, &src);
  }
  //! @brief Return Maximum Scalar Double-Precision FP Value (SSE2).
  inline void maxsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MAXSD, &dst, &src);
  }

  //! @brief Memory Fence (SSE2).
  inline void mfence()
  {
    __emitX86(INST_MFENCE);
  }

  //! @brief Return Minimum Packed DP-FP Values (SSE2).
  inline void minpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MINPD, &dst, &src);
  }
  //! @brief Return Minimum Packed DP-FP Values (SSE2).
  inline void minpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MINPD, &dst, &src);
  }

  //! @brief Return Minimum Scalar DP-FP Value (SSE2).
  inline void minsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MINSD, &dst, &src);
  }
  //! @brief Return Minimum Scalar DP-FP Value (SSE2).
  inline void minsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MINSD, &dst, &src);
  }

  //! @brief Move Aligned DQWord (SSE2).
  inline void movdqa(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVDQA, &dst, &src);
  }
  //! @brief Move Aligned DQWord (SSE2).
  inline void movdqa(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVDQA, &dst, &src);
  }

  //! @brief Move Aligned DQWord (SSE2).
  inline void movdqa(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVDQA, &dst, &src);
  }

  //! @brief Move Unaligned Double Quadword (SSE2).
  inline void movdqu(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVDQU, &dst, &src);
  }
  //! @brief Move Unaligned Double Quadword (SSE2).
  inline void movdqu(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVDQU, &dst, &src);
  }

  //! @brief Move Unaligned Double Quadword (SSE2).
  inline void movdqu(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVDQU, &dst, &src);
  }

  //! @brief Extract Packed SP-FP Sign Mask (SSE2).
  inline void movmskps(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVMSKPS, &dst, &src);
  }

  //! @brief Extract Packed DP-FP Sign Mask (SSE2).
  inline void movmskpd(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVMSKPD, &dst, &src);
  }

  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  inline void movsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVSD, &dst, &src);
  }
  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  inline void movsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVSD, &dst, &src);
  }

  //! @brief Move Scalar Double-Precision FP Value (SSE2).
  inline void movsd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVSD, &dst, &src);
  }

  //! @brief Move Aligned Packed Double-Precision FP Values (SSE2).
  inline void movapd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVAPD, &dst, &src);
  }

  //! @brief Move Aligned Packed Double-Precision FP Values (SSE2).
  inline void movapd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVAPD, &dst, &src);
  }

  //! @brief Move Quadword from XMM to MMX Technology Register (SSE2).
  inline void movdq2q(const MMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVDQ2Q, &dst, &src);
  }

  //! @brief Move Quadword from MMX Technology to XMM Register (SSE2).
  inline void movq2dq(const XMMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_MOVQ2DQ, &dst, &src);
  }

  //! @brief Move High Packed Double-Precision FP Value (SSE2).
  inline void movhpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVHPD, &dst, &src);
  }

  //! @brief Move High Packed Double-Precision FP Value (SSE2).
  inline void movhpd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVHPD, &dst, &src);
  }

  //! @brief Move Low Packed Double-Precision FP Value (SSE2).
  inline void movlpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVLPD, &dst, &src);
  }

  //! @brief Move Low Packed Double-Precision FP Value (SSE2).
  inline void movlpd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVLPD, &dst, &src);
  }

  //! @brief Store Double Quadword Using Non-Temporal Hint (SSE2).
  inline void movntdq(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVNTDQ, &dst, &src);
  }

  //! @brief Store Store DWORD Using Non-Temporal Hint (SSE2).
  inline void movnti(const Mem& dst, const Register& src)
  {
    __emitX86(INST_MOVNTI, &dst, &src);
  }

  //! @brief Store Packed Double-Precision FP Values Using Non-Temporal Hint (SSE2).
  inline void movntpd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVNTPD, &dst, &src);
  }

  //! @brief Move Unaligned Packed Double-Precision FP Values (SSE2).
  inline void movupd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVUPD, &dst, &src);
  }

  //! @brief Move Unaligned Packed Double-Precision FP Values (SSE2).
  inline void movupd(const Mem& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVUPD, &dst, &src);
  }

  //! @brief Packed DP-FP Multiply (SSE2).
  inline void mulpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MULPD, &dst, &src);
  }
  //! @brief Packed DP-FP Multiply (SSE2).
  inline void mulpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MULPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Multiply (SSE2).
  inline void mulsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MULSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Multiply (SSE2).
  inline void mulsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MULSD, &dst, &src);
  }

  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void orpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ORPD, &dst, &src);
  }
  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void orpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ORPD, &dst, &src);
  }

  //! @brief Pack with Signed Saturation (SSE2).
  inline void packsswb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PACKSSWB, &dst, &src);
  }
  //! @brief Pack with Signed Saturation (SSE2).
  inline void packsswb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PACKSSWB, &dst, &src);
  }

  //! @brief Pack with Signed Saturation (SSE2).
  inline void packssdw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PACKSSDW, &dst, &src);
  }
  //! @brief Pack with Signed Saturation (SSE2).
  inline void packssdw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PACKSSDW, &dst, &src);
  }

  //! @brief Pack with Unsigned Saturation (SSE2).
  inline void packuswb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PACKUSWB, &dst, &src);
  }
  //! @brief Pack with Unsigned Saturation (SSE2).
  inline void packuswb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PACKUSWB, &dst, &src);
  }

  //! @brief Packed BYTE Add (SSE2).
  inline void paddb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDB, &dst, &src);
  }
  //! @brief Packed BYTE Add (SSE2).
  inline void paddb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDB, &dst, &src);
  }

  //! @brief Packed WORD Add (SSE2).
  inline void paddw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDW, &dst, &src);
  }
  //! @brief Packed WORD Add (SSE2).
  inline void paddw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDW, &dst, &src);
  }

  //! @brief Packed DWORD Add (SSE2).
  inline void paddd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDD, &dst, &src);
  }
  //! @brief Packed DWORD Add (SSE2).
  inline void paddd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDD, &dst, &src);
  }

  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PADDQ, &dst, &src);
  }
  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDQ, &dst, &src);
  }

  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDQ, &dst, &src);
  }
  //! @brief Packed QWORD Add (SSE2).
  inline void paddq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDQ, &dst, &src);
  }

  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDSB, &dst, &src);
  }
  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDSB, &dst, &src);
  }

  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDSW, &dst, &src);
  }
  //! @brief Packed Add with Saturation (SSE2).
  inline void paddsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDSW, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDUSB, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDUSB, &dst, &src);
  }

  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PADDUSW, &dst, &src);
  }
  //! @brief Packed Add Unsigned with Saturation (SSE2).
  inline void paddusw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PADDUSW, &dst, &src);
  }

  //! @brief Logical AND (SSE2).
  inline void pand(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PAND, &dst, &src);
  }
  //! @brief Logical AND (SSE2).
  inline void pand(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PAND, &dst, &src);
  }

  //! @brief Logical AND Not (SSE2).
  inline void pandn(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PANDN, &dst, &src);
  }
  //! @brief Logical AND Not (SSE2).
  inline void pandn(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PANDN, &dst, &src);
  }

  //! @brief Spin Loop Hint (SSE2).
  inline void pause()
  {
    __emitX86(INST_PAUSE);
  }

  //! @brief Packed Average (SSE2).
  inline void pavgb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PAVGB, &dst, &src);
  }
  //! @brief Packed Average (SSE2).
  inline void pavgb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PAVGB, &dst, &src);
  }

  //! @brief Packed Average (SSE2).
  inline void pavgw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PAVGW, &dst, &src);
  }
  //! @brief Packed Average (SSE2).
  inline void pavgw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PAVGW, &dst, &src);
  }

  //! @brief Packed Compare for Equal (BYTES) (SSE2).
  inline void pcmpeqb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPEQB, &dst, &src);
  }
  //! @brief Packed Compare for Equal (BYTES) (SSE2).
  inline void pcmpeqb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQB, &dst, &src);
  }

  //! @brief Packed Compare for Equal (WORDS) (SSE2).
  inline void pcmpeqw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPEQW, &dst, &src);
  }
  //! @brief Packed Compare for Equal (WORDS) (SSE2).
  inline void pcmpeqw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQW, &dst, &src);
  }

  //! @brief Packed Compare for Equal (DWORDS) (SSE2).
  inline void pcmpeqd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPEQD, &dst, &src);
  }
  //! @brief Packed Compare for Equal (DWORDS) (SSE2).
  inline void pcmpeqd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQD, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (BYTES) (SSE2).
  inline void pcmpgtb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPGTB, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (BYTES) (SSE2).
  inline void pcmpgtb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTB, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (WORDS) (SSE2).
  inline void pcmpgtw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPGTW, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (WORDS) (SSE2).
  inline void pcmpgtw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTW, &dst, &src);
  }

  //! @brief Packed Compare for Greater Than (DWORDS) (SSE2).
  inline void pcmpgtd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPGTD, &dst, &src);
  }
  //! @brief Packed Compare for Greater Than (DWORDS) (SSE2).
  inline void pcmpgtd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTD, &dst, &src);
  }

  //! @brief Packed Signed Integer Word Maximum (SSE2).
  inline void pmaxsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMAXSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Maximum (SSE2).
  inline void pmaxsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Maximum (SSE2).
  inline void pmaxub(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMAXUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Maximum (SSE2).
  inline void pmaxub(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXUB, &dst, &src);
  }

  //! @brief Packed Signed Integer Word Minimum (SSE2).
  inline void pminsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMINSW, &dst, &src);
  }
  //! @brief Packed Signed Integer Word Minimum (SSE2).
  inline void pminsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINSW, &dst, &src);
  }

  //! @brief Packed Unsigned Integer Byte Minimum (SSE2).
  inline void pminub(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMINUB, &dst, &src);
  }
  //! @brief Packed Unsigned Integer Byte Minimum (SSE2).
  inline void pminub(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINUB, &dst, &src);
  }

  //! @brief Move Byte Mask (SSE2).
  inline void pmovmskb(const Register& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVMSKB, &dst, &src);
  }

  //! @brief Packed Multiply High (SSE2).
  inline void pmulhw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULHW, &dst, &src);
  }
  //! @brief Packed Multiply High (SSE2).
  inline void pmulhw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULHW, &dst, &src);
  }

  //! @brief Packed Multiply High Unsigned (SSE2).
  inline void pmulhuw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULHUW, &dst, &src);
  }
  //! @brief Packed Multiply High Unsigned (SSE2).
  inline void pmulhuw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULHUW, &dst, &src);
  }

  //! @brief Packed Multiply Low (SSE2).
  inline void pmullw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULLW, &dst, &src);
  }
  //! @brief Packed Multiply Low (SSE2).
  inline void pmullw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULLW, &dst, &src);
  }

  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMULUDQ, &dst, &src);
  }
  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULUDQ, &dst, &src);
  }

  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULUDQ, &dst, &src);
  }
  //! @brief Packed Multiply to QWORD (SSE2).
  inline void pmuludq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULUDQ, &dst, &src);
  }

  //! @brief Bitwise Logical OR (SSE2).
  inline void por(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_POR, &dst, &src);
  }
  //! @brief Bitwise Logical OR (SSE2).
  inline void por(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_POR, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslld(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslld(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSLLD, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslld(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLD, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSLLQ, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllq(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLQ, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSLLW, &dst, &src);
  }
  //! @brief Packed Shift Left Logical (SSE2).
  inline void psllw(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLW, &dst, &src);
  }

  //! @brief Packed Shift Left Logical (SSE2).
  inline void pslldq(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSLLDQ, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psrad(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psrad(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRAD, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psrad(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRAD, &dst, &src);
  }

  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psraw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psraw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRAW, &dst, &src);
  }
  //! @brief Packed Shift Right Arithmetic (SSE2).
  inline void psraw(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRAW, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBB, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBB, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBW, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBW, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBD, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBD, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubq(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSUBQ, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubq(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBQ, &dst, &src);
  }

  //! @brief Packed Subtract (SSE2).
  inline void psubq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBQ, &dst, &src);
  }
  //! @brief Packed Subtract (SSE2).
  inline void psubq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBQ, &dst, &src);
  }

  //! @brief Packed Multiply and Add (SSE2).
  inline void pmaddwd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMADDWD, &dst, &src);
  }
  //! @brief Packed Multiply and Add (SSE2).
  inline void pmaddwd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMADDWD, &dst, &src);
  }

  //! @brief Shuffle Packed DWORDs (SSE2).
  inline void pshufd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFD, &dst, &src, &imm8);
  }
  //! @brief Shuffle Packed DWORDs (SSE2).
  inline void pshufd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFD, &dst, &src, &imm8);
  }

  //! @brief Shuffle Packed High Words (SSE2).
  inline void pshufhw(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFHW, &dst, &src, &imm8);
  }
  //! @brief Shuffle Packed High Words (SSE2).
  inline void pshufhw(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFHW, &dst, &src, &imm8);
  }

  //! @brief Shuffle Packed Low Words (SSE2).
  inline void pshuflw(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFLW, &dst, &src, &imm8);
  }
  //! @brief Shuffle Packed Low Words (SSE2).
  inline void pshuflw(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PSHUFLW, &dst, &src, &imm8);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrld(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrld(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRLD, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrld(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLD, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRLQ, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlq(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLQ, &dst, &src);
  }

  //! @brief DQWord Shift Right Logical (MMX).
  inline void psrldq(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLDQ, &dst, &src);
  }

  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSRLW, &dst, &src);
  }
  //! @brief Packed Shift Right Logical (SSE2).
  inline void psrlw(const XMMRegister& dst, const Immediate& src)
  {
    __emitX86(INST_PSRLW, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBSB, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBSB, &dst, &src);
  }

  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBSW, &dst, &src);
  }
  //! @brief Packed Subtract with Saturation (SSE2).
  inline void psubsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBSW, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBUSB, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBUSB, &dst, &src);
  }

  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSUBUSW, &dst, &src);
  }
  //! @brief Packed Subtract with Unsigned Saturation (SSE2).
  inline void psubusw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSUBUSW, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhbw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKHBW, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhbw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHBW, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhwd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKHWD, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhwd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHWD, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhdq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKHDQ, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhdq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHDQ, &dst, &src);
  }

  //! @brief Unpack High Data (SSE2).
  inline void punpckhqdq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKHQDQ, &dst, &src);
  }
  //! @brief Unpack High Data (SSE2).
  inline void punpckhqdq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKHQDQ, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpcklbw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKLBW, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpcklbw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLBW, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpcklwd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKLWD, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpcklwd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLWD, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpckldq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKLDQ, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpckldq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLDQ, &dst, &src);
  }

  //! @brief Unpack Low Data (SSE2).
  inline void punpcklqdq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PUNPCKLQDQ, &dst, &src);
  }
  //! @brief Unpack Low Data (SSE2).
  inline void punpcklqdq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PUNPCKLQDQ, &dst, &src);
  }

  //! @brief Bitwise Exclusive OR (SSE2).
  inline void pxor(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PXOR, &dst, &src);
  }
  //! @brief Bitwise Exclusive OR (SSE2).
  inline void pxor(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PXOR, &dst, &src);
  }

  //! @brief Compute Square Roots of Packed DP-FP Values (SSE2).
  inline void sqrtpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SQRTPD, &dst, &src);
  }
  //! @brief Compute Square Roots of Packed DP-FP Values (SSE2).
  inline void sqrtpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SQRTPD, &dst, &src);
  }

  //! @brief Compute Square Root of Scalar DP-FP Value (SSE2).
  inline void sqrtsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SQRTSD, &dst, &src);
  }
  //! @brief Compute Square Root of Scalar DP-FP Value (SSE2).
  inline void sqrtsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SQRTSD, &dst, &src);
  }

  //! @brief Packed DP-FP Subtract (SSE2).
  inline void subpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SUBPD, &dst, &src);
  }
  //! @brief Packed DP-FP Subtract (SSE2).
  inline void subpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SUBPD, &dst, &src);
  }

  //! @brief Scalar DP-FP Subtract (SSE2).
  inline void subsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_SUBSD, &dst, &src);
  }
  //! @brief Scalar DP-FP Subtract (SSE2).
  inline void subsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_SUBSD, &dst, &src);
  }

  //! @brief Scalar Unordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void ucomisd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_UCOMISD, &dst, &src);
  }
  //! @brief Scalar Unordered DP-FP Compare and Set EFLAGS (SSE2).
  inline void ucomisd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_UCOMISD, &dst, &src);
  }

  //! @brief Unpack and Interleave High Packed Double-Precision FP Values (SSE2).
  inline void unpckhpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_UNPCKHPD, &dst, &src);
  }
  //! @brief Unpack and Interleave High Packed Double-Precision FP Values (SSE2).
  inline void unpckhpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_UNPCKHPD, &dst, &src);
  }

  //! @brief Unpack and Interleave Low Packed Double-Precision FP Values (SSE2).
  inline void unpcklpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_UNPCKLPD, &dst, &src);
  }
  //! @brief Unpack and Interleave Low Packed Double-Precision FP Values (SSE2).
  inline void unpcklpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_UNPCKLPD, &dst, &src);
  }

  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void xorpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_XORPD, &dst, &src);
  }
  //! @brief Bit-wise Logical OR for DP-FP Data (SSE2).
  inline void xorpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_XORPD, &dst, &src);
  }

  // -------------------------------------------------------------------------
  // [SSE3]
  // -------------------------------------------------------------------------

  //! @brief Packed DP-FP Add/Subtract (SSE3).
  inline void addsubpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ADDSUBPD, &dst, &src);
  }
  //! @brief Packed DP-FP Add/Subtract (SSE3).
  inline void addsubpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ADDSUBPD, &dst, &src);
  }

  //! @brief Packed SP-FP Add/Subtract (SSE3).
  inline void addsubps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_ADDSUBPS, &dst, &src);
  }
  //! @brief Packed SP-FP Add/Subtract (SSE3).
  inline void addsubps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_ADDSUBPS, &dst, &src);
  }

  //! @brief Store Integer with Truncation (SSE3).
  inline void fisttp(const Mem& dst)
  {
    __emitX86(INST_FISTTP, &dst);
  }

  //! @brief Packed DP-FP Horizontal Add (SSE3).
  inline void haddpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_HADDPD, &dst, &src);
  }
  //! @brief Packed DP-FP Horizontal Add (SSE3).
  inline void haddpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_HADDPD, &dst, &src);
  }

  //! @brief Packed SP-FP Horizontal Add (SSE3).
  inline void haddps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_HADDPS, &dst, &src);
  }
  //! @brief Packed SP-FP Horizontal Add (SSE3).
  inline void haddps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_HADDPS, &dst, &src);
  }

  //! @brief Packed DP-FP Horizontal Subtract (SSE3).
  inline void hsubpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_HSUBPD, &dst, &src);
  }
  //! @brief Packed DP-FP Horizontal Subtract (SSE3).
  inline void hsubpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_HSUBPD, &dst, &src);
  }

  //! @brief Packed SP-FP Horizontal Subtract (SSE3).
  inline void hsubps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_HSUBPS, &dst, &src);
  }
  //! @brief Packed SP-FP Horizontal Subtract (SSE3).
  inline void hsubps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_HSUBPS, &dst, &src);
  }

  //! @brief Load Unaligned Integer 128 Bits (SSE3).
  inline void lddqu(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_LDDQU, &dst, &src);
  }

  //! @brief Set Up Monitor Address (SSE3).
  inline void monitor()
  {
    __emitX86(INST_MONITOR);
  }

  //! @brief Move One DP-FP and Duplicate (SSE3).
  inline void movddup(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVDDUP, &dst, &src);
  }
  //! @brief Move One DP-FP and Duplicate (SSE3).
  inline void movddup(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVDDUP, &dst, &src);
  }

  //! @brief Move Packed SP-FP High and Duplicate (SSE3).
  inline void movshdup(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVSHDUP, &dst, &src);
  }
  //! @brief Move Packed SP-FP High and Duplicate (SSE3).
  inline void movshdup(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVSHDUP, &dst, &src);
  }

  //! @brief Move Packed SP-FP Low and Duplicate (SSE3).
  inline void movsldup(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_MOVSLDUP, &dst, &src);
  }
  //! @brief Move Packed SP-FP Low and Duplicate (SSE3).
  inline void movsldup(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVSLDUP, &dst, &src);
  }

  //! @brief Monitor Wait (SSE3).
  inline void mwait()
  {
    __emitX86(INST_MWAIT);
  }

  // -------------------------------------------------------------------------
  // [SSSE3]
  // -------------------------------------------------------------------------

  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSIGNB, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSIGNB, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSIGNB, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSIGNB, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSIGNW, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSIGNW, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSIGNW, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSIGNW, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSIGND, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSIGND, &dst, &src);
  }

  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSIGND, &dst, &src);
  }
  //! @brief Packed SIGN (SSSE3).
  inline void psignd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSIGND, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PHADDW, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHADDW, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHADDW, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHADDW, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PHADDD, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHADDD, &dst, &src);
  }

  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHADDD, &dst, &src);
  }
  //! @brief Packed Horizontal Add (SSSE3).
  inline void phaddd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHADDD, &dst, &src);
  }

  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PHADDSW, &dst, &src);
  }
  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHADDSW, &dst, &src);
  }

  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHADDSW, &dst, &src);
  }
  //! @brief Packed Horizontal Add and Saturate (SSSE3).
  inline void phaddsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHADDSW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PHSUBW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHSUBW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHSUBW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHSUBW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PHSUBD, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHSUBD, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHSUBD, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract (SSSE3).
  inline void phsubd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHSUBD, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PHSUBSW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHSUBSW, &dst, &src);
  }

  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHSUBSW, &dst, &src);
  }
  //! @brief Packed Horizontal Subtract and Saturate (SSSE3).
  inline void phsubsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHSUBSW, &dst, &src);
  }

  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMADDUBSW, &dst, &src);
  }
  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMADDUBSW, &dst, &src);
  }

  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMADDUBSW, &dst, &src);
  }
  //! @brief Multiply and Add Packed Signed and Unsigned Bytes (SSSE3).
  inline void pmaddubsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMADDUBSW, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PABSB, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PABSB, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PABSB, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PABSB, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PABSW, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PABSW, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PABSW, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PABSW, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PABSD, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PABSD, &dst, &src);
  }

  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PABSD, &dst, &src);
  }
  //! @brief Packed Absolute Value (SSSE3).
  inline void pabsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PABSD, &dst, &src);
  }

  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PMULHRSW, &dst, &src);
  }
  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULHRSW, &dst, &src);
  }

  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULHRSW, &dst, &src);
  }
  //! @brief Packed Multiply High with Round and Scale (SSSE3).
  inline void pmulhrsw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULHRSW, &dst, &src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const MMRegister& dst, const MMRegister& src)
  {
    __emitX86(INST_PSHUFB, &dst, &src);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const MMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSHUFB, &dst, &src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PSHUFB, &dst, &src);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void pshufb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PSHUFB, &dst, &src);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const MMRegister& dst, const MMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PALIGNR, &dst, &src, &imm8);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const MMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PALIGNR, &dst, &src, &imm8);
  }

  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PALIGNR, &dst, &src, &imm8);
  }
  //! @brief Packed Shuffle Bytes (SSSE3).
  inline void palignr(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PALIGNR, &dst, &src, &imm8);
  }

  // -------------------------------------------------------------------------
  // [SSE4.1]
  // -------------------------------------------------------------------------

  //! @brief Blend Packed DP-FP Values (SSE4.1).
  inline void blendpd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_BLENDPD, &dst, &src, &imm8);
  }
  //! @brief Blend Packed DP-FP Values (SSE4.1).
  inline void blendpd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_BLENDPD, &dst, &src, &imm8);
  }

  //! @brief Blend Packed SP-FP Values (SSE4.1).
  inline void blendps(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_BLENDPS, &dst, &src, &imm8);
  }
  //! @brief Blend Packed SP-FP Values (SSE4.1).
  inline void blendps(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_BLENDPS, &dst, &src, &imm8);
  }

  //! @brief Variable Blend Packed DP-FP Values (SSE4.1).
  inline void blendvpd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_BLENDVPD, &dst, &src);
  }
  //! @brief Variable Blend Packed DP-FP Values (SSE4.1).
  inline void blendvpd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_BLENDVPD, &dst, &src);
  }

  //! @brief Variable Blend Packed SP-FP Values (SSE4.1).
  inline void blendvps(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_BLENDVPS, &dst, &src);
  }
  //! @brief Variable Blend Packed SP-FP Values (SSE4.1).
  inline void blendvps(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_BLENDVPS, &dst, &src);
  }

  //! @brief Dot Product of Packed DP-FP Values (SSE4.1).
  inline void dppd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_DPPD, &dst, &src, &imm8);
  }
  //! @brief Dot Product of Packed DP-FP Values (SSE4.1).
  inline void dppd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_DPPD, &dst, &src, &imm8);
  } 

  //! @brief Dot Product of Packed SP-FP Values (SSE4.1).
  inline void dpps(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_DPPS, &dst, &src, &imm8);
  }
  //! @brief Dot Product of Packed SP-FP Values (SSE4.1).
  inline void dpps(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_DPPS, &dst, &src, &imm8);
  } 

  //! @brief Extract Packed SP-FP Value @brief (SSE4.1).
  inline void extractps(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_EXTRACTPS, &dst, &src, &imm8);
  }
  //! @brief Extract Packed SP-FP Value @brief (SSE4.1).
  inline void extractps(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_EXTRACTPS, &dst, &src, &imm8);
  }

  //! @brief Load Double Quadword Non-Temporal Aligned Hint (SSE4.1).
  inline void movntdqa(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_MOVNTDQA, &dst, &src);
  }

  //! @brief Compute Multiple Packed Sums of Absolute Difference (SSE4.1).
  inline void mpsadbw(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_MPSADBW, &dst, &src, &imm8);
  }
  //! @brief Compute Multiple Packed Sums of Absolute Difference (SSE4.1).
  inline void mpsadbw(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_MPSADBW, &dst, &src, &imm8);
  }

  //! @brief Pack with Unsigned Saturation (SSE4.1).
  inline void packusdw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PACKUSDW, &dst, &src);
  }
  //! @brief Pack with Unsigned Saturation (SSE4.1).
  inline void packusdw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PACKUSDW, &dst, &src);
  }

  //! @brief Variable Blend Packed Bytes (SSE4.1).
  inline void pblendvb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PBLENDVB, &dst, &src);
  }
  //! @brief Variable Blend Packed Bytes (SSE4.1).
  inline void pblendvb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PBLENDVB, &dst, &src);
  } 

  //! @brief Blend Packed Words (SSE4.1).
  inline void pblendw(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PBLENDW, &dst, &src, &imm8);
  }
  //! @brief Blend Packed Words (SSE4.1).
  inline void pblendw(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PBLENDW, &dst, &src, &imm8);
  }

  //! @brief Compare Packed Qword Data for Equal (SSE4.1).
  inline void pcmpeqq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPEQQ, &dst, &src);
  }
  //! @brief Compare Packed Qword Data for Equal (SSE4.1).
  inline void pcmpeqq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPEQQ, &dst, &src);
  }

  //! @brief Extract Byte (SSE4.1).
  inline void pextrb(const Register& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRB, &dst, &src, &imm8);
  }
  //! @brief Extract Byte (SSE4.1).
  inline void pextrb(const Mem& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRB, &dst, &src, &imm8);
  }

  //! @brief Extract Dword (SSE4.1).
  inline void pextrd(const Register& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRD, &dst, &src, &imm8);
  }
  //! @brief Extract Dword (SSE4.1).
  inline void pextrd(const Mem& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRD, &dst, &src, &imm8);
  }

  //! @brief Extract Dword (SSE4.1).
  inline void pextrq(const Register& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRQ, &dst, &src, &imm8);
  }
  //! @brief Extract Dword (SSE4.1).
  inline void pextrq(const Mem& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRQ, &dst, &src, &imm8);
  }

  //! @brief Extract Word (SSE4.1).
  inline void pextrw(const Register& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRW, &dst, &src, &imm8);
  }
  //! @brief Extract Word (SSE4.1).
  inline void pextrw(const Mem& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PEXTRW, &dst, &src, &imm8);
  }

  //! @brief Packed Horizontal Word Minimum (SSE4.1).
  inline void phminposuw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PHMINPOSUW, &dst, &src);
  }
  //! @brief Packed Horizontal Word Minimum (SSE4.1).
  inline void phminposuw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PHMINPOSUW, &dst, &src);
  } 

  //! @brief Insert Byte (SSE4.1).
  inline void pinsrb(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRB, &dst, &src, &imm8);
  }
  //! @brief Insert Byte (SSE4.1).
  inline void pinsrb(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRB, &dst, &src, &imm8);
  }

  //! @brief Insert Dword (SSE4.1).
  inline void pinsrd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRD, &dst, &src, &imm8);
  }
  //! @brief Insert Dword (SSE4.1).
  inline void pinsrd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRD, &dst, &src, &imm8);
  }

  //! @brief Insert Word (SSE2).
  inline void pinsrw(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRW, &dst, &src, &imm8);
  }
  //! @brief Insert Word (SSE2).
  inline void pinsrw(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PINSRW, &dst, &src, &imm8);
  }

  //! @brief Maximum of Packed Word Integers (SSE4.1).
  inline void pmaxuw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMAXUW, &dst, &src);
  }
  //! @brief Maximum of Packed Word Integers (SSE4.1).
  inline void pmaxuw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXUW, &dst, &src);
  }

  //! @brief Maximum of Packed Signed Byte Integers (SSE4.1).
  inline void pmaxsb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMAXSB, &dst, &src);
  }
  //! @brief Maximum of Packed Signed Byte Integers (SSE4.1).
  inline void pmaxsb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXSB, &dst, &src);
  }

  //! @brief Maximum of Packed Signed Dword Integers (SSE4.1).
  inline void pmaxsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMAXSD, &dst, &src);
  }
  //! @brief Maximum of Packed Signed Dword Integers (SSE4.1).
  inline void pmaxsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXSD, &dst, &src);
  }

  //! @brief Maximum of Packed Unsigned Dword Integers (SSE4.1).
  inline void pmaxud(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMAXUD, &dst, &src);
  }
  //! @brief Maximum of Packed Unsigned Dword Integers (SSE4.1).
  inline void pmaxud(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMAXUD, &dst, &src);
  }

  //! @brief Minimum of Packed Signed Byte Integers (SSE4.1).
  inline void pminsb(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMINSB, &dst, &src);
  }
  //! @brief Minimum of Packed Signed Byte Integers (SSE4.1).
  inline void pminsb(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINSB, &dst, &src);
  }

  //! @brief Minimum of Packed Word Integers (SSE4.1).
  inline void pminuw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMINUW, &dst, &src);
  }
  //! @brief Minimum of Packed Word Integers (SSE4.1).
  inline void pminuw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINUW, &dst, &src);
  }

  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminud(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMINUD, &dst, &src);
  }
  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminud(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINUD, &dst, &src);
  }

  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminsd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMINSD, &dst, &src);
  }
  //! @brief Minimum of Packed Dword Integers (SSE4.1).
  inline void pminsd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMINSD, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVSXBW, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVSXBW, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVSXBD, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVSXBD, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVSXBQ, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxbq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVSXBQ, &dst, &src);
  }

  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxwd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVSXWD, &dst, &src);
  }
  //! @brief Packed Move with Sign Extend (SSE4.1).
  inline void pmovsxwd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVSXWD, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovsxwq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVSXWQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovsxwq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVSXWQ, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovsxdq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVSXDQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovsxdq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVSXDQ, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbw(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVZXBW, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbw(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVZXBW, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVZXBD, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVZXBD, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVZXBQ, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxbq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVZXBQ, &dst, &src);
  }

  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxwd(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVZXWD, &dst, &src);
  }
  //! @brief Packed Move with Zero Extend (SSE4.1).
  inline void pmovzxwd(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVZXWD, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovzxwq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVZXWQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovzxwq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVZXWQ, &dst, &src);
  }

  //! @brief (SSE4.1).
  inline void pmovzxdq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMOVZXDQ, &dst, &src);
  }
  //! @brief (SSE4.1).
  inline void pmovzxdq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMOVZXDQ, &dst, &src);
  }

  //! @brief Multiply Packed Signed Dword Integers (SSE4.1).
  inline void pmuldq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULDQ, &dst, &src);
  }
  //! @brief Multiply Packed Signed Dword Integers (SSE4.1).
  inline void pmuldq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULDQ, &dst, &src);
  } 

  //! @brief Multiply Packed Signed Integers and Store Low Result (SSE4.1).
  inline void pmulld(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PMULLD, &dst, &src);
  }
  //! @brief Multiply Packed Signed Integers and Store Low Result (SSE4.1).
  inline void pmulld(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PMULLD, &dst, &src);
  } 

  //! @brief Logical Compare (SSE4.1).
  inline void ptest(const XMMRegister& op1, const XMMRegister& op2)
  {
    __emitX86(INST_PTEST, &op1, &op2);
  }
  //! @brief Logical Compare (SSE4.1).
  inline void ptest(const XMMRegister& op1, const Mem& op2)
  {
    __emitX86(INST_PTEST, &op1, &op2);
  }

  //! Round Packed SP-FP Values @brief (SSE4.1).
  inline void roundps(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDPS, &dst, &src, &imm8);
  }
  //! Round Packed SP-FP Values @brief (SSE4.1).
  inline void roundps(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDPS, &dst, &src, &imm8);
  }

  //! @brief Round Scalar SP-FP Values (SSE4.1).
  inline void roundss(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDSS, &dst, &src, &imm8);
  }
  //! @brief Round Scalar SP-FP Values (SSE4.1).
  inline void roundss(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDSS, &dst, &src, &imm8);
  }

  //! @brief Round Packed DP-FP Values (SSE4.1).
  inline void roundpd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDPD, &dst, &src, &imm8);
  }
  //! @brief Round Packed DP-FP Values (SSE4.1).
  inline void roundpd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDPD, &dst, &src, &imm8);
  }

  //! @brief Round Scalar DP-FP Values (SSE4.1).
  inline void roundsd(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDSD, &dst, &src, &imm8);
  }
  //! @brief Round Scalar DP-FP Values (SSE4.1).
  inline void roundsd(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_ROUNDSD, &dst, &src, &imm8);
  }

  // -------------------------------------------------------------------------
  // [SSE4.2]
  // -------------------------------------------------------------------------

  //! @brief Accumulate CRC32 Value (polynomial 0x11EDC6F41) (SSE4.2).
  inline void crc32(const Register& dst, const Register& src)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_GPD) || dst.isRegType(REG_GPQ));
    __emitX86(INST_CRC32, &dst, &src);
  }
  //! @brief Accumulate CRC32 Value (polynomial 0x11EDC6F41) (SSE4.2).
  inline void crc32(const Register& dst, const Mem& src)
  {
    ASMJIT_ASSERT(dst.isRegType(REG_GPD) || dst.isRegType(REG_GPQ));
    __emitX86(INST_CRC32, &dst, &src);
  }

  //! @brief Packed Compare Explicit Length Strings, Return Index (SSE4.2).
  inline void pcmpestri(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPESTRI, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Explicit Length Strings, Return Index (SSE4.2).
  inline void pcmpestri(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPESTRI, &dst, &src, &imm8);
  }

  //! @brief Packed Compare Explicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpestrm(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPESTRM, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Explicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpestrm(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPESTRM, &dst, &src, &imm8);
  }

  //! @brief Packed Compare Implicit Length Strings, Return Index (SSE4.2).
  inline void pcmpistri(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPISTRI, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Implicit Length Strings, Return Index (SSE4.2).
  inline void pcmpistri(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPISTRI, &dst, &src, &imm8);
  }

  //! @brief Packed Compare Implicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpistrm(const XMMRegister& dst, const XMMRegister& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPISTRM, &dst, &src, &imm8);
  }
  //! @brief Packed Compare Implicit Length Strings, Return Mask (SSE4.2).
  inline void pcmpistrm(const XMMRegister& dst, const Mem& src, const Immediate& imm8)
  {
    __emitX86(INST_PCMPISTRM, &dst, &src, &imm8);
  }

  //! @brief Compare Packed Data for Greater Than (SSE4.2).
  inline void pcmpgtq(const XMMRegister& dst, const XMMRegister& src)
  {
    __emitX86(INST_PCMPGTQ, &dst, &src);
  }
  //! @brief Compare Packed Data for Greater Than (SSE4.2).
  inline void pcmpgtq(const XMMRegister& dst, const Mem& src)
  {
    __emitX86(INST_PCMPGTQ, &dst, &src);
  }

  //! @brief Return the Count of Number of Bits Set to 1 (SSE4.2).
  inline void popcnt(const Register& dst, const Register& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    ASMJIT_ASSERT(src.type() == dst.type());
    __emitX86(INST_POPCNT, &dst, &src);
  }
  //! @brief Return the Count of Number of Bits Set to 1 (SSE4.2).
  inline void popcnt(const Register& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    __emitX86(INST_POPCNT, &dst, &src);
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
  inline void amd_prefetch(const Mem& mem)
  {
    __emitX86(INST_AMD_PREFETCH, &mem);
  }

  //! @brief Prefetch and set cache to modified (3dNow - Amd).
  //!
  //! The PREFETCHW instruction loads the prefetched line and sets the 
  //! cache-line state to Modified, in anticipation of subsequent data 
  //! writes to the line. The PREFETCH instruction, by contrast, typically
  //! sets the cache-line state to Exclusive (depending on the hardware 
  //! implementation).
  inline void amd_prefetchw(const Mem& mem)
  {
    __emitX86(INST_AMD_PREFETCHW, &mem);
  }

  // -------------------------------------------------------------------------
  // [Intel only]
  // -------------------------------------------------------------------------

  //! @brief Move Data After Swapping Bytes (SSE3 - Intel Atom).
  inline void movbe(const Register& dst, const Mem& src)
  {
    ASMJIT_ASSERT(!dst.isRegType(REG_GPB));
    __emitX86(INST_MOVBE, &dst, &src);
  }

  //! @brief Move Data After Swapping Bytes (SSE3 - Intel Atom).
  inline void movbe(const Mem& dst, const Register& src)
  {
    ASMJIT_ASSERT(!src.isRegType(REG_GPB));
    __emitX86(INST_MOVBE, &dst, &src);
  }
};

} // AsmJit namespace

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // _MSC_VER

// [Guard]
#endif // _ASMJITSERIALIZERX86X64_H
