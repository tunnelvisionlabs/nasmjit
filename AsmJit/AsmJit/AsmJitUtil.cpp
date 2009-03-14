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

// [Dependencies]
#include "AsmJitBuild.h"
#include "AsmJitUtil.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Buffer]
// ============================================================================

bool Buffer::realloc(SysInt to)
{
  if (capacity() < to)
  {
    SysInt len = offset();

    UInt8 *newdata;
    if (_data)
      newdata = (UInt8*)ASMJIT_REALLOC(_data, to);
    else
      newdata = (UInt8*)ASMJIT_MALLOC(to);
    if (!newdata) return false;

    _data = newdata;
    _cur = newdata + len;
    _max = newdata + to;
    _max -= (to >= _growThreshold) ? _growThreshold : to;

    _capacity = to;
  }

  return true;
}

bool Buffer::grow()
{
  SysInt to = _capacity;

  if (to < 512)
    to = 1024;
  else if (to > 65536)
    to += 65536;
  else
    to <<= 1;

  return realloc(to);
}

void Buffer::clear()
{
  _cur = _data;
}

void Buffer::free()
{
  if (!_data) return;
  ASMJIT_FREE(_data);

  _data = NULL;
  _cur = NULL;
  _max = NULL;
  _capacity = 0;
}

UInt8* Buffer::take()
{
  UInt8* data = _data;

  _data = NULL;
  _cur = NULL;
  _max = NULL;
  _capacity = 0;

  return data;
}

// ============================================================================
// [AsmJit::Zone]
// ============================================================================

Zone::Zone(SysUInt chunkSize)
{
  _chunks = NULL;
  _total = 0;
  _chunkSize = chunkSize;
}

Zone::~Zone()
{
  freeAll();
}

void* Zone::alloc(SysUInt size)
{
  Chunk* cur = _chunks;

  if (!cur || cur->remain() < size)
  {
    cur = (Chunk*)ASMJIT_MALLOC(sizeof(Chunk) - (sizeof(UInt8)*4) + _chunkSize);
    if (!cur) return NULL;

    cur->prev = _chunks;
    cur->pos = 0;
    cur->size = _chunkSize;
    _chunks = cur;
  }

  UInt8* p = cur->data + cur->pos;
  cur->pos += size;
  return (void*)p;
}

void Zone::freeAll()
{
  Chunk* cur = _chunks;

  while (cur)
  {
    Chunk* prev = cur->prev;
    ASMJIT_FREE(cur);
    cur = prev;
  }

  _chunks = NULL;
  _total = 0;
}

} // AsmJit namespace
