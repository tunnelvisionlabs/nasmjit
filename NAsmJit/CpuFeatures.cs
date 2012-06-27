namespace NAsmJit
{
    using System;

    [Flags]
    public enum CpuFeatures : uint
    {
        /// <summary>Cpu has RDTSC instruction.</summary>
        RDTSC = 1U << 0,
        /// <summary>Cpu has RDTSCP instruction.</summary>
        RDTSCP = 1U << 1,
        /// <summary>Cpu has CMOV instruction (conditional move)</summary>
        CMOV = 1U << 2,
        /// <summary>Cpu has CMPXCHG8B instruction</summary>
        CMPXCHG8B = 1U << 3,
        /// <summary>Cpu has CMPXCHG16B instruction (64 bit processors)</summary>
        CMPXCHG16B = 1U << 4,
        /// <summary>Cpu has CLFUSH instruction</summary>
        CLFLUSH = 1U << 5,
        /// <summary>Cpu has PREFETCH instruction</summary>
        PREFETCH = 1U << 6,
        /// <summary>Cpu supports LAHF and SAHF instrictions.</summary>
        LAHF_SAHF = 1U << 7,
        /// <summary>Cpu supports FXSAVE and FXRSTOR instructions.</summary>
        FXSR = 1U << 8,
        /// <summary>Cpu supports FXSAVE and FXRSTOR instruction optimizations (FFXSR).</summary>
        FFXSR = 1U << 9,

        /// <summary>Cpu has MMX.</summary>
        MMX = 1U << 10,
        /// <summary>Cpu has extended MMX.</summary>
        MMX_EXT = 1U << 11,
        /// <summary>Cpu has 3dNow</summary>!
        AMD3DNOW = 1U << 12,
        /// <summary>Cpu has enchanced 3dNow</summary>!
        AMD3DNOW_EXT = 1U << 13,
        /// <summary>Cpu has SSE.</summary>
        SSE = 1U << 14,
        /// <summary>Cpu has SSE2.</summary>
        SSE2 = 1U << 15,
        /// <summary>Cpu has SSE3.</summary>
        SSE3 = 1U << 16,
        /// <summary>Cpu has Supplemental SSE3 (SSSE3).</summary>
        SSSE3 = 1U << 17,
        /// <summary>Cpu has SSE4.A.</summary>
        SSE4_A = 1U << 18,
        /// <summary>Cpu has SSE4.1.</summary>
        SSE4_1 = 1U << 19,
        /// <summary>Cpu has SSE4.2.</summary>
        SSE4_2 = 1U << 20,
        /// <summary>Cpu has AVX.</summary>
        AVX = 1U << 22,
        /// <summary>Cpu has Misaligned SSE (MSSE).</summary>
        MSSE = 1U << 23,
        /// <summary>Cpu supports MONITOR and MWAIT instructions.</summary>
        MONITOR_MWAIT = 1U << 24,
        /// <summary>Cpu supports MOVBE instruction.</summary>
        MOVBE = 1U << 25,
        /// <summary>Cpu supports POPCNT instruction.</summary>
        POPCNT = 1U << 26,
        /// <summary>Cpu supports LZCNT instruction.</summary>
        LZCNT = 1U << 27,
        /// <summary>Cpu supports PCLMULDQ set of instructions.</summary>
        PCLMULDQ = 1U << 28,
        /// <summary>Cpu supports multithreading.</summary>
        MULTI_THREADING = 1U << 29,
        /// <summary>Cpu supports execute disable bit (execute protection).</summary>
        EXECUTE_DISABLE_BIT = 1U << 30,
        /// <summary>Cpu supports 64 bits.</summary>
        X64_BIT = 1U << 31
    }
}
