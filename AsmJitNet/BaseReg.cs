namespace AsmJitNet
{
    using System;

    public class BaseReg : Operand
    {
        private readonly int _code;

        public BaseReg(int code, int size)
            : base(size: size)
        {
            _code = code;

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
