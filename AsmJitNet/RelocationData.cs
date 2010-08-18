namespace AsmJitNet2
{
    using System;

    public sealed class RelocationData
    {
        public RelocationType Type
        {
            get;
            set;
        }

        public int Size
        {
            get;
            set;
        }

        public long Offset
        {
            get;
            set;
        }

        public IntPtr Destination
        {
            get;
            set;
        }
    }
}
