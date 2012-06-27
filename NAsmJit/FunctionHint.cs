namespace NAsmJit
{
    using System;

    /// <summary>
    /// Function hints
    /// </summary>
    [Flags]
    public enum FunctionHints
    {
        None = 0,

        /// <summary>
        /// Use push/pop sequences instead of mov sequences in function prolog and epilog
        /// </summary>
        PushPopSequence = 1 << 0,

        /// <summary>
        /// Make naked function (without using ebp/erp in prolog/epilog)
        /// </summary>
        Naked = 1 << 1,

        /// <summary>
        /// Add emms instruction to the function epilog
        /// </summary>
        Emms = 1 << 2,

        /// <summary>
        /// Add sfence instruction to the function epilog
        /// </summary>
        StoreFence = 1 << 3,

        /// <summary>
        /// Add lfence instruction to the function epilog
        /// </summary>
        LoadFence = 1 << 4
    }
}
