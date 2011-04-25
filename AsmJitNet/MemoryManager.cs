namespace AsmJitNet
{
    using System;

    public abstract class MemoryManager
    {
        private static MemoryManager _global = new VirtualMemoryManager();

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

        public abstract IntPtr Alloc(long CodeSize, MemoryAllocType allocType = MemoryAllocType.Freeable);

        public abstract bool Free(IntPtr address);

        /// <summary>
        /// Free some tail memory.
        /// </summary>
        /// <param name="address"></param>
        /// <param name="used"></param>
        /// <returns></returns>
        public abstract bool Shrink(IntPtr address, IntPtr used);

        public abstract void FreeAll();
    }
}
