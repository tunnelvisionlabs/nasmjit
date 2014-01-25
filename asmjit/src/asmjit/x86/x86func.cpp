// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Export]
#define ASMJIT_EXPORTS

// [Guard]
#include "../build.h"
#if defined(ASMJIT_BUILD_X86) || defined(ASMJIT_BUILD_X64)

// [Dependencies - AsmJit]
#include "../base/assert.h"
#include "../base/intutil.h"
#include "../base/string.h"
#include "../x86/x86defs.h"
#include "../x86/x86func.h"

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {
namespace x86x64 {

// ============================================================================
// [asmjit::X86X64FuncDecl - FindArgByReg]
// ============================================================================

uint32_t X86X64FuncDecl::findArgByReg(uint32_t rClass, uint32_t rIndex) const {
  for (uint32_t i = 0; i < _argCount; i++) {
    const FuncInOut& arg = getArg(i);

    if (arg.getRegIndex() == rIndex && x86VarTypeToRegClass(arg.getVarType()) == rClass)
      return i;
  }

  return kInvalidValue;
}

// ============================================================================
// [asmjit::X86X64FuncDecl - SetPrototype - InitCallingConvention]
// ============================================================================

static void X86X64FuncDecl_initConvention(X86X64FuncDecl* self, uint32_t convention) {
  uint32_t i;

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  self->_argStackSize = 0;
  self->_redZoneSize = 0;
  self->_spillZoneSize = 0;

  self->_convention = static_cast<uint8_t>(convention);
  self->_calleePopsStack = false;
  self->_direction = kFuncDirRtl;

  self->_passed.reset();
  self->_preserved.reset();

  for (i = 0; i < ASMJIT_ARRAY_SIZE(self->_passedOrderGp); i++)
    self->_passedOrderGp[i] = kInvalidReg;

  for (i = 0; i < ASMJIT_ARRAY_SIZE(self->_passedOrderXmm); i++)
    self->_passedOrderXmm[i] = kInvalidReg;

  // --------------------------------------------------------------------------
  // [X86 Calling Conventions]
  // --------------------------------------------------------------------------

  // TODO: [Func] Port.

#if defined(ASMJIT_HOST_X86)
  self->_preserved.set(kRegClassGp,
    IntUtil::mask(kRegIndexBx) |
    IntUtil::mask(kRegIndexSp) |
    IntUtil::mask(kRegIndexBp) |
    IntUtil::mask(kRegIndexSi) |
    IntUtil::mask(kRegIndexDi));

  switch (convention) {
    // ------------------------------------------------------------------------
    // [CDecl]
    // ------------------------------------------------------------------------

    case kFuncConvCDecl:
      break;

    // ------------------------------------------------------------------------
    // [StdCall]
    // ------------------------------------------------------------------------

    case kFuncConvStdCall:
      self->_calleePopsStack = true;
      break;

    // ------------------------------------------------------------------------
    // [MS-ThisCall]
    // ------------------------------------------------------------------------

    case kFuncConvMsThisCall:
      self->_calleePopsStack = true;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexCx));
      self->_passedOrderGp[0] = kRegIndexCx;
      break;

    // ------------------------------------------------------------------------
    // [MS-FastCall]
    // ------------------------------------------------------------------------

    case kFuncConvMsFastCall:
      self->_calleePopsStack = true;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexCx) |
        IntUtil::mask(kRegIndexDx));
      self->_passedOrderGp[0] = kRegIndexCx;
      self->_passedOrderGp[1] = kRegIndexDx;
      break;

    // ------------------------------------------------------------------------
    // [Borland-FastCall]
    // ------------------------------------------------------------------------

    case kFuncConvBorlandFastCall:
      self->_calleePopsStack = true;
      self->_direction = kFuncDirLtr;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexAx) |
        IntUtil::mask(kRegIndexDx) |
        IntUtil::mask(kRegIndexCx));
      self->_passedOrderGp[0] = kRegIndexAx;
      self->_passedOrderGp[1] = kRegIndexDx;
      self->_passedOrderGp[2] = kRegIndexCx;
      break;

    // ------------------------------------------------------------------------
    // [Gcc-FastCall]
    // ------------------------------------------------------------------------

    case kFuncConvGccFastCall:
      self->_calleePopsStack = true;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexCx) |
        IntUtil::mask(kRegIndexDx));
      self->_passedOrderGp[0] = kRegIndexCx;
      self->_passedOrderGp[1] = kRegIndexDx;
      break;

    // ------------------------------------------------------------------------
    // [Gcc-Regparm(1)]
    // ------------------------------------------------------------------------

    case kFuncConvGccRegParm1:
      self->_calleePopsStack = false;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexAx));
      self->_passedOrderGp[0] = kRegIndexAx;
      break;

    // ------------------------------------------------------------------------
    // [Gcc-Regparm(2)]
    // ------------------------------------------------------------------------

    case kFuncConvGccRegParm2:
      self->_calleePopsStack = false;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexAx) |
        IntUtil::mask(kRegIndexDx));
      self->_passedOrderGp[0] = kRegIndexAx;
      self->_passedOrderGp[1] = kRegIndexDx;
      break;

    // ------------------------------------------------------------------------
    // [Gcc-Regparm(3)]
    // ------------------------------------------------------------------------

    case kFuncConvGccRegParm3:
      self->_calleePopsStack = false;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexAx) |
        IntUtil::mask(kRegIndexDx) |
        IntUtil::mask(kRegIndexCx));
      self->_passedOrderGp[0] = kRegIndexAx;
      self->_passedOrderGp[1] = kRegIndexDx;
      self->_passedOrderGp[2] = kRegIndexCx;
      break;

    // ------------------------------------------------------------------------
    // [Illegal]
    // ------------------------------------------------------------------------

    default:
      // Illegal calling convention.
      ASMJIT_ASSERT(!"Reached");
  }
