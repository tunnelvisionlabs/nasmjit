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
#include "AsmJitBuild.h"
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
struct State;
struct Variable;

//! @addtogroup AsmJit_Compiler
//! @{

// ============================================================================
// [Constants]
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
//!
//! Calling convention is scheme how function arguments are passed into 
//! function and how functions returns values. In assembler programming
//! it's needed to always comply with function calling conventions, because
//! even small inconsistency can cause undefined behavior or crash.
//!
//! List of calling conventions for 32 bit x86 mode:
//! - @c CALL_CONV_CDECL - Calling convention for C runtime.
//! - @c CALL_CONV_STDCALL - Calling convention for WinAPI functions.
//! - @c CALL_CONV_MSTHISCALL - Calling convention for C++ members under 
//!      Windows (produced by MSVC and all MSVC compatible compilers).
//! - @c CALL_CONV_MSFASTCALL - Fastest calling convention that can be used
//!      by MSVC compiler.
//! - @c CALL_CONV_BORNANDFASTCALL - Borland fastcall convention.
//! - @c CALL_CONV_GCCFASTCALL_2 - GCC fastcall convention with 2 register
//!      arguments.
//! - @c CALL_CONV_GCCFASTCALL_3 - GCC fastcall convention with 3 register
//!      arguments.
//!
//! List of calling conventions for 64 bit x86 mode (x64):
//! - @c CALL_CONV_X64W - Windows 64 bit calling convention (WIN64 ABI).
//! - @c CALL_CONV_X64U - Unix 64 bit calling convention (AMD64 ABI).
//!
//! There is also @c CALL_CONV_DEFAULT that is defined to fit best to your 
//! compiler.
//!
//! These types are used together with @c AsmJit::Compiler::newFunction() 
//! method.
enum CALL_CONV
{
  //! @brief Calling convention is invalid (can't be used).
  CALL_CONV_NONE = 0,

  // [X64 Calling Conventions]

  //! @brief X64 calling convention for Windows platform.
  //!
  //! For first four arguments are used these registers:
  //! - 1. 32/64 bit integer or floating point argument - rcx/xmm0
  //! - 2. 32/64 bit integer or floating point argument - rdx/xmm1
  //! - 3. 32/64 bit integer or floating point argument - r8/xmm2
  //! - 4. 32/64 bit integer or floating point argument - r9/xmm3
  //!
  //! Note first four arguments here means arguments at positions from 1 to 4
  //! (included). For example if second argument is not passed by register then
  //! rdx/xmm1 register is unused.
  //!
  //! All other arguments are pushed on the stack in right-to-left direction.
  //! Stack is aligned by 16 bytes. There is 32 bytes shadow space on the stack
  //! that can be used to save up to four 64 bit registers (probably designed to
  //! be used to save first four arguments passed in registers).
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
  CALL_CONV_X64W = 1,

  //! @brief X64 calling convention for Unix platforms (AMD64 ABI).
  //!
  //! First six 32 or 64 bit integer arguments are passed in rdi, rsi, rdx, 
  //! rcx, r8, r9 registers. First eight floating point or XMM arguments 
  //! are passed in xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7 registers.
  //! This means that in registers can be transferred up to 14 arguments total.
  //!
  //! There is also RED ZONE below the stack pointer that can be used for 
  //! temporary storage. The red zone is the space from [rsp-128] to [rsp-8].
  //! 
  //! Arguments direction:
  //! - Right to Left (Except for arguments passed in registers).
  //!
  //! Stack is cleaned by:
  //! - Caller.
  //!
  //! Return value:
  //! - Integer types - RAX register.
  //! - Floating points - XMM0 register.
  //!
  //! Stack is always aligned by 16 bytes.
  CALL_CONV_X64U = 2,

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
  CALL_CONV_CDECL = 3,

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
  CALL_CONV_STDCALL = 4,

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
  CALL_CONV_MSTHISCALL = 5,

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
  CALL_CONV_MSFASTCALL = 6,

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
  CALL_CONV_BORLANDFASTCALL = 7,

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
  CALL_CONV_GCCFASTCALL_2 = 8,

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
  CALL_CONV_GCCFASTCALL_3 = 9,

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

//! @brief Variable type.
//!
//! Variable type is used by @c AsmJit::Function::newVariable() method and can
//! be also retrieved by @c AsmJit::VariableRef::type().
enum VARIABLE_TYPE
{
  //! @brief Invalid variable type (don't use).
  VARIABLE_TYPE_NONE = 0,

  //! @brief Variable is 32 bit integer (@c Int32).
  VARIABLE_TYPE_INT32 = 1,
  //! @brief Variable is 32 bit unsigned integer (@c UInt32).
  VARIABLE_TYPE_UINT32 = 1,

  //! @var VARIABLE_TYPE_INT64
  //! @brief Variable is 64 bit signed integer (@c Int64).
  //! @note Can be used only in 64 bit mode.

  //! @var VARIABLE_TYPE_UINT64
  //! @brief Variable is 64 bit unsigned integer (@c UInt64).
  //! @note Can be used only in 64 bit mode.

  //! @var VARIABLE_TYPE_SYSINT
  //! @brief Variable is system wide integer (@c Int32 or @c Int64).
  //! @var VARIABLE_TYPE_SYSUINT
  //! @brief Variable is system wide unsigned integer (@c UInt32 or @c UInt64).
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
  //!
  //! TODO: Float registers allocation is not supported.
  VARIABLE_TYPE_FLOAT = 3,
  //! @brief Variable is DP-FP number (double).
  //!
  //! TODO: Double registers allocation  is not supported.
  VARIABLE_TYPE_DOUBLE = 4,

  //! @brief Variable is MM register / memory location.
  VARIABLE_TYPE_MM = 5,
  //! @brief Variable is XMM register / memory location.
  VARIABLE_TYPE_XMM = 6,

