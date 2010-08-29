namespace AsmJitNet
{
    /// <summary>
    /// Label (jump target or data location)
    /// </summary>
    public sealed class Label : Operand
    {
        public static readonly Label Empty = new Label();

        /// <summary>
        /// Create new, unassociated label
        /// </summary>
        private Label()
        {
        }

        public Label(int id)
            : base(id | OperandIdTypeLabel)
        {
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
