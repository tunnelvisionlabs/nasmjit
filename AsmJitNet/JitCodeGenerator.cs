namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

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

        public override void Generate(out IntPtr destination, Assembler assembler)
        {
            if (assembler == null)
                throw new ArgumentNullException("assembler");
            Contract.EndContractBlock();

            // Disallow empty code generation.
            long codeSize = assembler.CodeSize;
            if (codeSize == 0)
            {
                destination = IntPtr.Zero;
                throw new AssemblerException("The assembler has no code to generate.");
            }

            // Switch to global memory manager if not provided.
            MemoryManager memmgr = MemoryManager ?? MemoryManager.Global;

            IntPtr p = memmgr.Alloc(codeSize, AllocType);
            if (p == IntPtr.Zero)
            {
                destination = IntPtr.Zero;
                throw new JitException("Out of virtual memory.");
            }

            // This is the last step. Relocate the code and return it.
            assembler.RelocCode(p);
            destination = p;
        }
    }
}