  //! @brief Count of variable types.
  _VARIABLE_TYPE_COUNT
};

//! @brief State of variable.
//!
//! Variable state can be retrieved by @c AsmJit::VariableRef::state().
enum VARIABLE_STATE
{
  //! @brief Variable is currently not used.
  //!
  //! Variables of this state are not used or they are currently not 
  //! initialized (short time after @c AsmJit::VariableRef::alloc() call).
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

//! @brief Variable alloc mode.
//! @internal
enum VARIABLE_ALLOC
{
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
// [AsmJit::Variable]
// ============================================================================

//! @brief Variable.
//!
//! Variables reresents registers or memory locations that can be allocated by
//! @c Function. Each function arguments are also allocated as variables, so
//! accessing function arguments is similar to accessing function variables.
//!
//! Variables can be declared by @c AsmJit::Function::newVariable() or by 
//! declaring function arguments by AsmJit::Compiler::newFunction(). Compiler
//! always returns variables as pointers to @c Variable instance.
//!
//! Variable instances are never accessed directly, instead there are wrappers.
//! Wrappers are designed to simplify variables management and it's lifetime.
//! Because variables are based on reference counting, each variable that is
//! returned from @c Compiler needs to be wrapped in @c VariableRef or similar
//! (@c Int32Ref, @c Int64Ref, @c SysIntRef, @c PtrRef, @c MMRef, @c XMMRef)
//! classes. Each wrapper class is designed to wrap specific variable type. For
//! example integer should be always wrapped into @c Int32Ref or @c Int64Ref,
//! MMX register to @c MMRef, etc...
//!
//! Variable wrapping is also needed, because it's lifetime is based on 
//! reference counting. Each variable returned from compiler has reference 
//! count equal to zero! So wrapper class increases it to one (or more that
//! depends how much you wrapped it) and destroying wrapper means decreasing
//! reference count. If reference count is decreased to zero, variable life
//! ends, it's marked as unused and compiler can reuse it later.
//!
//! @sa @c VariableRef, @c Int32Ref, @c Int64Ref, @c SysIntRef, @c PtrRef, 
//! @c MMRef, @c XMMRef.
struct ASMJIT_API Variable
{
  // [Typedefs]

  //! @brief Custom alloc function type.
  typedef void (*AllocFn)(Variable* v);
  //! @brief Custom spill function type.
  typedef void (*SpillFn)(Variable* v);

  // [Construction / Destruction]

  //! @brief Create a new @a Variable instance.
  //!
  //! Always use @c AsmJit::Function::newVariable() method to create 
  //! @c Variable.
  Variable(Compiler* c, Function* f, UInt8 type);
  //! @brief Destroy variable instance.
  //!
  //! Never destroy @c Variable instance created by @c Compiler.
  virtual ~Variable();

  // [Methods]

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
  void setPriority(UInt8 priority);
  //! @brief Set varialbe preferred register.
  inline void setPreferredRegister(UInt8 code) { _preferredRegister = code; }
  //! @brief Set variable changed state.
  inline void setChanged(UInt8 changed) { _changed = changed; }

  //! @brief Memory operand that will be always pointed to variable memory address. */
  inline const Mem& memoryOperand() const { return *_memoryOperand; }

  // [Reference counting]

  //! @brief Increase reference count and return itself.
  Variable* ref();
  //! @brief Decrease reference count. If reference is decreased to zero, 
  //! variable is marked as unused and deallocated.
  void deref();

  // [Code Generation]

  //! @brief Allocate variable to register.
  //! @param mode Allocation mode (see @c VARIABLE_ALLOC enum)
  //! @param prefferedRegister Preffered register to use (see @c AsmJit::REG enum).
  inline void alloc(
    UInt8 mode = VARIABLE_ALLOC_READWRITE, 
    UInt8 preferredRegister = NO_REG);
  //! @brief Spill variable (move to memory).
  inline void spill();
  //! @brief Unuse variable
  //!
  //! This will completely destroy variable. After @c unuse() you can use
  //! @c alloc() to allocate it again.
  inline void unuse();

  // [Custom Spill / Restore]

  //! @brief Return @c true if this variable uses custom alloc or spill 
  //! functions (this means bypassing built-in functions).
  //!
  //! @note If alloc or spill function is set, variable is marked as custom
  //! and there is no chance to move it to / from stack. For example mmx zero
  //! register can be implemented in allocFn() that will emit pxor(mm, mm).
  //! Variable will never spill into stack in this case.
  inline bool isCustom() const
  { return _allocFn != NULL || _spillFn != NULL; }

  //! @brief Get custom alloc function.
  inline AllocFn allocFn() const { return _allocFn; }
  //! @brief Get custom spill function.
  inline SpillFn spillFn() const { return _spillFn; }
  //! @brief Get custom data pointer.
  inline void* data() const { return _data; }

  //! @brief Set custom alloc function.
  inline void setAllocFn(AllocFn fn) { _allocFn = fn; }
  //! @brief Set custom spill function.
  inline void setSpillFn(SpillFn fn) { _spillFn = fn; }
  //! @brief Set custom data.
  inline void setData(void* data) { _data = data; }

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

  //! @brief Custom alloc function (or NULL).
  AllocFn _allocFn;

  //! @brief Custom spill function (or NULL).
  SpillFn _spillFn;

  //! @brief Custom data that can be used by custom spill and restore functions.
  void* _data;

  friend struct Compiler;
  friend struct Function;
  friend struct VariableRef;

  // disable copy
  ASMJIT_DISABLE_COPY(Variable);
};

// ============================================================================
// [AsmJit::XXXRef]
// ============================================================================

//! @brief Base class for variable wrappers.
//!
//! @c VariableRef class is designed to manage @c Variable instances. It's 
//! based on reference counting and if reference gets to zero (in destructor), 
//! variable is freed by compiler. This helps with scoping variables and 
//! minimizes mistakes that can be done with manual allocation / freeing.
//!
//! @note Compiler can reuse existing variables if reference gets zero.
//!
//! @sa @c Variable,
//!     @c Int32Ref, @c Int64Ref, @c SysIntRef, @c PtrRef, @c MMRef, @c XMMRef.
struct VariableRef
{
  // [Typedefs]

  typedef Variable::AllocFn AllocFn;
  typedef Variable::SpillFn SpillFn;

  // [Construction / Destruction]

  //! @brief Create new uninitialized variable reference.
  //!
  //! Using uninitialized variable reference is forbidden.
  inline VariableRef() : _v(NULL) {}

  //! @brief Reference variable @a v (@a v can't be @c NULL).
  inline VariableRef(Variable* v) : _v(v->ref()) {}

  //! @brief Dereference variable if it's wrapped.
  inline ~VariableRef() { if (_v) _v->deref(); }

  inline VariableRef& operator=(Variable* v)
  {
    if (_v) _v->deref();
    _v = v->ref();
  }

  //! @brief Return @c Variable instance.
  inline Variable* v() { return _v; }

  // [Methods]

  //! @brief Return variable type, see @c VARIABLE_TYPE.
  inline UInt8 type() const { ASMJIT_ASSERT(_v); return _v->type(); }
  //! @brief Return variable size (in bytes).
  inline UInt8 size() const { ASMJIT_ASSERT(_v); return _v->size(); }
  //! @brief Return variable state, see @c VARIABLE_STATE.
  inline UInt8 state() const { ASMJIT_ASSERT(_v); return _v->state(); }

  inline void use(Variable* v) { ASMJIT_ASSERT(_v == NULL); _v = v->ref(); }

