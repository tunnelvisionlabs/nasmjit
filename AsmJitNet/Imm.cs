namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class Imm : Operand
    {
        private bool _isUnsigned;
        private IntPtr _value;

        public Imm(IntPtr i)
        {
            _isUnsigned = false;
            _value = i;
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
    }
}