#endif // ASMJIT_HOST

  // --------------------------------------------------------------------------
  // [X64 Calling Conventions]
  // --------------------------------------------------------------------------

#if defined(ASMJIT_HOST_X64)
  switch (convention) {
    // ------------------------------------------------------------------------
    // [X64-Windows]
    // ------------------------------------------------------------------------

    case kFuncConvX64W:
      self->_spillZoneSize = 32;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexCx   ) |
        IntUtil::mask(kRegIndexDx   ) |
        IntUtil::mask(kRegIndexR8   ) |
        IntUtil::mask(kRegIndexR9   ));
      self->_passedOrderGp[0] = kRegIndexCx;
      self->_passedOrderGp[1] = kRegIndexDx;
      self->_passedOrderGp[2] = kRegIndexR8;
      self->_passedOrderGp[3] = kRegIndexR9;

      self->_passed.set(kRegClassXy,
        IntUtil::mask(kRegIndexXmm0 ) |
        IntUtil::mask(kRegIndexXmm1 ) |
        IntUtil::mask(kRegIndexXmm2 ) |
        IntUtil::mask(kRegIndexXmm3 ));
      self->_passedOrderXmm[0] = kRegIndexXmm0;
      self->_passedOrderXmm[1] = kRegIndexXmm1;
      self->_passedOrderXmm[2] = kRegIndexXmm2;
      self->_passedOrderXmm[3] = kRegIndexXmm3;

      self->_preserved.set(kRegClassGp,
        IntUtil::mask(kRegIndexBx   ) |
        IntUtil::mask(kRegIndexSp   ) |
        IntUtil::mask(kRegIndexBp   ) |
        IntUtil::mask(kRegIndexSi   ) |
        IntUtil::mask(kRegIndexDi   ) |
        IntUtil::mask(kRegIndexR12  ) |
        IntUtil::mask(kRegIndexR13  ) |
        IntUtil::mask(kRegIndexR14  ) |
        IntUtil::mask(kRegIndexR15  ));

      self->_preserved.set(kRegClassXy,
        IntUtil::mask(kRegIndexXmm6 ) |
        IntUtil::mask(kRegIndexXmm7 ) |
        IntUtil::mask(kRegIndexXmm8 ) |
        IntUtil::mask(kRegIndexXmm9 ) |
        IntUtil::mask(kRegIndexXmm10) |
        IntUtil::mask(kRegIndexXmm11) |
        IntUtil::mask(kRegIndexXmm12) |
        IntUtil::mask(kRegIndexXmm13) |
        IntUtil::mask(kRegIndexXmm14) |
        IntUtil::mask(kRegIndexXmm15));
      break;

    // ------------------------------------------------------------------------
    // [X64-Unix]
    // ------------------------------------------------------------------------

    case kFuncConvX64U:
      self->_redZoneSize = 128;

      self->_passed.set(kRegClassGp,
        IntUtil::mask(kRegIndexDi  ) |
        IntUtil::mask(kRegIndexSi  ) |
        IntUtil::mask(kRegIndexDx  ) |
        IntUtil::mask(kRegIndexCx  ) |
        IntUtil::mask(kRegIndexR8  ) |
        IntUtil::mask(kRegIndexR9  ));
      self->_passedOrderGp[0] = kRegIndexDi;
      self->_passedOrderGp[1] = kRegIndexSi;
      self->_passedOrderGp[2] = kRegIndexDx;
      self->_passedOrderGp[3] = kRegIndexCx;
      self->_passedOrderGp[4] = kRegIndexR8;
      self->_passedOrderGp[5] = kRegIndexR9;

      self->_passed.set(kRegClassXy,
        IntUtil::mask(kRegIndexXmm0) |
        IntUtil::mask(kRegIndexXmm1) |
        IntUtil::mask(kRegIndexXmm2) |
        IntUtil::mask(kRegIndexXmm3) |
        IntUtil::mask(kRegIndexXmm4) |
        IntUtil::mask(kRegIndexXmm5) |
        IntUtil::mask(kRegIndexXmm6) |
        IntUtil::mask(kRegIndexXmm7));
      self->_passedOrderXmm[0] = kRegIndexXmm0;
      self->_passedOrderXmm[1] = kRegIndexXmm1;
      self->_passedOrderXmm[2] = kRegIndexXmm2;
      self->_passedOrderXmm[3] = kRegIndexXmm3;
      self->_passedOrderXmm[4] = kRegIndexXmm4;
      self->_passedOrderXmm[5] = kRegIndexXmm5;
      self->_passedOrderXmm[6] = kRegIndexXmm6;
      self->_passedOrderXmm[7] = kRegIndexXmm7;

      self->_preserved.set(kRegClassGp,
        IntUtil::mask(kRegIndexBx  ) |
        IntUtil::mask(kRegIndexSp  ) |
        IntUtil::mask(kRegIndexBp  ) |
        IntUtil::mask(kRegIndexR12 ) |
        IntUtil::mask(kRegIndexR13 ) |
        IntUtil::mask(kRegIndexR14 ) |
        IntUtil::mask(kRegIndexR15 ));
      break;

    // ------------------------------------------------------------------------
    // [Illegal]
    // ------------------------------------------------------------------------

    default:
      // Illegal calling convention.
      ASMJIT_ASSERT(!"Reached");
  }
