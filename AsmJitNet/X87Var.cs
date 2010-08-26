namespace AsmJitNet
{
    public class X87Var : BaseVar
    {
        public X87Var()
            : base(RegType.X87, VariableType.X87)
        {
        }

        public X87Var(int id, RegType registerCode, VariableType variableType)
            : base(registerCode, variableType)
        {
            Id = id;
            Size = 12;
        }
    }
}
