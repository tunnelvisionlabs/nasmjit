﻿namespace NAsmJit
{
    using System.Diagnostics.Contracts;

    public class MMVar : BaseVar, IMmOperand
    {
        public MMVar()
            : base(RegType.MM, VariableType.MM)
        {
        }

        public MMVar(int id, RegType registerType, VariableType variableType)
            : base(id, 8, registerType, variableType)
        {
        }

        public static MMVar FromData(CompilerVar vdata)
        {
            Contract.Requires(vdata != null);

            return new MMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
        }
    }
}
