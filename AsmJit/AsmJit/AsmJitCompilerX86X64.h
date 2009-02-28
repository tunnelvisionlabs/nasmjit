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
#ifndef _ASMJITCOMPILERX86X64_H
#define _ASMJITCOMPILERX86X64_H

// [Dependencies]
#include "AsmJitConfig.h"

#include "AsmJitAssembler.h"
#include "AsmJitSerializer.h"
#include "AsmJitUtil.h"

#include <string.h>

// a little bit C++
#include <new>

namespace AsmJit {

// forward declarations
struct Comment;
struct Compiler;
struct Emittable;
struct Epilog;
struct Function;
struct Instruction;
struct Prolog;
struct Variable;

//! @addtogroup AsmJit_Compiler
//! @{

// ============================================================================
// [Constants]
// ============================================================================

//! @brief Emmitable type.
enum EMITTABLE_TYPE
{
  //! @brief Emittable is invalid (can't be used).
  EMITTABLE_NONE = 0,
  //! @brief Emittable is comment (no code).
  EMITTABLE_COMMENT,
  //! @brief Emittable is .align directive.
  EMITTABLE_ALIGN,
  //! @brief Emittable is single instruction.
  EMITTABLE_INSTRUCTION,
  //! @brief Emittable is block of instructions.
  EMITTABLE_BLOCK,
  //! @brief Emittable is function declaration.
  EMITTABLE_FUNCTION,
  //! @brief Emittable is function prolog.
  EMITTABLE_PROLOGUE,
  //! @brief Emittable is function epilog.
  EMITTABLE_EPILOGUE,
  //! @brief Emittable is target (bound label).
  EMITTABLE_TARGET
};

//! @brief Calling convention type.
enum CALL_CONV
{
  //! @brief Calling convention is invalid (can't be used).
  CALL_CONV_NONE = 0,

  // [X86 Calling Conventions]

  //! @brief Cdecl calling convention (used by C runtime).
  //!
  //! Compatible across MSVC and GCC.
  //!
  //! Arguments direction:
  //! - Right to Left
  //!
  //! Stack is cleaned by:
  //! - Caller.
  CALL_CONV_CDECL = 1,

  //! @brief Stdcall calling convention (used by WinAPI).
  //!
  //! Compatible across MSVC and GCC.
  //!
  //! Arguments direction:
  //! - Right to Left
  //!
  //! Stack is cleaned by:
  //! - Callee.
  //!
  //! Return value:
  //! - Integer types - EAX:EDX registers.
  //! - Floating points - st(0) register.
  CALL_CONV_STDCALL = 2,

  //! @brief MSVC specific calling convention used by MSVC/Intel compilers
  //! for struct/class methods.
  //!
  //! This is MSVC (and Intel) only calling convention used in Windows
  //! world for C++ class methods. Implicit 'this' pointer is stored in
  //! ECX register instead of storing it on the stack.
  //!
  //! Arguments direction:
  //! - Right to Left (except this pointer in ECX)
  //!
  //! Stack is cleaned by:
  //! - Callee.
  //!
  //! Return value:
  //! - Integer types - EAX:EDX registers.
  //! - Floating points - st(0) register.
  //!
  //! C++ class methods that have variable count of arguments uses different
  //! calling convention called cdecl.
  //!
  //! @note This calling convention is always used by MSVC for class methods,
  //! it's implicit and there is no way how to override it.
  CALL_CONV_MSTHISCALL = 3,

  //! @brief MSVC specific fastcall.
  //!
  //! Two first parameters (evaluated from left-to-right) are in ECX:EDX 
  //! registers, all others on the stack in right-to-left order.
  //!
  //! Arguments direction:
  //! - Right to Left (except to first two integer arguments in ECX:EDX)
  //!
  //! Stack is cleaned by:
  //! - Callee.
  //!
  //! Return value:
  //! - Integer types - EAX:EDX registers.
  //! - Floating points - st(0) register.
  //!
  //! @note This calling convention differs to GCC one in stack cleaning
  //! mechanism.
  CALL_CONV_MSFASTCALL = 4,

  //! @brief Borland specific fastcall with 2 parameters in registers.
  //!
  //! Two first parameters (evaluated from left-to-right) are in ECX:EDX 
  //! registers, all others on the stack in left-to-right order.
  //!
  //! Arguments direction:
  //! - Left to Right (except to first two integer arguments in ECX:EDX)
  //!
  //! Stack is cleaned by:
  //! - Callee.
  //!
  //! Return value:
  //! - Integer types - EAX:EDX registers.
  //! - Floating points - st(0) register.
  //!
  //! @note Arguments on the stack are in left-to-right order that differs
  //! to other fastcall conventions used in different compilers.
  CALL_CONV_BORLANDFASTCALL = 5,

  //! @brief GCC specific fastcall with 2 parameters in registers.
  //!
  //! Two first parameters (evaluated from left-to-right) are in ECX:EDX 
  //! registers, all others on the stack in right-to-left order.
  //!
  //! Arguments direction:
  //! - Right to Left (except to first two integer arguments in ECX:EDX)
  //!
  //! Stack is cleaned by:
  //! - Caller.
  //!
  //! Return value:
  //! - Integer types - EAX:EDX registers.
  //! - Floating points - st(0) register.
  //!
  //! @note This calling convention differs to MSVC one in stack cleaning
  //! mechanism.
  CALL_CONV_GCCFASTCALL_2 = 6,

  //! @brief GCC specific fastcall with 3 parameters in registers.
  //!
  //! Three first parameters (evaluated from left-to-right) are in 
  //! ECX:EDX:EAX registers, all others on the stack in right-to-left order.
  //!
  //! Arguments direction:
  //! - Right to Left (except to first three integer arguments in ECX:EDX:EAX)
  //!
  //! Stack is cleaned by:
  //! - Caller.
  //!
  //! Return value:
  //! - Integer types - EAX:EDX registers.
  //! - Floating points - st(0) register.
  CALL_CONV_GCCFASTCALL_3 = 7,

  // [X64 Calling Conventions]

