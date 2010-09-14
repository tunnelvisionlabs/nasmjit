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

// [Guard]
#ifndef _ASMJIT_DEFS_H
#define _ASMJIT_DEFS_H

// [Dependencies]
#include "Build.h"

namespace AsmJit {

//! @addtogroup AsmJit_Core
//! @{

// ============================================================================
// [AsmJit::MEMORY_ALLOC_TYPE]
// ============================================================================

//! @brief Types of allocation used by @c AsmJit::MemoryManager::alloc() method.
enum MEMORY_ALLOC_TYPE
{
  //! @brief Allocate memory that can be freed by @c AsmJit::MemoryManager::free()
  //! method.
  MEMORY_ALLOC_FREEABLE,
  //! @brief Allocate pernament memory that will be never freed.
  MEMORY_ALLOC_PERNAMENT
};

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
  //! You can also get this error code if you are under x64 (64-bit x86) and
  //! you tried to decode instruction using AH, BH, CH or DH register with REX
  //! prefix. These registers can't be accessed if REX prefix is used and AsmJit
  //! didn't check for this situation in intrinsics (@c Compiler takes care of
  //! this and rearrange registers if needed).
  //!
  //! Examples that will raise @c ERROR_ILLEGAL_INSTRUCTION error (a is
  //! @c Assembler instance):
  //!
  //! @code
  //! a.mov(dword_ptr(eax), al); // Invalid address size.
  //! a.mov(byte_ptr(r10), ah);  // Undecodable instruction (AH used with r10
  //!                            // which can be encoded only using REX prefix)
  //! @endcode
  //!
  //! @note In debug mode you get assertion failure instead of setting error
  //! code.
  ERROR_ILLEGAL_INSTRUCTION = 4,
  //! @brief Illegal addressing used (unencodable).
  ERROR_ILLEGAL_ADDRESING = 5,
  //! @brief Short jump instruction used, but displacement is out of bounds.
  ERROR_ILLEGAL_SHORT_JUMP = 6,

  //! @brief No function defined.
  ERROR_NO_FUNCTION = 7,
  //! @brief Function generation is not finished by using @c Compiler::endFunction()
  //! or something bad happened during generation related to function. This can
  //! be missing emittable, etc...
  ERROR_INCOMPLETE_FUNCTION = 8,

  //! @brief Compiler can't allocate registers, because all of them are used.
  //!
  //! @note AsmJit is able to spill registers so this error really shouldn't
  //! happen unless all registers have priority 0 (which means never spill).
  ERROR_NOT_ENOUGH_REGISTERS = 9,
  //! @brief Compiler can't allocate one register to multiple destinations.
  //!
  //! This error can only happen using special instructions like cmpxchg8b and
  //! others where there are more destination operands (implicit).
  ERROR_REGISTERS_OVERLAP = 10,

  //! @brief Tried to call function using incompatible argument.
  ERROR_INCOMPATIBLE_ARGUMENT = 11,
  //! @brief Incompatible return value.
  ERROR_INCOMPATIBLE_RETURN_VALUE = 12,

  //! @brief Count of error codes by AsmJit. Can grow in future.
  _ERROR_COUNT
};

// ============================================================================
// [AsmJit::OPERAND_TYPE]
// ============================================================================

//! @brief Operand types that can be encoded in @c Op operand.
enum OPERAND_TYPE
{
  //! @brief Operand is none, used only internally (not initialized Operand).
  //!
  //! This operand is not valid.
  OPERAND_NONE = 0x00,

  //! @brief Operand is register.
  OPERAND_REG = 0x01,
  //! @brief Operand is memory.
  OPERAND_MEM = 0x02,
  //! @brief Operand is immediate.
  OPERAND_IMM = 0x04,
  //! @brief Operand is label.
  OPERAND_LABEL = 0x08,
  //! @brief Operand is variable.
  OPERAND_VAR = 0x10
};

// ============================================================================
// [AsmJit::OPERAND_MEM_TYPE]
// ============================================================================

