namespace NAsmJit
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

        [Obsolete]
        public IntPtr Address
        {
            get
            {
                return Destination;
            }

            set
            {
                Destination = value;
            }
        }
    }
}
