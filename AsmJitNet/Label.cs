﻿namespace AsmJitNet
{
    public sealed class Label : Operand
    {
        public static readonly Label Empty = new Label();

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