namespace AsmJitNet
{
    public sealed class MMReg : BaseReg
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
