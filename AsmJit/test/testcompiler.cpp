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
// COMPILER CLASS IS NOT READY YET, SO USE AT YOUR OWN RISK.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../AsmJit/AsmJitAssembler.h"
#include "../AsmJit/AsmJitCompiler.h"
#include "../AsmJit/AsmJitVM.h"
#include "../AsmJit/AsmJitPrettyPrinter.h"

// This is type of function we will generate
typedef void (*MyFn)(int*, int*);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // STEP 1: Create function.
  Assembler a;

  PrettyPrinter logger;
  a.setLogger(&logger);

  int x[] = { -1, -1 };
  int y[] = {  5,  5 };

  // Use compiler to make a function
  {
    Compiler c;
    Function& f = *c.newFunction(CALL_CONV_DEFAULT, BuildFunction2<int*, int*>());

    VariableRef a1(f.argument(0));
    VariableRef a2(f.argument(1));

    VariableRef v1(f.newVariable(VARIABLE_TYPE_PTR));
    VariableRef v2(f.newVariable(VARIABLE_TYPE_PTR));

    //c.mov(v1.m(), 1);
    //c.mov(v2.m(), 2);

    a1.alloc();
    a2.alloc();

    //v1.alloc();
    //v2.alloc();

    //c.mov(a1.r(), v1.r());
    //c.mov(a2.r(), v2.r());

    //c.mov(nax, a1.m());
    //c.mov(ncx, a2.m());

    c.movq(mm0, ptr(a1.r()));
    c.paddd(mm0, ptr(a2.r()));
    c.movq(ptr(a1.r()), mm0);
    c.emms();

    //f.modifyGpRegisters(1 << RID_EBX);

    c.endFunction();
    c.build(a);
  }
  // ==========================================================================

  // ==========================================================================
  // STEP 2: Alloc execution-enabled memory
  SysUInt vsize;
  void *vmem = VM::alloc(a.codeSize(), &vsize, true);
  if (!vmem) 
  {
    printf("AsmJit::VM::alloc() - Failed to allocate execution-enabled memory.\n");
    return 1;
  }

  // Relocate generated code to vmem.
  a.relocCode(vmem);

  // Cast vmem to our function and call the code.
  function_cast<MyFn>(vmem)(x, y);
  printf("%d %d\n", x[0], x[1]);

  // Memory should be freed, but use VM::free() to do that.
  VM::free(vmem, vsize);
  // ==========================================================================

  return 0;
}