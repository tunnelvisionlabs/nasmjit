// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// Create simple DWORD memory copy function for 32/64-bit x86 platform, this
// is enchanced version that's using X86Compiler class:
//
// void memcpy32(uint32_t* dst, const uint32_t* src, sysuint_t len);

// AsmJit library
#include <AsmJit/AsmJit.h>

// C library - printf
#include <stdio.h>

// It isn't needed to include namespace here, but when generating assembly it's
// easier to use directly eax, rax, ... instead of AsmJit::eax, AsmJit::rax, ...
using namespace AsmJit;

// This is type of function we will generate.
typedef void (*MemCpy32Fn)(uint32_t*, const uint32_t*, sysuint_t);

int main(int argc, char* argv[])
{
  // ==========================================================================
  // Part 1:

  // Create Compiler.
  X86Compiler c;

  FileLogger logger(stderr);
  c.setLogger(&logger);

  // Tell compiler the function prototype we want. It allocates variables representing
  // function arguments that can be accessed through Compiler or Function instance.
  c.newFunc(kX86FuncConvDefault, FuncBuilder3<Void, uint32_t*, const uint32_t*, sysuint_t>());

  // Try to generate function without prolog/epilog code:
  c.getFunc()->setHint(kFuncHintNaked, true);

  // Create labels.
  Label L_Loop = c.newLabel();
  Label L_Exit = c.newLabel();

  // Function arguments.
  GpVar dst(c.getGpArg(0));
  GpVar src(c.getGpArg(1));
  GpVar cnt(c.getGpArg(2));

  // Allocate loop variables registers (if they are not allocated already).
  c.alloc(dst);
  c.alloc(src);
  c.alloc(cnt);

  // Exit if length is zero.
  c.test(cnt, cnt);
  c.jz(L_Exit);

  // Loop.
  c.bind(L_Loop);

  // Copy DWORD (4 bytes).
  GpVar tmp(c.newGpVar(kX86VarTypeGpd));
  c.mov(tmp, dword_ptr(src));
  c.mov(dword_ptr(dst), tmp);

  // Increment dst/src pointers.
  c.add(src, 4);
  c.add(dst, 4);

  // Loop until cnt is not zero.
  c.dec(cnt);
  c.jnz(L_Loop);

  // Exit.
  c.bind(L_Exit);

  // Finish.
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Part 2:

  // Make JIT function.
  MemCpy32Fn fn = asmjit_cast<MemCpy32Fn>(c.make());

  // Ensure that everything is ok.
  if (!fn)
  {
    printf("Error making jit function (%u).\n", c.getError());
    return 1;
  }

  // Create some data.
  uint32_t dstBuffer[128];
  uint32_t srcBuffer[128];
  
  // Call the JIT function.
  fn(dstBuffer, srcBuffer, 128);
  
  // Free the JIT function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
