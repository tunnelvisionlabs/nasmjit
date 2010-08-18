namespace AsmJitNet2
{
    using System;

    [Flags]
    public enum VariableAlloc
    {
        Register = 0x10,
        Memory = 0x20,
        Read = 0x01,
        Write = 0x02,
        ReadWrite = Read | Write
    }
}
