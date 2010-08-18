namespace AsmJitNet2
{
    public class XMMVar : BaseVar
    {
        public XMMVar()
            : base(RegType.XMM, VariableType.XMM)
        {
        }

        public XMMVar(int id, int size, RegType registerCode, VariableType variableType)
            : base(registerCode, variableType)
        {
            Id = id;
            Size = checked((byte)size);
        }
    }
}