//! @brief Type of memory operand.
enum OPERAND_MEM_TYPE
{
  //! @brief Operand is combination of register(s) and displacement (native).
  OPERAND_MEM_NATIVE = 0,
  //! @brief Operand is label.
  OPERAND_MEM_LABEL = 1,
  //! @brief Operand is absolute memory location (supported mainly in 32-bit 
  //! mode)
  OPERAND_MEM_ABSOLUTE = 2,
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

  //! @brief Emit hints added to jcc() instructions.
  //!
  //! Default: @c true.
  //!
  //! @note This is X86/X64 property only.
  PROPERTY_JUMP_HINTS = 1
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
// [EMITTABLE_TYPE]
// ============================================================================

//! @brief Emmitable type.
//!
//! For each emittable that is used by @c Compiler must be defined it's type.
//! Compiler can optimize instruction stream by analyzing emittables and each
//! type is hint for it. The most used emittables are instructions
//! (@c EMITTABLE_INSTRUCTION).
enum EMITTABLE_TYPE
{
  //! @brief Emittable is invalid (can't be used).
  EMITTABLE_NONE = 0,
  //! @brief Emittable is dummy (used as a mark) (@ref EDummy).
  EMITTABLE_DUMMY,
  //! @brief Emittable is comment (no code) (@ref EComment).
  EMITTABLE_COMMENT,
  //! @brief Emittable is embedded data (@ref EData).
  EMITTABLE_EMBEDDED_DATA,
  //! @brief Emittable is .align directive (@ref EAlign).
  EMITTABLE_ALIGN,
  //! @brief Emittable is variable hint (alloc, spill, use, unuse, ...) (@ref EVariableHint).
  EMITTABLE_VARIABLE_HINT,
  //! @brief Emittable is single instruction (@ref EInstruction).
  EMITTABLE_INSTRUCTION,
  //! @brief Emittable is block of instructions.
  EMITTABLE_BLOCK,
  //! @brief Emittable is function declaration (@ref EFunction).
  EMITTABLE_FUNCTION,
  //! @brief Emittable is function prolog (@ref EProlog).
  EMITTABLE_PROLOG,
  //! @brief Emittable is function epilog (@ref EEpilog).
  EMITTABLE_EPILOG,
  //! @brief Emittable is end of function (@ref EFunctionEnd).
  EMITTABLE_FUNCTION_END,
  //! @brief Emittable is target (bound label).
  EMITTABLE_TARGET,
  //! @brief Emittable is jump table (@ref EJmp).
  EMITTABLE_JUMP_TABLE,
  //! @brief Emittable is function call (@ref ECall).
  EMITTABLE_CALL,
  //! @brief Emittable is return (@ref ERet).
  EMITTABLE_RET
};

// ============================================================================
// [AsmJit::VARIABLE_STATE]
// ============================================================================

//! @brief State of variable.
//!
//! @note State of variable is used only during make process and it's not
//! visible to the developer.
enum VARIABLE_STATE
{
  //! @brief Variable is currently not used.
  VARIABLE_STATE_UNUSED = 0,

  //! @brief Variable is in register.
  //!
  //! Variable is currently allocated in register.
  VARIABLE_STATE_REGISTER = 1,

  //! @brief Variable is in memory location or spilled.
  //!
  //! Variable was spilled from register to memory or variable is used for
  //! memory only storage.
  VARIABLE_STATE_MEMORY = 2
};

// ============================================================================
// [AsmJit::VARIABLE_ALLOC_FLAGS]
// ============================================================================

//! @brief Variable alloc mode.
enum VARIABLE_ALLOC
{
  //! @brief Variable can be allocated in register.
  VARIABLE_ALLOC_REGISTER = 0x10,
  //! @brief Variable can be allocated in memory.
  VARIABLE_ALLOC_MEMORY = 0x20,

  //! @brief Allocating variable to read only.
  //!
  //! Read only variables are used to optimize variable spilling. If variable
  //! is some time ago deallocated and it's not marked as changed (so it was
  //! all the life time read only) then spill is simply NOP (no mov instruction
  //! is generated to move it to it's home memory location).
  VARIABLE_ALLOC_READ = 0x1,

