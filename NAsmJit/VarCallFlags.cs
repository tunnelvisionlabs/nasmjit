namespace NAsmJit
{
    using System;

    [Flags]
    public enum VarCallFlags
    {
        None = 0,

        IN_GP = 0x0001,
        IN_MM = 0x0002,
        IN_XMM = 0x0004,
        IN_STACK = 0x0008,

        OUT_EAX = 0x0010,
        OUT_EDX = 0x0020,
        OUT_ST0 = 0x0040,
        OUT_ST1 = 0x0080,
        OUT_MM0 = 0x0100,
        OUT_XMM0 = 0x0400,
        OUT_XMM1 = 0x0800,

        IN_MEM_PTR = 0x1000,
        CALL_OPERAND_REG = 0x2000,
        CALL_OPERAND_MEM = 0x4000,

        UnuseAfterUse = 0x8000,
    }
}
