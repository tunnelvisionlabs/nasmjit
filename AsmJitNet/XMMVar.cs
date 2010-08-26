namespace AsmJitNet
{
    public class XMMVar : BaseVar
    {
        public XMMVar()
            : base(RegType.XMM, VariableType.XMM)
        {
        }

        public XMMVar(int id, RegType registerCode, VariableType variableType)
            : base(registerCode, variableType)
        {
            Id = id;
            Size = 16;
        }
    }
}
