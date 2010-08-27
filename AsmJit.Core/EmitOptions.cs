namespace AsmJitNet
{
    using System;

    /// <summary>
    /// Emit options, mainly for internal purposes.
    /// </summary>
    [Flags]
    public enum EmitOptions
    {
        None = 0,

        /// <summary>
        /// Force REX prefix to be emitted.
        /// </summary>
        /// <remarks>
        /// This option should be used carefully, because there are unencodable
        /// combinations. If you want to access ah, bh, ch or dh registers then you
        /// can't emit REX prefix and it will cause an illegal instruction error.
        /// </remarks>
        RexPrefix = 1 << 0,

        /// <summary>
        /// Tell Assembler or Compiler to emit and validate lock prefix.
        /// </summary>
        /// <remarks>
        /// If this option is used and instruction doesn't support LOCK prefix then
        /// invalid instruction error is generated.
        /// </remarks>
        LockPrefix = 1 << 1,

        /// <summary>
        /// Emit short/near jump or conditional jump instead of far one, saving some bytes.
        /// </summary>
        ShortJump = 1 << 2,
    }
}
