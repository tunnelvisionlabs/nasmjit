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
#include "AsmJitConfig.h"

#include <stdlib.h>
#include <string.h>

namespace AsmJit {

//! @addtogroup AsmJit_Util
//! @{

//! @brief Template used to store array of POD data structures.
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
    memmove(_data + 1, _data, sizeof(T) * (_length++));
    memcpy(_data, &item, sizeof(T));
    return true;
  }

  //! @brief Append @a item to vector.
  bool append(const T& item)
  {
    if (_length == _capacity && !_grow()) return false;
    memcpy(_data + (_length++), &item, sizeof(T));
    return true;
  }

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