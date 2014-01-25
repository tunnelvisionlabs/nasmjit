// [AsmJit/WinRemoteRuntime]
// Contribution for remote process handling.
//
// [License]
// Zlib - See COPYING file in this package.
#include <asmjit/base.h>

// [Guard - Windows]
#if defined(ASMJIT_WINDOWS)
#include "winremoteruntime.h"

namespace asmjit {
namespace contrib {

WinRemoteRuntime::WinRemoteRuntime(HANDLE hProcess) :
  _hProcess(hProcess),
  _memoryManager(hProcess) {

  // We are patching another process so enable keep-virtual-memory option.
  _memoryManager.setKeepVirtualMemory(true);
}

WinRemoteRuntime::~WinRemoteRuntime() {
}

uint32_t WinRemoteRuntime::add(void** dest, BaseAssembler* assembler) {
  // Disallow generation of no code.
  size_t codeSize = assembler->getCodeSize();

  if (codeSize == 0) {
    *dest = NULL;
    return kErrorNoFunction;
  }

  // Allocate temporary memory where the code will be stored and relocated.
  void* codeData = ::malloc(codeSize);

  if (codeData == NULL) {
    *dest = NULL;
    return kErrorNoHeapMemory;
  }

  // Allocate a pernament remote process memory.
  void* processMemPtr = _memoryManager.alloc(codeSize, kMemAllocPermanent);

  if (processMemPtr == NULL) {
    ::free(codeData);
    *dest = NULL;
    return kErrorNoVirtualMemory;
  }

  // Relocate and write the code to the process memory.
  assembler->relocCode(codeData, (uintptr_t)processMemPtr);

  ::WriteProcessMemory(hProcess, processMemPtr, codeData, codeSize, NULL);
  ::free(codeData);

  *dest = processMemPtr;
  return kErrorOk;
}

} // contrib namespace
} // asmjit namespace

// [Guard - Windows]
#endif // ASMJIT_WINDOWS
