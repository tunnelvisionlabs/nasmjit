// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// This file is used as a dummy test. It's changed during development.

// [Dependencies - AsmJit]
#include <AsmJit/AsmJit.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is type of function we will generate
typedef void (*MyFn)(void);

static void dummyFunc(void) {}

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Log compiler output.
  FileLogger logger(stderr);
  logger.setLogBinary(true);

  // Create compiler.
  /*
  X86Compiler c;
  c.setLogger(&logger);

  c.newFunc(kX86FuncConvDefault, FuncBuilder0<Void>());
  c.getFunc()->setHint(kFuncHintNaked, true);

  X86CompilerFuncCall* ctx = c.call((void*)dummyFunc);
  ctx->setPrototype(kX86FuncConvDefault, FuncBuilder0<Void>());

  c.endFunction();
  */

  X86Compiler c;
  c.setLogger(&logger);

  c.newFunc(kX86FuncConvDefault, FuncBuilder0<void>());
  c.getFunc()->setHint(kFuncHintNaked, true);

  Label l1 = c.newLabel();
  Label l2 = c.newLabel();

  GpVar var1(c.newGpVar());
  GpVar var2(c.newGpVar());

  c.jmp(l2);
  c.bind(l1);
  c.bind(l2);

  c.ret();
  c.endFunction();

  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  // Call it.
  // printf("Result %llu\n", (unsigned long long)fn());

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
