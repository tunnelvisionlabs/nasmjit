namespace AsmJitNet
{
    public sealed class Label : Operand
    {
        public Label()
        {
        }

        public Label(int id)
        {
            Id = id | OperandIdTypeLabel;
        }

        public override OperandType OperandType
        {
            get
            {
                return OperandType.Label;
            }
        }
    }
}