  //! @brief X64 calling convention for Windows platform.
  //!
  //! TODOC
  //!
  //! Arguments direction:
  //! - Right to Left (except for first 4 parameters that's in registers)
  //!
  //! Stack is cleaned by:
  //! - Caller.
  //!
  //! Return value:
  //! - Integer types - RAX register.
  //! - Floating points - XMM0 register.
  //!
  //! Stack is always aligned by 16 bytes.
  //!
  //! More informations about this calling convention can be found on MSDN:
  //! http://msdn.microsoft.com/en-us/library/9b372w95.aspx .
  CALL_CONV_X64W = 16,

  //! @brief X64 calling convention for Unix platforms.
  //!
  //! TODOC
  //!
  //! Arguments direction:
  //! - Right to Left
  //!
  //! Stack is cleaned by:
  //! - Caller.
  //!
  //! Return value:
  //! - Integer types - RAX register.
  //! - Floating points - XMM0 register.
  //!
  //! Stack is always aligned by 16 bytes.
  CALL_CONV_X64U = 17,

  // [Preferred Calling Convention]

  //! @def CALL_CONV_DEFAULT
  //! @brief Default calling convention for current platform / operating 
  //! system.

#if defined(ASMJIT_X86)
  CALL_CONV_DEFAULT = CALL_CONV_CDECL
#else
# if defined(WIN32) || defined(_WIN32) || defined(WINDOWS)
  CALL_CONV_DEFAULT = CALL_CONV_X64W
# else
  CALL_CONV_DEFAULT = CALL_CONV_X64U
# endif
#endif // ASMJIT_X86
};

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

//! @brief Argument type.
enum VARIABLE_TYPE
{
  //! @brief Invalid variable type (don't use).
  VARIABLE_TYPE_NONE = 0,

  //! @brief Variable is integer (Int32).
  VARIABLE_TYPE_INT32 = 1,
  //! @brief Variable is unsigned integer (UInt32).
  VARIABLE_TYPE_UINT32 = 1,

  //! @def VARIABLE_TYPE_SYSINT
  //! @brief Variable is system wide integer (Int32 or Int64).
  //! @def VARIABLE_TYPE_SYSUINT
  //! @brief Variable is system wide unsigned integer (UInt32 or UInt64).
#if defined(ASMJIT_X86)
  VARIABLE_TYPE_SYSINT = VARIABLE_TYPE_INT32,
  VARIABLE_TYPE_SYSUINT = VARIABLE_TYPE_UINT32,
#else
  VARIABLE_TYPE_INT64 = 2,
  VARIABLE_TYPE_UINT64 = 2,
  VARIABLE_TYPE_SYSINT = VARIABLE_TYPE_INT32,
  VARIABLE_TYPE_SYSUINT = VARIABLE_TYPE_UINT32,
#endif

  //! @brief Variable is pointer or reference to memory (or any type).
  VARIABLE_TYPE_PTR = VARIABLE_TYPE_SYSUINT,

  //! @brief Variable is SP-FP number (float).
  VARIABLE_TYPE_FLOAT = 3,
  //! @brief Variable is DP-FP number (double).
  VARIABLE_TYPE_DOUBLE = 4,

  //! @brief Variable is MM register / memory location.
  VARIABLE_TYPE_MM = 5,
  //! @brief Variable is XMM register / memory location.
  VARIABLE_TYPE_XMM = 6,

  //! @brief Count of variable types.
  _VARIABLE_TYPE_COUNT
};

//! @brief State of variable.
enum VARIABLE_STATE
{
  //! @brief Variable is currently not used.
  VARIABLE_STATE_UNUSED = 0,
  //! @brief Variable is in register.
  VARIABLE_STATE_REGISTER = 1,
  //! @brief Variable is in memory location or spilled.
  VARIABLE_STATE_MEMORY = 2
};

//! @brief Variable alloc mode.
enum VARIABLE_ALLOC
{
  VARIABLE_ALLOC_READ = 0x1,
  VARIABLE_ALLOC_WRITE = 0x2,
  VARIABLE_ALLOC_READWRITE = 0x3
};

// ============================================================================
// [AsmJit::Variable]
// ============================================================================

//! @brief Variable.
struct ASMJIT_API Variable
{
  Variable(Compiler* c, Function* f, UInt8 type);
  virtual ~Variable();

  //! @brief Return compiler that owns this variable.
  inline Compiler* compiler() const { return _compiler; }
  //! @brief Return function that owns this variable.
  inline Function* function() const { return _function; }

  //! @brief Return reference count.
  inline SysUInt refCount() const { return _refCount; }
  //! @brief Return spill count.
  inline SysUInt spillCount() const { return _spillCount; }
  //! @brief Return reuse count.
  inline SysUInt reuseCount() const { return _reuseCount; }
  //! @brief Return register access statistics.
  inline SysUInt registerAccessCount() const { return _registerAccessCount; }
  //! @brief Return memory access statistics.
  inline SysUInt memoryAccessCount() const { return _memoryAccessCount; }

  //! @brief Return variable type, see @c VARIABLE_TYPE.
  inline UInt8 type() const { return _type; }
  //! @brief Return variable size (in bytes).
  inline UInt8 size() const { return _size; }
  //! @brief Return variable state, see @c VARIABLE_STATE.
  inline UInt8 state() const { return _state; }
  //! @brief Return variable priority.
  //!
  //! Variable priority is used for spilling. Lower number means less chance
  //! to spill. Zero means that variable can't be never spilled.
  inline UInt8 priority() const { return _priority; }
  //! @brief Return variable register code (where it now lives), or NO_REG if
  //! it's only in memory (spilled).
  inline UInt8 registerCode() const { return _registerCode; }
  //! @brief Return variable preferred register.
  inline UInt8 preferredRegister() const { return _preferredRegister; }

  //! @brief Return variable changed state.
  inline UInt8 changed() const { return _changed; }
  
  //! @brief Return variable stack offset.
  //!
  //! @note Stack offsets can be changed by Compiler, don't use this 
  //! to generate memory operands.
  inline SysInt stackOffset() const { return _stackOffset; }

  //! @brief Set variable priority.
  inline void setPriority(UInt8 priority) { _priority = priority; }
  //! @brief Set varialbe preferred register.
  inline void setPreferredRegister(UInt8 code) { _preferredRegister = code; }
  //! @brief Set variable changed state.
  inline void setChanged(UInt8 changed) { _changed = changed; }

  //! @brief Memory operand that will be always pointed to variable memory address. */
  inline const Mem& memoryOperand() const { return *_memoryOperand; }

