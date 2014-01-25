// [AsmJit/WinRemoteRuntime]
// Contribution for remote process handling.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_CONTRIB_WINREMOTERUNTIME_H
#define _ASMJIT_CONTRIB_WINREMOTERUNTIME_H

// [Dependencies]
#include <asmjit/base.h>

// [Guard - Windows]
#if defined(ASMJIT_WINDOWS)

namespace asmjit {
namespace contrib {

// ============================================================================
// [asmjit::WinRemoteRuntime]
// ============================================================================

//! @brief WinRemoteRuntime can be used to inject code to a remote process.
struct WinRemoteRuntime : public BaseRuntime {
  ASMJIT_NO_COPY(WinRemoteRuntime)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  //! @brief Create a @c WinRemoteRuntime instance for a given @a hProcess.
  WinRemoteRuntime(HANDLE hProcess);

  //! @brief Destroy the @c WinRemoteRuntime instance.
  virtual ~WinRemoteRuntime();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the remote process handle.
  inline HANDLE getProcess() const { return _hProcess; }

  //! @brief Get the virtual memory manager.
  inline VirtualMemoryManager* getMemoryManager() { return &_memoryManager; }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual uint32_t add(void** dest, BaseAssembler* assembler);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Process.
  HANDLE _hProcess;
  
  //! @brief Virtual memory manager.
  VirtualMemoryManager _memoryManager;
};

} // contrib namespace
} // asmjit namespace

// [Guard - Windows]
#endif // ASMJIT_WINDOWS

// [Guard]
#endif // _ASMJIT_CONTRIB_WINREMOTERUNTIME_H
