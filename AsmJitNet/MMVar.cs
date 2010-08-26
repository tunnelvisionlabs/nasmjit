namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public class MMVar : BaseVar
    {
        public MMVar()
            : base(RegType.MM, VariableType.MM)
        {
        }

        public MMVar(int id, RegType registerCode, VariableType variableType)
            : base(id, 8, registerCode, variableType)
        {
        }
    }
}
