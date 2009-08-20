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
typedef AsmJit::SysInt (*MyFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunction(CALL_CONV_DEFAULT, BuildFunction8<SysUInt, SysUInt, SysUInt, SysUInt, SysUInt, SysUInt, SysUInt, SysUInt>());

  PtrRef p1(c.argument(0));
  PtrRef p2(c.argument(1));
  PtrRef p3(c.argument(2));
  PtrRef p4(c.argument(3));
  PtrRef p5(c.argument(4));
  PtrRef p6(c.argument(5));
  PtrRef p7(c.argument(6));
  PtrRef p8(c.argument(7));

  PtrRef eax(c.newVariable(VARIABLE_TYPE_PTR, 0, REG_RAX));
  PtrRef ebx(c.newVariable(VARIABLE_TYPE_PTR, 0, REG_RBX));
  PtrRef ecx(c.newVariable(VARIABLE_TYPE_PTR, 0, REG_RCX));
  PtrRef edx(c.newVariable(VARIABLE_TYPE_PTR, 0, REG_RDX));
  PtrRef edi(c.newVariable(VARIABLE_TYPE_PTR, 0, REG_RDI));
  PtrRef esi(c.newVariable(VARIABLE_TYPE_PTR, 0, REG_RSI));

  c.add(p1.r(), 1);
  c.add(p2.r(), 2);
  c.add(p3.r(), 3);
  c.add(p4.r(), 4);
  c.add(p5.r(), 5);
  c.add(p6.r(), 6);
  c.add(p7.r(), 7);
  c.add(p8.r(), 8);
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make function
  MyFn fn = function_cast<MyFn>(c.make());

  // If function is not needed again it should be freed.
  MemoryManager::global()->free((void*)fn);
  // ==========================================================================

  return 0;
}
