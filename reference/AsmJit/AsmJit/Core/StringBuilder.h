// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_CORE_STRINGBUILDER_H
#define _ASMJIT_CORE_STRINGBUILDER_H

// [Dependencies - AsmJit]
#include "../Core/Assert.h"
#include "../Core/Defs.h"

namespace AsmJit {

//! @addtogroup AsmJit_Core
//! @{

// ============================================================================
// [AsmJit::StringBuilder]
// ============================================================================

//! @brief String builder.
//!
//! String builder was designed to be able to build a string using append like
//! operation to append numbers, other strings, or signle characters. It can
//! allocate it's own buffer or use a buffer created on the stack.
//!
//! String builder contains method specific to AsmJit functionality, used for
//! logging or HTML output.
struct StringBuilder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ASMJIT_API StringBuilder() ASMJIT_NOTHROW;
  ASMJIT_API ~StringBuilder() ASMJIT_NOTHROW;

  inline StringBuilder(_DontInitialize&) ASMJIT_NOTHROW {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

public:
  //! @brief Get string builder capacity.
  inline size_t getCapacity() const ASMJIT_NOTHROW
  { return _capacity; }

  //! @brief Get length.
  inline size_t getLength() const ASMJIT_NOTHROW
  { return _length; }

  //! @brief Get null-terminated string data.
  inline char* getData() ASMJIT_NOTHROW
  { return _data; }

  //! @brief Get null-terminated string data (const).
  inline const char* getData() const ASMJIT_NOTHROW
  { return _data; }

  // --------------------------------------------------------------------------
  // [Prepare / Reserve]
  // --------------------------------------------------------------------------

  //! @brief Prepare to set/append.
  ASMJIT_API char* prepare(uint32_t op, size_t len) ASMJIT_NOTHROW;

  //! @brief Reserve @a to bytes in string builder.
  ASMJIT_API bool reserve(size_t to) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  //! @brief Clear the content in String builder.
  ASMJIT_API void clear() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  ASMJIT_API bool _opString(uint32_t op, const char* str, size_t len = kInvalidSize) ASMJIT_NOTHROW;
  ASMJIT_API bool _opVFormat(uint32_t op, const char* fmt, va_list ap) ASMJIT_NOTHROW; 
  ASMJIT_API bool _opChars(uint32_t op, char c, size_t len) ASMJIT_NOTHROW;
  ASMJIT_API bool _opNumber(uint32_t op, uint64_t i, uint32_t base = 0, size_t width = 0, uint32_t flags = 0) ASMJIT_NOTHROW;
  ASMJIT_API bool _opHex(uint32_t op, const void* data, size_t length) ASMJIT_NOTHROW;

  //! @brief Replace the current content by @a str of @a len.
  inline bool setString(const char* str, size_t len = kInvalidSize) ASMJIT_NOTHROW
  { return _opString(kStringBuilderOpSet, str, len); }

  //! @brief Replace the current content by formatted string @a fmt.
  inline bool setVFormat(const char* fmt, va_list ap) ASMJIT_NOTHROW
  { return _opVFormat(kStringBuilderOpSet, fmt, ap); }

  //! @brief Replace the current content by formatted string @a fmt.
  ASMJIT_API bool setFormat(const char* fmt, ...) ASMJIT_NOTHROW;

  //! @brief Replace the current content by @a c of @a len.
  inline bool setChars(char c, size_t len) ASMJIT_NOTHROW
  { return _opChars(kStringBuilderOpSet, c, len); }

  //! @brief Replace the current content by @a i..
  inline bool setNumber(uint64_t i, uint32_t base = 0, size_t width = 0, uint32_t flags = 0) ASMJIT_NOTHROW
  { return _opNumber(kStringBuilderOpSet, i, base, width, flags); }

  //! @brief Append @a str of @a len.
  inline bool appendString(const char* str, size_t len = kInvalidSize) ASMJIT_NOTHROW
  { return _opString(kStringBuilderOpAppend, str, len); }

  //! @brief Append a formatted string @a fmt to the current content.
  inline bool appendVFormat(const char* fmt, va_list ap) ASMJIT_NOTHROW
  { return _opVFormat(kStringBuilderOpAppend, fmt, ap); }

  //! @brief Append a formatted string @a fmt to the current content.
  ASMJIT_API bool appendFormat(const char* fmt, ...) ASMJIT_NOTHROW;

  //! @brief Append @a c of @a len.
  inline bool appendChars(char c, size_t len) ASMJIT_NOTHROW
  { return _opChars(kStringBuilderOpAppend, c, len); }

  //! @brief Append @a i.
  inline bool appendNumber(uint64_t i, uint32_t base = 0, size_t width = 0, uint32_t flags = 0) ASMJIT_NOTHROW
  { return _opNumber(kStringBuilderOpAppend, i, base, width, flags); }

  //! @brief Check for equality with other @a str.
  ASMJIT_API bool eq(const char* str, size_t len = kInvalidSize) const ASMJIT_NOTHROW;

  //! @brief Check for equality with StringBuilder @a other.
  inline bool eq(const StringBuilder& other) const ASMJIT_NOTHROW
  { return eq(other._data); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  inline bool operator==(const StringBuilder& other) const ASMJIT_NOTHROW { return  eq(other); }
  inline bool operator!=(const StringBuilder& other) const ASMJIT_NOTHROW { return !eq(other); }

  inline bool operator==(const char* str) const ASMJIT_NOTHROW { return  eq(str); }
  inline bool operator!=(const char* str) const ASMJIT_NOTHROW { return !eq(str); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief String data.
  char* _data;
  //! @brief Length.
  size_t _length;
  //! @brief Capacity.
  size_t _capacity;
  //! @brief Whether the string can be freed.
  size_t _canFree;

  ASMJIT_NO_COPY(StringBuilder)
};

// ============================================================================
// [AsmJit::StringBuilderT]
// ============================================================================

template<size_t N>
struct StringBuilderT : public StringBuilder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  inline StringBuilderT() :
    StringBuilder(_DontInitialize())
  {
    _data = _embeddedData;
    _data[0] = 0;

    _length = 0;
    _capacity = 0;
    _canFree = false;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Embedded data.
  char _embeddedData[(N + sizeof(uintptr_t)) & ~(sizeof(uintptr_t) - 1)];

  ASMJIT_NO_COPY(StringBuilderT<N>)
};

//! @}

} // AsmJit namespace

#endif // _ASMJIT_CORE_STRINGBUILDER_H
