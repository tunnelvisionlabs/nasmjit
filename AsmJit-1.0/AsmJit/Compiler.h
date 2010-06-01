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
#ifndef _ASMJIT_COMPILER_H
#define _ASMJIT_COMPILER_H

// [Dependencies]
#include "Build.h"
#include "Defs.h"
#include "Operand.h"
#include "Util.h"

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Assembler;
struct Compiler;
struct CompilerContext;
struct CompilerCore;
struct CompilerIntrinsics;

struct ForwardJumpData;

struct VarData;
struct VarAllocRecord;
struct StateData;

struct Emittable;
struct EAlign;
struct EComment;
struct EData;
struct EEpilog;
struct EFunction;
struct EInstruction;
struct EJmpInstruction;
struct EProlog;

// ============================================================================
// [AsmJit::TypeToId]
// ============================================================================

#if defined(ASMJIT_HAS_PARTIAL_TEMPLATE_SPECIALIZATION)

template<typename T>
struct TypeToId
{
#if defined(ASMJIT_DOXYGEN)
  enum { Id = INVALID_VALUE };
#endif // ASMJIT_DOXYGEN
};

template<typename T>
struct TypeToId<T*> { enum { Id = VARIABLE_TYPE_PTR }; };

#else

// Same trict is used in Qt, Boost, Fog and all other libraries that need
// something similar.
//
// It's easy. It's needed to use sizeof() to determine the size
// of return value of this function. If size will be sizeof(char)
// (this is our type) then type is pointer, otherwise it's not.
template<typename T>
char TypeToId_NoPtiHelper(T*(*)());
// And specialization.
void* TypeToId_NoPtiHelper(...);

template<typename T>
struct TypeToId
{
  // TypeInfo constants
  enum
  {
    // This is the hackery result.
    Id = (sizeof(char) == sizeof( TypeToId_NoPtiHelper((T(*)())0) )
      ? VARIABLE_TYPE_PTR
      : INVALID_VALUE)
  };
};

#endif // ASMJIT_HAS_PARTIAL_TEMPLATE_SPECIALIZATION

#define ASMJIT_DECLARE_TYPE_AS_ID(__T__, __Id__) \
  template<> \
  struct TypeToId<__T__> { enum { Id = __Id__ }; }

// ============================================================================
// [AsmJit::Function Builder]
// ============================================================================

//! @brief Class used to build function without arguments.
struct FunctionBuilder0
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    return NULL;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 0;
  }
};

//! @brief Class used to build function with 1 argument.
template<typename P0>
struct FunctionBuilder1
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 1;
  }
};

//! @brief Class used to build function with 2 arguments.
template<typename P0, typename P1>
struct FunctionBuilder2
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 2;
  }
};

//! @brief Class used to build function with 3 arguments.
template<typename P0, typename P1, typename P2>
struct FunctionBuilder3
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 3;
  }
};

//! @brief Class used to build function with 4 arguments.
template<typename P0, typename P1, typename P2, typename P3>
struct FunctionBuilder4
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 4;
  }
};

//! @brief Class used to build function with 5 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4>
struct FunctionBuilder5
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id,
      TypeToId<P4>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 5;
  }
};

//! @brief Class used to build function with 6 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunctionBuilder6
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id,
      TypeToId<P4>::Id,
      TypeToId<P5>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 6;
  }
};

//! @brief Class used to build function with 7 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunctionBuilder7
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id,
      TypeToId<P4>::Id,
      TypeToId<P5>::Id,
      TypeToId<P6>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 7;
  }
};

//! @brief Class used to build function with 8 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunctionBuilder8
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id,
      TypeToId<P4>::Id,
      TypeToId<P5>::Id,
      TypeToId<P6>::Id,
      TypeToId<P7>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 8;
  }
};

//! @brief Class used to build function with 9 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
struct FunctionBuilder9
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id,
      TypeToId<P4>::Id,
      TypeToId<P5>::Id,
      TypeToId<P6>::Id,
      TypeToId<P7>::Id,
      TypeToId<P8>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 9;
  }
};

