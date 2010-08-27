namespace AsmJitNet
{
    public enum InstructionFlags : byte
    {
        NONE = 0x00,

        JUMP = 0x01,

        MOV = 0x02,

        FPU = 0x04,

        LOCKABLE = 0x08,

        SPECIAL = 0x10,

        SPECIAL_MEM = 0x20,
    }
}
