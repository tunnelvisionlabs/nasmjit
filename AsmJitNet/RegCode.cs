namespace AsmJitNet
{
    public enum RegCode
    {
        // --------------------------------------------------------------------------
        // [8-bit Registers]
        // --------------------------------------------------------------------------

        AL = (int)RegType.GPB_LO,
        CL,
        DL,
        BL,
#if ASMJIT_X64
        SPL,
        BPL,
        SIL,
        DIL,
#endif // ASMJIT_X64

#if ASMJIT_X64
        R8B,
        R9B,
        R10B,
        R11B,
        R12B,
        R13B,
        R14B,
        R15B,
#endif // ASMJIT_X64

        AH = (int)RegType.GPB_HI,
        CH,
        DH,
        BH,

        // --------------------------------------------------------------------------
        // [16-bit Registers]
        // --------------------------------------------------------------------------

        AX = (int)RegType.GPW,
        CX,
        DX,
        BX,
        SP,
        BP,
        SI,
        DI,
#if ASMJIT_X64
        R8W,
        R9W,
        R10W,
        R11W,
        R12W,
        R13W,
        R14W,
        R15W,
#endif // ASMJIT_X64

        // --------------------------------------------------------------------------
        // [32-bit Registers]
        // --------------------------------------------------------------------------

        EAX = (int)RegType.GPD,
        ECX,
        EDX,
        EBX,
        ESP,
        EBP,
        ESI,
        EDI,
#if ASMJIT_X64
        R8D,
        R9D,
        R10D,
        R11D,
        R12D,
        R13D,
        R14D,
        R15D,
#endif // ASMJIT_X64

        // --------------------------------------------------------------------------
        // [64-bit Registers]
        // --------------------------------------------------------------------------

#if ASMJIT_X64
        RAX = (int)RegType.GPQ,
        RCX,
        RDX,
        RBX,
        RSP,
        RBP,
        RSI,
        RDI,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15,
#endif // ASMJIT_X64

        // --------------------------------------------------------------------------
        // [MM Registers]
        // --------------------------------------------------------------------------

        MM0 = (int)RegType.MM,
        MM1,
        MM2,
        MM3,
        MM4,
        MM5,
        MM6,
        MM7,

        // --------------------------------------------------------------------------
        // [XMM Registers]
        // --------------------------------------------------------------------------

        XMM0 = (int)RegType.XMM,
        XMM1,
        XMM2,
        XMM3,
        XMM4,
        XMM5,
        XMM6,
        XMM7,
#if ASMJIT_X64
        XMM8,
        XMM9,
        XMM10,
        XMM11,
        XMM12,
        XMM13,
        XMM14,
        XMM15,
#endif // ASMJIT_X64

        // --------------------------------------------------------------------------
        // [Native registers (depends if processor runs in 32 bit or 64 bit mode)]
        // --------------------------------------------------------------------------

        NAX = (int)RegType.GPN,
        NCX,
        NDX,
        NBX,
        NSP,
        NBP,
        NSI,
        NDI
    }
}
