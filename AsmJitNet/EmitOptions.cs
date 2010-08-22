namespace AsmJitNet
{
    using System;

    [Flags]
    public enum EmitOptions
    {
        None = 0,
        LockPrefix = 1 << 0,
        RexPrefix = 1 << 1,
    }
}
