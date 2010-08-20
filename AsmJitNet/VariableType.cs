namespace AsmJitNet2
{
    public enum VariableType
    {
        Invalid = -1,
        GPD = 0,
        GPQ = 1,
#if ASMJIT_X86
        GPN = GPD,
#elif ASMJIT_X64
        GPN = GPQ,
#endif
        X87 = 2,
        X87_1F = 3,
        X87_1D = 4,
        MM = 5,
        XMM = 6,
        XMM_1F = 7,
        XMM_4F = 8,
        XMM_1D = 9,
        XMM_2D = 10,

        INT32 = GPD,
        INT64 = GPQ,
        INTPTR = GPN,

#if ASMJIT_X86
        FLOAT = X87_1F,
        DOUBLE = X87_1D
#elif ASMJIT_X64
        FLOAT = XMM_1F,
        DOUBLE = XMM_1D
#endif
    }
}
