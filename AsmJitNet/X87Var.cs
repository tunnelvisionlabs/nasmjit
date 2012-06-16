namespace AsmJitNet
{
    public class X87Var : BaseVar, IX87Operand
    {
        public X87Var()
            : base(RegType.X87, VariableType.X87)
        {
        }

        public X87Var(int id, RegType registerType, VariableType variableType)
            : base(id, 12, registerType, variableType)
        {
        }
    }
}