  //! @brief Allocate variable to register.
  //! @param mode Allocation mode (see @c VARIABLE_ALLOC enum)
  //! @param prefferedRegister Preffered register to use (see @c REG enum).
  inline void alloc(
    UInt8 mode = VARIABLE_ALLOC_READWRITE, 
    UInt8 preferredRegister = NO_REG)
  {
    ASMJIT_ASSERT(_v);
    _v->alloc(mode, preferredRegister);
  }

  //! @brief Spill variable (move to memory).
  inline void spill()
  {
    ASMJIT_ASSERT(_v);
    _v->spill();
  }

  //! @brief Unuse variable
  //!
  //! This will completely destroy variable. After @c unuse() you can use
  //! @c alloc() to allocate it again.
  inline void unuse()
  {
    ASMJIT_ASSERT(_v);
    _v->unuse();
  }

  //! @brief Destroy variable (@c VariableRef can't be used anymore after destroy).
  inline void destroy()
  {
    ASMJIT_ASSERT(_v);
    _v->deref();
    _v = NULL;
  }

  //! @brief Get variable preffered register.
  inline UInt8 preferredRegister() const { ASMJIT_ASSERT(_v); return _v->preferredRegister(); }
  //! @brief Set variable preffered register to @a code.
  //! @param code Preffered register code (see @c AsmJit::REG enum).
  inline void setPreferredRegister(UInt8 code) { ASMJIT_ASSERT(_v); _v->setPreferredRegister(code); }

  //! @brief Get variable priority.
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

  // [Reference counting]

  //! @brief Increase reference count and return @c Variable instance.
  inline Variable* ref() { ASMJIT_ASSERT(_v); return _v->ref(); }

  // [Custom Spill / Restore]

  //! @brief Return @c true if variable uses custom alloc / spill functions.
  //!
  //! @sa @c AsmJit::Variable::isCustom() method.
  inline bool isCustom() const { ASMJIT_ASSERT(_v); return _v->isCustom(); }

  //! @brief Get custom restore function.
  inline AllocFn allocFn() const { ASMJIT_ASSERT(_v); return _v->allocFn(); }
  //! @brief Get custom spill function.
  inline SpillFn spillFn() const { ASMJIT_ASSERT(_v); return _v->spillFn(); }
  //! @brief Get custom data pointer.
  inline void* data() const { ASMJIT_ASSERT(_v); return _v->data(); }

  //! @brief Set custom restore function.
  inline void setAllocFn(AllocFn fn) { ASMJIT_ASSERT(_v); _v->setAllocFn(fn); }
  //! @brief Set custom spill function.
  inline void setSpillFn(SpillFn fn) { ASMJIT_ASSERT(_v); _v->setSpillFn(fn); }
  //! @brief Set custom data.
  inline void setData(void* data) { ASMJIT_ASSERT(_v); _v->setData(data); }

protected:
  Variable* _v;

  // disable copy
  ASMJIT_DISABLE_COPY(VariableRef);
};

//! @brief 32 bit integer variable wrapper.
struct Int32Ref : public VariableRef
{
  // [Construction / Destruction]

  inline Int32Ref() : VariableRef() {}
  inline Int32Ref(Variable* v) : VariableRef(v) {}

  // [Registers]

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
//! @brief 64 bit integer variable wrapper.
struct Int64Ref : public VariableRef
{
  // [Construction / Destruction]

  inline Int64Ref() : VariableRef() {}
  inline Int64Ref(Variable* v) : VariableRef(v) {}

  // [Registers]

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

//! @brief MMX variable wrapper.
struct MMRef : public VariableRef
{
  // [Construction / Destruction]

  inline MMRef() : VariableRef() {}
  inline MMRef(Variable* v) : VariableRef(v) {}

  // [Registers]

  inline MMRegister r() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_mm(_v->registerCode()); }
  inline MMRegister c() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_mm(_v->registerCode()); }
  inline MMRegister x() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_mm(_v->registerCode()); }
};

//! @brief SSE variable wrapper.
struct XMMRef : public VariableRef
{
  // [Construction / Destruction]

  inline XMMRef() : VariableRef() {}
  inline XMMRef(Variable* v) : VariableRef(v) {}

  // [Registers]

  inline XMMRegister r() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READWRITE); return mk_xmm(_v->registerCode()); }
  inline XMMRegister c() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_READ     ); return mk_xmm(_v->registerCode()); }
  inline XMMRegister x() const { ASMJIT_ASSERT(_v); _v->alloc(VARIABLE_ALLOC_WRITE    ); return mk_xmm(_v->registerCode()); }
};

#if defined(ASMJIT_X86)
typedef Int32Ref SysIntRef;
#else
typedef Int64Ref SysIntRef;
#endif

//! @brief Pointer variable wrapper (same as system integer).
typedef SysIntRef PtrRef;

// ============================================================================
// [AsmJit::State]
// ============================================================================

//! @brief Contains informations about current register state.
//!
//! @note Always use StateRef to manage register states and don't create State
//! directly. Instead use @c AsmJit::Function::saveState() and 
//! @c AsmJit::Function::restoreState() methods.
struct ASMJIT_API State
{
  // [Construction / Destruction]

  State(Compiler* c, Function* f);
  virtual ~State();

  union Data 
  {
    //! @brief All variables in one array.
    Variable* _v[16+8+16];

    struct {
      //! @brief Regeral purpose registers.
      Variable* _gp[16];
      //! @brief MMX registers.
      Variable* _mm[8];
      //! @brief XMM registers.
      Variable* _xmm[16];
    };
  };

private:
  //! @brief Clear state.
  void _clear();

  //! @brief Save function state (there is no code generated when saving state).
  void _save();
  //! @brief Restore function state, can spill and alloc registers.
  void _restore();

  //! @brief Set function state to current state.
  //!
  //! @note This method is similar to @c _restore(), but it will not alloc or 
  //! spill registers.
  void _set();

  //! @brief Compiler this state is related to.
  Compiler* _compiler;

  //! @brief Function this state is related to.
  Function* _function;

  //! @brief State data.
  Data _data;

  friend struct Compiler;
  friend struct Function;
  friend struct StateRef;

  // disable copy
  ASMJIT_DISABLE_COPY(State);
};

// ============================================================================
// [AsmJit::StateRef]
// ============================================================================

//! @brief State wrapper used to manage @c State's.
struct StateRef
{
  inline StateRef(State* state) :
    _state(state)
  {
  }

  inline ~StateRef();

  //! @brief Return managed @c State instance.
  inline State* state() const { return _state; }

private:
  State* _state;

  // disable copy
  ASMJIT_DISABLE_COPY(StateRef);
};

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

//! @brief Emmitable.
//!
//! Emittable is object that can emit single or more instructions. To
//! createyour interface it's needed to override virtual method @c emit().
struct ASMJIT_API Emittable
{
  // [Construction / Destruction]

