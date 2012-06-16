namespace AsmJitNet
{
    public sealed class XMMReg : BaseReg, IXmmOperand
    {
        public XMMReg(RegCode code)
            : base(code, 16)
        {
        }

        public XMMReg(RegType type, RegIndex index)
            : this((RegCode)type | (RegCode)index)
        {
        }
    }
}
