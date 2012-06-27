namespace AsmJitNet
{
    public enum VariableType
    {
        Invalid = -1,
        GPD = 0,
        GPQ = 1,

//#if ASMJIT_X86
//        // Use VariableInfo.NativeVariableType instead
//        GPN = GPD,
//#elif ASMJIT_X64
//        // Use VariableInfo.NativeVariableType instead
//        GPN = GPQ,
//#endif

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
        //// Use VariableInfo.NativeVariableType instead
        //INTPTR = GPN,

//#if ASMJIT_X86
//        // Use VariableInfo.FloatVariableType instead
//        FLOAT = X87_1F,
//        // Use VariableInfo.DoubleVariableType instead
//        DOUBLE = X87_1D
//#elif ASMJIT_X64
//        // Use VariableInfo.FloatVariableType instead
//        FLOAT = XMM_1F,
//        // Use VariableInfo.DoubleVariableType instead
//        DOUBLE = XMM_1D
//#endif
    }
}
