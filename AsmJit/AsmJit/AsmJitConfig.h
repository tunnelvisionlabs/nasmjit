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

// This file is designed to be changeable. Platform specific changes
// should be applied to this file and this guarantes and never versions
// of BlitJit library will never overwrite generated config files.
//
// So modify this will by your build system or hand.

// [Guard]
#ifndef _ASMJITCONFIG_H
#define _ASMJITCONFIG_H

// [Include]
//
// Here should be optional include files that's needed fo successfuly
// use macros defined here. Remember, AsmJit uses only AsmJit namespace
// and all macros are used within it. So for example crash handler is
// not called as AsmJit::crash(0) in ASMJIT_CRASH() macro, but simply
// as crash(0).
#include <stdlib.h>

// [AsmJit - OS]
#define ASMJIT_WINDOWS 1
#define ASMJIT_POSIX 2

#if defined(WINDOWS) || defined(_WIN32) || defined(_WIN64)
# define ASMJIT_OS ASMJIT_WINDOWS
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
      defined(__NetBSD__) || defined(__DragonFly__)
# define ASMJIT_OS ASMJIT_POSIX
#endif

#if !defined(ASMJIT_OS) || ASMJIT_OS < 1
# error "AsmJit - Define ASMJIT_OS macro to your operating system"
#endif // ASMJIT_OS

// [AsmJit - API]
#if !defined(ASMJIT_API)
# define ASMJIT_API
#endif // ASMJIT_API

#if !defined(ASMJIT_VAR)
# define ASMJIT_VAR extern
#endif // ASMJIT_VAR

// [AsmJit - Memory]
#if !defined(ASMJIT_MALLOC)
# define ASMJIT_MALLOC ::malloc
#endif // ASMJIT_MALLOC

#if !defined(ASMJIT_REALLOC)
# define ASMJIT_REALLOC ::realloc
#endif // ASMJIT_REALLOC

#if !defined(ASMJIT_FREE)
# define ASMJIT_FREE ::free
#endif // ASMJIT_FREE

// [AsmJit - Crash handler]
namespace AsmJit
{
  static void crash(int* ptr = 0) { *ptr = 0; }
}

// [AsmJit - Architecture]
// define it only if it's not defined. In some systems we can
// use -D command in compiler to bypass this autodetection.
#if !defined(ASMJIT_X86) && !defined(ASMJIT_X64)
# if defined(__x86_64__) || defined(_WIN64)
#  define ASMJIT_X64 // x86-64
# else
#  define ASMJIT_X86
# endif
#endif

// [AsmJit - Calling Conventions]
#if defined(ASMJIT_X86)
# if defined(__GNUC__)
#  define ASMJIT_FASTCALL_2 __attribute__((regparm(2)))
#  define ASMJIT_FASTCALL_3 __attribute__((regparm(3)))
#  define ASMJIT_STDCALL    __attribute__((stdcall))
#  define ASMJIT_CDECL      __attribute__((cdecl))
# else
#  define ASMJIT_FASTCALL_2 __fastcall
#  define ASMJIT_STDCALL    __stdcall
#  define ASMJIT_CDECL      __cdecl
# endif
#else
# define ASMJIT_FASTCALL_2
# define ASMJIT_STDCALL
# define ASMJIT_CDECL
#endif // ASMJIT_X86

// [AsmJit - Types]
namespace AsmJit
{
  typedef char Int8;
  typedef unsigned char UInt8;
  typedef short Int16;
  typedef unsigned short UInt16;
  typedef int Int32;
  typedef unsigned int UInt32;

#if defined(_MSC_VER)
  typedef __int64 Int64;
  typedef unsigned __int64 UInt64;
#else // GCC, other compilers ?
  typedef long long Int64;
  typedef unsigned long long UInt64;
#endif

#if defined(ASMJIT_X86)
  typedef Int32 SysInt;
  typedef UInt32 SysUInt;
#else
  typedef Int64 SysInt;
  typedef UInt64 SysUInt;
#endif
}

#if defined(_MSC_VER)
# define ASMJIT_INT64_C(num) num##i64
# define ASMJIT_UINT64_C(num) num##ui64
#else
# define ASMJIT_INT64_C(num) num##LL
# define ASMJIT_UINT64_C(num) num##ULL
#endif

// [BlitJit - Debug]
#if defined(DEBUG) || 1
# if !defined(ASMJIT_CRASH)
#  define ASMJIT_CRASH() crash()
# endif
# if !defined(ASMJIT_ASSERT)
#  define ASMJIT_ASSERT(exp) do { if (!(exp)) ASMJIT_CRASH(); } while(0)
# endif
#else
# if !defined(ASMJIT_CRASH)
#  define ASMJIT_CRASH() do {} while(0)
# endif
# if !defined(ASMJIT_ASSERT)
#  define ASMJIT_ASSERT(exp) do {} while(0)
# endif
#endif // DEBUG

// [Guard]
#endif // _ASMJITCONFIG_H
