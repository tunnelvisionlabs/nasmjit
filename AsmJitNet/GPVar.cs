namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public class GPVar : BaseVar
    {
        public GPVar()
            : base(Register.NativeRegisterType, VariableInfo.NativeVariableType)
        {
        }

        public GPVar(int id, int size, RegType registerType, VariableType variableType)
            : base(id, size, registerType, variableType)
        {
        }

        public static GPVar FromData(VarData vdata)
        {
            Contract.Requires(vdata != null);

            return new GPVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
        }
    }
}
