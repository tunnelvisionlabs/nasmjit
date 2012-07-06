namespace NAsmJit
{
    internal class VarCallRecord
    {
        public CompilerVar vdata
        {
            get;
            set;
        }

        public VarCallFlags Flags
        {
            get;
            set;
        }

        public byte InCount
        {
            get;
            set;
        }

        public byte InDone
        {
            get;
            set;
        }

        public byte OutCount
        {
            get;
            set;
        }

        public byte OutDone
        {
            get;
            set;
        }
    }
}
