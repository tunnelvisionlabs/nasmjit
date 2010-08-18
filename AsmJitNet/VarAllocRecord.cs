namespace AsmJitNet2
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

        public RegIndex RegIndex
        {
            get;
            set;
        }
    }
}
