// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// Recursive function call test.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/X86.h>

// Type of generated function.
typedef int (*MyFn)(int);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  X86Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  X86CompilerFuncCall* ctx;
  Label skip(c.newLabel());

  X86CompilerFuncDecl* func = c.newFunc(kX86FuncConvDefault, FuncBuilder1<int, int>());
  func->setHint(kFuncHintNaked, true);

  GpVar var(c.getGpArg(0));
  c.cmp(var, imm(1));
  c.jle(skip);

  GpVar tmp(c.newGpVar(kX86VarTypeInt32));
  c.mov(tmp, var);
  c.dec(tmp);

  ctx = c.call(func->getEntryLabel());
  ctx->setPrototype(kX86FuncConvDefault, FuncBuilder1<int, int>());
  ctx->setArgument(0, tmp);
  ctx->setReturn(tmp);
  c.mul(c.newGpVar(kX86VarTypeInt32), var, tmp);

  c.bind(skip);
  c.ret(var);
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  printf("Factorial 5 == %d\n", fn(5));

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
