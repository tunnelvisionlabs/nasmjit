namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public static class Mem2
    {
        public static Mem ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, 0);
        }

        public static Mem byte_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem mmword_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem xmmword_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem sysint_ptr(GPVar @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, (AsmJitNet.Size)IntPtr.Size);
        }

        private static Mem MemPtrBuild(Label label, GPVar index, int shift, int displacement, Size size)
        {
            Contract.Requires(label != null);
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            Mem m = new Mem(label, (IntPtr)displacement, (int)size)
            {
                Index = (RegIndex)index.Id,
                Shift = shift
            };

            return m;
        }

        private static Mem MemPtrBuild(GPVar @base, int displacement, Size size)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem((RegIndex)@base.Id, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPVar @base, GPVar index, int shift, int displacement, Size size)
        {
            Contract.Requires(@base != null);
            Contract.Requires(index != null);
            Contract.Requires(shift >= 0 && shift <= 3);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem((RegIndex)@base.Id, (RegIndex)index.Id, shift, (IntPtr)displacement, (int)size);
        }
    }
}
