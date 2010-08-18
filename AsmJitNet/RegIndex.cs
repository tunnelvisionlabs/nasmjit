namespace AsmJitNet2
{
    public enum RegIndex
    {
        Invalid = -1,

        Mask = 0x00FF,

        //! @brief ID for AX/EAX/RAX registers.
        Eax = 0,
        //! @brief ID for CX/ECX/RCX registers.
        Ecx = 1,
        //! @brief ID for DX/EDX/RDX registers.
        Edx = 2,
        //! @brief ID for BX/EBX/RBX registers.
        Ebx = 3,
        //! @brief ID for SP/ESP/RSP registers.
        Esp = 4,
        //! @brief ID for BP/EBP/RBP registers.
        Ebp = 5,
        //! @brief ID for SI/ESI/RSI registers.
        Esi = 6,
        //! @brief ID for DI/EDI/RDI registers.
        Edi = 7,

#if ASMJIT_X64
        //! @brief ID for AX/EAX/RAX registers.
        Rax = 0,
        //! @brief ID for CX/ECX/RCX registers.
        Rcx = 1,
        //! @brief ID for DX/EDX/RDX registers.
        Rdx = 2,
        //! @brief ID for BX/EBX/RBX registers.
        Rbx = 3,
        //! @brief ID for SP/ESP/RSP registers.
        Rsp = 4,
        //! @brief ID for BP/EBP/RBP registers.
        Rbp = 5,
        //! @brief ID for SI/ESI/RSI registers.
        Rsi = 6,
        //! @brief ID for DI/EDI/RDI registers.
        Rdi = 7,

        //! @brief ID for r8 register (additional register introduced by 64-bit architecture).
        R8 = 8,
        //! @brief ID for R9 register (additional register introduced by 64-bit architecture).
        R9 = 9,
        //! @brief ID for R10 register (additional register introduced by 64-bit architecture).
        R10 = 10,
        //! @brief ID for R11 register (additional register introduced by 64-bit architecture).
        R11 = 11,
        //! @brief ID for R12 register (additional register introduced by 64-bit architecture).
        R12 = 12,
        //! @brief ID for R13 register (additional register introduced by 64-bit architecture).
        R13 = 13,
        //! @brief ID for R14 register (additional register introduced by 64-bit architecture).
        R14 = 14,
        //! @brief ID for R15 register (additional register introduced by 64-bit architecture).
        R15 = 15,
#endif

        //! @brief ID for mm0 register.
        Mm0 = 0,
        //! @brief ID for mm1 register.
        Mm1 = 1,
        //! @brief ID for mm2 register.
        Mm2 = 2,
        //! @brief ID for mm3 register.
        Mm3 = 3,
        //! @brief ID for mm4 register.
        Mm4 = 4,
        //! @brief ID for mm5 register.
        Mm5 = 5,
        //! @brief ID for mm6 register.
        Mm6 = 6,
        //! @brief ID for mm7 register.
        Mm7 = 7,

        //! @brief ID for xmm0 register.
        Xmm0 = 0,
        //! @brief ID for xmm1 register.
        Xmm1 = 1,
        //! @brief ID for xmm2 register.
        Xmm2 = 2,
        //! @brief ID for xmm3 register.
        Xmm3 = 3,
        //! @brief ID for xmm4 register.
        Xmm4 = 4,
        //! @brief ID for xmm5 register.
        Xmm5 = 5,
        //! @brief ID for xmm6 register.
        Xmm6 = 6,
        //! @brief ID for xmm7 register.
        Xmm7 = 7,

#if ASMJIT_X64
        //! @brief ID for xmm8 register (additional register introduced by 64-bit architecture).
        Xmm8 = 8,
        //! @brief ID for xmm9 register (additional register introduced by 64-bit architecture).
        Xmm9 = 9,
        //! @brief ID for xmm10 register (additional register introduced by 64-bit architecture).
        Xmm10 = 10,
        //! @brief ID for xmm11 register (additional register introduced by 64-bit architecture).
        Xmm11 = 11,
        //! @brief ID for xmm12 register (additional register introduced by 64-bit architecture).
        Xmm12 = 12,
        //! @brief ID for xmm13 register (additional register introduced by 64-bit architecture).
        Xmm13 = 13,
        //! @brief ID for xmm14 register (additional register introduced by 64-bit architecture).
        Xmm14 = 14,
        //! @brief ID for xmm15 register (additional register introduced by 64-bit architecture).
        Xmm15 = 15
#endif
    }
}
