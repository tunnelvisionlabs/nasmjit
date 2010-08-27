namespace AsmJitNet
{
    public class XMMVar : BaseVar
    {
        public XMMVar()
            : base(RegType.XMM, VariableType.XMM)
        {
        }

        public XMMVar(int id, RegType registerType, VariableType variableType)
            : base(id, 16, registerType, variableType)
        {
        }
    }
}
