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

// This is type of function we will generate
typedef sysint_t (*MyFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  X86Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunc(kX86FuncConvDefault, FuncBuilder0<sysint_t>());

  GpVar v0(c.newGpVar());
  GpVar v1(c.newGpVar());
  GpVar v2(c.newGpVar());
  GpVar v3(c.newGpVar());
  GpVar v4(c.newGpVar());

  c.xor_(v0, v0);

  c.mov(v1, 1);
  c.mov(v2, 2);
  c.mov(v3, 3);
  c.mov(v4, 4);

  c.add(v0, v1);
  c.add(v0, v2);
  c.add(v0, v3);
  c.add(v0, v4);

  c.ret(v0);
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());
  int result = (int)fn();

  printf("Result from JIT function: %d\n", result);
  printf("Status: %s\n", result == 10 ? "Success" : "Failure");

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
