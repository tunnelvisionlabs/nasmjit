namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class Imm : Operand
    {
        private readonly IntPtr _value;
        private readonly bool _isUnsigned;

        public Imm(IntPtr i)
            : this(i, false)
        {
        }

        public Imm(IntPtr i, bool isUnsigned)
        {
            _value = i;
            _isUnsigned = isUnsigned;
        }

        public override OperandType OperandType
        {
            get
            {
                return OperandType.Imm;
            }
        }

        public IntPtr Value
        {
            get
            {
                return _value;
            }
        }

        public bool IsUnsigned
        {
            get
            {
                return _isUnsigned;
            }
        }

        public static implicit operator Imm(int i)
        {
            Contract.Ensures(Contract.Result<Imm>() != null);

            return new Imm((IntPtr)i);
        }

        public static implicit operator Imm(IntPtr i)
        {
            Contract.Ensures(Contract.Result<Imm>() != null);

            return new Imm(i);
        }

        public override string ToString()
        {
            return "0x" + _value.ToString("X") + (IsUnsigned ? "U" : string.Empty);
        }
    }
}