  // reference counting
  Variable* ref();
  void deref();

  // code generation
  inline void alloc(UInt8 mode = VARIABLE_ALLOC_READWRITE);
  inline void spill();
  inline void unuse();

private:
  //! @brief Set variable stack offset.
  //! @internal
  inline void setStackOffset(SysInt stackOffset) { _stackOffset = stackOffset; }

  //! @brief Set most members by one shot.
  inline void setAll(
    UInt8 type, UInt8 size, UInt8 state, UInt8 priority, 
    UInt8 registerCode, UInt8 preferredRegister,
    SysInt stackOffset)
  {
    _type = type;
    _size = size;
    _state = state;
    _priority = priority;
    _registerCode = registerCode;
    _preferredRegister = preferredRegister;
    _stackOffset = stackOffset;
  }

  //! @brief Compiler that owns this variable.
  Compiler* _compiler;
  //! @brief Function that owns this variable.
  Function* _function;

  //! @brief Reference count.
  SysUInt _refCount;
  //! @brief How many times was variable spilled.
  SysUInt _spillCount;
  //! @brief How many times was variable reused.
  SysUInt _reuseCount;
  //! @brief Register access count.
  SysUInt _registerAccessCount;
  //! @brief Memory access count.
  SysUInt _memoryAccessCount;

  //! @brief Variable type, see @c VARIABLE_TYPE.
  UInt8 _type;
  //! @brief Variable size (in bytes).
  UInt8 _size;
  //! @brief Variable state, see @c VARIABLE_STATE.
  UInt8 _state;
  //! @brief Variable priority.
  UInt8 _priority;

  //! @brief Register code if variable state is @c VARIABLE_STATE_REGISTER.
  UInt8 _registerCode;
  //! @brief Default register where to alloc variable.
  UInt8 _preferredRegister;

  //! @brief true if variable in register was changed and when spilling it 
  //! needs to be copied into memory location.
  UInt8 _changed;

  //! @brief Reserved for future use.
  UInt8 _reserved;

  //! @brief Stack location.
  SysInt _stackOffset;

  //! @brief Variable memory operand.
  Mem* _memoryOperand;

  friend struct Compiler;
  friend struct Function;

  // disable copy
  inline Variable(const Variable& other);
  inline Variable& operator=(const Variable& other);

  friend struct VariableRef;
};

// ============================================================================
// [AsmJit::VariableXXXRef]
// ============================================================================

//! @brief Base class for variable references.
//!
//! @c VariableRef class is designed to manage @c Variable instances. It's 
//! based on reference counting and if reference gets to zero (in destructor), 
//! variable is freed by compiler.
//!
//! @note Compiler can reuse existing variables.
struct VariableRef
{
  inline VariableRef() : _v(NULL) {}
  inline VariableRef(Variable* v) : _v(v->ref()) {}
  inline ~VariableRef() { if (_v) _v->deref(); }

  inline Variable* v() { return _v; }

  //! @brief Return variable type, see @c VARIABLE_TYPE.
  inline UInt8 type() const { ASMJIT_ASSERT(_v); return _v->type(); }
  //! @brief Return variable size (in bytes).
  inline UInt8 size() const { ASMJIT_ASSERT(_v); return _v->size(); }
  //! @brief Return variable state, see @c VARIABLE_STATE.
  inline UInt8 state() const { ASMJIT_ASSERT(_v); return _v->state(); }

  inline void use(Variable* v) { ASMJIT_ASSERT(_v == NULL); _v = v->ref(); }

  //! @brief Allocate variable to register.
  inline void alloc(UInt8 mode = VARIABLE_ALLOC_READWRITE)
  { ASMJIT_ASSERT(_v); _v->alloc(mode); }

  //! @brief Spill variable from register (or do nothing).
  inline void spill() { ASMJIT_ASSERT(_v); _v->spill(); }

  //! @brief Unuse variable (all changes lost).
  inline void unuse() { ASMJIT_ASSERT(_v); _v->unuse(); }

  //! @brief Destroy variable (@c VariableRef can't be used anymore after destroy).
  inline void destroy() { ASMJIT_ASSERT(_v); _v->deref(); _v = NULL; }

  inline UInt8 preferredRegister() const { ASMJIT_ASSERT(_v); return _v->preferredRegister(); }
  inline void setPreferredRegister(UInt8 code) { ASMJIT_ASSERT(_v); _v->setPreferredRegister(code); }

  //! @brief Return variable priority.
  inline UInt8 priority() const { ASMJIT_ASSERT(_v); return _v->priority(); }
  //! @brief Set variable priority.
  inline void setPriority(UInt8 priority) { ASMJIT_ASSERT(_v); _v->setPriority(priority); }

  //! @brief Return if variable changed state.
  inline UInt8 changed() const { ASMJIT_ASSERT(_v); return _v->changed(); }
  //! @brief Set variable changed state.
  inline void setChanged(UInt8 changed) { ASMJIT_ASSERT(_v); _v->setChanged(changed); }

  //! @brief Return memory address operand.
  //!
  //! @note Getting memory address operand will always call @c spill().
  inline const Mem& m() const { ASMJIT_ASSERT(_v); _v->spill(); return *_v->_memoryOperand; }

protected:
  Variable* _v;
};

//! @brief 32 bit integer reference.
struct Int32Ref : public VariableRef
{
  inline Int32Ref() : VariableRef() {}
  inline Int32Ref(Variable* v) : VariableRef(v) {}

  inline Register r  () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
  inline Register r8 () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpb(_v->registerCode() & REGCODE_MASK); }
  inline Register r16() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpw(_v->registerCode() & REGCODE_MASK); }
  inline Register r32() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
#if defined(ASMJIT_X64)
  inline Register r64() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
#endif // ASMJIT_X64

  inline Register c  () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
  inline Register c8 () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpb(_v->registerCode() & REGCODE_MASK); }
  inline Register c16() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpw(_v->registerCode() & REGCODE_MASK); }
  inline Register c32() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
#if defined(ASMJIT_X64)
  inline Register c64() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
#endif // ASMJIT_X64

  inline Register x  () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
  inline Register x8 () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpb(_v->registerCode() & REGCODE_MASK); }
  inline Register x16() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpw(_v->registerCode() & REGCODE_MASK); }
  inline Register x32() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
