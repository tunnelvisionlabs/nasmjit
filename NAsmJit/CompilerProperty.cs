namespace NAsmJit
{
    using System;

    [Flags]
    public enum CompilerProperties
    {
        None = 0,
        OptimizeAlign = 1 << 0,
        JumpHints = 1 << 1,
    }
}
