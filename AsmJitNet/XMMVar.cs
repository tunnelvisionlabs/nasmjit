namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public class XMMVar : BaseVar, IXmmOperand
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
            Contract.Requires(vdata != null);

            return new XMMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
        }
    }
}
