namespace AsmJitNet
{
    using System;

    /// <summary>
    /// Base class for all registers.
    /// </summary>
    public abstract class BaseReg : Operand
    {
        private readonly RegCode _code;

        protected BaseReg(RegCode code, int size)
            : base(size: size)
        {
            _code = code;

            if (RegisterType == 0)
                throw new ArgumentException();
        }

        protected BaseReg(RegType type, RegIndex index, int size)
            : this((RegCode)type | (RegCode)index, size)
        {
        }

        public sealed override OperandType OperandType
        {
            get
            {
                return OperandType.Reg;
            }
        }

        /// <summary>
        /// Gets the register code.
        /// </summary>
        public RegCode Code
        {
            get
            {
                return _code;
            }
        }

        /// <summary>
        /// Gets the register type.
        /// </summary>
        public RegType RegisterType
        {
            get
            {
                return (RegType)((int)Code & (int)RegType.MASK);
            }
        }

        /// <summary>
        /// Gets the register index.
        /// </summary>
        public RegIndex RegisterIndex
        {
            get
            {
                return (RegIndex)((int)Code & (int)RegIndex.Mask);
            }
        }

        public override string ToString()
        {
            return Code.ToString();
        }
    }
}
