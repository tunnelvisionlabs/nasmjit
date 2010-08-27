namespace AsmJitNet
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

        public override int Generate(out IntPtr destination, Assembler assembler)
        {
            // Disallow empty code generation.
            long codeSize = assembler.CodeSize;
            if (codeSize == 0)
            {
                destination = IntPtr.Zero;
                return Errors.NoFunction;
            }

            // Switch to global memory manager if not provided.
            MemoryManager memmgr = MemoryManager ?? MemoryManager.Global;

            IntPtr p = memmgr.Alloc(codeSize, AllocType);
            if (p == IntPtr.Zero)
            {
                destination = IntPtr.Zero;
                return Errors.NoVirtualMemory;
            }

            // This is the last step. Relocate the code and return it.
            assembler.RelocCode(p);

            destination = p;
            return Errors.None;
        }
    }
}
