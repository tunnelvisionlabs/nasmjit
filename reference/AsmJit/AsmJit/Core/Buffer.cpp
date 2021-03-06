// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

#define _ASMJIT_BEING_COMPILED

// [Dependencies - AsmJit]
#include "../Core/Buffer.h"
#include "../Core/Defs.h"

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Buffer]
// ============================================================================

void Buffer::emitData(const void* ptr, size_t len) ASMJIT_NOTHROW
{
  size_t max = getCapacity() - getOffset();

  if (max < len && !realloc(getOffset() + len))
  {
    return;
  }

  memcpy(_cur, ptr, len);
  _cur += len;
}

bool Buffer::realloc(size_t to) ASMJIT_NOTHROW
{
  if (getCapacity() < to)
  {
    size_t len = getOffset();
    uint8_t *newdata;

    if (_data != NULL)
      newdata = (uint8_t*)ASMJIT_REALLOC(_data, to);
    else
      newdata = (uint8_t*)ASMJIT_MALLOC(to);

    if (newdata == NULL)
      return false;

    _data = newdata;
    _cur = newdata + len;
    _max = newdata + to;
    _max -= (to >= kBufferGrow) ? kBufferGrow : to;

    _capacity = to;
  }

  return true;
}

bool Buffer::grow() ASMJIT_NOTHROW
{
  size_t to = _capacity;

  if (to < 512)
    to = 1024;
  else if (to > 65536)
    to += 65536;
  else
    to <<= 1;

  return realloc(to);
}

void Buffer::reset() ASMJIT_NOTHROW
{
  if (_data == NULL)
    return;
  ASMJIT_FREE(_data);

  _data = NULL;
  _cur = NULL;
  _max = NULL;
  _capacity = 0;
}

uint8_t* Buffer::take() ASMJIT_NOTHROW
{
  uint8_t* data = _data;

  _data = NULL;
  _cur = NULL;
  _max = NULL;
  _capacity = 0;

  return data;
}

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"
