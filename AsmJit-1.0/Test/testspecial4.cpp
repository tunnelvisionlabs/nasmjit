// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// This file is used to test setcc instruction generation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>

// This is type of function we will generate
typedef void (*MyFn)(int, int, char*);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  X86Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunc(kX86FuncConvDefault, FuncBuilder3<Void, int, int, char*>());
  c.getFunc()->setHint(kFuncHintNaked, true);

  GpVar src0(c.getGpArg(0));
  GpVar src1(c.getGpArg(1));
  GpVar dst0(c.getGpArg(2));

  c.cmp(src0, src1);
  c.setz(byte_ptr(dst0));

  // Finish.
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  // Results storage.
  char r[4];

  // Call it
  fn(0, 0, &r[0]); // We are expecting 1 (0 == 0).
  fn(0, 1, &r[1]); // We are expecting 0 (0 != 1).
  fn(1, 0, &r[2]); // We are expecting 0 (1 != 0).
  fn(1, 1, &r[3]); // We are expecting 1 (1 == 1).

  printf("Result from JIT function: %d %d %d %d\n", r[0], r[1], r[2], r[3]);
  printf("Status: %s\n", (r[0] == 1 && r[1] == 0 && r[2] == 0 && r[3] == 1) ? "Success" : "Failure");

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