  Emittable(Compiler* c, UInt32 type);
  virtual ~Emittable();

  // [Methods]

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
  ASMJIT_DISABLE_COPY(Emittable);
};

// ============================================================================
// [AsmJit::Comment]
// ============================================================================

//! @brief Emittable used to emit comment into @c Assembler logger.
struct ASMJIT_API Comment : public Emittable
{
  // [Construction / Destruction]

  Comment(Compiler* c, const char* str);
  virtual ~Comment();

  // [Methods]

  virtual void emit(Assembler& a);

  inline const char* str() const { return _str; }

private:
  const char* _str;
};

// ============================================================================
// [AsmJit::Align]
// ============================================================================

//! @brief Emittable used to align assembler code.
struct ASMJIT_API Align : public Emittable
{
  // [Construction / Destruction]

  Align(Compiler* c, SysInt size = 0);
  virtual ~Align();

  // [Methods]

  virtual void emit(Assembler& a);

  inline SysInt size() const { return _size; }
  inline void setSize(SysInt size) { _size = size; }

private:
  SysInt _size;
};

// ============================================================================
// [AsmJit::Instruction]
// ============================================================================

//! @brief Emittable that represents single instruction and its operands.
struct ASMJIT_API Instruction : public Emittable
{
  // [Construction / Destruction]

  Instruction(Compiler* c);
  Instruction(Compiler* c, UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);
  virtual ~Instruction();

  // [Methods]

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

//! @brief Template based type to variable ID converter.
template<typename T>
struct TypeAsId 
{
#if defined(ASMJIT_NODOC)
  enum { 
    //! @brief Variable id, see @c VARIABLE_TYPE enum
    Id = X
  };
#endif
};

// Skip documenting this
#if !defined(ASMJIT_NODOC)

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

#endif // !ASMJIT_NODOC

// ============================================================================
// [AsmJit::Function Builder]
// ============================================================================

//! @brief Class used to build function without arguments.
struct BuildFunction0
{
  inline const UInt32* args() const { return NULL; }
  inline SysUInt count() const { return 0; }
};

//! @brief Class used to build function with 1 argument.
template<typename P0>
struct BuildFunction1
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id }; return data; }
  inline SysUInt count() const { return 1; }
};

//! @brief Class used to build function with 2 arguments.
template<typename P0, typename P1>
struct BuildFunction2
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id }; return data; }
  inline SysUInt count() const { return 2; }
};

//! @brief Class used to build function with 3 arguments.
template<typename P0, typename P1, typename P2>
struct BuildFunction3
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id }; return data; }
  inline SysUInt count() const { return 3; }
};

//! @brief Class used to build function with 4 arguments.
template<typename P0, typename P1, typename P2, typename P3>
struct BuildFunction4
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id }; return data; }
  inline SysUInt count() const { return 4; }
};

//! @brief Class used to build function with 5 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4>
struct BuildFunction5
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id }; return data; }
  inline SysUInt count() const { return 5; }
};

//! @brief Class used to build function with 6 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
struct BuildFunction6
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id, TypeAsId<P5>::Id }; return data; }
  inline SysUInt count() const { return 6; }
};

//! @brief Class used to build function with 7 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct BuildFunction7
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id, TypeAsId<P5>::Id, TypeAsId<P6>::Id }; return data; }
  inline SysUInt count() const { return 7; }
};

//! @brief Class used to build function with 8 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct BuildFunction8
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id, TypeAsId<P5>::Id, TypeAsId<P6>::Id, TypeAsId<P7>::Id }; return data; }
  inline SysUInt count() const { return 8; }
};

//! @brief Class used to build function with 9 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
struct BuildFunction9
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id, TypeAsId<P5>::Id, TypeAsId<P6>::Id, TypeAsId<P7>::Id, TypeAsId<P8>::Id }; return data; }
  inline SysUInt count() const { return 9; }
};

//! @brief Class used to build function with 9 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
struct BuildFunction10
{
  inline const UInt32* args() const { static const UInt32 data[] = { TypeAsId<P0>::Id, TypeAsId<P1>::Id, TypeAsId<P2>::Id, TypeAsId<P3>::Id, TypeAsId<P4>::Id, TypeAsId<P5>::Id, TypeAsId<P6>::Id, TypeAsId<P7>::Id, TypeAsId<P8>::Id, TypeAsId<P9>::Id }; return data; }
  inline SysUInt count() const { return 10; }
};

// ============================================================================
// [AsmJit::Function]
// ============================================================================

//! @brief Function emittable used to generate C/C++ functions.
//!
//! Functions are base blocks for generating assembler output. Each generated
//! assembler stream needs standard entry and leave sequences thats compatible 
//! to operating system conventions (ABI).
//!
//! Function class can be used to generate entry (prolog) and leave (epilog)
//! sequences that is compatible to a given calling convention and to allocate
//! and manage variables that can be allocated to registers or spilled.
//!
//! @note To create function use @c AsmJit::Compiler::newFunction() method, do
//! not create @c Function instances by different ways.
//!
//! @sa @c State, @c StateRef, @c Variable, @c VariableRef.
struct ASMJIT_API Function : public Emittable
{
  // [Construction / Destruction]

  //! @brief Create new @c Function instance.
  //!
  //! @note Always use @c AsmJit::Compiler::newFunction() to create @c Function
  //! instance.
  Function(Compiler* c);
  //! @brief Destroy @c Function instance.
  virtual ~Function();

  // [Methods]

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

  void alloc(Variable* v, 
    UInt8 mode = VARIABLE_ALLOC_READWRITE, 
    UInt8 preferredRegister = NO_REG);
  void spill(Variable* v);
  void unuse(Variable* v);

  Variable* _getSpillCandidate(UInt8 type);

  void _allocAs(Variable* v, UInt8 mode, UInt32 code);
  void _allocReg(UInt8 code, Variable* v);
  void _freeReg(UInt8 code);

  //! @brief Return size of alignment on the stack.
  //!
  //! Stack is aligned to 16 bytes by default. For X64 platforms there is 
  //! no extra code needed to align stack to 16 bytes, because it's default
  //! stack alignment.
  inline SysInt stackAlignmentSize() const { return _stackAlignmentSize; }

  //! @brief Size needed to save registers in prolog / epilog.
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

  //! @brief Mark general purpose registers in the given @a mask as used.
  inline void useGpRegisters(UInt32 mask) { _usedGpRegisters |= mask; }
  //! @brief Mark mmx registers in the given @a mask as used.
  inline void useMmRegisters(UInt32 mask) { _usedMmRegisters |= mask; }
  //! @brief Mark sse registers in the given @a mask as used.
  inline void useXmmRegisters(UInt32 mask) { _usedXmmRegisters |= mask; }

