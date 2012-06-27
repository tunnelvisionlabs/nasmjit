namespace NAsmJit
{
    /// <summary>
    /// Operand types that can be encoded in operand
    /// </summary>
    public enum OperandType
    {
        /// <summary>
        /// Operand is none. This operand is not valid.
        /// </summary>
        None = 0,

        /// <summary>
        /// Operand is register
        /// </summary>
        Reg = 0x01,

        /// <summary>
        /// Operand is memory
        /// </summary>
        Mem = 0x02,

        /// <summary>
        /// Operand is immediate
        /// </summary>
        Imm = 0x04,

        /// <summary>
        /// Operand is label
        /// </summary>
        Label = 0x08,

        /// <summary>
        /// Operand is variable
        /// </summary>
        Var = 0x10
    }
}
