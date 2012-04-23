// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// This file is used as a function call test (calling functions inside
// the generated code).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>

// Type of generated function.
typedef void (*MyFn)(void);

// Function that is called inside the generated one. Because this test is 
// mainly about register arguments, we need to use the fastcall calling 
// convention under 32-bit mode.
static void ASMJIT_FASTCALL simpleFn(int a) {}

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

  c.newFunc(kX86FuncConvDefault, FuncBuilder0<Void>());
  c.getFunc()->setHint(kFuncHintNaked, true);

  // Call a function.
  GpVar address(c.newGpVar());
  GpVar argument(c.newGpVar(kX86VarTypeGpd));

  c.mov(address, imm((sysint_t)(void*)simpleFn));

  c.mov(argument, imm(1));
  ctx = c.call(address);
  ctx->setPrototype(kX86FuncConvCompatFastCall, FuncBuilder1<Void, int>());
  ctx->setArgument(0, argument);
  c.unuse(argument);

  c.mov(argument, imm(2));
  ctx = c.call(address);
  ctx->setPrototype(kX86FuncConvCompatFastCall, FuncBuilder1<Void, int>());
  ctx->setArgument(0, argument);

  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  fn();

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
