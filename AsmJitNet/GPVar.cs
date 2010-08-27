namespace AsmJitNet
{
    public class GPVar : BaseVar
    {
        public GPVar()
            : base(RegType.GPN, VariableType.GPN)
        {
        }

        public GPVar(int id, int size, RegType registerType, VariableType variableType)
            : base(id, size, registerType, variableType)
        {
        }
    }
}