//! @brief Class used to build function with 9 arguments.
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
struct FunctionBuilder10
{
  inline const uint32_t* getArgs() const ASMJIT_NOTHROW
  {
    static const uint32_t data[] =
    {
      TypeToId<P0>::Id,
      TypeToId<P1>::Id,
      TypeToId<P2>::Id,
      TypeToId<P3>::Id,
      TypeToId<P4>::Id,
      TypeToId<P5>::Id,
      TypeToId<P6>::Id,
      TypeToId<P7>::Id,
      TypeToId<P8>::Id,
      TypeToId<P9>::Id
    };
    return data;
  }

  inline uint32_t getCount() const ASMJIT_NOTHROW
  {
    return 10;
  }
};

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

//! @brief Emmitable.
//!
//! Emittable is object that can emit single or more instructions. To
//! create your custom emittable it's needed to override abstract virtual
//! method @c emit().
//!
//! When you are finished serializing instructions to the @c Compiler and you
//! call @c Compiler::make(), it will first call @c prepare() method for each
//! emittable in list, then @c emit() and then @c post(). Prepare can be
//! used to calculate something that can be only calculated when everything
//! is finished (for example @c Function uses @c prepare() to relocate memory
//! home for all memory/spilled variables). @c emit() should be used to emit
//! instruction or multiple instructions into @a Assembler stream, and
//! @c post() is here to allow emitting embedded data (after function
//! declaration), etc.
struct ASMJIT_API Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create new emittable.
  //!
  //! Never create @c Emittable by @c new operator or on the stack, use
  //! @c Compiler::newObject template to do that.
  Emittable(Compiler* c, uint32_t type) ASMJIT_NOTHROW;

  //! @brief Destroy emittable.
  //!
  //! @note Never destroy emittable using @c delete keyword, @c Compiler
  //! manages all emittables in internal memory pool and it will destroy
  //! all emittables after you destroy it.
  virtual ~Emittable() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit and Helpers]
  // --------------------------------------------------------------------------

  //! @brief Step 1. Extract emittable variables, update statistics, ...
  virtual void prepare(CompilerContext& c) ASMJIT_NOTHROW;
  //! @brief Step 2. Translate instruction, alloc variables, ...
  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;
  //! @brief Step 3. Emit to @c Assembler.
  virtual void emit(Assembler& a) ASMJIT_NOTHROW;
  //! @brief Step 4. Last post step (verify, add data, etc).
  virtual void post(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Type / Offset]
  // --------------------------------------------------------------------------

  //! @brief Get emittable type, see @c EMITTABLE_TYPE.
  inline uint32_t getType() const ASMJIT_NOTHROW { return _type; }

  //! @brief Get emittable offset in the stream
  //!
  //! Emittable offset is not byte offset, each emittable increments offset by 1
  //! and this value is then used by register allocator. Emittable offset is
  //! set by compiler by the register allocator, don't use it in your code.
  inline uint32_t getOffset() const ASMJIT_NOTHROW { return _offset; }

  // --------------------------------------------------------------------------
  // [Compiler]
  // --------------------------------------------------------------------------

  //! @brief Get associated compiler instance.
  inline Compiler* getCompiler() const ASMJIT_NOTHROW { return _compiler; }

  // --------------------------------------------------------------------------
  // [Emittables List]
  // --------------------------------------------------------------------------

  //! @brief Get previous emittable in list.
  inline Emittable* getPrev() const ASMJIT_NOTHROW { return _prev; }
  //! @brief Get next emittable in list.
  inline Emittable* getNext() const ASMJIT_NOTHROW { return _next; }

  // --------------------------------------------------------------------------
  // [Comment]
  // --------------------------------------------------------------------------

  //! @brief Get comment string.
  inline const char* getComment() const ASMJIT_NOTHROW { return _comment; }

  //! @brief Set comment string to @a str.
  void setComment(const char* str) ASMJIT_NOTHROW;

  //! @brief Format comment string using @a fmt string and variable argument list.
  void setCommentF(const char* fmt, ...) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Compiler where this emittable is connected to.
  Compiler* _compiler;

  //! @brief Type of emittable, see @c EMITTABLE_TYPE.
  uint32_t _type;
  //! @brief Emittable offset.
  uint32_t _offset;

  //! @brief Previous emittable.
  Emittable* _prev;
  //! @brief Next emittable.
  Emittable* _next;

  //! @brief Embedded comment string (also used by @c Comment emittable).
  const char* _comment;

