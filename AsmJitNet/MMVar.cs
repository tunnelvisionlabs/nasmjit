namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public class MMVar : BaseVar
    {
        public MMVar()
            : base(RegType.MM, VariableType.MM)
        {
        }

        public MMVar(int id, int size, RegType registerCode, VariableType variableType)
            : base(registerCode, variableType)
        {
            Contract.Requires(size >= byte.MinValue && size <= byte.MaxValue);

            Id = id;
            Size = checked((byte)size);
        }
    }
}
