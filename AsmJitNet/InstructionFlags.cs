namespace AsmJitNet2
{
    public enum InstructionFlags : byte
    {
        NONE = 0x00,

        JUMP = 0x01,

        MOV = 0x02,

        FPU = 0x04,

        SPECIAL = 0x08
    }
}
