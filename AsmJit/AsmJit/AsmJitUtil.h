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
#ifndef _ASMJITUTIL_H
#define _ASMJITUTIL_H

// [Dependencies]
#include "AsmJitBuild.h"

#include <stdlib.h>
#include <string.h>

namespace AsmJit {

//! @addtogroup AsmJit_Util
//! @{

// ============================================================================
// [AsmJit::function_cast<>]
// ============================================================================

//! @brief Cast used to cast pointer to function. It's like reinterpret_cast<>, 
//! but uses internally C style cast to work with MinGW.
template<typename T, typename Z>
static inline T function_cast(Z* p) { return (T)p; }

// ============================================================================
// [AsmJit::isIntX]
// ============================================================================

//! @brief Returns @c true if a given integer @a x is signed 8 bit integer
static inline bool isInt8(SysInt x) { return x >= -128 && x <= 127; }
//! @brief Returns @c true if a given integer @a x is unsigned 8 bit integer
static inline bool isUInt8(SysInt x) { return x >= 0 && x <= 255; }

//! @brief Returns @c true if a given integer @a x is signed 16 bit integer
static inline bool isInt16(SysInt x) { return x >= -32768 && x <= 32767; }
//! @brief Returns @c true if a given integer @a x is unsigned 16 bit integer
static inline bool isUInt16(SysInt x) { return x >= 0 && x <= 65535; }

//! @brief Returns @c true if a given integer @a x is signed 16 bit integer
static inline bool isInt32(SysInt x)
{
#if defined(ASMJIT_X86)
  return true;
#else
  return x >= ASMJIT_INT64_C(-2147483648) && x <= ASMJIT_INT64_C(2147483647);
#endif
}
//! @brief Returns @c true if a given integer @a x is unsigned 16 bit integer
static inline bool isUInt32(SysInt x)
{
#if defined(ASMJIT_X86)
  return x >= 0;
#else
  return x >= 0 && x <= ASMJIT_INT64_C(4294967295);
#endif
}

// ============================================================================
// [AsmJit::floatAsInt32, int32AsFloat]
// ============================================================================

//! @brief used to cast float to 32 bit integer and vica versa.
//!
//! @internal
union I32FPUnion
{
  Int32 i;
  float f;
};

//! @brief used to cast double to 64 bit integer and vica versa.
//!
//! @internal
union I64FPUnion
{
  Int64 i;
  double f;
};

//! @brief Binary casts 32 bit integer to float.
static inline float int32AsFloat(Int32 i)
{
  I32FPUnion u;
  u.i = i;
  return u.f;
}

//! @brief Binary casts float to 32 bit integer.
static inline Int32 floatAsInt32(float f)
{
  I32FPUnion u;
  u.f = f;
  return u.i;
}

//! @brief Binary casts 64 bit integer to double.
static inline double int64AsDouble(Int64 i)
{
  I64FPUnion u;
  u.i = i;
  return u.f;
}

//! @brief Binary casts double to 64 bit integer.
static inline Int64 doubleAsInt64(double f)
{
  I64FPUnion u;
  u.f = f;
  return u.i;
}

// ============================================================================
// [AsmJit::(X)MMData]
// ============================================================================

//! @brief Structure used for MMX specific data (64 bits).
union MMData
{
  Int8   sb[8];
  UInt8  ub[8];
  Int16  sw[4];
  UInt16 uw[4];
  Int32  sd[2];
  UInt32 ud[2];
  Int64  sq[1];
  UInt64 uq[1];

  float  sf[2];

  inline void set_sb(Int8 x0, Int8 x1, Int8 x2, Int8 x3, Int8 x4, Int8 x5, Int8 x6, Int8 x7)
  { sb[0] = x0; sb[1] = x1; sb[2] = x2; sb[3] = x3; sb[4] = x4; sb[5] = x5; sb[6] = x6; sb[7] = x7; }

  inline void set_ub(UInt8 x0, UInt8 x1, UInt8 x2, UInt8 x3, UInt8 x4, UInt8 x5, UInt8 x6, UInt8 x7)
  { ub[0] = x0; ub[1] = x1; ub[2] = x2; ub[3] = x3; ub[4] = x4; ub[5] = x5; ub[6] = x6; ub[7] = x7; }

  inline void set_sw(Int16 x0, Int16 x1, Int16 x2, Int16 x3)
  { sw[0] = x0; sw[1] = x1; sw[2] = x2; sw[3] = x3; }

  inline void set_uw(UInt16 x0, UInt16 x1, UInt16 x2, UInt16 x3)
  { uw[0] = x0; uw[1] = x1; uw[2] = x2; uw[3] = x3; }

