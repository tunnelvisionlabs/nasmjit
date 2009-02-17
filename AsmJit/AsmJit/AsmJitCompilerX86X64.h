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

namespace AsmJit {

// forward declarations
struct Compiler;
struct Emittable;
struct Epilogue;
struct Function;
struct Instruction;
struct Prologue;
struct Variable;

// typedefs
typedef PodVector<Emittable*> EmittableList;
typedef PodVector<Variable*> VariableList;

//! @addtogroup AsmJit_Compiler
//! @{

//! @brief Emmitable type.
enum EMITTABLE_TYPE
{
  //! @brief Emittable is invalid (can't be used).
  EMITTABLE_NONE = 0,
  //! @brief Emittable is single instruction.
  EMITTABLE_INSTRUCTION = 1,
  //! @brief Emittable is block of instructions.
  EMITTABLE_BLOCK = 2,
  //! @brief Emittable is function declaration.
  EMITTABLE_FUNCTION = 3,
  //! @brief Emittable is function prologue.
  EMITTABLE_PROLOGUE = 4,
  //! @brief Emittable is function epilogue.
  EMITTABLE_EPILOGUE = 5
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

  //! @def CALL_CONV_PREFERRED
  //! @brief Default calling convention for current platform / operating 
  //! system.

#if defined(ASMJIT_X86)
  CALL_CONV_PREFERRED = CALL_CONV_CDECL
#else
# if defined(WIN23) || defined(_WIN32) || defined(WINDOWS)
  CALL_CONV_PREFERRED = CALL_CONV_X64W
# else
  CALL_CONV_PREFERRED = CALL_CONV_X64U
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
  ARGUMENT_LEFT_TO_RIGHT = 0,
  //! @brief Arguments are passer right ro left
  //!
  //! This is default argument direction in C programming.
  ARGUMENT_RIGHT_TO_LEFT = 1
};

//! @brief Argument type.
enum ARGUMENT_TYPE
{
  //! @brief Invalid argument type (don't use).
  ARGUMENT_NONE = 0,

  //! @brief Argument in integer (Int32).
  ARGUMENT_INT = 1,
  //! @brief Argument in unsigned integer (UInt32).
  ARGUMENT_UINT = 2,

  //! @def ARGUMENT_SYSINT
  //! @brief Argument in system wide integer (Int32 or Int64).
  //! @def ARGUMENT_SYSUINT
  //! @brief Argument in system wide unsigned integer (UInt32 or UInt64).
#if defined(ASMJIT_X86)
  ARGUMENT_SYSINT = 1,
  ARGUMENT_SYSUINT = 2,
#else
  ARGUMENT_SYSINT = 3,
  ARGUMENT_SYSUINT = 4,
#endif

  //! @brief Argument is SP-FP number (float).
  ARGUMENT_FLOAT = 5,
  //! @brief Argument is DP-FP number (double).
  ARGUMENT_DOUBLE = 6,

  //! @brief Argument is pointer or reference to any type.
  ARGUMENT_PTR
};

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

private:
  //! @brief Compiler where this emittable is associated to.
  Compiler* _compiler;
  //! @brief Type of emittable, see @c EMITTABLE_TYPE.
  UInt32 _type;

  // disable copy
  inline Emittable(const Emittable& other);
  inline Emittable& operator=(const Emittable& other);
};

//! @brief Instruction emittable.
struct ASMJIT_API Instruction : public Emittable
{
  Instruction(Compiler* c);
  virtual ~Instruction();

  //! @brief Return instruction code, see @c INST_CODE.
  inline UInt32 code() const { return _code; }

  //! @brief Return array of operands (3 operands total).
  inline Operand* ops() { return _o; }

  //! @brief Return first instruction operand.
  inline Operand& o1() { return _o[0]; }
  //! @brief Return first instruction operand.
  inline const Operand& o1() const { return _o[0]; }

