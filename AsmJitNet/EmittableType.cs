﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsmJitNet2
{
    public enum EmittableType
    {
        None,
        Dummy,
        Comment,
        EmbeddedData,
        Align,
        VariableHint,
        Instruction,
        Block,
        Function,
        Prolog,
        Epilog,
        Target,
        JumpTable,
        Call,
        Return
    }
}
