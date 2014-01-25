// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

#if !defined(_ASMJIT_BUILD_H)
#include "../build.h"
#endif // !_ASMJIT_BUILD_H

// ============================================================================
// [MSVC]
// ============================================================================

#if defined(_MSC_VER)

// Disable some warnings we know about
#pragma warning(push)
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 4201) // nameless struct/union
#pragma warning(disable: 4244) // '+=' : conversion from 'int' to 'x', possible
                               // loss of data
#pragma warning(disable: 4251) // struct needs to have dll-interface to be used
                               // by clients of struct ...
#pragma warning(disable: 4275) // non dll-interface struct ... used as base for
                               // dll-interface struct
#pragma warning(disable: 4355) // this used in base member initializer list
#pragma warning(disable: 4480) // specifying underlying type for enum
#pragma warning(disable: 4800) // forcing value to bool 'true' or 'false'

// Rename symbols.
#if !defined(vsnprintf)
#define ASMJIT_DEFINED_VSNPRINTF
#define vsnprintf _vsnprintf
#endif // !vsnprintf

#if !defined(snprintf)
#define ASMJIT_DEFINED_SNPRINTF
#define snprintf _snprintf
#endif // !snprintf

#endif // _MSC_VER

// ============================================================================
// [GNUC]
// ============================================================================

#if defined(__GNUC__)
// GCC warnings fix: I can't understand why GCC has no interface to push/pop
// specific warnings.
// # if (__GNUC__ * 10000  + __GNUC_MINOR__ * 100  + __GNUC_PATCHLEVEL__) >= 402001
// #  pragma GCC diagnostic ignored "-w"
// # endif
#endif // __GNUC__