  //! @brief Return second instruction operand.
  inline Operand& o2() { return _o[1]; }
  //! @brief Return second instruction operand.
  inline const Operand& o2() const { return _o[1]; }

  //! @brief Return third instruction operand.
  inline Operand& o3() { return _o[2]; }
  //! @brief Return third instruction operand.
  inline const Operand& o3() const { return _o[2]; }

  inline void setInst(UInt32 code)
  {
    _code = code;
    memset(&_o[0], 0, sizeof(Operand)*3);
  }

  inline void setInst(UInt32 code, const Operand& o1)
  {
    _code = code;
    _o[0] = o1;
    memset(&_o[1], 0, sizeof(Operand)*2);
  }

  inline void setInst(UInt32 code, const Operand& o1, const Operand& o2)
  {
    _code = code;
    _o[0] = o1;
    _o[1] = o2;
    memset(&_o[2], 0, sizeof(Operand)*1);
  }

  inline void setInst(UInt32 code, const Operand& o1, const Operand& o2, const Operand& o3)
  {
    _code = code;
    _o[0] = o1;
    _o[1] = o2;
    _o[2] = o3;
  }

private:
  //! @brief Instruction code, see @c INST_CODE.
  UInt32 _code;
  //! @brief Instruction operands.
  Operand _o[3];
};

//! @brief Function emittable.
struct ASMJIT_API Function : public Emittable
{
  struct Arg;

  Function(Compiler* c);
  virtual ~Function();

  virtual void emit(Assembler& a);

  //! @brief Returns @c true if function contains prologue and epilogue.
  bool hasPrologEpilog();

  //! @brief Sets function calling convention.
  void setCallingConvention(UInt32 cconv);

  //! @brief Sets function arguments (must be done after correct calling 
  //! convention is set).
  void setArguments(const UInt32* args, SysUInt len);

  //! @brief Returns size of alignment on the stack.
  //!
  //! Stack is aligned to 16 bytes by default. For X64 platforms there is 
  //! no extra code needed to align stack to 16 bytes, because it's default
  //! stack alignment.
  inline SysInt alignmentSize() const { return _alignmentSize; }

  //! @brief Returns size of variables on the stack.
  //!
  //! This variable is always aligned to 16 bytes for each platform.
  inline SysInt variablesSize() const { return _variablesSize; }

  //! @brief Returns function calling convention, see @c CALL_CONV.
  inline UInt32 callingConvention() const { return _callingConvention; }

  //! @brief Returns @c true if callee pops the stack by ret() instruction.
  //!
  //! Stdcall calling convention is designed to pop the stack by callee,
  //! but all calling conventions used in MSVC extept cdecl does that.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 calleePopsStack() const { return _calleePopsStack; }

  //! @brief Returns registers used to pass first integer parameters by current
  //! calling convention.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline const UInt32* argumentsGpnRegisters() const { return _argumentsGpnRegisters; }

  //! @brief Returns registers used to pass first SP-FP or DP-FPparameters by 
  //! current calling convention.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline const UInt32* argumentsSseRegisters() const { return _argumentsSseRegisters; }

  //! @brief Returns bitmask of general purpose registers that's preserved 
  //! (non-volatile).
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 preserveGpnRegisters() const { return _preserveGpnRegisters; }
  //! @brief Returns bitmask of sse registers that's preserved (non-volatile).
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 preserveSseRegisters() const { return _preserveSseRegisters; }

  //! @brief Returns direction of arguments passed on the stack.
  //!
  //! Direction should be always @c ARGUMENT_RIGHT_TO_LEFT.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  inline UInt32 argumentsDirection() const { return _argumentsDirection; }

  //! @brief Returns list of arguments.
  inline const Arg* argumentsList() const { return _argumentsList; }

  //! @brief Returns count of arguments.
  inline UInt32 argumentsCount() const { return _argumentsCount; }

  //! @brief Returns stack size of all function arguments (passed on the 
  //! stack).
  inline UInt32 argumentsStackSize() const { return _argumentsStackSize; }

