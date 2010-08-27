﻿namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public class MMVar : BaseVar
    {
        public MMVar()
            : base(RegType.MM, VariableType.MM)
        {
        }

        public MMVar(int id, RegType registerType, VariableType variableType)
            : base(id, 8, registerType, variableType)
        {
        }
    }
}
