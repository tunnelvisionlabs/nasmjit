namespace AsmJitNet
{
    public class GPReg : BaseReg, IGpOperand
    {
        public GPReg(RegType type, RegIndex index)
            : this((RegCode)type | (RegCode)index)
        {
        }

        public GPReg(RegCode code)
            : base(code, 1 << (((int)code & (int)RegType.MASK) >> 12))
        {
        }
    }
}