#if defined(ASMJIT_X64)
  inline Register x64() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
#endif // ASMJIT_X64
};

#if defined(ASMJIT_X64)
//! @brief 64 bit integer reference.
struct Int64Ref : public VariableRef
{
  inline Int64Ref() : VariableRef() {}
  inline Int64Ref(Variable* v) : VariableRef(v) {}

  inline Register r  () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
  inline Register r8 () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpb(_v->registerCode() & REGCODE_MASK); }
  inline Register r16() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpw(_v->registerCode() & REGCODE_MASK); }
  inline Register r32() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
  inline Register r64() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_gpq(_v->registerCode() & REGCODE_MASK); }

  inline Register c  () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
  inline Register c8 () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpb(_v->registerCode() & REGCODE_MASK); }
  inline Register c16() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpw(_v->registerCode() & REGCODE_MASK); }
  inline Register c32() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
  inline Register c64() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_gpq(_v->registerCode() & REGCODE_MASK); }

  inline Register x  () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
  inline Register x8 () const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpb(_v->registerCode() & REGCODE_MASK); }
  inline Register x16() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpw(_v->registerCode() & REGCODE_MASK); }
  inline Register x32() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpd(_v->registerCode() & REGCODE_MASK); }
  inline Register x64() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_gpq(_v->registerCode() & REGCODE_MASK); }
};
#endif // ASMJIT_X64

//! @brief MM reference.
struct MMRef : public VariableRef
{
  inline MMRef() : VariableRef() {}
  inline MMRef(Variable* v) : VariableRef(v) {}

  inline MMRegister r() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_mm(_v->registerCode()); }
  inline MMRegister c() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_mm(_v->registerCode()); }
  inline MMRegister x() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_mm(_v->registerCode()); }
};

//! @brief XMM reference.
struct XMMRef : public VariableRef
{
  inline XMMRef() : VariableRef() {}
  inline XMMRef(Variable* v) : VariableRef(v) {}

  inline XMMRegister r() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_xmm(_v->registerCode()); }
  inline XMMRegister c() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_xmm(_v->registerCode()); }
  inline XMMRegister x() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_xmm(_v->registerCode()); }
};

#if defined(ASMJIT_X86)
typedef Int32Ref SysIntRef;
#else
typedef Int64Ref SysIntRef;
#endif

typedef SysIntRef PtrRef;

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

//! @brief Emmitable.
//!
//! Emittable is object that can emit single or more instructions. To
//! createyour interface it's needed to override virtual method @c emit().
struct ASMJIT_API Emittable
{
  Emittable(Compiler* c, UInt32 type);
  virtual ~Emittable();

  //! @brief Prepare for emitting (optional).
  virtual void prepare();
  //! @brief Emit instruction stream.
  virtual void emit(Assembler& a) = 0;
  //! @brief Post emit (optional).
  virtual void postEmit(Assembler& a);

  //! @brief Return compiler instance where this emittable is connected to.
  inline Compiler* compiler() const { return _compiler; }
  //! @brief Return emittable type, see @c EMITTABLE_TYPE.
  inline UInt32 type() const { return _type; }

protected:
  //! @brief Compiler where this emittable is connected to.
  Compiler* _compiler;
  //! @brief Type of emittable, see @c EMITTABLE_TYPE.
  UInt32 _type;

private:
  // disable copy
  inline Emittable(const Emittable& other);
  inline Emittable& operator=(const Emittable& other);
};

// ============================================================================
// [AsmJit::Comment]
// ============================================================================

struct ASMJIT_API Comment : public Emittable
{
  Comment(Compiler* c, const char* str);
  virtual ~Comment();

  virtual void emit(Assembler& a);

  inline const char* str() const { return _str; }

private:
  const char* _str;
};

// ============================================================================
// [AsmJit::Align]
// ============================================================================

struct ASMJIT_API Align : public Emittable
{
  Align(Compiler* c, SysInt size = 0);
  virtual ~Align();

  virtual void emit(Assembler& a);

  inline SysInt size() const { return _size; }
  inline void setSize(SysInt size) { _size = size; }

private:
  SysInt _size;
};

// ============================================================================
// [AsmJit::Instruction]
// ============================================================================

//! @brief Instruction emittable.
struct ASMJIT_API Instruction : public Emittable
{
  Instruction(Compiler* c);
  Instruction(Compiler* c, UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);
  virtual ~Instruction();

  virtual void emit(Assembler& a);

  //! @brief Return instruction code, see @c INST_CODE.
  inline UInt32 code() const { return _code; }

  //! @brief Return array of operands (3 operands total).
  inline Operand* const * ops() { return _o; }

  //! @brief Return first instruction operand.
  inline Operand* o1() const { return _o[0]; }

  //! @brief Return second instruction operand.
  inline Operand* o2() const { return _o[1]; }

  //! @brief Return third instruction operand.
  inline Operand* o3() const { return _o[2]; }

  //! @brief Set instruction code.
  //!
  //! Please do not modify instruction code if you are not know what you are 
  //! doing. Incorrect instruction code or operands can assert() in runtime.
  inline void setCode(UInt32 code) { _code = code; }

private:
  //! @brief Instruction code, see @c INST_CODE.
  UInt32 _code;
  //! @brief Instruction operands.
  Operand *_o[3];
  //! @brief Static array for instruction operands (cache)
  Operand _ocache[3];
};

// ============================================================================
// [AsmJit::TypeAsId]
// ============================================================================

template<typename T>
struct TypeAsId {};

template<typename T>
struct TypeAsId<T*> { enum { Id = VARIABLE_TYPE_PTR }; };

#define __DECLARE_TYPE_AS_ID(__T__, __Id__) \
template<> \
struct TypeAsId<__T__> { enum { Id = __Id__ }; }

__DECLARE_TYPE_AS_ID(Int32, VARIABLE_TYPE_INT32);
__DECLARE_TYPE_AS_ID(UInt32, VARIABLE_TYPE_UINT32);
#if defined(ASMJIT_X64)
__DECLARE_TYPE_AS_ID(Int64, VARIABLE_TYPE_INT64);
__DECLARE_TYPE_AS_ID(UInt64, VARIABLE_TYPE_UINT64);
#endif // ASMJIT_X64
__DECLARE_TYPE_AS_ID(float, VARIABLE_TYPE_FLOAT);
__DECLARE_TYPE_AS_ID(double, VARIABLE_TYPE_DOUBLE);

