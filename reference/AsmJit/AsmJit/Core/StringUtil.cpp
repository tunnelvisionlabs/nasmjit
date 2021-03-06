// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

#define _ASMJIT_BEING_COMPILED

// [Dependencies - AsmJit]
#include "../Core/Assert.h"
#include "../Core/StringUtil.h"

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::StringUtil]
// ============================================================================

static const char letters[] = "0123456789ABCDEF";

char* StringUtil::copy(char* dst, const char* src, size_t len) ASMJIT_NOTHROW
{
  if (src == NULL)
    return dst;

  if (len == kInvalidSize)
  {
    while (*src) *dst++ = *src++;
  }
  else
  {
    memcpy(dst, src, len);
    dst += len;
  }

  return dst;
}

char* StringUtil::fill(char* dst, const int c, size_t len) ASMJIT_NOTHROW
{
  memset(dst, c, len);
  return dst + len;
}

char* StringUtil::hex(char* dst, const uint8_t* src, size_t len) ASMJIT_NOTHROW
{
  for (size_t i = len; i; i--, dst += 2, src += 1)
  {
    dst[0] = letters[(src[0] >> 4) & 0xF];
    dst[1] = letters[(src[0]     ) & 0xF];
  }

  return dst;
}

// Not too efficient, but this is mainly for debugging:)
char* StringUtil::utoa(char* dst, uintptr_t i, size_t base) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(base <= 16);

  char buf[128];
  char* p = buf + 128;

  do {
    uintptr_t b = i % base;
    *--p = letters[b];
    i /= base;
  } while (i);

  return StringUtil::copy(dst, p, (size_t)(buf + 128 - p));
}

char* StringUtil::itoa(char* dst, intptr_t i, size_t base) ASMJIT_NOTHROW
{
  if (i < 0)
  {
    *dst++ = '-';
    i = -i;
  }

  return StringUtil::utoa(dst, (size_t)i, base);
}

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"
