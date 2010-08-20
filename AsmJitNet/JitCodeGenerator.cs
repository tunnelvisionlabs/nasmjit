namespace AsmJitNet2
{
    using System;
    using Debug = System.Diagnostics.Debug;

    public class JitCodeGenerator : CodeGenerator
    {
        private readonly MemoryManager _memoryManager;
        private readonly MemoryAllocType _allocType;

        public JitCodeGenerator(MemoryManager memoryManager = null, MemoryAllocType allocType = MemoryAllocType.Freeable)
        {
            _memoryManager = memoryManager;
            _allocType = allocType;
        }

        public MemoryManager MemoryManager
        {
            get
            {
                return _memoryManager;
            }
        }

        public MemoryAllocType AllocType
        {
            get
            {
                return _allocType;
            }
        }

        public override int Alloc(out IntPtr addressPtr, out IntPtr addressBase, long codeSize)
        {
            Debug.Assert(codeSize > 0);

            // Switch to global memory manager if not provided.
            MemoryManager memmgr = MemoryManager ?? MemoryManager.Global;

            IntPtr p = memmgr.Alloc(codeSize, AllocType);

            addressPtr = p;
            addressBase = p;

            return p != IntPtr.Zero ? Errors.None : Errors.NoVirtualMemory;
        }
    }
}
