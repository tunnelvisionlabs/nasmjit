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
#include "../base/intutil.h"
#include "../x86/x86cpu.h"

// 2009-02-05: Thanks to Mike Tajmajer for VC7.1 compiler support. It shouldn't
// affect x64 compilation, because x64 compiler starts with VS2005 (VC8.0).
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#include <intrin.h>
#endif // _MSC_VER >= 1400

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {
namespace x86x64 {

// ============================================================================
// [asmjit::x86x64::hostCpuId]
// ============================================================================

// This is messy, I know. Cpuid is implemented as intrinsic in VS2005, but
// we should support other compilers as well. Main problem is that MS compilers
// in 64-bit mode not allows to use inline assembler, so we need intrinsic and
// we need also asm version.

// hostCpuId() and detectCpuInfo() for x86 and x64 platforms begins here.
#if defined(ASMJIT_HOST_X86) || defined(ASMJIT_HOST_X64)
void hostCpuId(uint32_t inEax, uint32_t inEcx, CpuId* result) {

#if defined(_MSC_VER)
// 2009-02-05: Thanks to Mike Tajmajer for supporting VC7.1 compiler.
// ASMJIT_HOST_X64 is here only for readibility, only VS2005 can compile 64-bit code.
# if _MSC_VER >= 1400 || defined(ASMJIT_HOST_X64)
  // Done by intrinsics.
  __cpuidex(reinterpret_cast<int*>(result->i), inEax, inEcx);
# else // _MSC_VER < 1400
  uint32_t cpuid_eax = inEax;
  uint32_t cpuid_ecx = inCax;
  uint32_t* cpuid_out = result->i;

  __asm {
    mov     eax, cpuid_eax
    mov     ecx, cpuid_ecx
    mov     edi, cpuid_out
    cpuid
    mov     dword ptr[edi +  0], eax
    mov     dword ptr[edi +  4], ebx
    mov     dword ptr[edi +  8], ecx
    mov     dword ptr[edi + 12], edx
  }
# endif // _MSC_VER < 1400

#elif defined(__GNUC__)
// Note, patched to preserve ebx/rbx register which is used by GCC.
# if defined(ASMJIT_HOST_X86)
#  define __myCpuId(inEax, inEcx, outEax, outEbx, outEcx, outEdx) \
  asm ("mov %%ebx, %%edi\n"  \
       "cpuid\n"             \
       "xchg %%edi, %%ebx\n" \
       : "=a" (outEax), "=D" (outEbx), "=c" (outEcx), "=d" (outEdx) : "a" (inEax), "c" (inEcx))
# else
#  define __myCpuId(inEax, inEcx, outEax, outEbx, outEcx, outEdx) \
  asm ("mov %%rbx, %%rdi\n"  \
       "cpuid\n"             \
       "xchg %%rdi, %%rbx\n" \
       : "=a" (outEax), "=D" (outEbx), "=c" (outEcx), "=d" (outEdx) : "a" (inEax), "c" (inEcx))
# endif
  __myCpuId(inEax, inEcx, result->eax, result->ebx, result->ecx, result->edx);
#endif // Compiler #ifdef.
}

// ============================================================================
// [asmjit::x86x64::cpuSimplifyBrandString]
// ============================================================================

static ASMJIT_INLINE void cpuSimplifyBrandString(char* s) {
  // Always clear the current character in the buffer. It ensures that there
  // is no garbage after the string NULL terminator.
  char* d = s;

  char prev = 0;
  char curr = s[0];
  s[0] = '\0';

  for (;;) {
    if (curr == 0)
      break;

    if (curr == ' ') {
      if (prev == '@' || s[1] == ' ' || s[1] == '@')
        goto _Skip;
    }

    d[0] = curr;
    d++;
    prev = curr;

_Skip:
    curr = *++s;
    s[0] = '\0';
  }

  d[0] = '\0';
}

// ============================================================================
// [asmjit::x86x64::CpuVendor]
// ============================================================================

struct CpuVendor {
  uint32_t id;
  char text[12];
};

static const CpuVendor cpuVendorTable[] = {
  { kCpuVendorAmd      , { 'A', 'M', 'D', 'i', 's', 'b', 'e', 't', 't', 'e', 'r', '!' } },
  { kCpuVendorAmd      , { 'A', 'u', 't', 'h', 'e', 'n', 't', 'i', 'c', 'A', 'M', 'D' } },
  { kCpuVendorVia      , { 'C', 'e', 'n', 't', 'a', 'u', 'r', 'H', 'a', 'u', 'l', 's' } },
  { kCpuVendorNSM      , { 'C', 'y', 'r', 'i', 'x', 'I', 'n', 's', 't', 'e', 'a', 'd' } },
  { kCpuVendorIntel    , { 'G', 'e', 'n', 'u', 'i', 'n', 'e', 'I', 'n', 't', 'e', 'l' } },
  { kCpuVendorTransmeta, { 'G', 'e', 'n', 'u', 'i', 'n', 'e', 'T', 'M', 'x', '8', '6' } },
  { kCpuVendorNSM      , { 'G', 'e', 'o', 'd', 'e', ' ', 'b', 'y', ' ', 'N', 'S', 'C' } },
  { kCpuVendorTransmeta, { 'T', 'r', 'a', 'n', 's', 'm', 'e', 't', 'a', 'C', 'P', 'U' } },
  { kCpuVendorVia      , { 'V', 'I', 'A',  0 , 'V', 'I', 'A',  0 , 'V', 'I', 'A',  0  } }
};

static ASMJIT_INLINE bool cpuVendorEq(const CpuVendor& info, const char* vendorString) {
  const uint32_t* a = reinterpret_cast<const uint32_t*>(info.text);
  const uint32_t* b = reinterpret_cast<const uint32_t*>(vendorString);

  return (a[0] == b[0]) & (a[1] == b[1]) & (a[2] == b[2]);
}

// ============================================================================
// [asmjit::x86x64::hostCpuDetect]
// ============================================================================

void hostCpuDetect(Cpu* out) {
  CpuId regs;

  uint32_t i;
  uint32_t maxId;

  // Clear everything except the '_size' member.
  ::memset(reinterpret_cast<uint8_t*>(out) + sizeof(uint32_t),
    0, sizeof(BaseCpu) - sizeof(uint32_t));

  // Fill safe defaults.
  ::memcpy(out->_vendorString, "Unknown", 8);
  out->_coresCount = BaseCpu::detectNumberOfCores();

  // Get vendor string/id.
  hostCpuId(0, 0, &regs);

  maxId = regs.eax;
  ::memcpy(out->_vendorString, &regs.ebx, 4);
  ::memcpy(out->_vendorString + 4, &regs.edx, 4);
  ::memcpy(out->_vendorString + 8, &regs.ecx, 4);

  for (i = 0; i < 3; i++) {
    if (cpuVendorEq(cpuVendorTable[i], out->_vendorString)) {
      out->_vendorId = cpuVendorTable[i].id;
      break;
    }
  }

  // Get feature flags in ecx/edx and family/model in eax.
  hostCpuId(1, 0, &regs);

  // Fill family and model fields.
  out->_family   = (regs.eax >> 8) & 0x0F;
  out->_model    = (regs.eax >> 4) & 0x0F;
  out->_stepping = (regs.eax     ) & 0x0F;

  // Use extended family and model fields.
  if (out->_family == 0x0F) {
    out->_family += ((regs.eax >> 20) & 0xFF);
    out->_model  += ((regs.eax >> 16) & 0x0F) << 4;
  }

  out->_processorType        = ((regs.eax >> 12) & 0x03);
  out->_brandIndex           = ((regs.ebx      ) & 0xFF);
  out->_flushCacheLineSize   = ((regs.ebx >>  8) & 0xFF) * 8;
  out->_maxLogicalProcessors = ((regs.ebx >> 16) & 0xFF);

  if (regs.ecx & 0x00000001U) out->addFeature(kCpuFeatureSse3);
  if (regs.ecx & 0x00000002U) out->addFeature(kCpuFeaturePclmulqdq);
  if (regs.ecx & 0x00000008U) out->addFeature(kCpuFeatureMonitorMWait);
  if (regs.ecx & 0x00000200U) out->addFeature(kCpuFeatureSsse3);
  if (regs.ecx & 0x00002000U) out->addFeature(kCpuFeatureCmpXchg16B);
  if (regs.ecx & 0x00080000U) out->addFeature(kCpuFeatureSse41);
  if (regs.ecx & 0x00100000U) out->addFeature(kCpuFeatureSse42);
  if (regs.ecx & 0x00400000U) out->addFeature(kCpuFeatureMovbe);
  if (regs.ecx & 0x00800000U) out->addFeature(kCpuFeaturePopcnt);
  if (regs.ecx & 0x02000000U) out->addFeature(kCpuFeatureAesni);
  if (regs.ecx & 0x40000000U) out->addFeature(kCpuFeatureRdrand);

  if (regs.edx & 0x00000010U) out->addFeature(kCpuFeatureRdtsc);
  if (regs.edx & 0x00000100U) out->addFeature(kCpuFeatureCmpXchg8B);
  if (regs.edx & 0x00008000U) out->addFeature(kCpuFeatureCmov);
  if (regs.edx & 0x00800000U) out->addFeature(kCpuFeatureMmx);
  if (regs.edx & 0x01000000U) out->addFeature(kCpuFeatureFxsr);
  if (regs.edx & 0x02000000U) out->addFeature(kCpuFeatureSse).addFeature(kCpuFeatureMmxExt);
  if (regs.edx & 0x04000000U) out->addFeature(kCpuFeatureSse).addFeature(kCpuFeatureSse2);
  if (regs.edx & 0x10000000U) out->addFeature(kCpuFeatureMultithreading);

  if (out->_vendorId == kCpuVendorAmd && (regs.edx & 0x10000000U)) {
    // AMD sets Multithreading to ON if it has more cores.
    if (out->_coresCount == 1)
      out->_coresCount = 2;
  }

  // Detect AVX.
  if (regs.ecx & 0x10000000U) {
    out->addFeature(kCpuFeatureAvx);

    if (regs.ecx & 0x00000800U) out->addFeature(kCpuFeatureXop);
    if (regs.ecx & 0x00004000U) out->addFeature(kCpuFeatureFma3);
    if (regs.ecx & 0x00010000U) out->addFeature(kCpuFeatureFma4);
    if (regs.ecx & 0x20000000U) out->addFeature(kCpuFeatureF16C);
  }

  // Detect new features if the processor supports CPUID-07.
  if (maxId >= 7) {
    hostCpuId(7, 0, &regs);

    if (regs.ebx & 0x00000001) out->addFeature(kCpuFeatureFsGsBase);
    if (regs.ebx & 0x00000008) out->addFeature(kCpuFeatureBmi);
    if (regs.ebx & 0x00000010) out->addFeature(kCpuFeatureHle);
    if (regs.ebx & 0x00000100) out->addFeature(kCpuFeatureBmi2);
    if (regs.ebx & 0x00000200) out->addFeature(kCpuFeatureRepMovsbStosbExt);
    if (regs.ebx & 0x00000800) out->addFeature(kCpuFeatureRtm);

    // AVX2 depends on AVX.
    if (out->hasFeature(kCpuFeatureAvx)) {
      if (regs.ebx & 0x00000020) out->addFeature(kCpuFeatureAvx2);
    }
  }

  // Calling cpuid with 0x80000000 as the in argument gets the number of valid
  // extended IDs.
  hostCpuId(0x80000000, 0, &regs);

  uint32_t maxExtId = IntUtil::iMin<uint32_t>(regs.eax, 0x80000004);
  uint32_t* brand = reinterpret_cast<uint32_t*>(out->_brandString);

  for (i = 0x80000001; i <= maxExtId; i++) {
    hostCpuId(i, 0, &regs);

    switch (i) {
      case 0x80000001:
        if (regs.ecx & 0x00000001U) out->addFeature(kCpuFeatureLahfSahf);
        if (regs.ecx & 0x00000020U) out->addFeature(kCpuFeatureLzcnt);
        if (regs.ecx & 0x00000040U) out->addFeature(kCpuFeatureSse4A);
        if (regs.ecx & 0x00000080U) out->addFeature(kCpuFeatureMsse);
        if (regs.ecx & 0x00000100U) out->addFeature(kCpuFeaturePrefetch);

        if (regs.edx & 0x00100000U) out->addFeature(kCpuFeatureExecuteDisableBit);
        if (regs.edx & 0x00200000U) out->addFeature(kCpuFeatureFfxsr);
        if (regs.edx & 0x00400000U) out->addFeature(kCpuFeatureMmxExt);
        if (regs.edx & 0x08000000U) out->addFeature(kCpuFeatureRdtscp);
        if (regs.edx & 0x40000000U) out->addFeature(kCpuFeature3dNowExt).addFeature(kCpuFeatureMmxExt);
        if (regs.edx & 0x80000000U) out->addFeature(kCpuFeature3dNow);
        break;

      case 0x80000002:
      case 0x80000003:
      case 0x80000004:
        *brand++ = regs.eax;
        *brand++ = regs.ebx;
        *brand++ = regs.ecx;
        *brand++ = regs.edx;
        break;

      default:
        // Additional features can be detected in the future.
        break;
    }
  }

  // Simplify the brand string (remove unnecessary spaces to make printing nicer).
  cpuSimplifyBrandString(out->_brandString);
}
#endif

} // x86x64 namespace
} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"

// [Guard]
#endif // ASMJIT_BUILD_X86 || ASMJIT_BUILD_X64
