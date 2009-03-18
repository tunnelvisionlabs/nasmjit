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

// [Guard]
#ifndef _ASMJITLOCK_H
#define _ASMJITLOCK_H

// [Dependencies]
#include "AsmJitBuild.h"

#if defined(ASMJIT_WINDOWS)
#include <windows.h>
#endif // ASMJIT_WINDOWS

#if defined(ASMJIT_POSIX)
#include <pthread.h>
#endif // ASMJIT_POSIX

namespace AsmJit {

//! @addtogroup AsmJit_Platform
//! @{

//! @brief Lock.
struct Lock
{
#if defined(ASMJIT_WINDOWS)
  typedef CRITICAL_SECTION Handle;
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
  typedef pthread_mutex_t Handle;
#endif // ASMJIT_POSIX

  inline Lock()
  {
#if defined(ASMJIT_WINDOWS)
    InitializeCriticalSection(&_handle);
    // InitializeLockAndSpinCount(&_handle, 2000);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_handle, &attr);
#endif // ASMJIT_POSIX
  }

  inline ~Lock()
  {
#if defined(ASMJIT_WINDOWS)
    DeleteCriticalSection(&_handle);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_destroy(&_handle);
#endif // ASMJIT_POSIX
  }

  inline Handle& handle()
  {
    return _handle;
  }

  inline const Handle& handle() const
  {
    return _handle;
  }

  inline void lock()
  { 
#if defined(ASMJIT_WINDOWS)
    EnterCriticalSection(&_handle);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_lock(&_handle);
#endif // ASMJIT_POSIX
  }

  inline void unlock()
  {
#if defined(ASMJIT_WINDOWS)
    LeaveCriticalSection(&_handle);
#endif // ASMJIT_WINDOWS
#if defined(ASMJIT_POSIX)
    pthread_mutex_unlock(&_handle);
#endif // ASMJIT_POSIX
  }

private:
  Handle _handle;

  // disable copy
  ASMJIT_DISABLE_COPY(Lock);
};

struct AutoLock
{
  //! @brief Locks @a target.
  inline AutoLock(Lock& target) : _target(target)
  {
    _target.lock();
  }

  //! @brief Unlocks target.
  inline ~AutoLock()
  {
    _target.unlock();
  }

private:
  //! @brief Pointer to target (lock).
  Lock& _target;

  // disable copy
  ASMJIT_DISABLE_COPY(AutoLock);
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITLOCK_H
