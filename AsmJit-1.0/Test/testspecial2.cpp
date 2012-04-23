// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// Test special instruction generation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>

// This is type of function we will generate
typedef void (*MyFn)(int32_t*, int32_t, int32_t, int32_t);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  X86Compiler c;

  // Log compiler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  {
    c.newFunc(kX86FuncConvDefault, FuncBuilder4<Void, int32_t*, int32_t, int32_t, int32_t>());

    GpVar dst0(c.getGpArg(0));
    GpVar v0(c.getGpArg(1));

    c.shl(v0, c.getGpArg(2));
    c.ror(v0, c.getGpArg(3));
    
    c.mov(dword_ptr(dst0), v0);
    c.endFunction();
  }
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  {
    int32_t out;
    int32_t v0 = 0x000000FF;
    int32_t expected = 0x0000FF00;

    fn(&out, v0, 16, 8);

    printf("out=%d (expected %d)\n", out, expected);
    printf("Status: %s\n", (out == expected) ? "Success" : "Failure");
  }

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
