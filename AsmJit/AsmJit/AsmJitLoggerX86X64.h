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
#ifndef _ASMJITLOGGERX86X64_H
#define _ASMJITLOGGERX86X64_H

// [Dependencies]
#include "AsmJitDefs.h"
#include "AsmJitSerializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace AsmJit {

//! @addtogroup AsmJit_Logging
//! @{

//! @brief Abstract logging class.
//!
//! This class can be inherited and reimplemented to fit into your logging
//! subsystem. When reimplementing use @c AsmJit::Logger::log() method to
//! log into your stream.
//!
//! This class also contain @c _enabled member that can be used to enable 
//! or disable logging.
struct ASMJIT_API Logger
{
  // [Construction / Destruction]

  //! @brief Create logger.
  Logger();
  //! @brief Destroy logger.
  virtual ~Logger();

  // [Methods]

  //! @brief Abstract method to log output.
  //!
  //! Default implementation that is in @c AsmJit::Logger is to do nothing. 
  //! It's virtual to fit to your logging system.
  virtual void log(const char* buf);

  //! @brief Log instruction with operands.
  virtual void logInstruction(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);
  //! @brief Log .align directive.
  virtual void logAlign(SysInt m);
  //! @brief Log label.
  virtual void logLabel(const Label* label);
  //! @brief Log printf like message.
  virtual void logFormat(const char* fmt, ...);

  //! @brief Return @c true if logging is enabled.
  inline bool enabled() const { return _enabled; }
  //! @brief Set logging to enabled or disabled.
  inline void setEnabled(bool enabled) { _enabled = enabled; }

  // [Statics]

  //! @brief Dump instruction @a code to @a buf and return destination size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpInstruction(char* buf, UInt32 code);

  //! @brief Dump operand @a op to @a buf and returns destination size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpOperand(char* buf, const Operand* op);

  //! @brief Dump register to @a buf and returns destination size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpRegister(char* buf, UInt8 type, UInt8 index);

  //! @brief Dump label @a label to @a buf and returns destination size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpLabel(char* buf, const Label* label);

  // [Variables]

protected:
  //! @brief Whether logger is enabled or disabled.
  //!
  //! Default @c true.
  bool _enabled;

  //! @brief Whether logger have logging stream.
  //!
  //! This value can be set by inherited classes to inform @c Logger that
  //! assigned stream is invalid. If @c _haveStream is false it means that
  //! logging messages from helper functions (@c logInstruction, @c logAlign,
  //! @c logLabel and @c logFormat) are not sent to main @c log() method.
  //!
  //! This is designed only to optimize cases that logger exists, but its
  //! stream not.
  //!
  //! Default @c true.
  bool _haveStream;

private:
  // disable copy
  ASMJIT_DISABLE_COPY(Logger);
};

//! @brief Logger that can log to standard C @c FILE* stream.
struct ASMJIT_API FileLogger : public Logger
{
  // [Construction / Destruction]

  //! @brief Create new @c FileLogger.
  //! @param stream FILE stream where logging will be sent (can be @c NULL 
  //! to disable logging).
  FileLogger(FILE* stream = NULL);

  // [Methods]

  virtual void log(const char* buf);

  //! @brief Get file stream.
  //! @note Return value can be @c NULL.
  inline FILE* stream() const { return _stream; }

  //! @brief Set file stream.
  //! @param stream FILE stream where logging will be sent (can be @c NULL 
  //! to disable logging).
  void setStream(FILE* stream);

  // [Variables]

private:
  FILE* _stream;
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITLOGGERX86X64_H
