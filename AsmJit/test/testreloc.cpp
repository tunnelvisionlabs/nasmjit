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

// This file is used to test relocations.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJitAssembler.h>
#include <AsmJit/AsmJitVM.h>

// This is type of function we will generate
typedef int (*MyFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Alloc execute enabled memory
  SysUInt vsize;
  UInt8 *vmem = (UInt8*)VM::alloc(4096 /* alloc more space */, &vsize, true);
  if (!vmem)
  {
    printf("AsmJit::VM::alloc() - Failed to allocate execution-enabled memory.\n");
    return 1;
  }

  // addresses to generated functions (first is pointer to vmem, but second
  // must be set after first function was relocated).
  UInt8* first = vmem;
  UInt8* second;
  // ==========================================================================

  // ==========================================================================
  // Create first function.
  Assembler a;

  // Immediate, we will overwrite it later
  // (this is used to show how relocation works)
  Relocable var = 0;

  // Prolog.
  a.push(nbp);
  a.mov(nbp, nsp);

  // Clear EAX/RAX
  a.xor_(nax, nax);

  // Here we will jump from another code. There is no sence to create labels,
  // because they will be invalidated by code relocation.
  SysInt jumpOffset = a.offset();

  // Add 'var' to EAX/RAX, EAX/RAX is also return value.
  a.add(nax, var);

  // Epilog.
  a.mov(nsp, nbp);
  a.pop(nbp);
  a.ret();

  // Overwrite immediate variable
  var.setValue(1024);
  a.overwrite(var);

  // Relocate generated code to vmem.
  a.relocCode(vmem);
  // ==========================================================================

  // Setup second function address
  second = vmem + a.codeSize();

  // ==========================================================================
  // Create second function (we will reuse 'a' instance to do that).
  a.clear();

  a.push(nbp);
  a.mov(nbp, nsp);

  a.mov(nax, 1024);

  // Make relative or absolute jump, ECX or RCX register will be used if
  // relative jump is not possible.
  a.jmp_ptr((UInt8*)vmem + jumpOffset, ncx);

  a.relocCode(second);
  // ==========================================================================

  // ==========================================================================
  // Cast vmem to our function and call the code.
  int result1 = function_cast<MyFn>(first)();
  int result2 = function_cast<MyFn>(second)();

  printf("Result from first function: %d\n", result1);
  printf("Result from second function: %d\n", result2);

  // Memory should be freed, but use VM::free() to do that.
  VM::free(vmem, vsize);
  // ==========================================================================

  return 0;
}
