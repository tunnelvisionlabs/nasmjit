// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// This file is used to test AsmJit register allocator.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>

// This is type of function we will generate
typedef void (*MyFn)(int*, int*);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  X86Compiler c;

  // Log assembler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  c.newFunc(kX86FuncConvDefault, FuncBuilder2<Void, int*, int*>());

  // Function arguments.
  GpVar a1(c.getGpArg(0));
  GpVar a2(c.getGpArg(1));

  // Create some variables.
  GpVar x1(c.newGpVar(kX86VarTypeGpd));
  GpVar x2(c.newGpVar(kX86VarTypeGpd));
  GpVar x3(c.newGpVar(kX86VarTypeGpd));
  GpVar x4(c.newGpVar(kX86VarTypeGpd));
  GpVar x5(c.newGpVar(kX86VarTypeGpd));
  GpVar x6(c.newGpVar(kX86VarTypeGpd));
  GpVar x7(c.newGpVar(kX86VarTypeGpd));
  GpVar x8(c.newGpVar(kX86VarTypeGpd));

  GpVar t(c.newGpVar(kX86VarTypeGpd));

  // Setup variables (use mov with reg/imm to se if register allocator works).
  c.mov(x1, 1);
  c.mov(x2, 2);
  c.mov(x3, 3);
  c.mov(x4, 4);
  c.mov(x5, 5);
  c.mov(x6, 6);
  c.mov(x7, 7);
  c.mov(x8, 8);

  // Make sum (addition)
  c.xor_(t, t);
  c.add(t, x1);
  c.add(t, x2);
  c.add(t, x3);
  c.add(t, x4);
  c.add(t, x5);
  c.add(t, x6);
  c.add(t, x7);
  c.add(t, x8);

  // Store result to a given pointer in first argument.
  c.mov(dword_ptr(a1), t);

  // Make sum (subtraction).
  c.xor_(t, t);
  c.sub(t, x1);
  c.sub(t, x2);
  c.sub(t, x3);
  c.sub(t, x4);
  c.sub(t, x5);
  c.sub(t, x6);
  c.sub(t, x7);
  c.sub(t, x8);

  // Store result to a given pointer in second argument.
  c.mov(dword_ptr(a2), t);

  // End of function.
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  // Call it.
  int x;
  int y;
  fn(&x, &y);

  printf("\nResults from JIT function: %d %d\n", x, y);
  printf("Status: %s\n", (x == 36 && y == -36) ? "Success" : "Failure");

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
