// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_CONFIG_H
#define _ASMJIT_CONFIG_H

// This file can be used to modify built-in features of AsmJit. AsmJit is by
// default compiled only for host processor to enable JIT compilation. Both
// Assembler and Compiler code generators are compiled by default. To enable
// cross-compiling.

// ============================================================================
// [AsmJit - Debug]
// ============================================================================

// #define ASMJIT_DEBUG        // Define to enable debugging.
// #define ASMJIT_NO_DEBUG     // Define to disable debugging.

// ============================================================================
// [AsmJit - Static]
// ============================================================================

// #define ASMJIT_STATIC       // Define to enable static-library build.
// #define ASMJIT_API          // Define to override ASMJIT_API decorator.

// ============================================================================
// [AsmJit - Features]
// ============================================================================

// If none of these is defined AsmJit will select host architecture by default.

// #define ASMJIT_BUILD_X86    // Define to enable x86 instruction set (32-bit).
// #define ASMJIT_BUILD_X64    // Define to enable x64 instruction set (64-bit).
// #define ASMJIT_BUILD_HOST   // Define to enable host instruction set.

// [Guard]
#endif // _ASMJIT_CONFIG_H
