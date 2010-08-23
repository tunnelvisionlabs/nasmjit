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

        public Mem(Label label, IntPtr displacement, int size = 0)
        {
            Size = (byte)size;
            _type = MemoryType.Label;
            _segmentPrefix = SegmentPrefix.None;

            _base = (RegIndex)label.Id;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(GPReg @base, IntPtr displacement, int size = 0)
        {
            Size = (byte)size;
            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = @base.RegisterIndex;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(GPVar @base, IntPtr displacement, int size = 0)
        {
            Size = (byte)size;
            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = (RegIndex)@base.Id;
            _index = RegIndex.Invalid;
            _shift = 0;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(GPReg @base, GPReg index, int shift, IntPtr displacement, int size = 0)
        {
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            Size = (byte)size;
            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = @base.RegisterIndex;
            _index = @index.RegisterIndex;
            _shift = (byte)shift;

            _target = IntPtr.Zero;
            _displacement = displacement;
        }

        public Mem(GPVar @base, GPVar index, int shift, IntPtr displacement, int size = 0)
        {
            if (shift < 0 || shift > 3)
                throw new ArgumentOutOfRangeException("shift");
            Contract.EndContractBlock();

            Size = (byte)size;
            _type = MemoryType.Native;
            _segmentPrefix = SegmentPrefix.None;

            _base = (RegIndex)@base.Id;
            _index = (RegIndex)index.Id;
            _shift = (byte)shift;

            _target = IntPtr.Zero;
            _displacement = displacement;
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

            internal set
            {
                _type = value;
            }
        }

        public SegmentPrefix SegmentPrefix
        {
            get
            {
                return _segmentPrefix;
            }

            internal set
            {
                _segmentPrefix = value;
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

            internal set
            {
                _shift = checked((byte)value);
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

        public static Mem ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, 0);
        }

        public static Mem byte_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem mmword_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem xmmword_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem sysint_ptr(GPReg @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, (AsmJitNet.Size)IntPtr.Size);
        }

        public static Mem ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, 0);
        }

        public static Mem byte_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.BYTE);
        }

        public static Mem word_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.WORD);
        }

        public static Mem dword_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DWORD);
        }

        public static Mem qword_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem tword_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.TWORD);
        }

        public static Mem dqword_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem mmword_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.QWORD);
        }

        public static Mem xmmword_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, AsmJitNet.Size.DQWORD);
        }

        public static Mem sysint_ptr(GPVar @base, int displacement = 0)
        {
            return MemPtrBuild(@base, displacement, (AsmJitNet.Size)IntPtr.Size);
        }

        private static Mem MemPtrBuild(Label label, int displacement, Size size)
        {
            return new Mem(label, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(Label label, GPReg index, int shift, int displacement, Size size)
        {
            throw new NotImplementedException();
            //return new Mem(label, index, shift, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(Label label, GPVar index, int shift, int displacement, Size size)
        {
            throw new NotImplementedException();
            //return new Mem(label, index, shift, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPReg @base, int displacement, Size size)
        {
            return new Mem(@base, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPReg @base, GPReg index, int shift, int displacement, Size size)
        {
            Contract.Requires(shift >= 0 && shift <= 3);

            return new Mem(@base, index, shift, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPVar @base, int displacement, Size size)
        {
            return new Mem(@base, (IntPtr)displacement, (int)size);
        }

        private static Mem MemPtrBuild(GPVar @base, GPVar index, int shift, int displacement, Size size)
        {
            Contract.Requires(shift >= 0 && shift <= 3);

            return new Mem(@base, index, shift, (IntPtr)displacement, (int)size);
        }
    }
}