  inline void set_sd(Int32 x0, Int32 x1)
  { sd[0] = x0; sd[1] = x1; }

  inline void set_ud(UInt32 x0, UInt32 x1)
  { ud[0] = x0; ud[1] = x1; }

  inline void set_sd(Int64 x0)
  { sq[0] = x0; }

  inline void set_ud(UInt64 x0)
  { uq[0] = x0; }

  inline void set_sf(float x0, float x1)
  { sf[0] = x0; sf[1] = x1; }
};

//! @brief Structure used for SSE specific data (128 bits).
union XMMData
{
  Int8   sb[16];
  UInt8  ub[16];
  Int16  sw[8];
  UInt16 uw[8];
  Int32  sd[4];
  UInt32 ud[4];
  Int64  sq[2];
  UInt64 uq[2];

  float  sf[4];
  double df[2];

  inline void set_sb(
    Int8 x0, Int8 x1, Int8 x2 , Int8 x3 , Int8 x4 , Int8 x5 , Int8 x6 , Int8 x7 ,
    Int8 x8, Int8 x9, Int8 x10, Int8 x11, Int8 x12, Int8 x13, Int8 x14, Int8 x15)
  {
    sb[0] = x0; sb[1] = x1; sb[ 2] = x2 ; sb[3 ] = x3 ; sb[4 ] = x4 ; sb[5 ] = x5 ; sb[6 ] = x6 ; sb[7 ] = x7 ;
    sb[8] = x8; sb[9] = x9; sb[10] = x10; sb[11] = x11; sb[12] = x12; sb[13] = x13; sb[14] = x14; sb[15] = x15; 
  }

  inline void set_ub(
    UInt8 x0, UInt8 x1, UInt8 x2 , UInt8 x3 , UInt8 x4 , UInt8 x5 , UInt8 x6 , UInt8 x7 ,
    UInt8 x8, UInt8 x9, UInt8 x10, UInt8 x11, UInt8 x12, UInt8 x13, UInt8 x14, UInt8 x15)
  {
    ub[0] = x0; ub[1] = x1; ub[ 2] = x2 ; ub[3 ] = x3 ; ub[4 ] = x4 ; ub[5 ] = x5 ; ub[6 ] = x6 ; ub[7 ] = x7 ;
    ub[8] = x8; ub[9] = x9; ub[10] = x10; ub[11] = x11; ub[12] = x12; ub[13] = x13; ub[14] = x14; ub[15] = x15; 
  }

  inline void set_sw(Int16 x0, Int16 x1, Int16 x2, Int16 x3, Int16 x4, Int16 x5, Int16 x6, Int16 x7)
  { sw[0] = x0; sw[1] = x1; sw[2] = x2; sw[3] = x3; sw[4] = x4; sw[5] = x5; sw[6] = x6; sw[7] = x7; }

  inline void set_uw(UInt16 x0, UInt16 x1, UInt16 x2, UInt16 x3, UInt16 x4, UInt16 x5, UInt16 x6, UInt16 x7)
  { uw[0] = x0; uw[1] = x1; uw[2] = x2; uw[3] = x3; uw[4] = x4; uw[5] = x5; uw[6] = x6; uw[7] = x7; }

  inline void set_sd(Int32 x0, Int32 x1, Int32 x2, Int32 x3)
  { sd[0] = x0; sd[1] = x1; sd[2] = x2; sd[3] = x3; }

  inline void set_ud(UInt32 x0, UInt32 x1, UInt32 x2, UInt32 x3)
  { ud[0] = x0; ud[1] = x1; ud[2] = x2; ud[3] = x3; }

  inline void set_sd(Int64 x0, Int64 x1)
  { sq[0] = x0; sq[1] = x1; }

  inline void set_ud(UInt64 x0, UInt64 x1)
  { uq[0] = x0; uq[1] = x1; }

  inline void set_sf(float x0, float x1, float x2, float x3)
  { sf[0] = x0; sf[1] = x1; sf[2] = x2; sf[3] = x3; }

