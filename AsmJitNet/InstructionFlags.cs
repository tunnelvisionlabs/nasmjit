namespace AsmJitNet
{
    using System;

    /// <summary>
    /// Instruction core flags
    /// </summary>
    [Flags]
    public enum InstructionFlags : byte
    {
        /// <summary>
        /// No flags
        /// </summary>
        NONE = 0x00,

        /// <summary>
        /// Instruction is jump, conditional jump, call or ret
        /// </summary>
        JUMP = 0x01,

        /// <summary>
        /// Instruction will overwrite first operand - o[0]
        /// </summary>
        MOV = 0x02,

        /// <summary>
        /// Instruction is X87 FPU
        /// </summary>
        FPU = 0x04,

        /// <summary>
        /// Instruction can be prepended using LOCK prefix (usable for multithreaded applications)
        /// </summary>
        LOCKABLE = 0x08,

        /// <summary>
        /// Instruction is special, this is for Compiler
        /// </summary>
        SPECIAL = 0x10,

        /// <summary>
        /// Instruction always performs memory access
        /// </summary>
        /// <remarks>
        /// This flag is always combined with SPECIAL and signalizes that
        /// there is implicit address which is accessed (usually EDI/RDI or ESI/EDI).
        /// </remarks>
        SPECIAL_MEM = 0x20,
    }
}
