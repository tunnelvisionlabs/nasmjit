// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

#define _ASMJIT_BEING_COMPILED

// [Dependencies - AsmJit]
#include "../Core/Defs.h"
#include "../Core/IntUtil.h"
#include "../Core/StringBuilder.h"

// [Dependencies - C]
#include <stdarg.h>

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// Should be placed in read-only memory.
static const char StringBuilder_empty[4] = { 0 };

// ============================================================================
// [AsmJit::StringBuilder - Construction / Destruction]
// ============================================================================

StringBuilder::StringBuilder() ASMJIT_NOTHROW :
  _data(const_cast<char*>(StringBuilder_empty)),
  _length(0),
  _capacity(0),
  _canFree(false)
{
}

StringBuilder::~StringBuilder() ASMJIT_NOTHROW
{
  if (_canFree)
    ASMJIT_FREE(_data);
}

// ============================================================================
// [AsmJit::StringBuilder - Prepare / Reserve]
// ============================================================================

char* StringBuilder::prepare(uint32_t op, size_t len) ASMJIT_NOTHROW
{
  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  if (op == kStringBuilderOpSet)
  {
    // We don't care here, but we can't return a NULL pointer since it indicates
    // failure in memory allocation.
    if (len == 0)
    {
      if (_data != StringBuilder_empty)
        _data[0] = 0;

      _length = 0;
      return _data;
    }

    if (_capacity < len)
    {
      if (len >= IntUtil::typeMax<size_t>() - sizeof(uintptr_t) * 2)
        return NULL;

      size_t to = IntUtil::align<size_t>(len, sizeof(uintptr_t));
      if (to < 256 - sizeof(uintptr_t))
        to = 256 - sizeof(uintptr_t);

      char* newData = static_cast<char*>(ASMJIT_MALLOC(to + sizeof(uintptr_t)));
      if (newData == NULL)
      {
        clear();
        return NULL;
      }

      if (_canFree)
        ASMJIT_FREE(_data);

      _data = newData;
      _capacity = to + sizeof(uintptr_t) - 1;
      _canFree = true;
    }

    _data[len] = 0;
    _length = len;

    ASMJIT_ASSERT(_length <= _capacity);
    return _data;
  }

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  else
  {
    // We don't care here, but we can't return a NULL pointer since it indicates
    // failure in memory allocation.
    if (len == 0)
      return _data + _length;

    // Overflow.
    if (IntUtil::typeMax<size_t>() - sizeof(uintptr_t) * 2 - _length < len)
      return NULL;

    size_t after = _length + len;
    if (_capacity < after)
    {
      size_t to = _capacity;

      if (to < 256)
        to = 256;

      while (to < 1024 * 1024 && to < after)
      {
        to *= 2;
      }

      if (to < after)
      {
        to = after;
        if (to < (IntUtil::typeMax<size_t>() - 1024 * 32))
          to = IntUtil::align<size_t>(to, 1024 * 32);
      }

      to = IntUtil::align<size_t>(to, sizeof(uintptr_t));
      char* newData = static_cast<char*>(ASMJIT_MALLOC(to + sizeof(uintptr_t)));

      if (newData == NULL)
        return NULL;

      ::memcpy(newData, _data, _length);

      if (_canFree)
        ASMJIT_FREE(_data);

      _data = newData;
      _capacity = to + sizeof(uintptr_t) - 1;
      _canFree = true;
    }

    char* ret = _data + _length;
    _data[after] = 0;
    _length = after;

    ASMJIT_ASSERT(_length <= _capacity);
    return ret;
  }
}

bool StringBuilder::reserve(size_t to) ASMJIT_NOTHROW
{
  if (_capacity >= to)
    return true;

  if (to >= IntUtil::typeMax<size_t>() - sizeof(uintptr_t) * 2)
    return false;

  to = IntUtil::align<size_t>(to, sizeof(uintptr_t));

  char* newData = static_cast<char*>(ASMJIT_MALLOC(to + sizeof(uintptr_t)));
  if (newData == NULL)
    return false;

  ::memcpy(newData, _data, _length + 1);
  if (_canFree)
    ASMJIT_FREE(_data);

  _data = newData;
  _capacity = to + sizeof(uintptr_t) - 1;
  _canFree = true;
  return true;
}

// ============================================================================
// [AsmJit::StringBuilder - Clear]
// ============================================================================

void StringBuilder::clear() ASMJIT_NOTHROW
{
  if (_data != StringBuilder_empty)
    _data[0] = 0;
  _length = 0;
}

// ============================================================================
// [AsmJit::StringBuilder - Methods]
// ============================================================================

bool StringBuilder::_opString(uint32_t op, const char* str, size_t len) ASMJIT_NOTHROW
{
  if (len == kInvalidSize)
    len = ::strlen(str);

  char* p = prepare(op, len);
  if (p == NULL)
    return false;

  ::memcpy(p, str, len);
  return true;
}

