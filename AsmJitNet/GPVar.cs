namespace AsmJitNet
{
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
    }
}