#undef __DECLARE_TYPE_AS_ID

// ============================================================================
// [AsmJit::Function Builder]
// ============================================================================

struct BuildFunction0
{
  inline const UInt32* args() const { return NULL; }
  inline SysUInt count() const { return 0; }
};

template<typename P0>
struct BuildFunction1
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id }; return data; }
  inline SysUInt count() const { return 1; }
};

template<typename P0, typename P1>
struct BuildFunction2
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id }; return data; }
  inline SysUInt count() const { return 2; }
};

template<typename P0, typename P1, typename P2>
struct BuildFunction3
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id }; return data; }
  inline SysUInt count() const { return 3; }
};

template<typename P0, typename P1, typename P2, typename P3>
struct BuildFunction4
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id }; return data; }
  inline SysUInt count() const { return 4; }
};

template<typename P0, typename P1, typename P2, typename P3, typename P4>
struct BuildFunction5
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id }; return data; }
  inline SysUInt count() const { return 5; }
};

template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
struct BuildFunction6
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id, TypeAsId<P5>::Id }; return data; }
  inline SysUInt count() const { return 6; }
};

// ============================================================================
// [AsmJit::Function]
// ============================================================================

//! @brief Function emittable.
struct ASMJIT_API Function : public Emittable
{
  Function(Compiler* c);
  virtual ~Function();

  virtual void prepare();
  virtual void emit(Assembler& a);

  // --------------------------------------------------------------------------
  // [Calling Convention / Function Arguments]
  // --------------------------------------------------------------------------

  //! @brief Set function prototype.
  //!
  //! This will set function calling convention and setup arguments variables.
  void setPrototype(UInt32 cconv, const UInt32* args, SysUInt count);

  //! @brief Set naked function to true or false (naked means no prolog / epilog code).
  void setNaked(UInt8 naked);

  //! @brief Enable or disable emms instruction in epilog.
  inline void setAllocableEbp(UInt8 aebp) { _allocableEbp = aebp; }

  //! @brief Enable or disable emms instruction in epilog.
  inline void setEmms(UInt8 emms) { _emms = emms; }

  //! @brief Return function calling convention, see @c CALL_CONV.
  inline UInt32 cconv() const { return _cconv; }

  //! @brief Return @c true if callee pops the stack by ret() instruction.
  //!
  //! Stdcall calling convention is designed to pop the stack by callee,
  //! but all calling conventions used in MSVC extept cdecl does that.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt8 calleePopsStack() const { return _calleePopsStack; }

  //! @brief Return @c true if function is naked (no prolog / epilog code).
  inline UInt8 naked() const { return _naked; }

  //! @brief Return @c true if EBP/RBP register can be allocated by register allocator.
  inline UInt8 allocableEbp() const { return _allocableEbp; }

  //! @brief Return @c true if function epilog contains emms instruction.
  inline UInt8 emms() const { return _emms; }

  //! @brief Return direction of arguments passed on the stack.
  //!
  //! Direction should be always @c ARGUMENT_DIR_RIGHT_TO_LEFT.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 cconvArgumentsDirection() const { return _cconvArgumentsDirection; }

  //! @brief Return registers used to pass first integer parameters by current
  //! calling convention.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline const UInt32* cconvArgumentsGp() const { return _cconvArgumentsGp; }

  //! @brief Return registers used to pass first SP-FP or DP-FPparameters by 
  //! current calling convention.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline const UInt32* cconvArgumentsXmm() const { return _cconvArgumentsXmm; }

  //! @brief Return bitmask of general purpose registers that's preserved 
  //! (non-volatile).
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 cconvPreservedGp() const { return _cconvPreservedGp; }
  //! @brief Return bitmask of sse registers that's preserved (non-volatile).
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 cconvPreservedXmm() const { return _cconvPreservedXmm; }

  // --------------------------------------------------------------------------
  // [Registers allocator / Variables]
  // --------------------------------------------------------------------------

  //! @brief Return argument at @a i.
  inline Variable* argument(SysInt i)
  {
    ASMJIT_ASSERT((SysUInt)i < _argumentsCount);
    return _variables[i];
  }

  //! @brief Create new variable
  Variable* newVariable(UInt8 type, UInt8 priority = 10, UInt8 preferredRegister = NO_REG);

  void alloc(Variable& v, UInt8 mode = VARIABLE_ALLOC_READWRITE);
  void spill(Variable& v);
  void unuse(Variable& v);

  Variable* _getSpillCandidate(UInt8 type);

  void _allocReg(UInt8 code);
  void _freeReg(UInt8 code);

  //! @brief Return size of alignment on the stack.
  //!
  //! Stack is aligned to 16 bytes by default. For X64 platforms there is 
  //! no extra code needed to align stack to 16 bytes, because it's default
  //! stack alignment.
  inline SysInt maxAlignmentStackSize() const { return _maxAlignmentStackSize; }

  inline SysInt prologEpilogStackSize() const { return _prologEpilogStackSize; }

  //! @brief Return size of variables on the stack.
  //!
  //! This variable is always aligned to 16 bytes for each platform.
  inline SysInt variablesStackSize() const { return _variablesStackSize; }

  //! @brief Return count of arguments.
  inline UInt32 argumentsCount() const { return _argumentsCount; }

  //! @brief Return stack size of all function arguments (passed on the 
  //! stack).
  inline UInt32 argumentsStackSize() const { return _argumentsStackSize; }

  //! @brief Return bitmask of all used (for actual context) general purpose registers.
  inline UInt32 usedGpRegisters() const { return _usedGpRegisters; }
  //! @brief Return bitmask of all used (for actual context) mmx registers.
  inline UInt32 usedMmRegisters() const { return _usedMmRegisters; }
  //! @brief Return bitmask of all used (for actual context) sse registers.
  inline UInt32 usedXmmRegisters() const { return _usedXmmRegisters; }

  inline void useGpRegisters(UInt32 mask) { _usedGpRegisters |= mask; }
  inline void useMmRegisters(UInt32 mask) { _usedMmRegisters |= mask; }
  inline void useXmmRegisters(UInt32 mask) { _usedXmmRegisters |= mask; }