private:
  friend struct CompilerCore;

  ASMJIT_DISABLE_COPY(Emittable)
};

// ============================================================================
// [AsmJit::EComment]
// ============================================================================

//! @brief Emittable used to emit comment into @c Assembler logger.
//!
//! Comments allows to comment your assembler stream for better debugging
//! and visualization what's happening. Comments are ignored if logger is
//! not set.
//!
//! Comment string can't be modified after comment was created.
struct ASMJIT_API EComment : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EComment(Compiler* c, const char* comment) ASMJIT_NOTHROW;
  virtual ~EComment() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void emit(Assembler& a) ASMJIT_NOTHROW;

private:
  friend struct CompilerCore;

  ASMJIT_DISABLE_COPY(EComment)
};

// ============================================================================
// [AsmJit::EData]
// ============================================================================

//! @brief Emittable used to emit comment into @c Assembler logger.
//!
//! @note This class is always allocated by @c AsmJit::Compiler.
struct ASMJIT_API EData : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EData(Compiler* c, const void* data, sysuint_t length) ASMJIT_NOTHROW;
  virtual ~EData() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void emit(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get pointer to embedded data.
  uint8_t* getData() const ASMJIT_NOTHROW { return (uint8_t*)_data; }

  //! @brief Get length of embedded data.
  sysuint_t getLength() const ASMJIT_NOTHROW { return _length; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  sysuint_t _length;
  uint8_t _data[sizeof(void*)];

  friend struct CompilerCore;

  ASMJIT_DISABLE_COPY(EData)
};

// ============================================================================
// [AsmJit::EAlign]
// ============================================================================

//! @brief Emittable used to align assembler code.
struct ASMJIT_API EAlign : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EAlign(Compiler* c, uint32_t size = 0) ASMJIT_NOTHROW;
  virtual ~EAlign() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void emit(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Align Size]
  // --------------------------------------------------------------------------

  //! @brief Get align size in bytes.
  inline uint32_t getSize() const ASMJIT_NOTHROW { return _size; }
  //! @brief Set align size in bytes to @a size.
  inline void setSize(uint32_t size) ASMJIT_NOTHROW { _size = size; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  uint32_t _size;

  friend struct CompilerCore;

  ASMJIT_DISABLE_COPY(EAlign)
};

// ============================================================================
// [AsmJit::ETarget]
// ============================================================================

//! @brief Target - the bound label.
struct ASMJIT_API ETarget : public Emittable
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ETarget(Compiler* c, const Label& target) ASMJIT_NOTHROW;
  virtual ~ETarget() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void translate(CompilerContext& c) ASMJIT_NOTHROW;
  virtual void emit(Assembler& a) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Return label bound to this target.
  inline const Label& getLabel() const ASMJIT_NOTHROW { return _label; }

  //! @brief Get first jmp instruction.
  inline EJmpInstruction* getFrom() const ASMJIT_NOTHROW { return _from; }

  //! @brief Get register allocator state for this target.
  inline StateData* getState() const ASMJIT_NOTHROW { return _state; }

  //! @brief Get number of jumps to this target.
  inline uint32_t getJumpsCount() const ASMJIT_NOTHROW { return _jumpsCount; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  Label _label;
  EJmpInstruction* _from;
  StateData* _state;

  uint32_t _jumpsCount;

  friend struct CompilerContext;
  friend struct CompilerCore;
  friend struct EInstruction;
  friend struct EJmpInstruction;
};

} // AsmJit namespace

// [Api-End]
#include "ApiEnd.h"

// ============================================================================
// [Platform Specific]
// ============================================================================

// [X86 / X64]
#if defined(ASMJIT_X86) || defined(ASMJIT_X64)
#include "CompilerX86X64.h"
#endif // ASMJIT_X86 || ASMJIT_X64

// [Guard]
#endif // _ASMJIT_COMPILER_H
