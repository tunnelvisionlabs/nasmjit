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
#ifndef _ASMJIT_DEFS_H
#define _ASMJIT_DEFS_H

// [Dependencies]
#include "Build.h"

namespace AsmJit {

//! @addtogroup AsmJit_Defs
//! @{

// ============================================================================
// [AsmJit::ERROR_CODE]
// ============================================================================

//! @brief AsmJit error codes.
enum ERROR_CODE
{
  //! @brief No error (success).
  //!
  //! This is default state and state you want.
  ERROR_NONE = 0,

  //! @brief Memory allocation error (@c ASMJIT_MALLOC returned @c NULL).
  ERROR_NO_HEAP_MEMORY = 1,

  //! @brief Virtual memory allocation error (@c VirtualMemory returned @c NULL).
  ERROR_NO_VIRTUAL_MEMORY = 2,

  //! @brief Unknown instruction. This happens only if instruction code is 
  //! out of bounds. Shouldn't happen.
  ERROR_UNKNOWN_INSTRUCTION = 3,

  //! @brief Illegal instruction, usually generated by AsmJit::AssemblerCore
  //! class when emitting instruction opcode. If this error is generated the
  //! target buffer is not affected by this invalid instruction.
  //!
  //! In debug mode you get assertion failure instead.
  ERROR_ILLEGAL_INSTRUCTION = 4,

  //! @brief Illegal addressing used (unencodable).
  ERROR_ILLEGAL_ADDRESING = 5,

  //! @brief Short jump instruction used, but displacement is out of bounds.
  ERROR_ILLEGAL_SHORT_JUMP = 6,

  //! @brief Count of error codes in AsmJit. Can grow in future.
  _ERROR_COUNT
};

//! @brief Translates error code (see @c ERROR_CODE) into text representation.
const char* errorCodeToString(UInt32 error) ASMJIT_NOTHROW;

// ============================================================================
// [AsmJit::OP]
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
  OP_LABEL = 4
};

// ============================================================================
// [AsmJit::SIZE]
// ============================================================================

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

// ============================================================================
// [AsmJit::RELOC_MODE]
// ============================================================================

//! @brief Relocation info.
enum RELOC_MODE
{
  //! @brief No relocation.
  RELOC_NONE = 0,

  //! @brief Overwrite relocation (immediates as constants).
  RELOC_OVERWRITE = 1
};

// ============================================================================
// [AsmJit::LABEL_STATE]
// ============================================================================

//! @brief Label state.
enum LABEL_STATE
{
  //! @brief Label is unused.
  LABEL_STATE_UNUSED = 0,
  //! @brief Label is linked (waiting to be bound)
  LABEL_STATE_LINKED = 1,
  //! @brief Label is bound
  LABEL_STATE_BOUND = 2
};

// ============================================================================
// [AsmJit::PROPERTY]
// ============================================================================

//! @brief @c Assembler/Compiler properties.
enum PROPERTY
{
  //! @brief Optimize align for current processor.
  //!
  //! Default: @c true.
  PROPERTY_OPTIMIZE_ALIGN = 0,

  //! @brief Force rex prefix emitting.
  //!
  //! Default: @c false.
  //!
  //! @note This is X86/X86 property only.
  PROPERTY_X86_FORCE_REX = 1,

  //! @brief Emit hints added to jcc() instructions.
  //!
  //! Default: @c true.
  //!
  //! @note This is X86/X86 property only.
  PROPERTY_X86_JCC_HINTS = 2
};

//! @}

} // AsmJit namespace

// ============================================================================
// [AsmJit::Definitions - X86 / X64]
// ============================================================================

#if defined(ASMJIT_X86) || defined(ASMJIT_X64)
#include "DefsX86X64.h"
#endif // ASMJIT_X86 || ASMJIT_X64

// [Guard]
#endif // _ASMJIT_DEFS_H
