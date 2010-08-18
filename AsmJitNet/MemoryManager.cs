namespace AsmJitNet2
{
    using System;

    public abstract class MemoryManager
    {
        private static MemoryManager _global = new DefaultMemoryManager();

        public static MemoryManager Global
        {
            get
            {
                return _global;
            }
        }

        public abstract long UsedBytes
        {
            get;
        }

        public abstract long AllocatedBytes
        {
            get;
        }

        public abstract IntPtr Alloc(long CodeSize, MemoryAllocType allocType);

        public abstract bool Free(IntPtr address);
    }
}
