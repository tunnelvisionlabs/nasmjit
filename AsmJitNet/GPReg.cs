namespace AsmJitNet
{
    public class GPReg : BaseReg
    {
        public GPReg()
            : base(InvalidValue, 0)
        {
        }

        public GPReg(RegCode code)
            : this((int)code)
        {
        }

        public GPReg(RegType type, RegIndex index)
            : this((int)type | (int)index)
        {
        }

        public GPReg(int code)
            : base(code, 1 << ((code & (int)RegType.MASK) >> 12))
        {
        }
    }
}
