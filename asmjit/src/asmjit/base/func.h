// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_BASE_FUNC_H
#define _ASMJIT_BASE_FUNC_H

// [Dependencies - AsmJit]
#include "../base/assert.h"
#include "../base/defs.h"

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {

// ============================================================================
// [Forward Declarations]
// ============================================================================

template<typename T>
struct TypeId;

// ============================================================================
// [asmjit::kFuncConv]
// ============================================================================

ASMJIT_ENUM(kFuncConv) {
  //! @brief Calling convention is invalid (can't be used).
  kFuncConvNone = 0
};

// ============================================================================
// [asmjit::kFuncHint]
// ============================================================================

//! @brief Function hints.
ASMJIT_ENUM(kFuncHint) {
  //! @brief Make a naked function (default true).
  //!
  //! Naked function is function without using standard prolog/epilog sequence).
  //!
  //! @section X86/X64
  //!
  //! Standard prolog sequence is:
  //!
  //!   "push zbp"
  //!   "mov zsp, zbp"
  //!   "sub zsp, StackAdjustment"
  //!
  //! which is equal to:
  //!
  //!   "enter StackAdjustment, 0"
  //!
  //! Standard epilog sequence is:
  //!
  //!   "mov zsp, zbp"
  //!   "pop zbp"
  //!   "ret"
  //!
  //! which is equal to:
  //!
  //!   "leave"
  //!   "ret"
  //!
  //! Naked functions can omit the prolog/epilog sequence. The advantage of
  //! doing such modification is that EBP/RBP register can be used by the
  //! register allocator which can result in less spills/allocs.
  kFuncHintNaked = 0,

  //! @brief Generate compact function prolog/epilog if possible.
  //!
  //! @section X86/X64
  //!
  //! Use shorter, but possible slower prolog/epilog sequence to save/restore
  //! registers.
  kFuncHintCompact = 1
};

// ============================================================================
// [asmjit::kFuncFlags]
// ============================================================================

//! @brief Function flags.
ASMJIT_ENUM(kFuncFlags) {
  //! @brief Whether the function is using naked (minimal) prolog / epilog.
  kFuncFlagIsNaked = 0x00000001,

  //! @brief Whether an another function is called from this function.
  kFuncFlagIsCaller = 0x00000002,

  //! @brief Whether the stack is not aligned to the required stack alignment,
  //! thus it has to be aligned manually.
  kFuncFlagIsStackMisaligned = 0x00000004,

  //! @brief Whether the stack pointer is adjusted by the stack size needed
  //! to save registers and function variables.
  //!
  //! @section X86/X64
  //!
  //! Stack pointer (ESP/RSP) is adjusted by 'sub' instruction in prolog and by
  //! 'add' instruction in epilog (only if function is not naked). If function
  //! needs to perform manual stack alignment more instructions are used to
  //! adjust the stack (like "and zsp, -Alignment").
  kFuncFlagIsStackAdjusted = 0x00000008,

  //! @brief Whether the function is finished using @c Compiler::endFunc().
  kFuncFlagIsFinished = 0x80000000
};

// ============================================================================
// [asmjit::kFuncDir]
// ============================================================================

//! @brief Function arguments direction.
ASMJIT_ENUM(kFuncDir) {
  //! @brief Arguments are passed left to right.
  //!
  //! This arguments direction is unusual to C programming, it's used by pascal
  //! compilers and in some calling conventions by Borland compiler).
  kFuncDirLtr = 0,
  //! @brief Arguments are passed right ro left
  //!
  //! This is default argument direction in C programming.
  kFuncDirRtl = 1
};

// ============================================================================
// [asmjit::kFuncStackInvalid]
// ============================================================================

enum {
  //! @brief Invalid stack offset in function or function parameter.
  kFuncStackInvalid = -1
};

// ============================================================================
// [asmjit::kFuncArg]
// ============================================================================

//! @brief Function argument (lo/hi) specification.
ASMJIT_ENUM(kFuncArg) {
  //! @brief Maxumum number of function arguments supported by AsmJit.
  kFuncArgCount = 16,
  //! @brief Extended maximum number of arguments (used internally).
  kFuncArgCountLoHi = kFuncArgCount * 2,

  //! @brief Index to the LO part of function argument (default).
  //!
  //! This value is typically omitted and added only if there is HI argument
  //! accessed.
  kFuncArgLo = 0,
  //! @brief Index to the HI part of function argument.
  //!
  //! HI part of function argument depends on target architecture. On x86 it's
  //! typically used to transfer 64-bit integers (they form a pair of 32-bit
  //! integers).
  kFuncArgHi = kFuncArgCount
};

// ============================================================================
// [asmjit::kFuncRet]
// ============================================================================

//! @brief Function return value (lo/hi) specification.
ASMJIT_ENUM(kFuncRet) {
  //! @brief Index to the LO part of function return value.
  kFuncRetLo = 0,
  //! @brief Index to the HI part of function return value.
  kFuncRetHi = 1
};

// ============================================================================
// [asmjit::TypeId]
// ============================================================================

#if defined(ASMJIT_HAS_PARTIAL_TEMPLATE_SPECIALIZATION)

#define ASMJIT_DECLARE_TYPE_CORE(_PtrId_) \
  template<typename T> \
  struct TypeId { \
    enum { \
      kId = static_cast<int>(::asmjit::kVarTypeInvalid) \
    }; \
  }; \
  \
  template<typename T> \
  struct TypeId<T*> { enum { kId = _PtrId_ }; }

#else

// Code without partial template specialization is a bit complex. We need to
// determine whether the size of the return value of this function is equal
// to sizeof(char) or sizeof(void*). Any sizeof() can be used to distinguish
// between these two, but these are commonly used in other libraries as well.
template<typename T>
char TypeId_NoPtiHelper(T*(*)());
// And specialization.
void* TypeId_NoPtiHelper(...);

#define ASMJIT_DECLARE_TYPE_CORE(_PtrId_) \
  template<typename T> \
  struct TypeId { \
    enum { \
      kId = (sizeof( ::asmjit::TypeId_NoPtiHelper((T(*)())0) ) == sizeof(char) \
        ? static_cast<int>(_PtrId_) \
        : static_cast<int>(::asmjit::kVarTypeInvalid)) \
    }; \
  }

#endif // ASMJIT_HAS_PARTIAL_TEMPLATE_SPECIALIZATION

//! @brief Declare C/C++ type-id mapped to @c asmjit::kVarType.
#define ASMJIT_DECLARE_TYPE_ID(_T_, _Id_) \
  template<> \
  struct TypeId<_T_> { enum { kId = _Id_ }; }

// ============================================================================
// [asmjit::FuncInOut]
// ============================================================================

//! @brief Function in/out (argument or a return value).
//!
//! This class contains function argument or return value translated from the
//! @ref FuncPrototype.
struct FuncInOut {
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE uint32_t getVarType() const { return _varType; }

  ASMJIT_INLINE bool hasRegIndex() const { return _regIndex != kInvalidReg; }
  ASMJIT_INLINE uint32_t getRegIndex() const { return _regIndex; }

  ASMJIT_INLINE bool hasStackOffset() const { return _stackOffset != kFuncStackInvalid; }
  ASMJIT_INLINE int32_t getStackOffset() const { return static_cast<int32_t>(_stackOffset); }

  //! @brief Get whether the argument / return value is assigned.
  ASMJIT_INLINE bool isSet() const {
    return (_regIndex != kInvalidReg) | (_stackOffset != kFuncStackInvalid);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset the function argument to "unassigned state".
  ASMJIT_INLINE void reset() { _packed = 0xFFFFFFFF; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union {
    struct {
      //! @brief Variable type, see @c kVarType.
      uint8_t _varType;
      //! @brief Register index if argument / return value is a register.
      uint8_t _regIndex;
      //! @brief Stack offset if argument / return value is on the stack.
      int16_t _stackOffset;
    };

    //! @brief All members packed into single 32-bit integer.
    uint32_t _packed;
  };
};

// ============================================================================
// [asmjit::FuncPrototype]
// ============================================================================

//! @brief Function prototype.
//!
//! Function prototype contains information about function return type, count
//! of arguments and their types. Function prototype is a low level structure
//! which doesn't contain platform specific or calling convention specific
//! information. Function prototype is used to create a @ref FuncDecl.
struct FuncPrototype {
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get function return value.
  ASMJIT_INLINE uint32_t getRet() const { return _ret; }

  //! @brief Get function arguments' IDs.
  ASMJIT_INLINE const uint32_t* getArgList() const { return _argList; }
  //! @brief Get count of function arguments.
  ASMJIT_INLINE uint32_t getArgCount() const { return _argCount; }

  //! @brief Get argument at index @a id.
  ASMJIT_INLINE uint32_t getArg(uint32_t id) const {
    ASMJIT_ASSERT(id < _argCount);
    return _argList[id];
  }

  //! @brief Set function definition - return type and arguments.
  ASMJIT_INLINE void _setPrototype(uint32_t ret, const uint32_t* argList, uint32_t argCount) {
    _ret = ret;
    _argList = argList;
    _argCount = argCount;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _ret;
  uint32_t _argCount;
  const uint32_t* _argList;
};

// ============================================================================
// [asmjit::FuncDecl]
// ============================================================================

//! @brief Function declaration.
struct FuncDecl {
  // --------------------------------------------------------------------------
  // [Accessors - Calling Convention]
  // --------------------------------------------------------------------------

  //! @brief Get function calling convention, see @c kFuncConv.
  ASMJIT_INLINE uint32_t getConvention() const { return _convention; }

  //! @brief Get whether the callee pops the stack.
  ASMJIT_INLINE uint32_t getCalleePopsStack() const { return _calleePopsStack; }

  //! @brief Get direction of arguments passed on the stack.
  //!
  //! Direction should be always @c kFuncDirRtl.
  //!
  //! @note This is related to used calling convention, it's not affected by
  //! number of function arguments or their types.
  ASMJIT_INLINE uint32_t getDirection() const { return _direction; }

  //! @brief Get stack size needed for function arguments passed on the stack.
  ASMJIT_INLINE uint32_t getArgStackSize() const { return _argStackSize; }
  //! @brief Get size of "Red Zone".
  ASMJIT_INLINE uint32_t getRedZoneSize() const { return _redZoneSize; }
  //! @brief Get size of "Spill Zone".
  ASMJIT_INLINE uint32_t getSpillZoneSize() const { return _spillZoneSize; }

  // --------------------------------------------------------------------------
  // [Accessors - Arguments and Return]
  // --------------------------------------------------------------------------

  //! @brief Get whether the function has a return value.
  ASMJIT_INLINE bool hasRet() const { return _retCount != 0; }
  //! @brief Get count of function return values.
  ASMJIT_INLINE uint32_t getRetCount() const { return _retCount; }

  //! @brief Get function return value.
  ASMJIT_INLINE FuncInOut& getRet(uint32_t index = kFuncRetLo) { return _retList[index]; }
  //! @brief Get function return value.
  ASMJIT_INLINE const FuncInOut& getRet(uint32_t index = kFuncRetLo) const { return _retList[index]; }

  //! @brief Get count of function arguments.
  ASMJIT_INLINE uint32_t getArgCount() const { return _argCount; }

  //! @brief Get function arguments array.
  ASMJIT_INLINE FuncInOut* getArgList() { return _argList; }
  //! @brief Get function arguments array (const).
  ASMJIT_INLINE const FuncInOut* getArgList() const { return _argList; }

  //! @brief Get function argument at index @a index.
  ASMJIT_INLINE FuncInOut& getArg(size_t index) {
    ASMJIT_ASSERT(index < kFuncArgCountLoHi);
    return _argList[index];
  }

  //! @brief Get function argument at index @a index.
  ASMJIT_INLINE const FuncInOut& getArg(size_t index) const {
    ASMJIT_ASSERT(index < kFuncArgCountLoHi);
    return _argList[index];
  }

  ASMJIT_INLINE void resetArg(size_t index) {
    ASMJIT_ASSERT(index < kFuncArgCountLoHi);
    _argList[index].reset();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Calling convention.
  uint8_t _convention;
  //! @brief Whether a callee pops stack.
  uint8_t _calleePopsStack : 1;
  //! @brief Direction for arguments passed on the stack, see @c kFuncDir.
  uint8_t _direction : 1;
  //! @brief Reserved #0 (alignment).
  uint8_t _reserved0 : 6;

  //! @brief Count of arguments (in @c _argList).
  uint8_t _argCount;
  //! @brief Count of return value(s).
  uint8_t _retCount;

  //! @brief Count of bytes consumed by arguments on the stack (aligned).
  uint32_t _argStackSize;

  //! @brief Size of "Red Zone".
  //!
  //! @note Used by AMD64-ABI (128 bytes).
  uint16_t _redZoneSize;

  //! @brief Size of "Spill Zone".
  //!
  //! @note Used by WIN64-ABI (32 bytes).
  uint16_t _spillZoneSize;

  //! @brief Function arguments (including HI arguments) mapped to physical
  //! registers and stack offset.
  FuncInOut _argList[kFuncArgCountLoHi];

  //! @brief Function return value(s).
  FuncInOut _retList[2];
};

// ============================================================================
// [asmjit::FuncBuilderX]
// ============================================================================

//! @brief Custom function builder for up to 32 function arguments.
struct FuncBuilderX : public FuncPrototype {
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE FuncBuilderX() {
    _setPrototype(kVarTypeInvalid, _builderArgList, 0);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Set return type to @a retType.
  ASMJIT_INLINE void setRet(uint32_t retType)
  { _ret = retType; }

  ASMJIT_INLINE void setArg(uint32_t id, uint32_t type) {
    ASMJIT_ASSERT(id < _argCount);
    _builderArgList[id] = type;
  }

  ASMJIT_INLINE void addArg(uint32_t type) {
    ASMJIT_ASSERT(_argCount < kFuncArgCount);
    _builderArgList[_argCount++] = type;
  }

  template<typename T>
  ASMJIT_INLINE void setRetT()
  { setRet(TypeId<ASMJIT_TYPE_TO_TYPE(T)>::kId); }

  template<typename T>
  ASMJIT_INLINE void setArgT(uint32_t id)
  { setArg(id, TypeId<ASMJIT_TYPE_TO_TYPE(T)>::kId); }

  template<typename T>
  ASMJIT_INLINE void addArgT()
  { addArg(TypeId<ASMJIT_TYPE_TO_TYPE(T)>::kId); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _builderArgList[kFuncArgCount];
};

//! @brief Class used to build function without arguments.
template<typename RET>
struct FuncBuilder0 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder0() {
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, NULL, 0);
  }
};

//! @brief Class used to build function with 1 argument.
template<typename RET, typename P0>
struct FuncBuilder1 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder1() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 2 args.
template<typename RET, typename P0, typename P1>
struct FuncBuilder2 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder2() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 3 args.
template<typename RET, typename P0, typename P1, typename P2>
struct FuncBuilder3 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder3() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 4 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3>
struct FuncBuilder4 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder4() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 5 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3, typename P4>
struct FuncBuilder5 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder5() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P4)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 6 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FuncBuilder6 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder6() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P4)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P5)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 7 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FuncBuilder7 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder7() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P4)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P5)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P6)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 8 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FuncBuilder8 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder8() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P4)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P5)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P6)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P7)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 9 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
struct FuncBuilder9 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder9() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P4)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P5)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P6)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P7)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P8)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

//! @brief Class used to build function with 10 args.
template<typename RET, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
struct FuncBuilder10 : public FuncPrototype {
  ASMJIT_INLINE FuncBuilder10() {
    static const uint32_t args[] = {
      TypeId<ASMJIT_TYPE_TO_TYPE(P0)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P1)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P2)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P3)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P4)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P5)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P6)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P7)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P8)>::kId,
      TypeId<ASMJIT_TYPE_TO_TYPE(P9)>::kId
    };
    _setPrototype(TypeId<ASMJIT_TYPE_TO_TYPE(RET)>::kId, args, ASMJIT_ARRAY_SIZE(args));
  }
};

} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"

// [Guard]
#endif // _ASMJIT_BASE_FUNC_H
