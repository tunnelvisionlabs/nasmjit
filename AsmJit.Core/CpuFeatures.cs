namespace AsmJitNet
{
    using System;

    [Flags]
    public enum CpuFeatures : uint
    {
        //! @brief Cpu has RDTSC instruction.
        RDTSC = 1U << 0,
        //! @brief Cpu has RDTSCP instruction.
        RDTSCP = 1U << 1,
        //! @brief Cpu has CMOV instruction (conditional move)
        CMOV = 1U << 2,
        //! @brief Cpu has CMPXCHG8B instruction
        CMPXCHG8B = 1U << 3,
        //! @brief Cpu has CMPXCHG16B instruction (64 bit processors)
        CMPXCHG16B = 1U << 4,
        //! @brief Cpu has CLFUSH instruction
        CLFLUSH = 1U << 5,
        //! @brief Cpu has PREFETCH instruction
        PREFETCH = 1U << 6,
        //! @brief Cpu supports LAHF and SAHF instrictions.
        LAHF_SAHF = 1U << 7,
        //! @brief Cpu supports FXSAVE and FXRSTOR instructions.
        FXSR = 1U << 8,
        //! @brief Cpu supports FXSAVE and FXRSTOR instruction optimizations (FFXSR).
        FFXSR = 1U << 9,

        //! @brief Cpu has MMX.
        MMX = 1U << 10,
        //! @brief Cpu has extended MMX.
        MMX_EXT = 1U << 11,
        //! @brief Cpu has 3dNow!
        AMD3DNOW = 1U << 12,
        //! @brief Cpu has enchanced 3dNow!
        AMD3DNOW_EXT = 1U << 13,
        //! @brief Cpu has SSE.
        SSE = 1U << 14,
        //! @brief Cpu has Misaligned SSE (MSSE).
        MSSE = 1U << 15,
        //! @brief Cpu has SSE2.
        SSE2 = 1U << 16,
        //! @brief Cpu has SSE3.
        SSE3 = 1U << 17,
        //! @brief Cpu has Supplemental SSE3 (SSSE3).
        SSSE3 = 1U << 18,
        //! @brief Cpu has SSE4.A.
        SSE4_A = 1U << 19,
        //! @brief Cpu has SSE4.1.
        SSE4_1 = 1U << 20,
        //! @brief Cpu has SSE4.2.
        SSE4_2 = 1U << 21,
        //! @brief Cpu has SSE5.
        SSE5 = 1U << 22,
        //! @brief Cpu supports MONITOR and MWAIT instructions.
        MONITOR_MWAIT = 1U << 23,
        //! @brief Cpu supports MOVBE instruction.
        MOVBE = 1U << 24,
        //! @brief Cpu supports POPCNT instruction.
        POPCNT = 1U << 25,
        //! @brief Cpu supports LZCNT instruction.
        LZCNT = 1U << 26,
        //! @brief Cpu supports multithreading.
        MULTI_THREADING = 1U << 29,
        //! @brief Cpu supports execute disable bit (execute protection).
        EXECUTE_DISABLE_BIT = 1U << 30,
        //! @brief Cpu supports 64 bits.
        X64_BIT = 1U << 31
    }
}