  inline void unuseGpRegisters(UInt32 mask) { _usedGpRegisters &= ~mask; }
  inline void unuseMmRegisters(UInt32 mask) { _usedMmRegisters &= ~mask; }
  inline void unuseXmmRegisters(UInt32 mask) { _usedXmmRegisters &= ~mask; }

  //! @brief Return bitmask of all changed general purpose registers during
  //! function execution (for generating optimized prolog / epilog).
  inline UInt32 modifiedGpRegisters() const { return _modifiedGpRegisters; }
  //! @brief Return bitmask of all changed mmx registers during
  //! function execution (for generating optimized prolog / epilog).
  inline UInt32 modifiedMmRegisters() const { return _modifiedMmRegisters; }
  //! @brief Return bitmask of all changed sse registers during
  //! function execution (for generating optimized prolog / epilog).
  inline UInt32 modifiedXmmRegisters() const { return _modifiedXmmRegisters; }

  inline void modifyGpRegisters(UInt32 mask) { _modifiedGpRegisters |= mask; }
  inline void modifyMmRegisters(UInt32 mask) { _modifiedMmRegisters |= mask; }
  inline void modifyXmmRegisters(UInt32 mask) { _modifiedXmmRegisters |= mask; }

  // --------------------------------------------------------------------------
  // [Labels]
  // --------------------------------------------------------------------------

  //! @brief Return function entry label.
  inline Label* entryLabel() const { return _entryLabel; }

  //! @brief Return prolog label (label after function prolog)
  inline Label* prologLabel() const { return _prologLabel; }

  //! @brief Return exit label.
  inline Label* exitLabel() const { return _exitLabel; }

private:
  // --------------------------------------------------------------------------
  // [Calling Convention / Function Arguments]
  // --------------------------------------------------------------------------

  //! @brief Sets function calling convention.
  void _setCallingConvention(UInt32 cconv);

  //! @brief Sets function arguments (must be done after correct calling 
  //! convention is set).
  void _setArguments(const UInt32* args, SysUInt len);

  //! @brief Calling convention, see @c CALL_CONV.
  UInt32 _cconv;

  //! @brief Callee pops stack;
  UInt8 _calleePopsStack;

  //! @brief Generate naked function?
  UInt8 _naked;

  //! @brief Whether EBP/RBP register can be used by register allocator.
  UInt8 _allocableEbp;

  //! @brief Generate emms instruction at the end of function.
  UInt8 _emms;

  //! @brief Direction for arguments passed on stack, see @c ARGUMENT_DIR.
  UInt32 _cconvArgumentsDirection;

  //! @brief List of registers that's used for first INT arguments instead of stack.
  UInt32 _cconvArgumentsGp[16];
  //! @brief List of registers that's used for first FP arguments instead of stack.
  UInt32 _cconvArgumentsXmm[16];

  //! @brief Bitmask for preserved general purpose registers.
  UInt32 _cconvPreservedGp;
  //! @brief Bitmask for preserved sse registers.
  UInt32 _cconvPreservedXmm;

  //! @brief Count of arguments (in @c _argumentsList).
  UInt32 _argumentsCount;

  //! @brief Count of bytes consumed by arguments on the stack.
  UInt32 _argumentsStackSize;

  // --------------------------------------------------------------------------
  // [Registers allocator / Variables]
  // --------------------------------------------------------------------------

  //! @brief Size of maximum alignment size on the stack.
  SysInt _maxAlignmentStackSize;
  //! @brief Size of prolog/epilog on the stack.
  SysInt _prologEpilogStackSize;
  //! @brief Size of all variables on the stack.
  SysInt _variablesStackSize;

  UInt32 _usedGpRegisters;
  UInt32 _usedMmRegisters;
  UInt32 _usedXmmRegisters;

  UInt32 _modifiedGpRegisters;
  UInt32 _modifiedMmRegisters;
  UInt32 _modifiedXmmRegisters;

  PodVector<Variable*> _variables;
  Variable* _lastUsedRegister;

  // --------------------------------------------------------------------------
  // [Labels]
  // --------------------------------------------------------------------------

  Label* _entryLabel;
  Label* _prologLabel;
  Label* _exitLabel;

  friend struct Compiler;
};

// Inlines that uses AsmJit::Function
inline void Variable::alloc(UInt8 mode) { function()->alloc(*this, mode); }
inline void Variable::spill() { function()->spill(*this); }
inline void Variable::unuse() { function()->unuse(*this); }

// ============================================================================
// [AsmJit::Prolog]
// ============================================================================

//! @brief Prolog emittable.
struct ASMJIT_API Prolog : public Emittable
{
  Prolog(Compiler* c, Function* f);
  virtual ~Prolog();

  virtual void emit(Assembler& a);

  inline Function* function() const { return _function; }

private:
  Function* _function;
  Label* _label;

  friend struct Compiler;
  friend struct Function;
};

// ============================================================================
// [AsmJit::Epilog]
// ============================================================================

//! @brief Epilog emittable.
struct ASMJIT_API Epilog : public Emittable
{
  Epilog(Compiler* c, Function* f);
  virtual ~Epilog();

  virtual void emit(Assembler& a);

  inline Function* function() const { return _function; }

private:
  Function* _function;
  Label* _label;

  friend struct Compiler;
  friend struct Function;
};

// ============================================================================
// [AsmJit::Target]
// ============================================================================

//! @brief Target.
//!
//! Target is bound label location.
struct ASMJIT_API Target : public Emittable
{
  Target(Compiler* c, Label* l);
  virtual ~Target();

  virtual void emit(Assembler& a);

  //! @brief Return label bound to this target.
  inline Label* label() const { return _label; }

private:
  Label* _label;
};

// ============================================================================
// [AsmJit::Zone]
// ============================================================================

//! @brief Memory allocator designed to fast alloc memory that will be freed
//! in one step.
//!
//! @note This is hackery for performance. Concept is that objects created
//! by @c Compiler are manager by compiler. This means that lifetime of 
//! these objects are same as compiler lifetime (that's short).
//!
//! All emittables are allocated by @c Compiler by this way.
struct ASMJIT_API Zone
{
  //! @brief Create new instance of @c Zone.
  Zone(SysUInt chunkSize);
  //! @brief Destroy zone instance.
  ~Zone();

  //! @brief Allocate @c size bytes of memory and return pointer to it.
  void* alloc(SysUInt size);
  //! @brief Free all allocated memory at once.
  void freeAll();

