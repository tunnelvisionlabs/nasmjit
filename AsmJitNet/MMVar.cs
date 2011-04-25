﻿namespace AsmJitNet
{
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

        public static MMVar FromData(VarData vdata)
        {
            return new MMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
        }
    }
}
