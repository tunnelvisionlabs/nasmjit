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

// This file is used to test AsmJit compiler.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJitAssembler.h>
#include <AsmJit/AsmJitCompiler.h>
#include <AsmJit/AsmJitLogger.h>
#include <AsmJit/AsmJitMemoryManager.h>

// This is type of function we will generate
typedef int (*MyFn)(int);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  Compiler c;

  // Log assembler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  // Equivalent C code to this function is:
  //
  // int fn(int a0)
  // {
  //   switch (a0 & 3)
  //   {
  //     case 0:
  //       return  0;
  //     case 1:
  //       return 10;
  //     case 2:
  //       return 20;
  //     case 3:
  //       return 30;
  //   }
  // }

  Function& f = *c.newFunction(CALL_CONV_DEFAULT, BuildFunction1<int>());

  // Possibilities to improve code:
  //   f.setNaked(true);
  //   f.setAllocableEbp(true);

  Int32Ref a0(f.argument(0));

  JumpTable* jumpTable = c.newJumpTable();
  Label* end = c.newLabel();

  c.and_(a0.r(), 3);
  c.jmp(ptr(jumpTable->target(), a0.r(), TIMES_4));

  // Jump #0
  c.bind(jumpTable->addLabel());
  c.mov(nax, imm(0));
  c.jmp(end);

  // Jump #1
  c.bind(jumpTable->addLabel());
  c.mov(nax, imm(10));
  c.jmp(end);

  // Jump #2
  c.bind(jumpTable->addLabel());
  c.mov(nax, imm(20));
  c.jmp(end);

  // Jump #3
  c.bind(jumpTable->addLabel());
  c.mov(nax, imm(30));
  c.jmp(end);

  // End
  c.bind(end);

  // End of function.
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make function.
  MyFn fn = function_cast<MyFn>(c.make());

  // Call it.
  for (SysUInt i = 0; i < 4; i++)
  {
    printf("Results from JIT function (%d): %d\n", i, fn(i));
  }

  // If function is not needed again it should be freed.
  MemoryManager::global()->free((void*)fn);
  // ==========================================================================

  return 0;
}
