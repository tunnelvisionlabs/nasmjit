namespace NAsmJit
{
    public sealed class MMReg : BaseReg, IMmOperand
    {
        public MMReg(RegCode code)
            : base(code, 8)
        {
        }

        public MMReg(RegType type, RegIndex index)
            : this((RegCode)type | (RegCode)index)
        {
        }
    }
}