  //! @brief Allocating variable to write only (overwrite).
  //!
  //! Overwriting means that if variable is in memory, there is no generated
  //! instruction to move variable from memory to register, because that
  //! register will be overwritten by next instruction. This is used as a
  //! simple optimization to improve generated code by @c Compiler.
  VARIABLE_ALLOC_WRITE = 0x2,

  //! @brief Allocating variable to read / write.
  //!
  //! Variable allocated for read / write is marked as changed. This means that
  //! if variable must be later spilled into memory, mov (or similar)
  //! instruction will be generated.
  VARIABLE_ALLOC_READWRITE = 0x3
};

// ============================================================================
// [AsmJit::VARIABLE_ALLOC_POLICY]
// ============================================================================

//! @brief Variable allocation method.
//!
//! Variable allocation method is used by compiler and it means if compiler
//! should first allocate preserved registers or not. Preserved registers are
//! registers that must be saved / restored by generated function.
//!
//! This option is for people who are calling C/C++ functions from JIT code so
//! Compiler can recude generating push/pop sequences before and after call,
//! respectively.
enum VARIABLE_ALLOC_POLICY
{
  //! @brief Allocate preserved registers first.
  VARIABLE_ALLOC_PRESERVED_FIRST,
  //! @brief Allocate preserved registers last (default).
  VARIABLE_ALLOC_PRESERVED_LAST
};

// ============================================================================
// [AsmJit::FUNCTION_HINT]
// ============================================================================

//! @brief Function hints.
enum FUNCTION_HINT
{
  //! @brief Use push/pop sequences instead of mov sequences in function prolog
  //! and epilog.
  FUNCTION_HINT_PUSH_POP_SEQUENCE = 0,
  //! @brief Make naked function (without using ebp/erp in prolog / epilog).
  FUNCTION_HINT_NAKED = 1,
  //! @brief Add emms instruction to the function epilog.
  FUNCTION_HINT_EMMS = 2,
  //! @brief Add sfence instruction to the function epilog.
  FUNCTION_HINT_SFENCE = 3,
  //! @brief Add lfence instruction to the function epilog.
  FUNCTION_HINT_LFENCE = 4
};

// ============================================================================
// [AsmJit::ARGUMENT_DIR]
// ============================================================================

//! @brief Arguments direction used by @c Function.
enum ARGUMENT_DIR
{
  //! @brief Arguments are passed left to right.
  //!
  //! This arguments direction is unusual to C programming, it's used by pascal
  //! compilers and in some calling conventions by Borland compiler).
  ARGUMENT_DIR_LEFT_TO_RIGHT = 0,
  //! @brief Arguments are passer right ro left
  //!
  //! This is default argument direction in C programming.
  ARGUMENT_DIR_RIGHT_TO_LEFT = 1
};

// ============================================================================
// [AsmJit::Constants]
// ============================================================================

enum {
  //! @brief Invalid operand identifier.
  INVALID_VALUE = 0xFFFFFFFF,

  //! @brief Operand id value mask (part used for IDs).
  OPERAND_ID_VALUE_MASK = 0x3FFFFFFF,
  //! @brief Operand id type mask (part used for operand type).
  OPERAND_ID_TYPE_MASK  = 0xC0000000,
  //! @brief Label operand mark id.
  OPERAND_ID_TYPE_LABEL = 0x40000000,
  //! @brief Variable operand mark id.
  OPERAND_ID_TYPE_VAR   = 0x80000000,
};

enum {
  //! @brief Maximum allowed arguments per function declaration / call.
  FUNC_MAX_ARGS = 32
};

// ============================================================================
// [AsmJit::API]
// ============================================================================

//! @brief Translates error code (see @c ERROR_CODE) into text representation.
ASMJIT_API const char* getErrorCodeAsString(uint32_t error) ASMJIT_NOTHROW;

//! @}

} // AsmJit namespace

// ============================================================================
// [Platform Specific]
//
// Following enums must be declared by platform specific header:
// - CALL_CONV - Calling convention.
// - VARIABLE_TYPE - Variable type.
// ============================================================================

#if defined(ASMJIT_X86) || defined(ASMJIT_X64)
#include "DefsX86X64.h"
#endif // ASMJIT_X86 || ASMJIT_X64

// [Guard]
#endif // _ASMJIT_DEFS_H
