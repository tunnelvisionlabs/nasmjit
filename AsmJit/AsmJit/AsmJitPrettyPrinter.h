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
#ifndef _ASMJITPRETTYPRINTER_H
#define _ASMJITPRETTYPRINTER_H

// [Dependencies]
#include "AsmJitAssembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace AsmJit {

//! @addtogroup AsmJit_Logging
//! @{

// ============================================================================
// [AsmJit::PrettyPrinter]
// ============================================================================

//! @brief Logger that prints assembler output as instruction with its operands
//! in Intel syntax.
//!
//! To use PrettyPrinter, use:
//!
//! @code
//! using namespace AsmJit;
//!
//! // Create Assembler instance
//! Assembler a;
//!
//! // Create PrettyPrinter instance and attach it to the Assembler
//! PrettyPrinter logger;
//! a.setLogger(&logger);
//! @endcode
struct PrettyPrinter : Assembler::Logger
{
  //! @brief Create new PrettyPrinter instance.
  PrettyPrinter();
  //! @brief Destroy PrettyPrinter instance.
  virtual ~PrettyPrinter();

  virtual void logInstruction(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);
  virtual void logAlign(SysInt m);
  virtual void logLabel(const Label* label);
  virtual void logFormat(const char* fmt, ...);

  virtual void log(const char* buf);

  //! @brief Logs instruction @a code to @a buf and returns it's size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpInstruction(char* buf, UInt32 code);

  //! @brief Logs operand @a op to @a buf and returns it's size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpOperand(char* buf, const Operand* op);

  //! @brief Logs register to @a buf and returns it's size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpRegister(char* buf, UInt8 type, UInt8 index);

  //! @brief Logs label @a label to @a buf and returns it's size.
  //!
  //! @note Output is not @c NULL terminated.
  static SysInt dumpLabel(char* buf, const Label* label);
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITPRETTYPRINTER_H
