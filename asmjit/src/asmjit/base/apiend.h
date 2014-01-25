// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// ============================================================================
// [MSVC]
// ============================================================================

#if defined(_MSC_VER)

// Pop disabled warnings by ApiBegin.h
#pragma warning(pop)

// Rename symbols back.
#if defined(ASMJIT_DEFINED_VSNPRINTF)
#undef ASMJIT_DEFINED_VSNPRINTF
#undef vsnprintf
#endif // ASMJIT_DEFINED_VSNPRINTF

#if defined(ASMJIT_DEFINED_SNPRINTF)
#undef ASMJIT_DEFINED_SNPRINTF
#undef snprintf
#endif // ASMJIT_DEFINED_SNPRINTF

#endif // _MSC_VER

// ============================================================================
// [GNUC]
// ============================================================================

#if defined(__GNUC__)
#endif // __GNUC__
