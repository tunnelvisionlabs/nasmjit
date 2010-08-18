namespace AsmJitNet2
{
    using System;

    public class BaseReg : Operand
    {
        private int _code;

        public BaseReg(int code, int size)
        {
            _code = code;
            Size = (byte)size;

            if (RegisterType == 0)
                throw new ArgumentException();
        }

        public override OperandType OperandType
        {
            get
            {
                return OperandType.Reg;
            }
        }

        public int Code
        {
            get
            {
                return _code;
            }
        }

        public RegType RegisterType
        {
            get
            {
                return (RegType)(Code & (int)RegType.MASK);
            }
        }

        public RegIndex RegisterIndex
        {
            get
            {
                return (RegIndex)(Code & (int)RegIndex.Mask);
            }
        }
    }
}
