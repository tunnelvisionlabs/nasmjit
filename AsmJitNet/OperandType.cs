namespace AsmJitNet2
{
    public enum OperandType
    {
        None = 0,
        Reg = 0x01,
        Mem = 0x02,
        Imm = 0x04,
        Label = 0x08,
        Var = 0x10
    }
}