  //! @brief Mark general purpose registers in the given @a mask as unused.
  inline void unuseGpRegisters(UInt32 mask) { _usedGpRegisters &= ~mask; }
  //! @brief Mark mmx registers in the given @a mask as unused.
  inline void unuseMmRegisters(UInt32 mask) { _usedMmRegisters &= ~mask; }
  //! @brief Mark sse registers in the given @a mask as unused.
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

  //! @brief Mark general purpose registers in the given @a mask as modified.
  inline void modifyGpRegisters(UInt32 mask) { _modifiedGpRegisters |= mask; }
  //! @brief Mark mmx registers in the given @a mask as modified.
  inline void modifyMmRegisters(UInt32 mask) { _modifiedMmRegisters |= mask; }
  //! @brief Mark sse registers in the given @a mask as modified.
  inline void modifyXmmRegisters(UInt32 mask) { _modifiedXmmRegisters |= mask; }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Save function current register state.
  //!
  //! To save function state always wrap returned value into @c StateRef:
  //!
  //! @code
  //! // Your function
  //! Function &f = ...;
  //!
  //! // Block
  //! {
  //!   // Save state
  //!   StateRef state(f.saveState());
  //!
  //!   // Your code ...
  //!
  //!   // Restore state (automatic by @c StateRef destructor).
  //! }
  //!
  //! @endcode
  State *saveState();

  //! @brief Restore function register state to @a state.
  //! @sa saveState().
  void restoreState(State* state);

  //! @brief Set function register state to @a state.
  void setState(State* state);

  // --------------------------------------------------------------------------
  // [Labels]
  // --------------------------------------------------------------------------

  //! @brief Return function entry label.
  //!
  //! Entry label can be used to call this function from another code that's
  //! being generated.
  inline Label* entryLabel() const { return _entryLabel; }

  //! @brief Return prolog label (label after function prolog)
  inline Label* prologLabel() const { return _prologLabel; }

  //! @brief Return exit label.
  //!
  //! Use exit label to jump to function epilog.
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
  SysInt _stackAlignmentSize;
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
  // [State]
  // --------------------------------------------------------------------------

  State _state;

  // --------------------------------------------------------------------------
  // [Labels]
  // --------------------------------------------------------------------------

  Label* _entryLabel;
  Label* _prologLabel;
  Label* _exitLabel;

  friend struct Compiler;
  friend struct State;
};

// Inlines that uses AsmJit::Function
inline void Variable::alloc(UInt8 mode, UInt8 preferredRegister) 
{ function()->alloc(this, mode, preferredRegister); }

inline void Variable::spill()
{ function()->spill(this); }

inline void Variable::unuse()
{ function()->unuse(this); }

inline StateRef::~StateRef()
{ if (_state) _state->_function->restoreState(_state); }

// ============================================================================
// [AsmJit::Prolog]
// ============================================================================

//! @brief Prolog emittable.
struct ASMJIT_API Prolog : public Emittable
{
  // [Construction / Destruction]

  Prolog(Compiler* c, Function* f);
  virtual ~Prolog();

  // [Methods]

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
  // [Construction / Destruction]

  Epilog(Compiler* c, Function* f);
  virtual ~Epilog();

  // [Methods]

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
  // [Construction / Destruction]

  Target(Compiler* c, Label* l);
  virtual ~Target();

  // [Methods]

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
//! All emittables, variables, labels and states allocated by @c Compiler are
//! allocated through @c Zone object.
struct ASMJIT_API Zone
{
  // [Construction / Destruction]

  //! @brief Create new instance of @c Zone.
  //! @param chunkSize Default size for one zone chunk.
  Zone(SysUInt chunkSize);

  //! @brief Destroy zone instance.
  ~Zone();

  // [Methods]

  //! @brief Allocate @c size bytes of memory and return pointer to it.
  //!
  //! Pointer allocated by this way will be valid until @c Zone object is
  //! destroyed. To create class by this way use placement @c new and 
  //! @c delete operators:
  //!
  //! @code
  //! // Example of allocating simple class
  //!
  //! // Your class
  //! class Object
  //! {
  //!   // members...
  //! };
  //!
  //! // Your function
  //! void f()
  //! {
  //!   // We are using AsmJit namespace
  //!   using namespace AsmJit
  //!
  //!   // Create zone object with chunk size of 65536 bytes.
  //!   Zone zone(65536);
  //!
  //!   // Create your objects using zone object allocating, for example:
  //!   Object* obj = new(zone.alloc(sizeof(YourClass))) Object();
  //! 
  //!   // ... lifetime of your objects ...
  //! 
  //!   // Destroy your objects:
  //!   obj->~Object();
  //!
  //!   // Zone destructor will free all memory allocated through it, 
  //!   // alternative is to call @c zone.freeAll().
  //! }
  //! @endcode
  void* alloc(SysUInt size);

  //! @brief Free all allocated memory at once.
  void freeAll();

  //! @brief Return total size of allocated objects - by @c alloc().
  inline SysUInt total() const { return _total; }
  //! @brief Return (default) chunk size.
  inline SysUInt chunkSize() const { return _chunkSize; }

