namespace AsmJitNet
{
    using System;

    /// <summary>
    /// Variable alloc mode.
    /// </summary>
    [Flags]
    public enum VariableAlloc
    {
        /// <summary>
        /// Allocating variable to read only.
        /// </summary>
        /// <remarks>
        /// Read only variables are used to optimize variable spilling. If variable
        /// is some time ago deallocated and it's not marked as changed (so it was
        /// all the life time read only) then spill is simply NOP (no mov instruction
        /// is generated to move it to it's home memory location).
        /// </remarks>
        Read = 0x01,

        /// <summary>
        /// Allocating variable to write only (overwrite).
        /// </summary>
        /// <remarks>
        /// Overwriting means that if variable is in memory, there is no generated
        /// instruction to move variable from memory to register, because that
        /// register will be overwritten by next instruction. This is used as a
        /// simple optimization to improve generated code by @c Compiler.
        /// </remarks>
        Write = 0x02,

        /// <summary>
        /// Allocating variable to read / write.
        /// </summary>
        /// <remarks>
        /// Variable allocated for read / write is marked as changed. This means that
        /// if variable must be later spilled into memory, mov (or similar)
        /// instruction will be generated.
        /// </remarks>
        ReadWrite = Read | Write,

        /// <summary>
        /// Variable can be allocated in register.
        /// </summary>
        Register = 0x04,

        /// <summary>
        /// Variable can be allocated in memory.
        /// </summary>
        Memory = 0x08,

        /// <summary>
        /// Unuse the variable after use.
        /// </summary>
        UnuseAfterUse = 0x10,

        /// <summary>
        /// Variable can be allocated only to one register (special allocation).
        /// </summary>
        Special = 0x20,
    }
}
