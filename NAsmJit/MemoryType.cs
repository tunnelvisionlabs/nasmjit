namespace NAsmJit
{
    /// <summary>
    /// Type of memory operand.
    /// </summary>
    public enum MemoryType
    {
        /// <summary>
        /// Operand is combination of register(s) and displacement (native).
        /// </summary>
        Native = 0,

        /// <summary>
        /// Operand is label.
        /// </summary>
        Label = 1,

        /// <summary>
        /// Operand is absolute memory location (supported mainly in 32-bit mode)
        /// </summary>
        Absolute = 2
    }
}
