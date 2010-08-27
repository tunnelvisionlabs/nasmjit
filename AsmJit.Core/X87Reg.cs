﻿namespace AsmJitNet
{
    public class X87Reg : BaseReg
    {
        public X87Reg(RegCode code)
            : base(code, 10)
        {
        }

        public X87Reg(RegType type, RegIndex index)
            : this((RegCode)type | (RegCode)index)
        {
        }
    }
}
