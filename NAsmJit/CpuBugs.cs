namespace NAsmJit
{
    using System;

    /// <summary>
    /// X86 CPU bugs.
    /// </summary>
    [Flags]
    public enum CpuBugs
    {
        None = 0,

        /// <summary>
        /// Whether the processor contains bug seen in some AMD-Opteron processors.
        /// </summary>
        AmdLockMb = 1 << 0
    }
}
