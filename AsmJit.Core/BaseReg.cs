namespace AsmJitNet
{
    using System;

    public abstract class BaseReg : Operand
    {
        private readonly RegCode _code;

        public BaseReg(RegCode code, int size)
            : base(size: size)
        {
            _code = code;

            if (RegisterType == 0)
                throw new ArgumentException();
        }

        public BaseReg(RegType type, RegIndex index, int size)
            : this((RegCode)type | (RegCode)index, size)
        {
        }

        public override OperandType OperandType
        {
            get
            {
                return OperandType.Reg;
            }
        }

        public RegCode Code
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
                return (RegType)((int)Code & (int)RegType.MASK);
            }
        }

        public RegIndex RegisterIndex
        {
            get
            {
                return (RegIndex)((int)Code & (int)RegIndex.Mask);
            }
        }
    }
}