  //! @brief Return total size of allocated objects - by @c alloc().
  inline SysUInt total() const { return _total; }
  //! @brief Return (default) chunk size.
  inline SysUInt chunkSize() const { return _chunkSize; }

  //! @brief One allocated chunk of memory.
  struct Chunk
  {
    //! @brief Link to previous chunk.
    Chunk* prev;
    //! @brief Position in this chunk.
    SysUInt pos;
    //! @brief Size of this chunk (in bytes).
    SysUInt size;

    //! @brief Data.
    UInt8 data[4];

    //! @brief Return count of remaining (unused) bytes in chunk.
    inline SysUInt remain() const { return size - pos; }
  };

private:
  //! @brief Last allocated chunk of memory.
  Chunk* _chunks;
  //! @brief Total size of allocated objects - by @c alloc() method.
  SysUInt _total;
  //! @brief One chunk size.
  SysUInt _chunkSize;
};

// ============================================================================
// [AsmJit::Compiler]
// ============================================================================

//! @brief Compiler.
//!
//! This class is used to store instruction stream and allows to modify
//! it on the fly. It uses different concept than @c AsmJit::Assembler class
//! and in fact @c AsmJit::Assembler is only used as a backend.
//!
//! Using of this class will generate slower code (means slower generation of
//! assembler, generated assembler is not slower) than using 
//! @c AsmJit::Assembler, but in some situations it can be more powerful and 
//! it can result in more readable code, because @c AsmJit::Compiler contains 
//! also register allocator and you can use variables instead of hardcoding 
//! registers.
struct ASMJIT_API Compiler : public Serializer
{
  typedef PodVector<Emittable*> EmittableList;
  typedef PodVector<Variable*> VariableList;
  typedef PodVector<Operand*> OperandList;

  // -------------------------------------------------------------------------
  // [Construction / Destruction]
  // -------------------------------------------------------------------------

  Compiler();
  virtual ~Compiler();

  // -------------------------------------------------------------------------
  // [Buffer]
  // -------------------------------------------------------------------------

  //! @brief Clear everything, but not deallocate buffers.
  void clear();

  //! @brief Free internal buffer, all emitters and NULL all pointers.
  void free();

  inline EmittableList& buffer() { return _buffer; }
  inline const EmittableList& buffer() const { return _buffer; }

  //! @brief Return current function.
  //!
  //! Use @c beginFunction() and @c endFunction() methods to begin / end
  //! function block. Each function must be also started by @c prolog()
  //! and ended by @c epilog() calls.
  inline Function* currentFunction() { return _currentFunction; }

  // -------------------------------------------------------------------------
  // [Logging]
  // -------------------------------------------------------------------------

  void comment(const char* fmt, ...);

  // -------------------------------------------------------------------------
  // [Function Builder]
  // -------------------------------------------------------------------------

  //! @brief Create a new function.
  //!
  //! @note To get current function use @c currentFunction() method.
  template<typename T>
  Function* newFunction(UInt32 cconv, const T& params)
  { return newFunction_(cconv, params.args(), params.count()); }

  //! @brief Create a new function.
  //!
  //! @note To get current function use @c currentFunction() method.
  Function* newFunction_(UInt32 cconv, const UInt32* args, SysUInt count);

  //! @brief Ends current function.
  Function* endFunction();

  //! @brief Create function prolog (begins a function).
  //!
  //! @note Compiler can optimize prologues and epilogues.
  Prolog* prolog(Function* f);

  //! @brief Create function epilog (ends a function).
  //!
  //! @note Compiler can optimize prologues and epilogues.
  Epilog* epilog(Function* f);

  // -------------------------------------------------------------------------
  // [Labels]
  // -------------------------------------------------------------------------

  Label* newLabel();

  // -------------------------------------------------------------------------
  // [Emit]
  // -------------------------------------------------------------------------

  void emit(Emittable* emittable, bool endblock = false);

  //! @brief Method that will emit everything to @c Assembler instance @a a.
  void build(Assembler& a);

  // -------------------------------------------------------------------------
  // [Memory Management]
  // -------------------------------------------------------------------------

  // Memory management in compiler has these rules:
  // - Everything created by compiler is freed by compiler.
  // - To get decent performance, compiler always uses larger buffer for 
  //   objects to allocate and when compiler instance is destroyed, this 
  //   buffer is freed. Destructors of active objects are called when 
  //   destroying compiler instance. Destructors of abadonded compiler
  //   objects are called immediately after abadonding it.

  template<typename T>
  inline T* newObject()
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this);
  }

  template<typename T, typename P1>
  inline T* newObject(P1 p1)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1);
  }

  template<typename T, typename P1, typename P2>
  inline T* newObject(P1 p1, P2 p2)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1, p2);
  }

  template<typename T, typename P1, typename P2, typename P3>
  inline T* newObject(P1 p1, P2 p2, P3 p3)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1, p2, p3);
  }

  template<typename T, typename P1, typename P2, typename P3, typename P4>
  inline T* newObject(P1 p1, P2 p2, P3 p3, P4 p4)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1, p2, p3, p4);
  }

  inline void* _allocObject(SysUInt size)
  { return _zone.alloc(size); }

  void _registerOperand(Operand* op);

  // -------------------------------------------------------------------------
  // [Intrinsics]
  // -------------------------------------------------------------------------

  void op_var32(UInt32 code, const Int32Ref& a)
  {
    if (a.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a.r32());
    else
      __emitX86(code, &a.m());
  }

#if defined(ASMJIT_X64)
  void op_var64(UInt32 code, const Int64Ref& a)
  {
    if (a.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a.r64());
    else
      __emitX86(code, &a.m());
  }
#endif // ASMJIT_X64

  void op_reg32_var32(UInt32 code, const Register& a, const Int32Ref& b)
  {
    if (b.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a, &b.r32());
    else
      __emitX86(code, &a, &b.m());
  }

#if defined(ASMJIT_X64)
  void op_reg64_var64(UInt32 code, const Register& a, const Int64Ref& b)
  {
    if (b.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a, &b.r64());
    else
      __emitX86(code, &a, &b.m());
  }
