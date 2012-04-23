// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// Test variable scope detection in loop.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJit.h>

// This is type of function we will generate
typedef void (*MyFn)(uint32_t*);

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
    c.newFunc(kX86FuncConvDefault, FuncBuilder1<Void, uint32_t*>());
    c.getFunc()->setHint(kFuncHintNaked, false);

    GpVar v0(c.newGpVar(kX86VarTypeGpd));
    GpVar v1(c.newGpVar(kX86VarTypeGpd));
    GpVar cnt(c.newGpVar(kX86VarTypeGpd));

    c.xor_(v0, v0);
    c.xor_(v1, v1);
    c.spill(v0);
    c.spill(v1);

    Label L(c.newLabel());
    c.mov(cnt, imm(32));
    c.bind(L);

    c.inc(v1);
    c.add(v0, v1);

    c.dec(cnt);
    c.jnz(L);

    GpVar a0(c.getGpArg(0));
    c.mov(dword_ptr(a0), v0);
    c.endFunction();
  }
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(c.make());

  {
    uint32_t out;
    uint32_t expected =  0+ 1+ 2+ 3+ 4+ 5+ 6+ 7+ 8+ 9+
                        10+11+12+13+14+15+16+17+18+19+
                        20+21+22+23+24+25+26+27+28+29+
                        30+31+32;

    fn(&out);

    printf("out=%u (should be %u)\n", out, expected);
    printf("Status: %s\n", (out == expected) ? "Success" : "Failure");
  }

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
