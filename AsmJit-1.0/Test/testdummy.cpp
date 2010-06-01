// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2010, Petr Kobalicek <kobalicek.petr@gmail.com>
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
typedef uint32_t (*MyFn)(uint32_t, uint32_t);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);
  c.setProperty(PROPERTY_LOG_BINARY, true);

  c.newFunction(CALL_CONV_DEFAULT, FunctionBuilder2<uint32_t, uint32_t>());
  c.getFunction()->setHint(FUNCTION_HINT_NAKED, true);

  GPVar var[20];
  int i;

  for (i = 0; i < ASMJIT_ARRAY_SIZE(var); i++)
  {
    var[i] = c.newGP();
  }

  c.alloc(var[0], eax);

  for (i = 0; i < ASMJIT_ARRAY_SIZE(var); i++)
  {
    c.mov(var[i], imm(i));
    //c.spill(var[i]);
  }

  GPVar j = c.newGP();
  Label r = c.newLabel();
  c.mov(j, 4);

  c.bind(r);
  for (i = ASMJIT_ARRAY_SIZE(var) - 1; i > 0; i--)
  {
    c.add(var[0], var[i]);
  }

  c.dec(j);
  c.jnz(r, HINT_NOT_TAKEN);

  c.ret(var[0]);
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make function
  MyFn fn = function_cast<MyFn>(c.make());

  printf("Result %u\n", fn(1, 2));

  // If function is not needed again it should be freed.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  // TODO: Remove
#if defined(_WIN32)
  system("pause");
#endif //

  return 0;
}
