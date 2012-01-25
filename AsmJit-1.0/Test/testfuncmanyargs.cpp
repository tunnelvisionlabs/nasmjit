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

// This file is used to test function with many arguments. Bug originally
// reported by Tilo Nitzsche for X64W and X64U calling conventions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>

// This is type of function we will generate
typedef void (*MyFn)(void*, void*, void*, void*, void*, void*, void*, void*);

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
    FunctionBuilder8<Void, void*, void*, void*, void*, void*, void*, void*, void*>());

  GPVar p1(c.argGP(0));
  GPVar p2(c.argGP(1));
  GPVar p3(c.argGP(2));
  GPVar p4(c.argGP(3));
  GPVar p5(c.argGP(4));
  GPVar p6(c.argGP(5));
  GPVar p7(c.argGP(6));
  GPVar p8(c.argGP(7));

  c.add(p1, 1);
  c.add(p2, 2);
  c.add(p3, 3);
  c.add(p4, 4);
  c.add(p5, 5);
  c.add(p6, 6);
  c.add(p7, 7);
  c.add(p8, 8);

  // Move some data into buffer provided by arguments so we can verify if it
  // really works without looking into assembler output.
  c.add(byte_ptr(p1), imm(1));
  c.add(byte_ptr(p2), imm(2));
  c.add(byte_ptr(p3), imm(3));
  c.add(byte_ptr(p4), imm(4));
  c.add(byte_ptr(p5), imm(5));
  c.add(byte_ptr(p6), imm(6));
  c.add(byte_ptr(p7), imm(7));
  c.add(byte_ptr(p8), imm(8));

  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  uint8_t var[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  MyFn fn = function_cast<MyFn>(c.make());
  fn(var, var, var, var, var, var, var, var);
  
  printf("Results: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
    var[0], var[1], var[2], var[3], 
    var[4], var[5], var[6], var[7], 
    var[8]);

  bool success = 
    var[0] == 0 && var[1] == 1 && var[2] == 2 && var[3] == 3 &&
    var[4] == 4 && var[5] == 5 && var[6] == 6 && var[7] == 7 &&
    var[8] == 8;
  printf("Status: %s\n", success ? "Success" : "Failure");

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
