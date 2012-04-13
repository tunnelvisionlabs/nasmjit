namespace AsmJitNet
{
    public sealed class SegmentReg : BaseReg
    {
        public SegmentReg(RegCode code)
            : base(code, 2)
        {
        }

        public SegmentReg(RegType type, RegIndex index)
            : this((RegCode)type | (RegCode)index)
        {
        }
    }
}
