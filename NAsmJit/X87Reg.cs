namespace NAsmJit
{
    public class X87Reg : BaseReg, IX87Operand
    {
        public X87Reg(RegCode code)
            : base(code, 10)
        {
        }

        public X87Reg(RegType type, RegIndex index)
            : this((RegCode)type | (RegCode)index)
        {
        }
    }
}
