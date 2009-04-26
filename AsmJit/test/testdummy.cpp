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

// This file is used as a dummy test. It's changed during development.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/Compiler.h>
#include <AsmJit/Logger.h>
#include <AsmJit/MemoryManager.h>

// This is type of function we will generate
typedef void (*MyFn)(AsmJit::UInt32*, AsmJit::UInt32*);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunction(CALL_CONV_DEFAULT, BuildFunction2<UInt32*, UInt32*>());

  SysIntRef a0(c.argument(0));
  SysIntRef a1(c.argument(1));

  XMMRef xmmzero(c.newVariable(VARIABLE_TYPE_XMM));
  XMMRef msk0(c.newVariable(VARIABLE_TYPE_XMM));
  XMMRef msk1(c.newVariable(VARIABLE_TYPE_XMM));

  c.pxor(xmmzero.x(), xmmzero.x());
  c.movd(msk0.r(), ptr(a0.r()));

  c.punpcklbw(msk0.r(), xmmzero.r());
  c.pshufd(msk0.r(), msk0.r(), imm(mm_shuffle(1, 0, 1, 0)));

  c.pshufhw(msk1.r(), msk0.r(), imm(mm_shuffle(3, 3, 3, 3)));
  c.pshuflw(msk1.r(), msk1.r(), imm(mm_shuffle(2, 2, 2, 2)));
  c.pshufhw(msk0.r(), msk0.r(), imm(mm_shuffle(1, 1, 1, 1)));
  c.pshuflw(msk0.r(), msk0.r(), imm(mm_shuffle(0, 0, 0, 0)));

  c.movdqu(ptr(a1.r()), msk0.r());
  c.movdqu(ptr(a1.r(), 16), msk1.r());

  // Finish
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make function
  MyFn fn = function_cast<MyFn>(c.make());

  // Call it
  UInt32 x[1] = { 0x11223344 };
  UInt32 y[8];
  fn(x, y);

  printf("Input : %0.8X\n", x[0]);
  printf("Output: %0.8X|%0.8X|%0.8X|%0.8X\n", y[3], y[2], y[1], y[0]);
  printf("Output: %0.8X|%0.8X|%0.8X|%0.8X\n", y[7], y[6], y[5], y[4]);

  // If function is not needed again it should be freed.
  MemoryManager::global()->free((void*)fn);
  // ==========================================================================

  return 0;
}