  inline void set_df(double x0, double x1)
  { df[0] = x0; df[1] = x1; }
};

// ============================================================================
// [AsmJit::Buffer]
// ============================================================================

//! @Brief Buffer used to store instruction stream in AsmJit.
//! 
//! This class can be dangerous, if you don't know how it workd. Assembler
//! instruction stream is usually constructed by multiple calling emit
//! functions that emits bytes, words, dwords or qwords. But to decrease
//! AsmJit library size and improve performance, we are not checking for
//! buffer overflow for each emit operation, but only once in highler level
//! emit instruction.
//!
//! So, if you want to use this class, you need to do buffer checking yourself
//! by using @c ensureSpace() method. It's designed to grow buffer. Threshold
//! for growing is named @c growThreshold() and it means count of bytes for
//! emitting single operation. Default size is set to 16 bytes, because x86
//! and x64 instruction can't be larget (so it's space to hold 1 instruction).
//!
//! Example using Buffer:
//!
//! @verbatim
//! // Buffer instance, growThreshold == 16
//! // (no memory allocated in constructor).
//! AsmJit::Buffer buf(16);
//!
//! // Begin of emit stream, ensure space can fail on out of memory error.
//! if (buf.ensureSpace()) 
//! {
//!   // here, you can emit up to 16 (growThreshold) bytes
//!   buf.emitByte(0x00);
//!   buf.emitByte(0x01);
//!   buf.emitByte(0x02);
//!   buf.emitByte(0x03);
//!   ...
//! }
//! @endverbatim
struct ASMJIT_API Buffer
{
  inline Buffer(SysInt growThreshold = 16) :
    _data(NULL),
    _cur(NULL),
    _max(NULL),
    _capacity(0),
    _growThreshold(growThreshold)
  {
  }

  inline ~Buffer()
  {
    if (_data) ASMJIT_FREE(_data);
  }

  //! @brief Return start of buffer.
  inline UInt8* data() const { return _data; }

  //! @brief Return current pointer in code buffer.
  inline UInt8* cur() const { return _cur; }

  //! @brief Return maximum pointer in code buffer for growing.
  inline UInt8* maximum() const { return _max; }

  //! @brief Return current offset in buffer (same as codeSize()).
  inline SysInt offset() const { return (SysInt)(_cur - _data); }

  //! @brief Return capacity of buffer.
  inline SysInt capacity() const { return _capacity; }

  //! @brief Return grow threshold.
  inline SysInt growThreshold() const { return _growThreshold; }

  //! @brief Ensure space for next instruction
  inline bool ensureSpace() { return (_cur >= _max) ? grow() : true; }

  //! @brief Sets offset to @a o and returns previous offset.
  //!
  //! This method can be used to truncate buffer or it's used to
  //! overwrite specific position in buffer by Assembler.
  inline SysInt toOffset(SysInt o) 
  {
    ASMJIT_ASSERT(o < _capacity);

    SysInt prev = (SysInt)(_cur - _data);
    _cur = _data + o;
    return prev;
  }

  //! @brief Reallocate buffer.
  //!
  //! It's only used for growing, buffer is never reallocated to smaller 
  //! number than current capacity() is.
  bool realloc(SysInt to);

  //! @brief Used to grow the buffer.
  //!
  //! It will typically realloc to twice size of capacity(), but if capacity()
  //! is large, it will use smaller steps.
  bool grow();

  //! @brief Clear everything, but not deallocate buffer.
  void clear();

  //! @brief Free buffer and NULL all pointers.
  void free();

  //! @brief Return buffer and NULL all pointers.
  UInt8* take();

  //! @brief Emit Byte.
  inline void emitByte(UInt8 x)
  {
    *_cur++ = x;
  }

  //! @brief Emit Word (2 bytes).
  inline void emitWord(UInt16 x)
  {
    *(UInt16 *)_cur = x;
    _cur += 2;
  }

  //! @brief Emit DWord (4 bytes).
  inline void emitDWord(UInt32 x)
  {
    *(UInt32 *)_cur = x;
    _cur += 4;
  }

  //! @brief Emit QWord (8 bytes).
  inline void emitQWord(UInt64 x)
  {
    *(UInt64 *)_cur = x;
    _cur += 8;
  }

  //! @brief Set byte at position @a pos.
  inline UInt8 getByteAt(SysInt pos) const
  {
    return *reinterpret_cast<const UInt8*>(_data + pos);
  }

  //! @brief Set word at position @a pos.
  inline UInt16 getWordAt(SysInt pos) const
  {
    return *reinterpret_cast<const UInt16*>(_data + pos);
  }

  //! @brief Set word at position @a pos.
  inline UInt32 getDWordAt(SysInt pos) const
  {
    return *reinterpret_cast<const UInt32*>(_data + pos);
  }

  //! @brief Set word at position @a pos.
  inline UInt64 getQWordAt(SysInt pos) const
  {
    return *reinterpret_cast<const UInt64*>(_data + pos);
  }

