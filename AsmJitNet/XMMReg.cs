namespace AsmJitNet
{
    public sealed class XMMReg : BaseReg
    {
        public XMMReg()
            : base(InvalidValue, 16)
        {
        }

        public XMMReg(RegCode code)
            : this((int)code)
        {
        }

        public XMMReg(RegType type, RegIndex index)
            : this((int)type | (int)index)
        {
        }

        public XMMReg(int code)
            : base(code, 16)
        {
        }
    }
}
