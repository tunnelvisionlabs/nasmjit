// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Export]
#define ASMJIT_EXPORTS

// [Dependencies - AsmJit]
#include "../base/assert.h"

// [Api-Begin]
#include "../base/apibegin.h"

// helpers
namespace asmjit {

// ============================================================================
// [asmjit::Assert]
// ============================================================================

void assertionFailed(const char* exp, const char* file, int line) {
  ::fprintf(stderr, "Assertion failed: %s\n, file %s, line %d\n", exp, file, line);
  ::abort();
}

} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"