  //! @brief Returns bitmask of all used general purpose registers.
  inline UInt32 usedGpnRegisters() const { return _usedGpnRegisters; }
  //! @brief Returns bitmask of all used mmx registers.
  inline UInt32 usedMmxRegisters() const { return _usedMmxRegisters; }
  //! @brief Returns bitmask of all used sse registers.
  inline UInt32 usedSseRegisters() const { return _usedSseRegisters; }

  //! @brief Returns bitmask of all changed general purpose registers during
  //! function execution (for generating optimized prologue / epilogue).
  inline UInt32 changedGpnRegisters() const { return _changedGpnRegisters; }
  //! @brief Returns bitmask of all changed mmx registers during
  //! function execution (for generating optimized prologue / epilogue).
  inline UInt32 changedMmxRegisters() const { return _changedMmxRegisters; }
  //! @brief Returns bitmask of all changed sse registers during
  //! function execution (for generating optimized prologue / epilogue).
  inline UInt32 changedSseRegisters() const { return _changedSseRegisters; }

  //! @brief Size of alignment (on the stack).
  SysInt _alignmentSize;
  //! @brief Size of variables (on the stack).
  SysInt _variablesSize;

  //! @brief Calling convention, see @c CALL_CONV.
  UInt32 _callingConvention;
  //! @brief Callee pops stack;
  UInt32 _calleePopsStack;
  //! @brief List of registers that's used for first INT arguments instead of stack.
  UInt32 _argumentsGpnRegisters[16];
  //! @brief List of registers that's used for first FP arguments instead of stack.
  UInt32 _argumentsSseRegisters[16];

  //! @brief Bitmask for preserved general purpose registers.
  UInt32 _preserveGpnRegisters;
  //! @brief Bitmask for preserved sse registers.
  UInt32 _preserveSseRegisters;

  //! @brief Direction for arguments passed on stack, see @c ARGUMENT_DIR.
  UInt32 _argumentsDirection;

  //! @brief Structure used to manage function arguments and their current
  //! state.
  //!
  //! It's designed also to enable saving / restoring state in custom code
  //! blocks.
  struct Arg
  {
    //! @brief Type of argument, see @c ARGUMENT_TYPE.
    UInt32 type;
    //! @brief Register if argument is passed in (see @c CALL_CONV).
    //!
    //! If argument is not passed in register, value is @c 0xFFFFFFFF.
    UInt32 reg;
    //! @brief Stack offset that can be used if argument is not passed
    //! in register.
    Int32 stackOffset;
  };

  //! @brief List of arguments (maximum is 32 arguments).
  Arg _argumentsList[32];

  //! @brief Count of arguments (in @c _argumentsList).
  UInt32 _argumentsCount;

  //! @brief Count of bytes consumed by arguments on the stack.
  UInt32 _argumentsStackSize;

  UInt32 _usedGpnRegisters;
  UInt32 _usedMmxRegisters;
  UInt32 _usedSseRegisters;

  UInt32 _changedGpnRegisters;
  UInt32 _changedMmxRegisters;
  UInt32 _changedSseRegisters;

  PodVector<Variable*> _variables;
};

//! @brief Prologue emittable.
struct ASMJIT_API Prologue : public Emittable
{
  Prologue(Compiler* c, Function* f);
  virtual ~Prologue();

  virtual void emit(Assembler& a);

  inline Function* function() const { return _function; }

private:
  Function* _function;
};

//! @brief Epilogue emittable.
struct ASMJIT_API Epilogue : public Emittable
{
  Epilogue(Compiler* c, Function* f);
  virtual ~Epilogue();

  virtual void emit(Assembler& a);

  inline Function* function() const { return _function; }

private:
  Function* _function;
};

//! @brief Variable.
struct ASMJIT_API Variable
{
  Variable(Compiler* c, Function* f);
  virtual ~Variable();

  inline Compiler* compiler() const { return _compiler; }
  inline Function* function() const { return _function; }

