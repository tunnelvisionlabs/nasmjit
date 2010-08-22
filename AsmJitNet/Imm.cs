namespace AsmJitNet
{
    using System;

    public class Imm : Operand
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
            return new Imm((IntPtr)i);
        }

        public static implicit operator Imm(IntPtr i)
        {
            return new Imm(i);
        }
    }
}
