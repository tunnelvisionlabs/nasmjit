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

// We are using sprintf() here.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "Assembler.h"
#include "Compiler.h"
#include "CpuInfo.h"
#include "Logger.h"
#include "Util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

Emittable::Emittable(Compiler* c, uint32_t type) ASMJIT_NOTHROW :
  _compiler(c),
  _next(NULL),
  _prev(NULL),
  _comment(NULL),
  _type(type),
  _offset(INVALID_VALUE)
{
}

Emittable::~Emittable() ASMJIT_NOTHROW
{
}

void Emittable::prepare(CompilerContext& c) ASMJIT_NOTHROW
{
  _offset = c._currentOffset;
}

void Emittable::translate(CompilerContext& c) ASMJIT_NOTHROW
{
}

void Emittable::emit(Assembler& a) ASMJIT_NOTHROW
{
}

void Emittable::post(Assembler& a) ASMJIT_NOTHROW
{
}

void Emittable::setComment(const char* str) ASMJIT_NOTHROW
{
  _comment = _compiler->getZone().zstrdup(str);
}

void Emittable::setCommentF(const char* fmt, ...) ASMJIT_NOTHROW
{
  // I'm really not expecting larger inline comments:)
  char buf[128];

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, 127, fmt, ap);
  va_end(ap);

  // I don't know if vsnprintf can produce non-null terminated string, in case
  // it can, we terminate it here.
  buf[127] = '\0';

  setComment(buf);
}

// ============================================================================
// [AsmJit::EComment]
// ============================================================================

EComment::EComment(Compiler* c, const char* str) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_COMMENT)
{
  setComment(str);
}

EComment::~EComment() ASMJIT_NOTHROW
{
}

void EComment::emit(Assembler& a) ASMJIT_NOTHROW
{
  if (a.getLogger() && a.getLogger()->isUsed())
  {
    a.getLogger()->logString(getComment());
  }
}

// ============================================================================
// [AsmJit::EData]
// ============================================================================

EData::EData(Compiler* c, const void* data, sysuint_t length) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_EMBEDDED_DATA)
{
  _length = length;
  memcpy(_data, data, length);
}

EData::~EData() ASMJIT_NOTHROW
{
}

void EData::emit(Assembler& a) ASMJIT_NOTHROW
{
  a.embed(_data, _length);
}

// ============================================================================
// [AsmJit::EAlign]
// ============================================================================

EAlign::EAlign(Compiler* c, uint32_t size) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_ALIGN), _size(size)
{
}

EAlign::~EAlign() ASMJIT_NOTHROW
{
}

void EAlign::emit(Assembler& a) ASMJIT_NOTHROW
{
  a.align(_size);
}

// ============================================================================
// [AsmJit::ETarget]
// ============================================================================

ETarget::ETarget(Compiler* c, const Label& label) ASMJIT_NOTHROW :
  Emittable(c, EMITTABLE_TARGET),
  _label(label),
  _from(NULL),
  _state(NULL),
  _jumpsCount(0)
{
}

ETarget::~ETarget() ASMJIT_NOTHROW
{
}

void ETarget::translate(CompilerContext& c) ASMJIT_NOTHROW
{
  c._unrecheable = false;
  if (_state == NULL) _state = c._saveState();
}

void ETarget::emit(Assembler& a) ASMJIT_NOTHROW
{
  a.bind(_label);
}

} // AsmJit namespace
