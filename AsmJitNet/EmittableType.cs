namespace AsmJitNet
{
    /// <summary>
    /// Emmitable type
    /// </summary>
    /// <remarks>
    /// For each emittable that is used by @c Compiler must be defined it's type.
    /// Compiler can optimize instruction stream by analyzing emittables and each
    /// type is hint for it. The most used emittables are instructions
    /// (@c EMITTABLE_INSTRUCTION).
    /// </remarks>
    public enum EmittableType
    {
        /// <summary>
        /// Emittable is invalid
        /// </summary>
        None,

        /// <summary>
        /// Emittable is dummy (used as a marker)
        /// </summary>
        Dummy,

        /// <summary>
        /// Emittable is comment (no code)
        /// </summary>
        Comment,

        /// <summary>
        /// Emittable is embedded data
        /// </summary>
        EmbeddedData,

        /// <summary>
        /// Emittable is .align directive
        /// </summary>
        Align,

        /// <summary>
        /// Emittable is variable hint (alloc, spill, use, unuse, ...)
        /// </summary>
        VariableHint,

        /// <summary>
        /// Emittable is a single instruction
        /// </summary>
        Instruction,

        /// <summary>
        /// Emittable is a block of instructions
        /// </summary>
        Block,

        /// <summary>
        /// Emittable is function declaration
        /// </summary>
        Function,

        /// <summary>
        /// Emittable is function prolog
        /// </summary>
        Prolog,

        /// <summary>
        /// Emittable is function epilog
        /// </summary>
        Epilog,

        /// <summary>
        /// Emittable is end of function
        /// </summary>
        FunctionEnd,

        /// <summary>
        /// Emittable is target (bound label)
        /// </summary>
        Target,

        /// <summary>
        /// Emittable is jump table
        /// </summary>
        JumpTable,

        /// <summary>
        /// Emittable is function call
        /// </summary>
        Call,

        /// <summary>
        /// Emittable is return
        /// </summary>
        Return
    }
}
