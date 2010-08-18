﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsmJitNet2
{
    public class GPVar : BaseVar
    {
        public GPVar()
            : base(RegType.GPN, VariableType.GPN)
        {
        }

        public GPVar(int id, int size, RegType regType, VariableType variableType)
            : base(regType, variableType)
        {
            Id = id;
            Size = checked((byte)size);
        }
    }
}
