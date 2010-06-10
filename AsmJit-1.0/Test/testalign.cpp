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

// Test function variables alignment (for sse2 code, 16-byte xmm variables).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/Compiler.h>
#include <AsmJit/Logger.h>
#include <AsmJit/MemoryManager.h>

using namespace AsmJit;

// Generated functions prototypes.
typedef sysint_t (*MyFn0)();
typedef sysint_t (*MyFn1)(sysint_t);
typedef sysint_t (*MyFn2)(sysint_t, sysint_t);
typedef sysint_t (*MyFn3)(sysint_t, sysint_t, sysint_t);

static void* compileFunction(int args, bool naked, bool pushPopSequence, bool spillGp)
{
  Compiler c;

  // Not enabled by default...
  FileLogger logger(stderr);
  c.setLogger(&logger);

  switch (args)
  {
    case 0:
      c.newFunction(CALL_CONV_DEFAULT, FunctionBuilder0());
      break;
    case 1:
      c.newFunction(CALL_CONV_DEFAULT, FunctionBuilder1<sysint_t>());
      break;
    case 2:
      c.newFunction(CALL_CONV_DEFAULT, FunctionBuilder2<sysint_t, sysint_t>());
      break;
    case 3:
      c.newFunction(CALL_CONV_DEFAULT, FunctionBuilder3<sysint_t, sysint_t, sysint_t>());
      break;
  }
  c.getFunction()->setHint(FUNCTION_HINT_NAKED, naked);
  c.getFunction()->setHint(FUNCTION_HINT_PUSH_POP_SEQUENCE, pushPopSequence);

  GPVar gvar(c.newGP());
  XMMVar xvar(c.newXMM(VARIABLE_TYPE_XMM));

  if (spillGp)
  {
    GPVar somevar(c.newGP());
    c.mov(somevar, imm(0));
    c.spill(somevar);
  }

  c.alloc(gvar, nax);
  c.lea(gvar, xvar.m());
  c.and_(gvar, imm(15));
  c.ret(gvar);
  c.endFunction();

  return c.make();
}

static bool testFunction(int args, bool naked, bool pushPopSequence, bool spillGp)
{
  void* fn = compileFunction(args, naked, pushPopSequence, spillGp);
  sysint_t result = 0;

  printf("Test function (args=%d, naked=%d, pushPop=%d, spillGp=%d):", args, naked, pushPopSequence, spillGp);

  switch (args)
  {
    case 0:
      result = AsmJit::function_cast<MyFn0>(fn)();
      break;
    case 1:
      result = AsmJit::function_cast<MyFn1>(fn)(1);
      break;
    case 2:
      result = AsmJit::function_cast<MyFn2>(fn)(1, 2);
      break;
    case 3:
      result = AsmJit::function_cast<MyFn3>(fn)(1, 2, 3);
      break;
  }

  printf(" result=%d (expected 0)\n", (int)result);

  MemoryManager::getGlobal()->free(fn);
  return result == 0;
}

#define TEST_FN(naked, pushPop, spillGp) \
  testFunction(0, naked, pushPop, spillGp); \
  testFunction(1, naked, pushPop, spillGp); \
  testFunction(2, naked, pushPop, spillGp); \
  testFunction(3, naked, pushPop, spillGp);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  TEST_FN(false, false, false);
  TEST_FN(false, false, true );

  TEST_FN(false, true , false);
  TEST_FN(false, true , true );

  if (sizeof(sysint_t) == 8)
  {
    // 64-bit system, naked function must be also align variables to 16-bytes.
    TEST_FN(true , false, false);
    TEST_FN(true , false, true );

    TEST_FN(true , true , false);
    TEST_FN(true , true , true );
  }

  return 0;
}
