namespace NAsmJit
{
    /// <summary>
    /// Item type
    /// </summary>
    /// <remarks>
    /// For each item that is used by @c Compiler must be defined it's type.
    /// Compiler can optimize instruction stream by analyzing items and each
    /// type is hint for it. The most used items are instructions
    /// <see cref="CompilerInstruction"/>.
    /// </remarks>
    public enum ItemType
    {
        /// <summary>
        /// Invalid item (can't be used).
        /// </summary>
        None,

        /// <summary>
        /// Item is mark, see <see cref="CompilerMark"/>.
        /// </summary>
        Mark,

        /// <summary>
        /// Item is comment, see <see cref="CompilerComment"/>.
        /// </summary>
        Comment,

        /// <summary>
        /// Item is embedded data, see <see cref="CompilerEmbed"/>.
        /// </summary>
        EmbeddedData,

        /// <summary>
        /// Item is .align directive, see <see cref="CompilerAlign"/>.
        /// </summary>
        Align,

        /// <summary>
        /// Item is variable hint (alloc, spill, use, unuse), see <see cref="CompilerHint"/>.
        /// </summary>
        VariableHint,

        /// <summary>
        /// Item is instruction, see <see cref="CompilerInst"/>.
        /// </summary>
        Instruction,

        /// <summary>
        /// Item is target, see <see cref="CompilerTarget"/>.
        /// </summary>
        Target,

        /// <summary>
        /// Item is function call, see <see cref="CompilerFuncCall"/>.
        /// </summary>
        Call,

        /// <summary>
        /// Item is function declaration, see <see cref="CompilerFuncDecl"/>.
        /// </summary>
        Function,

        /// <summary>
        /// Item is an end of the function, see <see cref="CompilerFuncEnd"/>.
        /// </summary>
        FunctionEnd,

        /// <summary>
        /// Item is function return, see <see cref="CompilerFuncRet"/>.
        /// </summary>
        Return,
    }
}
