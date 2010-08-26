namespace AsmJitNet
{
    public class GPVar : BaseVar
    {
        public GPVar()
            : base(RegType.GPN, VariableType.GPN)
        {
        }

        public GPVar(int id, int size, RegType regType, VariableType variableType)
            : base(id, size, regType, variableType)
        {
        }
    }
}
