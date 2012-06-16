namespace AsmJitNet
{
    public enum VariableHintKind
    {
        None,

        /// <summary>
        /// Alloc variable
        /// </summary>
        Alloc,

        /// <summary>
        /// Spill variable
        /// </summary>
        Spill,

        /// <summary>
        /// Save variable if modified
        /// </summary>
        Save,

        /// <summary>
        /// Save variable if modified and mark it as unused
        /// </summary>
        SaveAndUnuse,

        /// <summary>
        /// Mark variable as unused
        /// </summary>
        Unuse
    }
}