  //! @brief Set byte at position @a pos.
  inline void setByteAt(SysInt pos, UInt8 x)
  {
    *reinterpret_cast<UInt8*>(_data + pos) = x;
  }

  //! @brief Set word at position @a pos.
  inline void setWordAt(SysInt pos, UInt16 x)
  {
    *reinterpret_cast<UInt16*>(_data + pos) = x;
  }

  //! @brief Set word at position @a pos.
  inline void setDWordAt(SysInt pos, UInt32 x)
  {
    *reinterpret_cast<UInt32*>(_data + pos) = x;
  }

  //! @brief Set word at position @a pos.
  inline void setQWordAt(SysInt pos, UInt64 x)
  {
    *reinterpret_cast<UInt64*>(_data + pos) = x;
  }

  // All members are public, because they are accessed and modified by 
  // Assembler/Compiler directly.

  //! @brief Beginning position of buffer.
  UInt8* _data;
  //! @brief Current position in buffer.
  UInt8* _cur;
  //! @brief Maximum position in buffer for realloc.
  UInt8* _max;

  //! @brief Buffer capacity (in bytes).
  SysInt _capacity;

  //! @brief Grow threshold
  SysInt _growThreshold;
};

// ============================================================================
// [AsmJit::PodVector<>]
// ============================================================================

//! @brief Template used to store array of POD data.
//!
//! This template has these adventages over other vector<> templates:
//! - Non-copyable (designed to be non-copyable, we want it)
//! - No copy-on-write (some implementations of stl can use it)
//! - Optimized for working only with POD types
//! - Uses ASMJIT_... memory management macros
template <typename T>
struct PodVector
{
  //! @brief Create new instance of PodVector template. Data will not
  //! be allocated (will be NULL).
  inline PodVector() : _data(NULL), _length(0), _capacity(0)
  {
  }
  
  //! @brief Destroy PodVector and free all data.
  inline ~PodVector()
  {
    if (_data) ASMJIT_FREE(_data);
  }

  //! @brief Return vector data.
  inline T* data() { return _data; }
  //! @overload
  inline const T* data() const { return _data; }
  //! @brief Return vector length.
  inline SysUInt length() const { return _length; }
  //! @brief Return vector capacity (allocation capacity).
  inline SysUInt capacity() const { return _capacity; }

  //! @brief Clear vector data, but not free internal buffer.
  void clear()
  {
    _length = 0;
  }

  //! @brief Clear vector data and free internal buffer.
  void free()
  {
    if (_data) 
    {
      ASMJIT_FREE(_data);
      _data = 0;
      _length = 0;
      _capacity = 0;
    }
  }

  //! @brief Prepend @a item to vector.
  bool prepend(const T& item)
  {
    if (_length == _capacity && !_grow()) return false;

    memmove(_data + 1, _data, sizeof(T) * _length);
    memcpy(_data, &item, sizeof(T));

    _length++;
    return true;
  }

  bool insert(SysUInt index, const T& item)
  {
    ASMJIT_ASSERT(index <= _length);
    if (_length == _capacity && !_grow()) return false;

    T* dst = _data + index;
    memmove(dst + 1, dst, _length - index);
    memcpy(dst, &item, sizeof(T));

    _length++;
    return true;
  }

  //! @brief Append @a item to vector.
  bool append(const T& item)
  {
    if (_length == _capacity && !_grow()) return false;

    memcpy(_data + _length, &item, sizeof(T));

    _length++;
    return true;
  }

  //! @brief Return item at @a i position.
  inline T& operator[](SysUInt i) { ASMJIT_ASSERT(i < _length); return _data[i]; }
  //! @brief Return item at @a i position.
  inline const T& operator[](SysUInt i) const { ASMJIT_ASSERT(i < _length); return _data[i]; }

private:
  //! @brief Called to grow internal array.
  bool _grow()
  {
    return _realloc(_capacity < 16 ? 16 : _capacity << 1);
  }

  //! @brief Realloc internal array to fit @a to items.
  bool _realloc(SysUInt to)
  {
    ASMJIT_ASSERT(to >= _length);

    T* p = reinterpret_cast<T*>(_data 
      ? ASMJIT_REALLOC(_data, to * sizeof(T)) 
      : ASMJIT_MALLOC(to * sizeof(T)));
    if (!p) return false;

    _data = p;
    _capacity = to;
    return true;
  }

  T* _data;
  SysUInt _length;
  SysUInt _capacity;

  // disable copy
  inline PodVector(const PodVector<T>& other);
  inline PodVector<T>& operator=(const PodVector<T>& other);
};

//! @}

} // AsmJit namespace

#endif // _ASMJITUTIL_H