  inline UInt8 type() const { return _type; }
  inline UInt8 size() const { return _size; }
  inline UInt8 mapped() const { return _mapped; }
  inline UInt8 unmapPriority() const { return _unmapPriority; }

  inline SysInt index() const { return _index; }

  Compiler* _compiler;
  Function* _function;

  UInt8 _type;
  UInt8 _size;
  UInt8 _mapped;
  UInt8 _unmapPriority;
  SysInt _index;

private:
  // disable copy
  inline Variable(const Variable& other);
  inline Variable& operator=(const Variable& other);
};

//! @brief Compiler.
//!
//! This class is used to store instruction stream and allows to modify
//! it on the fly. It uses different concept than @c AsmJit::Assembler class
//! and in fact @c AsmJit::Assembler is only used as a backend.
//!
//! Using of this class will generate slower code than using 
//! @c AsmJit::Assembler, but in some situations it can be more powerful and 
//! it can result in more readable code, because @c AsmJit::Compiler contains 
//! also register allocator and you can use variables instead of hardcoding 
//! registers.
struct ASMJIT_API Compiler
{
  // -------------------------------------------------------------------------
  // [Construction / Destruction]
  // -------------------------------------------------------------------------

  Compiler();
  virtual ~Compiler();

  // -------------------------------------------------------------------------
  // [Internal Buffer]
  // -------------------------------------------------------------------------

  //! @brief Clear everything, but not deallocate buffers.
  void clear();

  //! @brief Free internal buffer, all emitters and NULL all pointers.
  void free();

  inline EmittableList& buffer() { return _buffer; }
  inline const EmittableList& buffer() const { return _buffer; }

  //! @brief Returns current function.
  //!
  //! Use @c beginFunction() and @c endFunction() methods to begin / end
  //! function block. Each function must be also started by @c prologue()
  //! and ended by @c epilogue() calls.
  inline Function* currentFunction() { return _currentFunction; }

  // -------------------------------------------------------------------------
  // [Function Builder]
  // -------------------------------------------------------------------------

  //! @brief Begins a new function.
  //!
  //! @note To get current function use @c currentFunction() method.
  Function* beginFunction(UInt32 callingConvention);

  //! @brief Ends current function.
  Function* endFunction();

  //! @brief Create function prologue (begins a function).
  //!
  //! @note Compiler can optimize prologues and epilogues.
  Prologue* prologue();

  //! @brief Create function epilogue (ends a function).
  //!
  //! @note Compiler can optimize prologues and epilogues.
  Epilogue* epilogue();

  // -------------------------------------------------------------------------
  // [Emit]
  // -------------------------------------------------------------------------

  void emit(Emittable* emittable, bool endblock = false);

  //! @brief Method that will emit everything to @c Assembler instance @a a.
  void build(Assembler& a);

  // -------------------------------------------------------------------------
  // [Memory Management]
  // -------------------------------------------------------------------------
#if 0
  // Only planned

  // Memory management in compiler has these rules:
  // - Everything created by compiler is freed by compiler.
  // - To get decent performance, compiler always uses larger buffer for 
  //   objects to allocate and when compiler instance is destroyed, this 
  //   buffer is freed. Destructors of created objects are never called.
  // REVISION NEEDED: This concept is not good, it's needed to call destructors

  template<typename T>
  inline T* newObject()
  {
    void* addr = _allocObject(sizeof(T));
    return
  }

  void* _allocObject(SysUInt size);
#endif

  // -------------------------------------------------------------------------
  // [Variables]
  // -------------------------------------------------------------------------

private:
  //! @brief List of emittables.
  EmittableList _buffer;

  //! @brief Position in @c _buffer.
  SysInt _currentPosition;

  //! @brief Current function.
  Function* _currentFunction;

private:
  // disable copy
  inline Compiler(const Compiler& other);
  inline Compiler& operator=(const Compiler& other);
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITCOMPILERX86X64_H
