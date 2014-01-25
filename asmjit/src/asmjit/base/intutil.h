// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_BASE_INTUTIL_H
#define _ASMJIT_BASE_INTUTIL_H

// [Dependencies - AsmJit]
#include "../base/assert.h"
#include "../base/globals.h"

#if defined(_MSC_VER)
#pragma intrinsic(_BitScanForward)
#endif // ASMJIT_OS_WINDOWS

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {

//! @addtogroup asmjit_base
//! @{

// ============================================================================
// [asmjit::IntTraits]
// ============================================================================

template<typename T>
struct IntTraits {
  enum {
    kIsSigned = (~static_cast<T>(0)) < static_cast<T>(0),
    kIsUnsigned = !kIsSigned,

    kIs8Bit = sizeof(T) == 1,
    kIs16Bit = sizeof(T) == 2,
    kIs32Bit = sizeof(T) == 4,
    kIs64Bit = sizeof(T) == 8,

    kIsIntPtr = sizeof(T) == sizeof(intptr_t)
  };
};

// ============================================================================
// [asmjit::IntUtil]
// ============================================================================

struct IntUtil {
  // --------------------------------------------------------------------------
  // [Float <-> Int]
  // --------------------------------------------------------------------------

  union Float {
    int32_t i;
    float f;
  };

  union Double {
    int64_t i;
    double d;
  };

  static ASMJIT_INLINE int32_t floatAsInt(float f) { Float m; m.f = f; return m.i; }
  static ASMJIT_INLINE float intAsFloat(int32_t i) { Float m; m.i = i; return m.f; }

  static ASMJIT_INLINE int64_t doubleAsInt(double d) { Double m; m.d = d; return m.i; }
  static ASMJIT_INLINE double intAsDouble(int64_t i) { Double m; m.i = i; return m.d; }

  // --------------------------------------------------------------------------
  // [AsmJit - Pack / Unpack]
  // --------------------------------------------------------------------------

  static ASMJIT_INLINE uint32_t pack32_4x8(uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3) {
#if defined(ASMJIT_HOST_LE)
    return u0 + (u1 << 8) + (u2 << 16) + (u3 << 24);
#else
    return (u0 << 24) + (u1 << 16) + (u2 << 8) + u3;
#endif // ASMJIT_HOST
  }

  static ASMJIT_INLINE uint64_t pack64_2x32(uint32_t u0, uint32_t u1) {
#if defined(ASMJIT_HOST_LE)
    return (static_cast<uint64_t>(u1) << 32) + u0;
#else
    return (static_cast<uint64_t>(u0) << 32) + u1;
#endif // ASMJIT_HOST
  }

  // --------------------------------------------------------------------------
  // [AsmJit - Min/Max]
  // --------------------------------------------------------------------------

  // NOTE: Because some environments declare min() and max() as macros, it has
  // been decided to use different name so we never collide with them.

  template<typename T>
  static ASMJIT_INLINE T iMin(const T& a, const T& b) { return a < b ? a : b; }

  template<typename T>
  static ASMJIT_INLINE T iMax(const T& a, const T& b) { return a > b ? a : b; }

  // --------------------------------------------------------------------------
  // [AsmJit - MaxUInt]
  // --------------------------------------------------------------------------

  template<typename T>
  static ASMJIT_INLINE T maxUInt() { return ~T(0); }

  // --------------------------------------------------------------------------
  // [AsmJit - InInterval]
  // --------------------------------------------------------------------------

  template<typename T>
  static ASMJIT_INLINE bool inInterval(const T& x, const T& start, const T& end)
  { return x >= start && x <= end; }

  // --------------------------------------------------------------------------
  // [AsmJit - IsInt/IsUInt]
  // --------------------------------------------------------------------------

  //! @brief Get whether the given integer @a x can be casted to a signed 8-bit
  //! integer.
  template<typename T>
  static ASMJIT_INLINE bool isInt8(T x) {
    if (IntTraits<T>::kIsSigned)
      return sizeof(T) <= sizeof(int8_t) ? true : x >= T(-128) && x <= T(127);
    else
      return x <= T(127);
  }

  //! @brief Get whether the given integer @a x can be casted to an unsigned 8-bit
  //! integer.
  template<typename T>
  static ASMJIT_INLINE bool isUInt8(T x) {
    if (IntTraits<T>::kIsSigned)
      return x >= T(0) && (sizeof(T) <= sizeof(uint8_t) ? true : x <= T(255));
    else
      return sizeof(T) <= sizeof(uint8_t) ? true : x <= T(255);
  }

  //! @brief Get whether the given integer @a x can be casted to a signed 16-bit
  //! integer.
  template<typename T>
  static ASMJIT_INLINE bool isInt16(T x) {
    if (IntTraits<T>::kIsSigned)
      return sizeof(T) <= sizeof(int16_t) ? true : x >= T(-32768) && x <= T(32767);
    else
      return x >= T(0) && (sizeof(T) <= sizeof(int16_t) ? true : x <= T(32767));
  }

  //! @brief Get whether the given integer @a x can be casted to an unsigned 16-bit
  //! integer.
  template<typename T>
  static ASMJIT_INLINE bool isUInt16(T x) {
    if (IntTraits<T>::kIsSigned)
      return x >= T(0) && (sizeof(T) <= sizeof(uint16_t) ? true : x <= T(65535));
    else
      return sizeof(T) <= sizeof(uint16_t) ? true : x <= T(65535);
  }

  //! @brief Get whether the given integer @a x can be casted to a signed 32-bit
  //! integer.
  template<typename T>
  static ASMJIT_INLINE bool isInt32(T x) {
    if (IntTraits<T>::kIsSigned)
      return sizeof(T) <= sizeof(int32_t) ? true : x >= T(-32768) && x <= T(32767);
    else
      return x >= T(0) && (sizeof(T) <= sizeof(int32_t) ? true : x <= T(2147483647));
  }

  //! @brief Get whether the given integer @a x can be casted to an unsigned 32-bit
  //! integer.
  template<typename T>
  static ASMJIT_INLINE bool isUInt32(T x) {
    if (IntTraits<T>::kIsSigned)
      return x >= T(0) && (sizeof(T) <= sizeof(uint32_t) ? true : x <= T(4294967295));
    else
      return sizeof(T) <= sizeof(uint32_t) ? true : x <= T(4294967295);
  }

  // --------------------------------------------------------------------------
  // [AsmJit - IsPowerOf2]
  // --------------------------------------------------------------------------

  template<typename T>
  static ASMJIT_INLINE bool isPowerOf2(T n) {
    return n != 0 && (n & (n - 1)) == 0;
  }

  // --------------------------------------------------------------------------
  // [AsmJit - Mask / Bits]
  // --------------------------------------------------------------------------

  static ASMJIT_INLINE uint32_t mask(uint32_t x) {
    ASMJIT_ASSERT(x < 32);
    return (1U << x);
  }

  static ASMJIT_INLINE uint32_t bits(uint32_t x) {
    // Shifting more bits that the type has has undefined behavior. Everything
    // we need is that application shouldn't crash because of that, but the
    // content of register after shift is not defined. So in case that the
    // requested shift is too large for the type we correct this undefined
    // behavior by setting all bits to ones (this is why we generate an overflow
    // mask).
    uint32_t overflow = static_cast<uint32_t>(
      -static_cast<int32_t>(x >= sizeof(uint32_t) * 8));

    return ((static_cast<uint32_t>(1) << x) - 1U) | overflow;
  }

  // --------------------------------------------------------------------------
  // [AsmJit - HasBit]
  // --------------------------------------------------------------------------

  static ASMJIT_INLINE bool hasBit(uint32_t x, uint32_t n)
  { return static_cast<bool>((x >> n) & 0x1); }

  // --------------------------------------------------------------------------
  // [AsmJit - BitCount]
  // --------------------------------------------------------------------------

  // From http://graphics.stanford.edu/~seander/bithacks.html .
  static ASMJIT_INLINE uint32_t bitCount(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555U);
    x = (x & 0x33333333U) + ((x >> 2) & 0x33333333U);
    return (((x + (x >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
  }

  // --------------------------------------------------------------------------
  // [AsmJit - FindFirstBit]
  // --------------------------------------------------------------------------

  static ASMJIT_INLINE uint32_t findFirstBitSlow(uint32_t mask) {
    // This is a reference (slow) implementation of findFirstBit(), used when
    // we don't have compiler support for this task. The implementation speed
    // has been improved to check for 2 bits per iteration.
    uint32_t i = 1;

    while (mask != 0) {
      uint32_t two = mask & 0x3;
      if (two != 0x0)
        return i - (two & 0x1);

      i += 2;
      mask >>= 2;
    }

    return 0xFFFFFFFFU;
  }

  static ASMJIT_INLINE uint32_t findFirstBit(uint32_t mask) {
#if defined(_MSC_VER)
    DWORD i;
    if (_BitScanForward(&i, mask))
    {
      ASMJIT_ASSERT(findFirstBitSlow(mask) == i);
      return static_cast<uint32_t>(i);
    }
    return 0xFFFFFFFFU;
#else
    return findFirstBitSlow(mask);
#endif
  }

  // --------------------------------------------------------------------------
  // [AsmJit - Misc]
  // --------------------------------------------------------------------------

  static ASMJIT_INLINE uint32_t keepNOnesFromRight(uint32_t mask, uint32_t nBits) {
    uint32_t m = 0x1;

    do {
      nBits -= (mask & m) == 0;
      m <<= 1;
      if (nBits == 0) {
        m -= 1;
        mask &= m;
        break;
      }
    } while (m);

    return mask;
  }

  static ASMJIT_INLINE uint32_t indexNOnesFromRight(uint8_t* dst, uint32_t mask, uint32_t nBits) {
    uint32_t totalBits = nBits;
    uint8_t i = 0;
    uint32_t m = 0x1;

    do {
      if (mask & m) {
        *dst++ = i;
        if (--nBits == 0)
          break;
      }

      m <<= 1;
      i++;
    } while (m);

    return totalBits - nBits;
  }

  // --------------------------------------------------------------------------
  // [AsmJit - Alignment]
  // --------------------------------------------------------------------------

  template<typename T>
  static ASMJIT_INLINE bool isAligned(T base, T alignment)
  { return (base % alignment) == 0; }

  //! @brief Align @a base to @a alignment.
  template<typename T>
  static ASMJIT_INLINE T alignTo(T base, T alignment)
  { return (base + (alignment - 1)) & ~(alignment - 1); }

  //! @brief Get delta required to align @a base to @a alignment.
  template<typename T>
  static ASMJIT_INLINE T deltaTo(T base, T alignment)
  { return alignTo(base, alignment) - base; }

  // --------------------------------------------------------------------------
  // [AsmJit - Round]
  // --------------------------------------------------------------------------

  template<typename T>
  static ASMJIT_INLINE T roundUp(T base, T alignment) {
    T over = base % alignment;
    return base + (over > 0 ? alignment - over : 0);
  }

  template<typename T>
  static ASMJIT_INLINE T roundUpToPowerOf2(T base) {
    // Implementation is from "Hacker's Delight" by Henry S. Warren, Jr.,
    // figure 3-3, page 48, where the function is called clp2.
    base -= 1;

    // I'm trying to make this portable and MSVC strikes me the warning C4293:
    //   "Shift count negative or too big, undefined behavior"
    // Fixing...
#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable: 4293)
#endif // _MSC_VER

    base = base | (base >> 1);
    base = base | (base >> 2);
    base = base | (base >> 4);

    if (sizeof(T) >= 2) base = base | (base >>  8);
    if (sizeof(T) >= 4) base = base | (base >> 16);
    if (sizeof(T) >= 8) base = base | (base >> 32);

#if defined(_MSC_VER)
# pragma warning(pop)
#endif // _MSC_VER

    return base + 1;
  }
};

// ============================================================================
// [asmjit::UInt64]
// ============================================================================

union UInt64 {
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64 fromUInt64(uint64_t val) {
    UInt64 data;
    data.setUInt64(val);
    return data;
  }

  ASMJIT_INLINE UInt64 fromUInt64(const UInt64& val) {
    UInt64 data;
    data.setUInt64(val);
    return data;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void reset() {
    if (kArchHost64Bit) {
      u64 = 0;
    }
    else {
      u32[0] = 0;
      u32[1] = 0;
    }
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE uint64_t getUInt64() const {
    return u64;
  }

  ASMJIT_INLINE UInt64& setUInt64(uint64_t val) {
    u64 = val;
    return *this;
  }

  ASMJIT_INLINE UInt64& setUInt64(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 = val.u64;
    }
    else {
      u32[0] = val.u32[0];
      u32[1] = val.u32[1];
    }
    return *this;
  }

  ASMJIT_INLINE UInt64& setPacked_2x32(uint32_t u0, uint32_t u1) {
    if (kArchHost64Bit) {
      u64 = IntUtil::pack64_2x32(u0, u1);
    }
    else {
      u32[0] = u0;
      u32[1] = u1;
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& add(uint64_t val) {
    u64 += val;
    return *this;
  }

  ASMJIT_INLINE UInt64& add(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 += val.u64;
    }
    else {
      u32[0] += val.u32[0];
      u32[1] += val.u32[1];
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Sub]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& sub(uint64_t val) {
    u64 -= val;
    return *this;
  }

  ASMJIT_INLINE UInt64& sub(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 -= val.u64;
    }
    else {
      u32[0] -= val.u32[0];
      u32[1] -= val.u32[1];
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [And]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& and_(uint64_t val) {
    u64 &= val;
    return *this;
  }

  ASMJIT_INLINE UInt64& and_(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 &= val.u64;
    }
    else {
      u32[0] &= val.u32[0];
      u32[1] &= val.u32[1];
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Or]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& or_(uint64_t val) {
    u64 |= val;
    return *this;
  }

  ASMJIT_INLINE UInt64& or_(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 |= val.u64;
    }
    else {
      u32[0] |= val.u32[0];
      u32[1] |= val.u32[1];
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Xor]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& xor_(uint64_t val) {
    u64 ^= val;
    return *this;
  }

  ASMJIT_INLINE UInt64& xor_(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 ^= val.u64;
    }
    else {
      u32[0] ^= val.u32[0];
      u32[1] ^= val.u32[1];
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Del]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& del(uint64_t val) {
    u64 &= ~val;
    return *this;
  }

  ASMJIT_INLINE UInt64& del(const UInt64& val) {
    if (kArchHost64Bit) {
      u64 &= ~val.u64;
    }
    else {
      u32[0] &= ~val.u32[0];
      u32[1] &= ~val.u32[1];
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE bool isZero() const
  { return kArchHost64Bit ? u64 == 0 : (u32[0] | u32[1]) == 0; }

  ASMJIT_INLINE bool isNonZero() const
  { return kArchHost64Bit ? u64 != 0 : (u32[0] | u32[1]) != 0; }

  ASMJIT_INLINE bool eq(uint64_t val) const
  { return u64 == val; }

  ASMJIT_INLINE bool eq(const UInt64& val) const
  { return kArchHost64Bit ? u64 == val.u64 : (u32[0] == val.u32[0]) & (u32[1] == val.u32[1]); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE UInt64& operator+=(uint64_t val) { return add(val); }
  ASMJIT_INLINE UInt64& operator+=(const UInt64& val) { return add(val); }

  ASMJIT_INLINE UInt64& operator-=(uint64_t val) { return sub(val); }
  ASMJIT_INLINE UInt64& operator-=(const UInt64& val) { return sub(val); }

  ASMJIT_INLINE UInt64& operator&=(uint64_t val) { return and_(val); }
  ASMJIT_INLINE UInt64& operator&=(const UInt64& val) { return and_(val); }

  ASMJIT_INLINE UInt64& operator|=(uint64_t val) { return or_(val); }
  ASMJIT_INLINE UInt64& operator|=(const UInt64& val) { return or_(val); }

  ASMJIT_INLINE UInt64& operator^=(uint64_t val) { return xor_(val); }
  ASMJIT_INLINE UInt64& operator^=(const UInt64& val) { return xor_(val); }

  ASMJIT_INLINE bool operator==(uint64_t val) const { return eq(val); }
  ASMJIT_INLINE bool operator==(const UInt64& val) const { return eq(val); }

  ASMJIT_INLINE bool operator!=(uint64_t val) const { return !eq(val); }
  ASMJIT_INLINE bool operator!=(const UInt64& val) const { return !eq(val); }

  ASMJIT_INLINE bool operator<(uint64_t val) const { return u64 < val; }
  ASMJIT_INLINE bool operator<(const UInt64& val) const { return u64 < val.u64; }

  ASMJIT_INLINE bool operator<=(uint64_t val) const { return u64 <= val; }
  ASMJIT_INLINE bool operator<=(const UInt64& val) const { return u64 <= val.u64; }

  ASMJIT_INLINE bool operator>(uint64_t val) const { return u64 > val; }
  ASMJIT_INLINE bool operator>(const UInt64& val) const { return u64 > val.u64; }

  ASMJIT_INLINE bool operator>=(uint64_t val) const { return u64 >= val; }
  ASMJIT_INLINE bool operator>=(const UInt64& val) const { return u64 >= val.u64; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint64_t u64;

  uint32_t u32[2];
  uint16_t u16[4];
  uint8_t u8[8];

  struct {
#if defined(ASMJIT_HOST_LE)
    uint32_t lo, hi;
#else
    uint32_t hi, lo;
#endif // ASMJIT_HOST_LE
  };
};

//! @}

} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"

// [Guard]
#endif // _ASMJIT_BASE_INTUTIL_H
