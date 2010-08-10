// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2010, Petr Kobalicek <kobalicek.petr@gmail.com>
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

// [Guard]
#ifndef _ASMJIT_PLATFORM_H
#define _ASMJIT_PLATFORM_H

// [Dependencies]
#include "Build.h"

#if defined(ASMJIT_WINDOWS)
#include <windows.h>
#endif // ASMJIT_WINDOWS

#if defined(ASMJIT_POSIX)
#include <pthread.h>
#endif // ASMJIT_POSIX

// [Api-Begin]
#include "ApiBegin.h"

namespace AsmJit {

//! @addtogroup AsmJit_Platform
//! @{

// ============================================================================
// [AsmJit::Assert]
// ============================================================================

//! @brief Called in debug build on assertion failure.
//! @param file Source file name where it happened.
//! @param line Line in the source file.
//! @param exp Expression what failed.
//!
//! If you have problems with assertions simply put a breakpoint into
//! AsmJit::assertionFailure() method (see AsmJit/Platform.cpp file) and see
//! call stack.
ASMJIT_API void assertionFailure(const char* file, int line, const char* exp);

// ============================================================================
// [AsmJit::Lock]
// ============================================================================

//! @brief Lock - used in thread-safe code for locking.
struct ASMJIT_HIDDEN Lock
{
#if defined(ASMJIT_WINDOWS)
  typedef CRITICAL_SECTION Handle;
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
  typedef pthread_mutex_t Handle;
#endif // ASMJIT_POSIX

  //! @brief Create a new @ref Lock instance.
  inline Lock() ASMJIT_NOTHROW
  {
#if defined(ASMJIT_WINDOWS)
    InitializeCriticalSection(&_handle);
    // InitializeLockAndSpinCount(&_handle, 2000);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_init(&_handle, NULL);
#endif // ASMJIT_POSIX
  }

  //! @brief Destroy the @ref Lock instance.
  inline ~Lock() ASMJIT_NOTHROW
  {
#if defined(ASMJIT_WINDOWS)
    DeleteCriticalSection(&_handle);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_destroy(&_handle);
#endif // ASMJIT_POSIX
  }

  //! @brief Get handle.
  inline Handle& getHandle() ASMJIT_NOTHROW
  {
    return _handle;
  }

  //! @overload
  inline const Handle& getHandle() const ASMJIT_NOTHROW
  {
    return _handle;
  }

  //! @brief Lock.
  inline void lock() ASMJIT_NOTHROW
  {
#if defined(ASMJIT_WINDOWS)
    EnterCriticalSection(&_handle);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_lock(&_handle);
#endif // ASMJIT_POSIX
  }

  //! @brief Unlock.
  inline void unlock() ASMJIT_NOTHROW
  {
#if defined(ASMJIT_WINDOWS)
    LeaveCriticalSection(&_handle);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_unlock(&_handle);
#endif // ASMJIT_POSIX
  }

private:
  //! @brief Handle.
  Handle _handle;

  // Disable copy.
  ASMJIT_DISABLE_COPY(Lock)
};

// ============================================================================
// [AsmJit::AutoLock]
// ============================================================================

//! @brief Scope auto locker.
struct ASMJIT_HIDDEN AutoLock
{
  //! @brief Locks @a target.
  inline AutoLock(Lock& target) ASMJIT_NOTHROW : _target(target)
  {
    _target.lock();
  }

  //! @brief Unlocks target.
  inline ~AutoLock() ASMJIT_NOTHROW
  {
    _target.unlock();
  }

private:
  //! @brief Pointer to target (lock).
  Lock& _target;

  // Disable copy.
  ASMJIT_DISABLE_COPY(AutoLock)
};

// ============================================================================
// [AsmJit::VirtualMemory]
// ============================================================================

//! @brief Class that helps with allocating memory for executing code
//! generated by JIT compiler.
//!
//! There are defined functions that provides facility to allocate and free
//! memory where can be executed code. If processor and operating system
//! supports execution protection then you can't run code from normally
//! malloc()'ed memory.
//!
//! Functions are internally implemented by operating system dependent way.
//! VirtualAlloc() function is used for Windows operating system and mmap()
//! for posix ones. If you want to study or create your own functions, look
//! at VirtualAlloc() or mmap() documentation (depends on you target OS).
//!
//! Under posix operating systems is also useable mprotect() function, that
//! can enable execution protection to malloc()'ed memory block.
struct ASMJIT_API VirtualMemory
{
  //! @brief Allocate virtual memory.
  //!
  //! Pages are readable/writeable, but they are not guaranteed to be
  //! executable unless 'canExecute' is true. Returns the address of
  //! allocated memory, or NULL if failed.
  static void* alloc(sysuint_t length, sysuint_t* allocated, bool canExecute) ASMJIT_NOTHROW;

  //! @brief Free memory allocated by @c alloc()
  static void free(void* addr, sysuint_t length) ASMJIT_NOTHROW;

#if defined(ASMJIT_WINDOWS)
  //! @brief Allocate virtual memory of @a hProcess.
  //!
  //! @note This function is windows specific and unportable.
  static void* allocProcessMemory(HANDLE hProcess, sysuint_t length, sysuint_t* allocated, bool canExecute) ASMJIT_NOTHROW;

  //! @brief Free virtual memory of @a hProcess.
  //!
  //! @note This function is windows specific and unportable.
  static void freeProcessMemory(HANDLE hProcess, void* addr, sysuint_t length) ASMJIT_NOTHROW;
#endif // ASMJIT_WINDOWS

  //! @brief Get the alignment guaranteed by alloc().
  static sysuint_t getAlignment() ASMJIT_NOTHROW;

  //! @brief Get size of single page.
  static sysuint_t getPageSize() ASMJIT_NOTHROW;
};

//! @}

} // AsmJit namespace

// [Api-End]
#include "ApiEnd.h"

// [Guard]
#endif // _ASMJIT_PLATFORM_H