  // [Chunk]

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

//! @brief Compiler - high level code generation.
//!
//! This class is used to store instruction stream and allows to modify
//! it on the fly. It uses different concept than @c AsmJit::Assembler class
//! and in fact @c AsmJit::Assembler is only used as a backend. Compiler never
//! emits machine code and each instruction you use is stored to instruction
//! array instead. This allows to modify instruction stream later and for 
//! example to reorder instructions to make better performance.
//!
//! Using @c AsmJit::Compiler moves code generation to higher level. Higher 
//! level constructs allows to write more abstract and extensible code that
//! is not possible with pure @c AsmJit::Assembler class. Because 
//! @c AsmJit::Compiler needs to create many objects and lifetime of these 
//! objects is small (same as @c AsmJit::Compiler lifetime itself) it uses 
//! very fast memory management model. This model allows to create object 
//! instances in nearly zero time (compared to @c malloc() or @c new() 
//! operators) so overhead by creating assembler stream by 
//! @c AsmJit::Compiler is minimized.
//!
//! <b>Code Generation</b>
//! 
//! First that is needed to know about compiler is that compiler never emits
//! machine code. It's used as a middleware between @c Assembler and your code.
//!
//! Example how to generate machine code:
//! 
//! @code
//! // Assembler instance is low level code generation class that emits 
//! // machine code.
//! Assembler a;
//!
//! // Compiler instance is high level code generation class that stores all
//! // instructions in internal representation.
//! Compiler c;
//!
//! // ... put your code using Compiler instance ...
//!
//! // Final step - generate code. AsmJit::Compiler::build() will serialize 
//! // all instructions into Assembler and this ensures generating real machine
//! // code.
//! c.build(a);
//! @endcode
//!
//! You can see that there is @c AsmJit::Compiler::build() function that emits
//! instruction into @c AsmJit::Assembler(). This layered architecture means 
//! that each class is used for something different and there is no code
//! duplication. You need @c AsmJit::Assembler in all cases, @c AsmJit::Compiler
//! is only comfortable way how to create more portable code in shorter time.
//!
//! <b>Functions</b>
//!
//! To build functions with @c Compiler, see AsmJit::Compiler::newFunction()
//! method.
//!
//! <b>Variables</b>
//!
//! Compiler also manages your variables and function arguments. Using manual
//! register allocation is not recommended way and it must be done carefully.
//! See @c AsmJit::VariableRef and related classes how to work with variables
//! and next example how to use AsmJit API to create function and manage them:
//!
//! @code
//! // Assembler instance
//! Assembler a;
//!
//! // Compiler and function declaration - void f(int*);
//! Compiler c;
//! Function& f = *c.newFunction(CALL_CONV_DEFAULT, BuildFunction1<int*>());
//!
//! // Get argument variable (it's pointer)
//! PtrRef a1(f.argument(0));
//!
//! // Create your variables
//! Int32Ref x1(f.newVariable(VARIABLE_TYPE_INT32));
//! Int32Ref x2(f.newVariable(VARIABLE_TYPE_INT32));
//!
//! // Init your variables
//! c.mov(x1.r(), 1);
//! c.mov(x2.r(), 2);
//!
//! // ... your code ...
//! c.add(x1.r(), x2.r());
//! // ... your code ...
//!
//! // Store result to a given pointer in first argument
//! c.mov(dword_ptr(a1.c()), x1.c());
//!
//! // Finish
//! c.endFunction();
//! c.build(a);
//! @endcode
//!
//! There was presented small code snippet with variables, but it's needed to 
//! explain it more. You can see that there are more variable types that can 
//! be used. Most useful variables that can be allocated to general purpose 
//! registers are variables wrapped to @c Int32Ref, @c Int64Ref, @c SysIntRef 
//! and @c PtrRef. Only @c Int64Ref is limited to 64 bit architecture. 
//! @c SysIntRef and @c PtrRef variables are equal and it's size depends to 
//! architecture (32 or 64 bits).
//!
//! Compiler is not using variables directly, instead you need to create the
//! function and create variables through @c AsmJit::Function. In code you will
//! always work with @c AsmJit::Compiler and @c AsmJit::Function together.
//!
//! Each variable contains state that describes where it is currently allocated
//! and if it's used. Life of variables is based on reference counting and if
//! variable is dereferenced to zero its life ends.
//!
//! Variable states:
//!
//! - Unused (@c AsmJit::VARIABLE_STATE_UNUSED) - State that is assigned to
//!   newly created variables or to not used variables (dereferenced to zero).
//! - In register (@c AsmJit::VARIABLE_STATE_REGISTER) - State that means that
//!   variable is currently allocated in register.
//! - In memory (@c AsmJit::VARIABLE_STATE_MEMORY) - State that means that
//!   variable is currently only in memory location.
//! 
//! When you create new variable, its state is always @c VARIABLE_STATE_UNUSED,
//! allocating it to register or spilling to memory changes this state to 
//! @c VARIABLE_STATE_REGISTER or @c VARIABLE_STATE_MEMORY, respectively. 
//! During variable lifetime it's usual that its state is changed multiple
//! times. To generate better code, you can control allocating and spilling
//! by using up to four types of methods that allows it (see next list).
//!
//! Explicit variable allocating / spilling methods:
//!
//! - @c VariableRef::alloc() - Explicit method to alloc variable into 
//!      register. You can use this before loops or code blocks.
//!
//! - @c VariableRef::spill() - Explicit method to spill variable. If variable
//!      is in register and you call this method, it's moved to its home memory
//!      location. If variable is not in register no operation is performed.
//!
//! Implicit variable allocating / spilling methods:
//!
//! - @c VariableRef::r() - Method used to allocate (if it's not previously
//!      allocated) variable to register for read / write. In most cases
//!      this is the right method to use in your code. If variable is in
//!      memory and you use this method it's allocated and moved to register.
//!      If variable is already in register or it's marked as unused this 
//!      method does nothing.
//! 
//! - @c VariableRef::x() - Method used to allocate variable for write only.
//!      In AsmJit this means completely overwrite it without using it's value.
//!      This method is helpful when you want to prevent from copying variable
//!      from memory to register to save one mov() instruction. If you want
//!      to clear or set your variable to something it's recommended to use
//!      @c VariableRef::x().
//!
//! - @c VariableRef::c() - Method used to use variable as a constant. 
//!      Constants means that you will not change that variable or you don't
//!      want to mark variable as changed. If variable is not marked as changed
//!      and spill happens you will save one mov() instruction that is needed
//!      to copy variable from register to its home address.
//!
//! - @c VariableRef::m() - Method used to access variable memory address. If
//!      variable is allocated in register and you call this method, it's 
//!      spilled, in all other cases it does nothing.
//!
//! Next example shows how allocating and spilling works:
//!
//! @code
//! // Small example to show how variable allocating and spilling works
//!
//! // Your compiler
//! Compiler c;
//!
//! // Your variable
//! Int32Ref var = ...;
//! 
//! // Make sure variable is spilled
//! var.spill();
//! 
//! // 1. Example: using var.r()
//! c.mov(var.r(), imm(0));
//! var.spill();
//! // Generated code:
//! //    mov var.reg, [var.home]
//! //    mov var.reg, 0
//! //    mov [var.home], var.reg
//! 
//! // 2. Example: using var.x()
//! c.mov(var.x(), imm(0));
//! var.spill();
//! // Generated code:
//! //    --- no alloc, .x() inhibits it.
//! //    mov var.reg, 0
//! //    mov [var.home], var.reg
//! 
//! // 3. Example: using var.c()
//! c.mov(var.c(), imm(0));
//! var.spill();
//! // Generated code:
//! //    mov var.reg, [var.home]
//! //    mov var.reg, 0
//! //    --- no spill, .c() means that you are not changing it, it's 'c'onstant
//! 
//! // 4. Example: using var.m()
//! c.mov(var.m(), imm(0));
//! var.spill();
//! // Generated code:
//! //    --- no alloc, because we are not allocating it
//! //    mov [var.home], 0
//! //    --- no spill, because variable is not allocated
//!
//! // 5. Example: using var.x(), setChanged()
//! c.mov(var.x(),imm(0));
//! var.setChanged(false);
//! var.spill();
//! // Generated code:
//! //    --- no alloc, .x() inhibits it.
//! //    mov var.reg, 0
//! //    --- no spill, setChanged(false) marked variable as unmodified
//! @endcode
//!
//! Please see AsmJit tutorials (testcompiler.cpp and testvariables.cpp) for 
//! more complete examples.
//!
//! <b>Intrinsics Extensions</b>
//!
//! Compiler supports extensions to intrinsics implemented in 
//! @c AsmJit::Serializer that enables to use variables in instructions without
//! specifying to use it as register or as memory operand. Sometimes is better
//! not to alloc variable for each read or write. There is limitation that you
//! can use variable without specifying if it's in register or in memory 
//! location only for one operand. This is because x86/x64 architecture not
//! allows to use two memory operands in one instruction and this could 
//! happen without this restriction (two variables in memory).
//!
//! @code
//! // Small example to show how intrinsics extensions works
//!
//! // Your compiler
//! Compiler c;
//!
//! // Your variable
//! Int32Ref var = ...;
//! 
//! // Make sure variable is spilled
//! var.spill();
//! 
//! // 1. Example: Allocated variable
//! var.alloc()
//! c.mov(var, imm(0));
//! var.spill();
//! // Generated code:
//! //    mov var.reg, [var.home]
//! //    mov var.reg, 0
//! //    mov [var.home], var.reg
//! 
//! // 2. Example: Memory variable
//! c.mov(var, imm(0));
//! var.spill();
//! // Generated code:
//! //    --- no alloc, we want variable in memory
//! //    mov [var.home], 0
//! //    --- no spill, becuase variable in not allocated
//! @endcode
//!
//! <b>Memory Management</b>
//!
//! @c Compiler Memory management follows these rules:
//! - Everything created by @c Compiler is always freed by @c Compiler.
//! - To get decent performance, compiler always uses larger buffer for 
//!   objects to allocate and when compiler instance is destroyed, this 
//!   buffer is freed. Destructors of active objects are called when 
//!   destroying compiler instance. Destructors of abadonded compiler
//!   objects are called immediately after abadonding it.
//!
//! This means that you can't use any @c Compiler object after destructing it,
//! it also means that each object like @c Label, @c Variable nad others are
//! created and managed by @c Compiler itself.
//!
//! <b>Differences summary to @c AsmJit::Assembler</b>
//!
//! - Instructions are not translated to machine code immediately.
//! - Each @c Label must be allocated by @c AsmJit::Compiler::newLabel().
//! - Contains function builder.
//! - Contains register allocator / variables management.
struct ASMJIT_API Compiler : public Serializer
{
  // -------------------------------------------------------------------------
  // [Typedefs]
  // -------------------------------------------------------------------------

