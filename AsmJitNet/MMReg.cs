namespace AsmJitNet
{
    public sealed class MMReg : BaseReg
    {
        public MMReg()
            : base(InvalidValue, 8)
        {
        }

        public MMReg(int code)
            : base(code, 8)
        {
        }
    }
}
