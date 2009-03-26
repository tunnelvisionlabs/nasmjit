// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// This file is used to test AsmJit compiler states.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJitCompiler.h>
#include <AsmJit/AsmJitLogger.h>
#include <AsmJit/AsmJitMemoryManager.h>

// This is type of function we will generate
typedef void (*MyFn)(void);

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create compiler.
  Compiler c;

  // Log assembler output.
  FileLogger logger(stderr);
  c.setLogger(&logger);

  Function& f = *c.newFunction(CALL_CONV_DEFAULT, BuildFunction0());

  // Possibilities to improve code:
  //   f.setNaked(true);
  //   f.setAllocableEbp(true);

  // Variables
  SysIntRef var1(f.newVariable(VARIABLE_TYPE_SYSINT));
  SysIntRef var2(f.newVariable(VARIABLE_TYPE_SYSINT));

  var1.x(REG_NAX);
  var2.x(REG_NBX);

  // Alloc and initialize
  c.mov(var1.x(REG_NAX), imm(33));
  c.mov(var2.x(REG_NAX), imm(44));

  // Simple test
  {
    // Save state
    StateRef state(f.saveState());
    // Spill
    var1.spill();
    var2.spill();
  } // Restore state

  // Complex test
  Label* L = c.newLabel();
  { // Save state
    StateRef s(f.saveState());

    // Spill first
    var1.spill();
    
    // Now, this is the complex test. We want to jump out, but Compiler must
    // save current state and restore it to previously saved 's'. This must
    // be done in extern label where will be implemented restore operation
    // and there will be jump back.
    c.jmp(L);
    // c.jumpWithRestore(L, s);

    // Spill second
    var2.spill();
  } // Restore state
  c.bind(L);

  // End of function.
  c.endFunction();
  // ==========================================================================

  // ==========================================================================
  // Make function.
  MyFn fn = function_cast<MyFn>(c.make());

  // Call it.
  fn();

  // If function is not needed again it should be freed.
  MemoryManager::global()->free((void*)fn);
  // ==========================================================================

  return 0;
}