#endif // ASMJIT_X64

  void op_var32_reg32(UInt32 code, const Int32Ref& a, const Register& b)
  {
    if (a.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a.r32(), &b);
    else
      __emitX86(code, &a.m(), &b);
  }

  void op_var32_imm(UInt32 code, const Int32Ref& a, const Immediate& b)
  {
    if (a.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a.r32(), &b);
    else
      __emitX86(code, &a.m(), &b);
  }

#if defined(ASMJIT_X64)
  void op_var64_reg64(UInt32 code, const Int64Ref& a, const Register& b)
  {
    if (a.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a.r64(), &b);
    else
      __emitX86(code, &a.m(), &b);
  }

  void op_var64_imm(UInt32 code, const Int64Ref& a, const Immediate& b)
  {
    if (a.state() == VARIABLE_STATE_REGISTER)
      __emitX86(code, &a.r64(), &b);
    else
      __emitX86(code, &a.m(), &b);
  }
#endif // ASMJIT_X64

  using Serializer::adc;
  using Serializer::add;
  using Serializer::and_;
  using Serializer::cmp;
  using Serializer::dec;
  using Serializer::inc;
  using Serializer::mov;
  using Serializer::neg;
  using Serializer::not_;
  using Serializer::or_;
  using Serializer::sbb;
  using Serializer::sub;
  using Serializer::xor_;

  inline void adc(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_ADC, dst, src); }
  inline void adc(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_ADC, dst, src); }
  inline void adc(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_ADC, dst, src); }

  inline void add(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_ADD, dst, src); }
  inline void add(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_ADD, dst, src); }
  inline void add(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_ADD, dst, src); }

  inline void and_(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_AND, dst, src); }
  inline void and_(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_AND, dst, src); }
  inline void and_(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_AND, dst, src); }

  inline void cmp(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_CMP, dst, src); }
  inline void cmp(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_CMP, dst, src); }
  inline void cmp(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_CMP, dst, src); }

  inline void dec(const Int32Ref& dst) { op_var32(INST_DEC, dst); }
  inline void inc(const Int32Ref& dst) { op_var32(INST_INC, dst); }
  inline void neg(const Int32Ref& dst) { op_var32(INST_NEG, dst); }
  inline void not_(const Int32Ref& dst) { op_var32(INST_NOT, dst); }

  inline void mov(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_MOV, dst, src); }
  inline void mov(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_MOV, dst, src); }
  inline void mov(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_MOV, dst, src); }

  inline void or_(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_OR, dst, src); }
  inline void or_(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_OR, dst, src); }
  inline void or_(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_OR, dst, src); }

  inline void sbb(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_SBB, dst, src); }
  inline void sbb(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_SBB, dst, src); }
  inline void sbb(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_SBB, dst, src); }

  inline void sub(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_SUB, dst, src); }
  inline void sub(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_SUB, dst, src); }
  inline void sub(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_SUB, dst, src); }

  inline void xor_(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_XOR, dst, src); }
  inline void xor_(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_XOR, dst, src); }
  inline void xor_(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_XOR, dst, src); }

#if defined(ASMJIT_X64)
  inline void adc(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_ADC, dst, src); }
  inline void adc(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_ADC, dst, src); }
  inline void adc(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_ADC, dst, src); }

  inline void add(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_ADD, dst, src); }
  inline void add(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_ADD, dst, src); }
  inline void add(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_ADD, dst, src); }

  inline void and_(const Register& dst, const Int32Ref& src) { op_reg32_var32(INST_AND, dst, src); }
  inline void and_(const Int32Ref& dst, const Register& src) { op_var32_reg32(INST_AND, dst, src); }
  inline void and_(const Int32Ref& dst, const Immediate& src) { op_var32_imm(INST_AND, dst, src); }

  inline void and_(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_AND, dst, src); }
  inline void and_(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_AND, dst, src); }
  inline void and_(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_AND, dst, src); }

  inline void cmp(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_CMP, dst, src); }
  inline void cmp(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_CMP, dst, src); }
  inline void cmp(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_CMP, dst, src); }

  inline void dec(const Int64Ref& dst) { op_var64(INST_DEC, dst); }
  inline void inc(const Int64Ref& dst) { op_var64(INST_INC, dst); }
  inline void neg(const Int64Ref& dst) { op_var64(INST_NEG, dst); }
  inline void not_(const Int64Ref& dst) { op_var64(INST_NOT, dst); }

  inline void mov(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_MOV, dst, src); }
  inline void mov(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_MOV, dst, src); }
  inline void mov(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_MOV, dst, src); }

  inline void or_(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_OR, dst, src); }
  inline void or_(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_OR, dst, src); }
  inline void or_(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_OR, dst, src); }

  inline void sbb(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_SBB, dst, src); }
  inline void sbb(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_SBB, dst, src); }
  inline void sbb(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_SBB, dst, src); }

  inline void sub(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_SUB, dst, src); }
  inline void sub(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_SUB, dst, src); }
  inline void sub(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_SUB, dst, src); }

  inline void xor_(const Register& dst, const Int64Ref& src) { op_reg64_var64(INST_XOR, dst, src); }
  inline void xor_(const Int64Ref& dst, const Register& src) { op_var64_reg64(INST_XOR, dst, src); }
  inline void xor_(const Int64Ref& dst, const Immediate& src) { op_var64_imm(INST_XOR, dst, src); }
#endif // ASMJIT_X64

  // -------------------------------------------------------------------------
  // [EmitX86]
  // -------------------------------------------------------------------------

  virtual void _emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);

  // -------------------------------------------------------------------------
  // [Align]
  // -------------------------------------------------------------------------

  virtual void align(SysInt m);

  // -------------------------------------------------------------------------
  // [Bind]
  // -------------------------------------------------------------------------

  virtual void bind(Label* label);

  // -------------------------------------------------------------------------
  // [Variables]
  // -------------------------------------------------------------------------
private:
  //! @brief List of emittables.
  EmittableList _buffer;

  //! @brief Position in @c _buffer.
  SysInt _currentPosition;

  //! @brief Operands list (operand id is index in this list, id 0 is not valid).
  OperandList _operands;

  //! @brief Current function.
  Function* _currentFunction;

  //! @brief Memory management.
  Zone _zone;

  //! @brief Label id counter (starts from 1).
  UInt32 _labelIdCounter;

  friend struct Instruction;
  friend struct Variable;
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITCOMPILERX86X64_H
