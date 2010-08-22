namespace AsmJitNet
{
    public class X87Reg : BaseReg
    {
        public X87Reg()
            : base(InvalidValue, 10)
        {
        }

        public X87Reg(int code)
            : base(code | (int)RegType.X87, 10)
        {
        }
    }
}
