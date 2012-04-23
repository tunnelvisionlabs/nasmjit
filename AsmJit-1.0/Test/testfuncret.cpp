// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// This file is used to test function with many arguments. Bug originally
// reported by Tilo Nitzsche for X64W and X64U calling conventions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>
// This is type of function we will generate.
typedef int (*MyFn)(int, int, int);

static int FuncA(int x, int y) { return x + y; }
static int FuncB(int x, int y) { return x * y; }

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  X86Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunc(kX86FuncConvDefault, FuncBuilder3<int, int, int, int>());
  {
    GpVar x(c.getGpArg(0));
    GpVar y(c.getGpArg(1));
    GpVar op(c.getGpArg(2));

    Label opAdd(c.newLabel());
    Label opMul(c.newLabel());

    c.cmp(op, 0);
    c.jz(opAdd);

    c.cmp(op, 1);
    c.jz(opMul);

    {
      GpVar result(c.newGpVar());
      c.mov(result, imm(0));
      c.ret(result);
    }

    {
      c.bind(opAdd);

      GpVar result(c.newGpVar());
      X86CompilerFuncCall* ctx = c.call((void*)FuncA);
      ctx->setPrototype(kX86FuncConvDefault, FuncBuilder2<int, int, int>());
      ctx->setArgument(0, x);
      ctx->setArgument(1, y);
      ctx->setReturn(result);
      c.ret(result);
    }

    {
      c.bind(opMul);

      GpVar result(c.newGpVar());
      X86CompilerFuncCall* ctx = c.call((void*)FuncB);
      ctx->setPrototype(kX86FuncConvDefault, FuncBuilder2<int, int, int>());
      ctx->setArgument(0, x);
      ctx->setArgument(1, y);
      ctx->setReturn(result);
      c.ret(result);
    }
  }
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());
  int result = fn(4, 8, 1);

  printf("Result from JIT function: %d (Expected 32) \n", result);
  printf("Status: %s\n", result == 32 ? "Success" : "Failure");

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
