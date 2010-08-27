namespace AsmJitNet
{
    using System;

    [Flags]
    public enum CpuBugs
    {
        None = 0,
        AmdLockMb = 1 << 0
    }
}
