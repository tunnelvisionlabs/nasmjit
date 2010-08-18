namespace AsmJitNet2
{
    public enum RegNum
    {
#if ASMJIT_X86
        Base = 8,
#else
        Base = 16,
#endif

        GP = Base,

        MM = 8,

        FPU = 8,

        XMM = Base,
    }
}
