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

// This file is used to test variable spilling bug, originally
// reported by Tilo Nitzsche for X64W and X64U calling conventions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/Compiler.h>
#include <AsmJit/Logger.h>
#include <AsmJit/MemoryManager.h>

// This is type of function we will generate
typedef void (*MyFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunction(CALL_CONV_DEFAULT, 
    FunctionBuilder8<sysuint_t, sysuint_t, sysuint_t, sysuint_t, sysuint_t, sysuint_t, sysuint_t, sysuint_t>());

  GPVar p1(c.argGP(0));
  GPVar p2(c.argGP(1));
  GPVar p3(c.argGP(2));
  GPVar p4(c.argGP(3));
  GPVar p5(c.argGP(4));
  GPVar p6(c.argGP(5));
  GPVar p7(c.argGP(6));
  GPVar p8(c.argGP(7));

  GPVar v1(c.newGP());
  GPVar v2(c.newGP());
  GPVar v3(c.newGP());
  GPVar v4(c.newGP());
  GPVar v5(c.newGP());
  GPVar v6(c.newGP());
  GPVar v7(c.newGP());
  GPVar v8(c.newGP());

  GPVar r1(c.newGP());
  GPVar r2(c.newGP());
  GPVar r3(c.newGP());
  GPVar r4(c.newGP());
  GPVar r5(c.newGP());
  GPVar r6(c.newGP());

  c.add(p1, 1);
  c.add(p2, 2);
  c.add(p3, 3);
  c.add(p4, 4);
  c.add(p5, 5);
  c.add(p6, 6);
  c.add(p7, 7);
  c.add(p8, 8);

  c.mov(v1, 10);
  c.mov(v2, 20);
  c.mov(v3, 30);
  c.mov(v4, 40);
  c.mov(v5, 50);
  c.mov(v6, 60);
  c.mov(v7, 70);
  c.mov(v8, 80);

  c.mov(r1, 100);
  c.mov(r2, 200);
  c.mov(r3, 300);
  c.mov(r4, 400);
  c.mov(r5, 500);
  c.mov(r6, 600);

  c.add(v1, 10);
  c.add(v2, 20);
  c.add(v3, 30);
  c.add(v4, 40);
  c.add(v5, 50);
  c.add(v6, 60);
  c.add(v7, 70);
  c.add(v8, 80);

  c.add(r1, 100);
  c.add(r2, 200);
  c.add(r3, 300);
  c.add(r4, 400);
  c.add(r5, 500);
  c.add(r6, 600);

  c.sub(p1, 1);
  c.sub(p2, 2);
  c.sub(p3, 3);
  c.sub(p4, 4);
  c.sub(p5, 5);
  c.sub(p6, 6);
  c.sub(p7, 7);
  c.sub(p8, 8);

  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = function_cast<MyFn>(c.make());

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