  //! @brief List of emittables used in @c Compiler.
  typedef PodVector<Emittable*> EmittableList;
  //! @brief List of variables used in @c Compiler.
  typedef PodVector<Variable*> VariableList;
  //! @brief List of operands used in @c Compiler.
  typedef PodVector<Operand*> OperandList;

  // -------------------------------------------------------------------------
  // [Construction / Destruction]
  // -------------------------------------------------------------------------

  //! @brief Create new (empty) instance of @c Compiler.
  Compiler();
  //! @brief Destroy @c Compiler instance.
  virtual ~Compiler();

  // -------------------------------------------------------------------------
  // [Buffer]
  // -------------------------------------------------------------------------

  //! @brief Clear everything, but not deallocate buffers.
  //!
  //! @note This method will destroy your code.
  void clear();

  //! @brief Free internal buffer, all emitters and NULL all pointers.
  //!
  //! @note This method will destroy your code.
  void free();

  //! @brief Return list of emmitables (@c Emittable).
  //!
  //! This list contains all emittables that will be emitted throught 
  //! @c build() method into @c AsmJit::Assembler. Emittables are stored
  //! in FIFO order, so first stored emittable is emitted first. See
  //! @c AsmJit::Emittable inheritance diagram for available emittables.
  //!
  //! You will probably never use list of emittables yourself, but it's
  //! public to allow manipulations that is not available in 
  //! @c AsmJit::Assembler class. It's also used in @c build() method to emit
  //! them all in correct order.
  inline EmittableList& buffer() { return _buffer; }
  //! @overload.
  inline const EmittableList& buffer() const { return _buffer; }

  //! @brief Return current function.
  //!
  //! This method can be called within @c newFunction() and @c endFunction()
  //! block to get current function you are working with. It's recommended
  //! to store @c AsmJit::Function pointer returned by @c newFunction<> method,
  //! because this allows you in future implement function sections outside of
  //! function itself (yeah, this is possible!).
  inline Function* currentFunction() { return _currentFunction; }

  // -------------------------------------------------------------------------
  // [Logging]
  // -------------------------------------------------------------------------

  //! @brief Emit a single comment line into @c Assembler logger.
  //!
  //! Emitting comments are useful to log something. Because assembler can be
  //! generated from AST or other data structures, you may sometimes need to
  //! log data characteristics or statistics.
  //!
  //! @note Emitting comment is not directly sent to logger, but instead it's
  //! stored in @c AsmJit::Compiler and emitted when @c build() method is
  //! called with all instructions together in correct order.
  void comment(const char* fmt, ...);

  // -------------------------------------------------------------------------
  // [Function Builder]
  // -------------------------------------------------------------------------

