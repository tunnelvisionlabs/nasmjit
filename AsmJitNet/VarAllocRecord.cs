namespace AsmJitNet
{
    public sealed class VarAllocRecord
    {
        public VarData VarData
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
