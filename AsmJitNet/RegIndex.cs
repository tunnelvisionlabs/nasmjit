﻿namespace AsmJitNet
{
    public enum RegIndex
    {
        Invalid = -1,

        Mask = 0x00FF,

        /// <summary>ID for AX/EAX/RAX registers.</summary>
        Eax = 0,
        /// <summary>ID for CX/ECX/RCX registers.</summary>
        Ecx = 1,
        /// <summary>ID for DX/EDX/RDX registers.</summary>
        Edx = 2,
        /// <summary>ID for BX/EBX/RBX registers.</summary>
        Ebx = 3,
        /// <summary>ID for SP/ESP/RSP registers.</summary>
        Esp = 4,
        /// <summary>ID for BP/EBP/RBP registers.</summary>
        Ebp = 5,
        /// <summary>ID for SI/ESI/RSI registers.</summary>
        Esi = 6,
        /// <summary>ID for DI/EDI/RDI registers.</summary>
        Edi = 7,

        /// <summary>ID for AX/EAX/RAX registers (64-bit only).</summary>
        Rax = 0,
        /// <summary>ID for CX/ECX/RCX registers (64-bit only).</summary>
        Rcx = 1,
        /// <summary>ID for DX/EDX/RDX registers (64-bit only).</summary>
        Rdx = 2,
        /// <summary>ID for BX/EBX/RBX registers (64-bit only).</summary>
        Rbx = 3,
        /// <summary>ID for SP/ESP/RSP registers (64-bit only).</summary>
        Rsp = 4,
        /// <summary>ID for BP/EBP/RBP registers (64-bit only).</summary>
        Rbp = 5,
        /// <summary>ID for SI/ESI/RSI registers (64-bit only).</summary>
        Rsi = 6,
        /// <summary>ID for DI/EDI/RDI registers (64-bit only).</summary>
        Rdi = 7,

        /// <summary>ID for r8 register (64-bit only).</summary>
        R8 = 8,
        /// <summary>ID for R9 register (64-bit only).</summary>
        R9 = 9,
        /// <summary>ID for R10 register (64-bit only).</summary>
        R10 = 10,
        /// <summary>ID for R11 register (64-bit only).</summary>
        R11 = 11,
        /// <summary>ID for R12 register (64-bit only).</summary>
        R12 = 12,
        /// <summary>ID for R13 register (64-bit only).</summary>
        R13 = 13,
        /// <summary>ID for R14 register (64-bit only).</summary>
        R14 = 14,
        /// <summary>ID for R15 register (64-bit only).</summary>
        R15 = 15,

        /// <summary>ID for mm0 register.</summary>
        Mm0 = 0,
        /// <summary>ID for mm1 register.</summary>
        Mm1 = 1,
        /// <summary>ID for mm2 register.</summary>
        Mm2 = 2,
        /// <summary>ID for mm3 register.</summary>
        Mm3 = 3,
        /// <summary>ID for mm4 register.</summary>
        Mm4 = 4,
        /// <summary>ID for mm5 register.</summary>
        Mm5 = 5,
        /// <summary>ID for mm6 register.</summary>
        Mm6 = 6,
        /// <summary>ID for mm7 register.</summary>
        Mm7 = 7,

        /// <summary>ID for xmm0 register.</summary>
        Xmm0 = 0,
        /// <summary>ID for xmm1 register.</summary>
        Xmm1 = 1,
        /// <summary>ID for xmm2 register.</summary>
        Xmm2 = 2,
        /// <summary>ID for xmm3 register.</summary>
        Xmm3 = 3,
        /// <summary>ID for xmm4 register.</summary>
        Xmm4 = 4,
        /// <summary>ID for xmm5 register.</summary>
        Xmm5 = 5,
        /// <summary>ID for xmm6 register.</summary>
        Xmm6 = 6,
        /// <summary>ID for xmm7 register.</summary>
        Xmm7 = 7,

        /// <summary>ID for xmm8 register (64-bit only).</summary>
        Xmm8 = 8,
        /// <summary>ID for xmm9 register (64-bit only).</summary>
        Xmm9 = 9,
        /// <summary>ID for xmm10 register (64-bit only).</summary>
        Xmm10 = 10,
        /// <summary>ID for xmm11 register (64-bit only).</summary>
        Xmm11 = 11,
        /// <summary>ID for xmm12 register (64-bit only).</summary>
        Xmm12 = 12,
        /// <summary>ID for xmm13 register (64-bit only).</summary>
        Xmm13 = 13,
        /// <summary>ID for xmm14 register (64-bit only).</summary>
        Xmm14 = 14,
        /// <summary>ID for xmm15 register (64-bit only).</summary>
        Xmm15 = 15
    }
}
