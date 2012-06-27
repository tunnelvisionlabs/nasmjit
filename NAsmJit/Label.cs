namespace NAsmJit
{
    /// <summary>
    /// Label (jump target or data location)
    /// </summary>
    public sealed class Label : Operand
    {
        public static readonly Label Empty = new Label();

        private readonly string _name;

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

        public Label(int id, string name)
            : base(id | OperandIdTypeLabel)
        {
            if (!string.IsNullOrEmpty(name))
                _name = name;
        }

        public override OperandType OperandType
        {
            get
            {
                return OperandType.Label;
            }
        }

        public string Name
        {
            get
            {
                if (_name != null)
                    return _name;

                return "L." + (Id & Operand.OperandIdValueMask);
            }
        }

        public override string ToString()
        {
            return Name;
        }
    }
}
