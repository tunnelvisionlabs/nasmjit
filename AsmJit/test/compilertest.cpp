// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2006-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
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

// This file is used to test AdvX86 code generation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../AsmJit/AsmJitAssembler.h"
#include "../AsmJit/AsmJitCompiler.h"
#include "../AsmJit/AsmJitVM.h"

// This is type of function we will generate
typedef int (*MyFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  SysInt i = 32;

  // ==========================================================================
  // STEP 1: Create function.
  Assembler a;

  // Prolog.
  a.push(nbp);
  a.mov(nbp, nsp);

  // Mov 1024 to EAX/RAX, EAX/RAX is also return value.
  a.mov_ptr(nax, &i);

  // Epilog.
  a.mov(nsp, nbp);
  a.pop(nbp);
  a.ret();
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
  int result = ( reinterpret_cast<MyFn>(vmem)() );
  printf("Result from jit function: %d\n", result);

  // Memory should be freed, but use VM::free() to do that.
  VM::free(vmem, vsize);
  // ==========================================================================

  return 0;
}
