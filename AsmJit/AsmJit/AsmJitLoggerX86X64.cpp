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

// We are using sprintf() here.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "AsmJitLogger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace AsmJit {

// ============================================================================
// [AsmJit::Logger]
// ============================================================================

Logger::Logger() :
  _enabled(true),
  _haveStream(true)
{
}

Logger::~Logger()
{
}

void Logger::logInstruction(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  if (!_enabled || !_haveStream) return;

  char buf[1024];
  char* p = buf;

  // dump instruction
  p += dumpInstruction(p, code);
  
  // dump operands
  if (!o1->isNone()) { *p++ = ' '; p += dumpOperand(p, o1); }
  if (!o2->isNone()) { *p++ = ','; *p++ = ' '; p += dumpOperand(p, o2); }
  if (!o3->isNone()) { *p++ = ','; *p++ = ' '; p += dumpOperand(p, o3); }
  
  *p++ = '\n';
  *p = '\0';
  log(buf);
}

void Logger::logAlign(SysInt m)
{
  if (!_enabled || !_haveStream) return;

  logFormat(".align %d\n", (Int32)m);
}

void Logger::logLabel(const Label* label)
{
  if (!_enabled || !_haveStream) return;

  char buf[1024];
  char* p = buf + dumpLabel(buf, label);
  *p++ = ':';
  *p++ = '\n';
  *p = '\0';
  log(buf);
}

void Logger::logFormat(const char* fmt, ...)
{
  if (!_enabled || !_haveStream) return;

  char buf[1024];

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, 1023, fmt, ap);
  va_end(ap);
  
  log(buf);
}

void Logger::log(const char* buf)
{
}

// ============================================================================
// [AsmJit::FileLogger]
// ============================================================================

FileLogger::FileLogger(FILE* stream)
  : _stream(NULL)
{
  setStream(stream);
}

void FileLogger::log(const char* buf)
{
  if (!_enabled || !_haveStream) return;

  fputs(buf, _stream);
}

//! @brief Set file stream.
void FileLogger::setStream(FILE* stream)
{
  _stream = stream;
  _haveStream = (stream != NULL);
}

} // AsmJit namespace