  //! @brief Create a new function.
  //!
  //! @param cconv Calling convention to use (see @c CALL_CONV enum)
  //! @param params Function arguments prototype.
  //!
  //! This method is usually used as a first step when generating functions
  //! by @c Compiler. First parameter @a cconv specifies function calling
  //! convention to use. Second parameter @a params specifies function
  //! arguments. To create function arguments are used templates 
  //! @c BuildFunction0<>, @c BuildFunction1<...>, @c BuildFunction2<...>, 
  //! etc...
  //!
  //! Templates with BuildFunction prefix are used to generate argument IDs
  //! based on real C++ types. See next example how to generate function with
  //! two 32 bit integer arguments.
  //!
  //! @code
  //! // Building function using AsmJit::Compiler example.
  //!
  //! // Compiler instance
  //! Compiler c;
  //!
  //! // Begin of function (also emits function @c Prolog)
  //! Function& f = *c.newFunction(
  //!   // Default calling convention (32 bit cdecl or 64 bit for host OS)
  //!   CALL_CONV_DEFAULT,
  //!   // Using function builder to generate arguments list
  //!   BuildFunction2<int, int>());
  //!
  //! // End of function (also emits function @c Epilog)
  //! c.endFunction();
  //! @endcode
  //!
  //! You can see that building functions is really easy. Previous code snipped
  //! will generate code for function with two 32 bit integer arguments. You 
  //! can access arguments by @c AsmJit::Function::argument() method. Arguments
  //! are indexed from 0 (like everything in C).
  //!
  //! @code
  //! // Accessing function arguments through AsmJit::Function example.
  //!
  //! // Compiler instance
  //! Compiler c;
  //!
  //! // Begin of function (also emits function @c Prolog)
  //! Function& f = *c.newFunction(
  //!   // Default calling convention (32 bit cdecl or 64 bit for host OS)
  //!   CALL_CONV_DEFAULT,
  //!   // Using function builder to generate arguments list
  //!   BuildFunction2<int, int>());
  //!
  //! // Arguments are like other variables, you need to reference them by
  //! // VariableRef types:
  //! Int32Ref a0 = f.argument(0);
  //! Int32Ref a1 = f.argument(1);
  //!
  //! // To allocate them to registers just use .alloc(), .r(), .x() or .c() 
  //! // variable methods:
  //! c.add(a0.r(), a1.r());
  //!
  //! // End of function (also emits function @c Epilog)
  //! c.endFunction();
  //! @endcode
  //!
  //! Arguments are like variables. How to manipulate with variables is
  //! documented in @c AsmJit::Compiler detail and @c AsmJit::VariableRef 
  //! class.
  //!
  //! @note To get current function use @c currentFunction() method or save
  //! pointer to @c AsmJit::Function returned by @c AsmJit::Compiler::newFunction<>
  //! method. Recommended is to save the pointer.
  //!
  //! @sa @c BuildFunction0, @c BuildFunction1, @c BuildFunction2, ...
  template<typename T>
  Function* newFunction(UInt32 cconv, const T& params)
  { return newFunction_(cconv, params.args(), params.count()); }

  //! @brief Create a new function (low level version).
  //!
  //! @param cconv Function calling convention (see @c AsmJit::CALL_CONV).
  //! @param args Function arguments (see @c AsmJit::VARIABLE_TYPE).
  //! @param count Arguments count.
  //!
  //! This method is internally called from @c newFunction() method and 
  //! contains arguments thats used internally by @c AsmJit::Compiler.
  //!
  //! @note To get current function use @c currentFunction() method.
  Function* newFunction_(UInt32 cconv, const UInt32* args, SysUInt count);

  //! @brief Ends current function.
  Function* endFunction();

  //! @brief Create function prolog (function begin section).
  //!
  //! Function prologs and epilogs are standardized sequences of instructions
  //! thats used to build functions. If you are using @c Function and 
  //! @c AsmJit::Compiler::newFunction() to make a function, keep in mind that
  //! it creates prolog (by @c newFunction()) and epilog (by @c endFunction())
  //! for you.
  //!
  //! @note Never use prolog after @c newFunction() method. It will create
  //! prolog for you!
  //!
  //! @note Compiler can optimize prologs and epilogs.
  //!
  //! @sa @c Prolog, @c Function.
  Prolog* prolog(Function* f);

  //! @brief Create function epilog (function leave section).
  //!
  //! Function prologs and epilogs are standardized sequences of instructions
  //! thats used to build functions. If you are using @c Function and 
  //! @c AsmJit::Compiler::newFunction() to make a function, keep in mind that
  //! it creates prolog (by @c newFunction()) and epilog (by @c endFunction())
  //! for you.
  //!
  //! @note Never use epilog before @c endFunction() method. It will create
  //! epilog for you!
  //!
  //! @note Compiler can optimize prologs and epilogs.
  //!
  //! @sa @c Epilog, @c Function.
  Epilog* epilog(Function* f);

  // -------------------------------------------------------------------------
  // [Labels]
  // -------------------------------------------------------------------------

  //! @brief Create and return new @a Label managed by compiler.
  //!
  //! Labels created by compiler are same objects as labels created for 
  //! @c Assembler. There is only one limitation that if you are using 
  //! @c Compiler each label must be created by @c AsmJit::Compiler::newLabel()
  //! method.
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

  //! @brief Create object managed by compiler internal memory manager.
  template<typename T>
  inline T* newObject()
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this);
  }

  //! @brief Create object managed by compiler internal memory manager.
  template<typename T, typename P1>
  inline T* newObject(P1 p1)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1);
  }

  //! @brief Create object managed by compiler internal memory manager.
  template<typename T, typename P1, typename P2>
  inline T* newObject(P1 p1, P2 p2)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1, p2);
  }

  //! @brief Create object managed by compiler internal memory manager.
  template<typename T, typename P1, typename P2, typename P3>
  inline T* newObject(P1 p1, P2 p2, P3 p3)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1, p2, p3);
  }

  //! @brief Create object managed by compiler internal memory manager.
  template<typename T, typename P1, typename P2, typename P3, typename P4>
  inline T* newObject(P1 p1, P2 p2, P3 p3, P4 p4)
  {
    void* addr = _allocObject(sizeof(T));
    return new(addr) T(this, p1, p2, p3, p4);
  }

  //! @brief Allocate memory using compiler internal memory manager.
  inline void* _allocObject(SysUInt size)
  { return _zone.alloc(size); }

  //! @brief Internal function that registers operand @a op in compiler.
  //!
  //! Operand registration means adding @a op to internal operands list and 
  //! setting operand id.
  //!
  //! @note Operand @a op should by allocated by @c Compiler or you must
  //! guarantee that it will be not destroyed before @c Compiler is destroyed.
  void _registerOperand(Operand* op);

  // -------------------------------------------------------------------------
  // [Intrinsics]
  // -------------------------------------------------------------------------

  //! @brief Intrinsics helper method.
  //! @internal
  void op_var32(UInt32 code, const Int32Ref& a);

  //! @brief Intrinsics helper method.
  //! @internal
  void op_reg32_var32(UInt32 code, const Register& a, const Int32Ref& b);

  //! @brief Intrinsics helper method.
  //! @internal
  void op_var32_reg32(UInt32 code, const Int32Ref& a, const Register& b);

  //! @brief Intrinsics helper method.
  //! @internal
  void op_var32_imm(UInt32 code, const Int32Ref& a, const Immediate& b);

#if defined(ASMJIT_X64)
  //! @brief Intrinsics helper method.
  //! @internal
  void op_var64(UInt32 code, const Int64Ref& a);

  //! @brief Intrinsics helper method.
  //! @internal
  void op_reg64_var64(UInt32 code, const Register& a, const Int64Ref& b);

  //! @brief Intrinsics helper method.
  //! @internal
  void op_var64_reg64(UInt32 code, const Int64Ref& a, const Register& b);

  //! @brief Intrinsics helper method.
  //! @internal
  void op_var64_imm(UInt32 code, const Int64Ref& a, const Immediate& b);
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

  //! @brief Zone memory management.
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
