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

        public static XMMVar FromData(VarData vdata)
        {
            return new XMMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
        }
    }
}