#endif // ASMJIT_HOST
}

// ============================================================================
// [asmjit::X86X64FuncDecl - SetPrototype - InitDefinition]
// ============================================================================

static void X86X64FuncDecl_initPrototype(X86X64FuncDecl* self, uint32_t ret, const uint32_t* argList, uint32_t argCount) {
  ASMJIT_ASSERT(argCount <= kFuncArgCount);

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  int32_t i = 0;
  int32_t stackOffset = 0;

  int32_t gpPos = 0;
  int32_t xmmPos = 0;

  uint32_t convention = self->_convention;
  uint32_t regSize;

  if (convention == kFuncConvX64W || convention == kFuncConvX64U)
    regSize = 8;
  else
    regSize = 4;

  self->_argCount = static_cast<uint8_t>(argCount);
  self->_retCount = 0;

  while (i < static_cast<int32_t>(argCount)) {
    FuncInOut& arg = self->getArg(i);

    arg._varType = static_cast<uint8_t>(argList[i]);
    arg._regIndex = kInvalidReg;
    arg._stackOffset = kFuncStackInvalid;

    i++;
  }

  while (i < kFuncArgCount) {
    self->_argList[i].reset();
    i++;
  }

  self->_retList[0].reset();
  self->_retList[1].reset();

  self->_argStackSize = 0;
  self->_used.reset();

  // --------------------------------------------------------------------------
  // [Return]
  // --------------------------------------------------------------------------

  // TODO: [Func] Port.

  // Handle return value.
  switch (ret) {
    case kVarTypeInt8:
    case kVarTypeUInt8:
    case kVarTypeInt16:
    case kVarTypeUInt16:
    case kVarTypeInt32:
    case kVarTypeUInt32:
#if defined(ASMJIT_HOST_X64)
    case kVarTypeInt64:
    case kVarTypeUInt64:
#endif // ASMJIT_HOST
      self->_retCount = 1;
      self->_retList[0]._varType = static_cast<uint8_t>(ret);
      self->_retList[0]._regIndex = kRegIndexAx;
      break;

#if defined(ASMJIT_HOST_X86)
    case kVarTypeInt64:
      self->_retCount = 2;
      self->_retList[0]._varType = kVarTypeUInt32;
      self->_retList[0]._regIndex = kRegIndexAx;
      self->_retList[1]._varType = kVarTypeInt32;
      self->_retList[1]._regIndex = kRegIndexDx;
      break;

    case kVarTypeUInt64:
      self->_retCount = 2;
      self->_retList[0]._varType = kVarTypeUInt32;
      self->_retList[0]._regIndex = kRegIndexAx;
      self->_retList[1]._varType = kVarTypeUInt32;
      self->_retList[1]._regIndex = kRegIndexDx;
      break;
#endif // ASMJIT_HOST

    case kVarTypeMm:
      self->_retCount = 1;
      self->_retList[0]._varType = static_cast<uint8_t>(ret);
      self->_retList[0]._regIndex = kRegIndexMm0;
      break;

#if defined(ASMJIT_HOST_X86)
    case kVarTypeFp32:
    case kVarTypeXmmSs:
      self->_retCount = 1;
      self->_retList[0]._varType = kVarTypeFp32;
      self->_retList[0]._regIndex = 0;
      break;

    case kVarTypeFp64:
    case kVarTypeXmmSd:
      self->_retCount = 1;
      self->_retList[0]._varType = kVarTypeFp64;
      self->_retList[0]._regIndex = 0;
      break;
#endif // ASMJIT_HOST

#if defined(ASMJIT_HOST_X64)
    case kVarTypeFp32:
    case kVarTypeXmmSs:
      self->_retCount = 1;
      self->_retList[0]._varType = kVarTypeXmmSs;
      self->_retList[0]._regIndex = kRegIndexXmm0;
      break;

    case kVarTypeFp64:
    case kVarTypeXmmSd:
      self->_retCount = 1;
      self->_retList[0]._varType = kVarTypeXmmSd;
      self->_retList[0]._regIndex = kRegIndexXmm0;
      break;
#endif // ASMJIT_HOST

    case kVarTypeFpEx:
      self->_retCount = 1;
      self->_retList[0]._varType = static_cast<uint8_t>(ret);
      self->_retList[0]._regIndex = 0;
      break;

    case kVarTypeXmm:
    case kVarTypeXmmPs:
    case kVarTypeXmmPd:
      self->_retCount = 1;
      self->_retList[0]._varType = static_cast<uint8_t>(ret);
      self->_retList[0]._regIndex = kRegIndexXmm0;
      break;

    default:
      break;
  }

  // --------------------------------------------------------------------------
  // [Arguments]
  // --------------------------------------------------------------------------

  if (self->_argCount == 0)
    return;

  switch (convention) {
#if defined(ASMJIT_BUILD_X64)
    case kFuncConvX64W: {
      int32_t max = argCount < 4 ? argCount : 4;

      // Register arguments (Gp/Xmm), always left-to-right.
      for (i = 0; i != max; i++) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (x86VarIsInt(varType)) {
          arg._regIndex = self->_passedOrderGp[i];
          self->_used.add(kRegClassGp, IntUtil::mask(arg.getRegIndex()));
        }
        else if (x86VarIsFloat(varType)) {
          arg._regIndex = self->_passedOrderXmm[i];
          self->_used.add(kRegClassXy, IntUtil::mask(arg.getRegIndex()));
        }
      }

      // Stack arguments (always right-to-left).
      for (i = argCount - 1; i != -1; i--) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (arg.hasRegIndex())
          continue;

        if (x86VarIsInt(varType)) {
          stackOffset -= 8; // Always 8 bytes.
          arg._stackOffset = stackOffset;
        }
        else if (x86VarIsFloat(varType)) {
          int32_t size = static_cast<int32_t>(_varInfo[varType].getSize());
          stackOffset -= size;
          arg._stackOffset = stackOffset;
        }
      }

      // 32 bytes shadow space (X64W calling convention specific).
      stackOffset -= 4 * 8;

      break;
    }

    case kFuncConvX64U: {
      // Register arguments (Gp), always left-to-right.
      for (i = 0; i != static_cast<int32_t>(argCount); i++) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (x86VarIsInt(varType) && gpPos < 32 && self->_passedOrderGp[gpPos] != kInvalidReg) {
          arg._regIndex = self->_passedOrderGp[gpPos++];
          self->_used.add(kRegClassGp, IntUtil::mask(arg.getRegIndex()));
        }
      }

      // Register arguments (Xmm), always left-to-right.
      for (i = 0; i != static_cast<int32_t>(argCount); i++) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (x86VarIsFloat(varType)) {
          arg._regIndex = self->_passedOrderXmm[xmmPos++];
          self->_used.add(kRegClassXy, IntUtil::mask(arg.getRegIndex()));
        }
      }

      // Stack arguments.
      for (i = argCount - 1; i != -1; i--) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (arg.hasRegIndex())
          continue;

        if (x86VarIsInt(varType)) {
          stackOffset -= 8;
          arg._stackOffset = static_cast<int16_t>(stackOffset);
        }
        else if (x86VarIsFloat(varType)) {
          int32_t size = (int32_t)_varInfo[varType].getSize();

          stackOffset -= size;
          arg._stackOffset = static_cast<int16_t>(stackOffset);
        }
      }
      break;
    }
