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
#ifndef _ASMJITCPUINFO_H
#define _ASMJITCPUINFO_H

// [Dependencies]
#include "AsmJitConfig.h"

namespace AsmJit {

//! @addtogroup AsmJit_CpuInfo
//! @{

#if defined(ASMJIT_X86) || defined(ASMJIT_X64)
//! @brief Structure used by cpuid() function.
union CpuId
{
  UInt32 i[4];
  struct
  {
    UInt32 eax;
    UInt32 ebx;
    UInt32 ecx;
    UInt32 edx;
  };
};

//! @brief Calls CPUID instruction with eax == @a in and returns result to @a out.
ASMJIT_API void cpuid(UInt32 in, CpuId* out);
#endif // ASMJIT_X86 || ASMJIT_X64

//! @brief Informations about host cpu.
struct CpuInfo
{
  char vendor[16];
  UInt32 family;
  UInt32 model;
  UInt32 numberOfProcessors;
  UInt32 features;
  UInt32 featuresExt;
  UInt32 bugs;

#if defined(ASMJIT_X86) || defined(ASMJIT_X64)
  struct X86ExtendedInfo
  {
    UInt32 steppingId;
    UInt32 processorType;
    UInt32 brandIndex;
    UInt32 clFlushCacheLineSize;
    UInt32 logicalProcessors;
    UInt32 apicPhysicalId;
  };
  X86ExtendedInfo x86ExtendedInfo;

  //! @brief X86/X64 CPU features.
  enum Feature
  {
    //! @brief Cpu has RDTSC instruction.
    Feature_RDTSC = 1U << 0,
    //! @brief Cpu has RDTSCP instruction.
    Feature_RDTSCP = 1U << 1,
    //! @brief Cpu has CMOV instruction (conditional move)
    Feature_CMOV = 1U << 2,
    //! @brief Cpu has CMPXCHG8B instruction
    Feature_CMPXCHG8B = 1U << 3,
    //! @brief Cpu has CMPXCHG16B instruction (64 bit processors)
    Feature_CMPXCHG16B = 1U << 4,
    //! @brief Cpu has CLFUSH instruction
    Feature_CLFSH = 1U << 5,
    //! @brief Cpu has PREFETCH instruction
    Feature_PREFETCH = 1U << 6,
    //! @brief Cpu supports LAHF and SAHF instrictions.
    Feature_LAHF_SAHF = 1U << 7,

    //! @brief Cpu has MMX.
    Feature_MMX = 1U << 10,
    //! @brief Cpu has extended MMX.
    Feature_MMXExt = 1U << 11,
    //! @brief Cpu has 3dNow!
    Feature_3dNow = 1U << 12,
    //! @brief Cpu has enchanced 3dNow!
    Feature_3dNowExt = 1U << 13,
    //! @brief Cpu has SSE.
    Feature_SSE = 1U << 14,
    //! @brief Cpu has Misaligned SSE (MSSE).
    Feature_MSSE = 1U << 15,
    //! @brief Cpu has SSE2.
    Feature_SSE2 = 1U << 16,
    //! @brief Cpu has SSE3.
    Feature_SSE3 = 1U << 17,
    //! @brief Cpu has Supplemental SSE3 (SSSE3).
    Feature_SSSE3 = 1U << 18,
    //! @brief Cpu has SSE4.A.
    Feature_SSE4_A = 1U << 19,
    //! @brief Cpu has SSE4.1.
    Feature_SSE4_1 = 1U << 20,
    //! @brief Cpu has SSE4.2.
    Feature_SSE4_2 = 1U << 21,
    //! @brief Cpu supports MONITOR and MWAIT instructions.
    Feature_MotitorMWait = 1U << 22,
    //! @brief Cpu supports POPCNT instruction.
    Feature_POPCNT = 1U << 23,
    //! @brief Cpu supports LZCNT instruction.
    Feature_LZCNT  = 1U << 24,
    //! @brief Cpu supports execute disable bit (execute protection).
    Feature_ExecuteDisableBit = 1U << 31
  };

  enum FeatureExt
  {
    FeatureExt_CPLQualifiedDebugStore        = 1U << 0,
    FeatureExt_EnhancedIntelSpeedStep        = 1U << 1,
    FeatureExt_ThermalMonitor2               = 1U << 2,
    FeatureExt_L1ContextId                   = 1U << 3,
    FeatureExt_XTPRUpdateControl             = 1U << 4,
    FeatureExt_PerfDebugCapabilityMSR        = 1U << 5,
    FeatureExt_CmpLegacy                     = 1U << 7,
    FeatureExt_SVM                           = 1U << 8,
    FeatureExt_ExtApicSpace                  = 1U << 9,
    FeatureExt_AltMovCr8                     = 1U << 10,
    FeatureExt_SKINITandDEV                  = 1U << 11,
    FeatureExt_SYSCALL_SYSRET                = 1U << 12,
    FeatureExt_FFXSR                         = 1U << 13,
    FeatureExt_1GBSupport                    = 1U << 14,
    FeatureExt_64Available                   = 1U << 20,
    FeatureExt_MultiThreading                = 1U << 30,
    FeatureExt_VirtualMachineExtensions      = 1U << 31,
  };

  enum Bug
  {
    Bug_AmdLockMB = 1U << 0
  };

#endif // ASMJIT_X86 || ASMJIT_X64
};

//! @brief Detects CPU features to CpuInfo structure @a i.
//!
//! @sa @c CpuInfo
ASMJIT_API void detectCpuInfo(CpuInfo* i);

ASMJIT_API CpuInfo* cpuInfo();

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITCPUINFO_H
