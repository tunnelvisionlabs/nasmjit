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

// This file is only included as an example and simple test if jit
// compiler works.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../AsmJit/AsmJitX86.h"
#include "../AsmJit/AsmJitVM.h"

// This is type of function we will generate
typedef int (*VoidFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // Create function by dynamic way.
  X86 a;

  // Prolog.
  a.push(nbp);
  a.mov(nbp, nsp);

  a.push(nbx);

  // Mov 1024 to EAX/RAX, EAX/RAX is also return value.
  //a.mov(nax, imm(1024));
  //a.mov(al, 10);
  //a.inc(nax);
  a.mov(nax, 1024);
  a.mov(ndx, 2048);
  a.cmp(nbx, 1);
  a.cmov(C_EQUAL, nax, ndx);
  a.mov(cl, 1);
  a.shl(ah, cl);
  a.shl(ax, 2);
  a.shr(nax, 1);
  //a.mov(r15b, 1);

  a.pop(nbx);

  // Epilog.
  a.mov(nsp, nbp);
  a.pop(nbp);
  a.ret();

  // Alloc execute enabled memory and call generated function.
  SysUInt vsize;
  void *vmem = VM::alloc(a.codeSize(), &vsize, true);
  memcpy(vmem, a.pData, a.codeSize());

  // Cast vmem to our function and call the code.
  int result = ( reinterpret_cast<VoidFn>(vmem)() );

  // Memory should be freed, but use VM::free() to do that.
  VM::free(vmem, vsize);

  printf("Result from jit function: %d\n", result);
  return 0;
}
