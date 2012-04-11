namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class Mem : Operand
    {
        private MemoryType _type;

        private SegmentPrefix _segmentPrefix;

        private RegIndex _base;

        private RegIndex _index;

        private byte _shift;

        private IntPtr _target;

        private IntPtr _displacement;

        public Mem()
        {
            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = RegIndex.Invalid;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = IntPtr.Zero;
        }

        public Mem(int id, int size = 0)
            : base(id, size)
        {
            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = RegIndex.Invalid;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = IntPtr.Zero;
        }

        public Mem(IntPtr target, IntPtr displacement, SegmentPrefix segmentPrefix, int size = 0)
            : base(size: size)
        {
            Contract.EndContractBlock();

            _type = MemoryType.Absolute;
            _segmentPrefix = segmentPrefix;

            _base = RegIndex.Invalid;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = target;
            _displacement = displacement;
        }

        public Mem(GPReg @base, IntPtr displacement, int size = 0)
            : base(size: size)
        {
            if (@base == null)
                throw new ArgumentNullException("base");
            Contract.EndContractBlock();

            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = @base.RegisterIndex;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(GPVar @base, IntPtr displacement, int size = 0)
            : base(size: size)
        {
            if (@base == null)
                throw new ArgumentNullException("base");
            Contract.EndContractBlock();

            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = (RegIndex)@base.Id;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(IntPtr target, GPReg index, int shift, IntPtr displacement, SegmentPrefix segmentPrefix, int size = 0)
            : base(size: size)
        {
            if (index == null)
                throw new ArgumentNullException("index");
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            _type = MemoryType.Absolute;
            _segmentPrefix = segmentPrefix;

            _base = RegIndex.Invalid;
            _index = index.RegisterIndex;
            _shift = (byte)shift;

            _target = target;
            _displacement = displacement;
        }

        public Mem(IntPtr target, GPVar index, int shift, IntPtr displacement, SegmentPrefix segmentPrefix, int size = 0)
            : base(size: size)
        {
            if (index == null)
                throw new ArgumentNullException("index");
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            _type = MemoryType.Absolute;
            _segmentPrefix = segmentPrefix;

            _base = RegIndex.Invalid;
            _index = (RegIndex)index.Id;
            _shift = (byte)shift;

            _target = target;
            _displacement = displacement;
        }

        public Mem(GPReg @base, GPReg index, int shift, IntPtr displacement, int size = 0)
            : base(size: size)
        {
            if (@base == null)
                throw new ArgumentNullException("base");
            if (index == null)
                throw new ArgumentNullException("index");
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = @base.RegisterIndex;
            _index = index.RegisterIndex;
            _shift = (byte)shift;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(GPVar @base, GPVar index, int shift, IntPtr displacement, int size = 0)
            : base(size: size)
        {
            if (@base == null)
                throw new ArgumentNullException("base");
            if (index == null)
                throw new ArgumentNullException("index");
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = (RegIndex)@base.Id;
            _index = (RegIndex)index.Id;
            _shift = (byte)shift;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(Label label, IntPtr displacement, int size = 0)
            : base(size: size)
        {
            if (label == null)
                throw new ArgumentNullException("label");

            _type = MemoryType.Label;
            _segmentPrefix = SegmentPrefix.None;

            _base = (RegIndex)label.Id;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(Label @base, GPReg index, int shift, IntPtr displacement, int size = 0)
            : this(@base, displacement, size)
        {
            if (@base == null)
                throw new ArgumentNullException("base");
            if (index == null)
                throw new ArgumentNullException("index");
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            _index = index.RegisterIndex;
            _shift = (byte)shift;
        }

        public Mem(Label @base, GPVar index, int shift, IntPtr displacement, int size = 0)
            : this(@base, displacement, size)
        {
            if (@base == null)
                throw new ArgumentNullException("base");
            if (index == null)
                throw new ArgumentNullException("index");
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            _index = (RegIndex)index.Id;
            _shift = (byte)shift;
        }

        public override OperandType OperandType
        {
            get
            {
                return OperandType.Mem;
            }
        }

        public MemoryType MemoryType
        {
            get
            {
                return _type;
            }
        }

        public SegmentPrefix SegmentPrefix
        {
            get
            {
                return _segmentPrefix;
            }
        }

        public bool HasBase
        {
            get
            {
                return Base != RegIndex.Invalid;
            }
        }

        public bool HasIndex
        {
            get
            {
                return Index != RegIndex.Invalid;
            }
        }

        public bool HasShift
        {
            get
            {
                return _shift != 0;
            }
        }

        public RegIndex Base
        {
            get
            {
                return _base;
            }

            set
            {
                _base = value;
            }
        }

        public RegIndex Index
        {
            get
            {
                return _index;
            }

            set
            {
                _index = value;
            }
        }

        public int Shift
        {
            get
            {
                return _shift;
            }
        }

        public IntPtr Target
        {
            get
            {
                return _target;
            }

            internal set
            {
                _target = value;
            }
        }

        public IntPtr Displacement
        {
            get
            {
                return _displacement;
            }

            set
            {
                _displacement = value;
            }
        }

        public static Mem ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, 0);
        }

        public static Mem byte_ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(IntPtr target, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, displacement, segmentPrefix, AsmJitNet.Size.DQWORD);
        }

        public static Mem ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, 0);
        }

        public static Mem byte_ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(IntPtr target, GPReg index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.DQWORD);
        }

        public static Mem ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, 0);
        }

        public static Mem byte_ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(IntPtr target, GPVar index, int shift, int displacement = 0, SegmentPrefix segmentPrefix = SegmentPrefix.None)
        {
            Contract.Requires(index != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrAbs(target, index, shift, displacement, segmentPrefix, AsmJitNet.Size.DQWORD);
        }

        public static Mem ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, 0);
        }

        public static Mem byte_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem mmword_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem xmmword_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem sysint_ptr(GPReg @base, int displacement = 0)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return MemPtrBuild(@base, displacement, (AsmJitNet.Size)IntPtr.Size);
        }

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

        private static Mem MemPtrAbs(IntPtr target, int displacement, SegmentPrefix segmentPrefix, Size size)
        {
            return new Mem(target, (IntPtr)displacement, segmentPrefix, (int)size);
        }

        private static Mem MemPtrAbs(IntPtr target, GPReg index, int shift, int displacement, SegmentPrefix segmentPrefix, Size size)
        {
            return new Mem(target, index, shift, (IntPtr)displacement, segmentPrefix, (int)size);
        }

        private static Mem MemPtrAbs(IntPtr target, GPVar index, int shift, int displacement, SegmentPrefix segmentPrefix, Size size)
        {
            return new Mem(target, index, shift, (IntPtr)displacement, segmentPrefix, (int)size);
        }

        private static Mem MemPtrBuild(Label label, int displacement, Size size)
        {
            Contract.Requires(label != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(label, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(Label label, GPReg index, int shift, int displacement, Size size)
        {
            Contract.Requires(label != null);
            Contract.Requires(index != null);
            Contract.Requires(shift >= 0 && shift <= 3);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(label, index, shift, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(Label label, GPVar index, int shift, int displacement, Size size)
        {
            Contract.Requires(label != null);
            Contract.Requires(index != null);
            Contract.Requires(shift >= 0 && shift <= 3);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(label, index, shift, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPReg @base, int displacement, Size size)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(@base, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPReg @base, GPReg index, int shift, int displacement, Size size)
        {
            Contract.Requires(@base != null);
            Contract.Requires(index != null);
            Contract.Requires(shift >= 0 && shift <= 3);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(@base, index, shift, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPVar @base, int displacement, Size size)
        {
            Contract.Requires(@base != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(@base, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPVar @base, GPVar index, int shift, int displacement, Size size)
        {
            Contract.Requires(@base != null);
            Contract.Requires(index != null);
            Contract.Requires(shift >= 0 && shift <= 3);
            Contract.Ensures(Contract.Result<Mem>() != null);

            return new Mem(@base, index, shift, (IntPtr)displacement, (int)size);
        }

        public override string ToString()
        {
            if (!HasBase)
            {
                return string.Format("[spilled var {0}]", Id & OperandIdValueMask);
            }

            string address = _base.ToString();

            if (HasIndex)
            {
                address += " + ";

                if (HasShift)
                    address += (int)Math.Pow(2, Shift) + "*";

                address += Index.ToString();
            }

            if (Displacement != IntPtr.Zero)
                address += " + " + Displacement.ToString();

            string size = string.Empty;

            switch (Size)
            {
            case 1:
                size = "byte ptr";
                break;

            case 2:
                size = "word ptr";
                break;

            case 4:
                size = "dword ptr";
                break;

            case 8:
                size = "qword ptr";
                break;

            case 10:
                size = "tword ptr";
                break;

            case 16:
                size = "dqword ptr";
                break;

            case 0:
                size = "ptr";
                break;

            default:
                size = "unknown ptr";
                break;
            }

            return string.Format("{0} [{1}]", size, address);
        }
    }
}
