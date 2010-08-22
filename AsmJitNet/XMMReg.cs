namespace AsmJitNet
{
    public sealed class XMMReg : BaseReg
    {
        public XMMReg()
            : base(InvalidValue, 16)
        {
        }

        public XMMReg(int code)
            : base(code, 16)
        {
        }
    }
}
