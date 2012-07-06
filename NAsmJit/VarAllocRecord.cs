namespace NAsmJit
{
    public sealed class VarAllocRecord
    {
        public CompilerVar VarData
        {
            get;
            set;
        }

        public VariableAlloc VarFlags
        {
            get;
            set;
        }

        /// <summary>
        /// Register mask (default is 0).
        /// </summary>
        public RegisterMask RegMask
        {
            get;
            set;
        }
    }
}
