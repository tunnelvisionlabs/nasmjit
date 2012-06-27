namespace AsmJitNet
{
    public sealed class VarMemBlock
    {
        public int Offset
        {
            get;
            set;
        }

        public int Size
        {
            get;
            set;
        }

        public VarMemBlock NextUsed
        {
            get;
            set;
        }

        public VarMemBlock NextFree
        {
            get;
            set;
        }
    }
}