#endif // ASMJIT_BUILD_X64

#if defined(ASMJIT_BUILD_X86)
    default: {
      // Register arguments (Integer), always left-to-right.
      for (i = 0; i != static_cast<int32_t>(argCount); i++) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (x86VarIsInt(varType) && gpPos < 16 && self->_passedOrderGp[gpPos] != kInvalidReg) {
          arg._regIndex = self->_passedOrderGp[gpPos++];
          self->_used.add(kRegClassGp, IntUtil::mask(arg.getRegIndex()));
        }
      }

      // Stack arguments.
      int32_t iStart = static_cast<int32_t>(argCount - 1);
      int32_t iEnd   = -1;
      int32_t iStep  = -1;

      if (self->_direction == kFuncDirLtr) {
        iStart = 0;
        iEnd   = static_cast<int32_t>(argCount);
        iStep  = 1;
      }

      for (i = iStart; i != iEnd; i += iStep) {
        FuncInOut& arg = self->getArg(i);
        uint32_t varType = arg.getVarType();

        if (arg.hasRegIndex())
          continue;

        if (x86VarIsInt(varType)) {
          stackOffset -= 4;
          arg._stackOffset = static_cast<int16_t>(stackOffset);
        }
        else if (x86VarIsFloat(varType)) {
          int32_t size = static_cast<int32_t>(_varInfo[varType].getSize());
          stackOffset -= size;
          arg._stackOffset = static_cast<int16_t>(stackOffset);
        }
      }
      break;
    }
