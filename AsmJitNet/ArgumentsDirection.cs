namespace AsmJitNet
{
    /// <summary>
    /// Arguments direction used by Function.
    /// </summary>
    public enum ArgumentsDirection
    {
        /// <summary>
        /// Arguments are passed left to right.
        /// </summary>
        /// <remarks>
        /// This arguments direction is unusual to C programming; it's used by Pascal compilers
        /// and in some calling conventions by the Borland compiler.
        /// </remarks>
        LeftToRight = 0,

        /// <summary>
        /// Arguments are passed right to left.
        /// </summary>
        /// <remarks>
        /// This is the default argument direction in C programming.
        /// </remarks>
        RightToLeft = 1,
    }
}