bool StringBuilder::_opChars(uint32_t op, char c, size_t len) ASMJIT_NOTHROW
{
  char* p = prepare(op, len);
  if (p == NULL)
    return false;

  ::memset(p, c, len);
  return true;
}

static const char StringBuilder_numbers[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

bool StringBuilder::_opNumber(uint32_t op, uint64_t i, uint32_t base, size_t width, uint32_t flags) ASMJIT_NOTHROW
{
  if (base < 2 || base > 36)
    base = 10;

  char buf[128];
  char* p = buf + ASMJIT_ARRAY_SIZE(buf);

  uint64_t orig = i;
  char sign = '\0';

  // --------------------------------------------------------------------------
  // [Sign]
  // --------------------------------------------------------------------------

  if ((flags & kStringBuilderNumSigned) != 0 && static_cast<int64_t>(i) < 0)
  {
    i = static_cast<uint64_t>(-static_cast<int64_t>(i));
    sign = '-';
  }
  else if ((flags & kStringBuilderNumShowSign) != 0)
  {
    sign = '+';
  }
  else if ((flags & kStringBuilderNumShowSpace) != 0)
  {
    sign = ' ';
  }

  // --------------------------------------------------------------------------
  // [Number]
  // --------------------------------------------------------------------------

  do {
    uint64_t d = i / base;
    uint64_t r = i % base;

    *--p = StringBuilder_numbers[r];
    i = d;
  } while (i);

  size_t numberLength = (size_t)(buf + ASMJIT_ARRAY_SIZE(buf) - p);

  // --------------------------------------------------------------------------
  // [Alternate Form]
  // --------------------------------------------------------------------------

  if ((flags & kStringBuilderNumAlternate) != 0)
  {
    if (base == 8)
    {
      if (orig != 0)
        *--p = '0';
    }
    if (base == 16)
    {
      *--p = 'x';
      *--p = '0';
    }
  }

  // --------------------------------------------------------------------------
  // [Width]
  // --------------------------------------------------------------------------

  if (sign != 0)
    *--p = sign;

  if (width > 256)
    width = 256;

  if (width <= numberLength)
    width = 0;
  else
    width -= numberLength;

  // --------------------------------------------------------------------------
  // Write]
  // --------------------------------------------------------------------------

  size_t prefixLength = (size_t)(buf + ASMJIT_ARRAY_SIZE(buf) - p) - numberLength;
  char* data = prepare(op, prefixLength + width + numberLength);

  if (data == NULL)
    return false;

  ::memcpy(data, p, prefixLength);
  data += prefixLength;

  ::memset(data, '0', width);
  data += width;

  ::memcpy(data, p + prefixLength, numberLength);
  return true;
}

bool StringBuilder::_opHex(uint32_t op, const void* data, size_t len) ASMJIT_NOTHROW
{
  if (len >= IntUtil::typeMax<size_t>() / 2)
    return false;

  char* dst = prepare(op, len);
  if (dst == NULL)
    return false;

  const char* src = static_cast<const char*>(data);
  for (size_t i = 0; i < len; i++, dst += 2, src += 1)
  {
    dst[0] = StringBuilder_numbers[(src[0] >> 4) & 0xF];
    dst[1] = StringBuilder_numbers[(src[0]     ) & 0xF];
  }

  return true;
}

bool StringBuilder::_opVFormat(uint32_t op, const char* fmt, va_list ap) ASMJIT_NOTHROW
{
  char buf[1024];

  vsnprintf(buf, ASMJIT_ARRAY_SIZE(buf), fmt, ap);
  buf[ASMJIT_ARRAY_SIZE(buf) - 1] = '\0';

  return _opString(op, buf);
}

bool StringBuilder::setFormat(const char* fmt, ...) ASMJIT_NOTHROW
{
  bool result;

  va_list ap;
  va_start(ap, fmt);
  result = _opVFormat(kStringBuilderOpSet, fmt, ap);
  va_end(ap);

  return result;
}

bool StringBuilder::appendFormat(const char* fmt, ...) ASMJIT_NOTHROW
{
  bool result;

  va_list ap;
  va_start(ap, fmt);
  result = _opVFormat(kStringBuilderOpAppend, fmt, ap);
  va_end(ap);

  return result;
}

bool StringBuilder::eq(const char* str, size_t len) const ASMJIT_NOTHROW
{
  const char* aData = _data;
  const char* bData = str;

  size_t aLength = _length;
  size_t bLength = len;

  if (bLength == kInvalidSize)
  {
    size_t i;
    for (i = 0; i < aLength; i++)
    {
      if (aData[i] != bData[i] || bData[i] == 0)
        return false;
    }

    return bData[i] == 0;
  }
  else
  {
    if (aLength != bLength)
      return false;

    return ::memcmp(aData, bData, aLength) == 0;
  }
}

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"