#endif // ASMJIT_BUILD_X86
  }

  // Modify the stack offset, thus in result all parameters would have positive
  // non-zero stack offset.
  for (i = 0; i < static_cast<int32_t>(argCount); i++) {
    FuncInOut& arg = self->getArg(i);
    if (!arg.hasRegIndex()) {
      arg._stackOffset += static_cast<uint16_t>(static_cast<int32_t>(regSize) - stackOffset);
    }
  }

  self->_argStackSize = static_cast<uint32_t>(-stackOffset);
}

bool X86X64FuncDecl::setPrototype(uint32_t convention, const FuncPrototype& prototype) {
  if (prototype.getArgCount() > kFuncArgCount)
    return false;

  X86X64FuncDecl_initConvention(this, convention);
  X86X64FuncDecl_initPrototype(this,
    prototype.getRet(),
    prototype.getArgList(),
    prototype.getArgCount());

  return true;
}

// ============================================================================
// [asmjit::X86X64FuncDecl - Reset]
// ============================================================================

void X86X64FuncDecl::reset() {
  uint32_t i;

  // --------------------------------------------------------------------------
  // [Core]
  // --------------------------------------------------------------------------

  _convention = kFuncConvNone;
  _calleePopsStack = false;
  _direction = kFuncDirRtl;
  _reserved0 = 0;

  _argCount = 0;
  _retCount = 0;

  _argStackSize = 0;
  _redZoneSize = 0;
  _spillZoneSize = 0;

  for (i = 0; i < ASMJIT_ARRAY_SIZE(_argList); i++)
    _argList[i].reset();

  _retList[0].reset();
  _retList[1].reset();

  _used.reset();
  _passed.reset();
  _preserved.reset();

  for (i = 0; i < ASMJIT_ARRAY_SIZE(_passedOrderGp); i++)
    _passedOrderGp[i] = kInvalidReg;

  for (i = 0; i < ASMJIT_ARRAY_SIZE(_passedOrderXmm); i++)
    _passedOrderXmm[i] = kInvalidReg;
}

} // x86x64 namespace
} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"

// [Guard]
#endif // ASMJIT_BUILD_X86 || ASMJIT_BUILD_X64
