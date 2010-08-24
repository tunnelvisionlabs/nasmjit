﻿namespace AsmJitNet
{
    public sealed class MMReg : BaseReg
    {
        public MMReg()
            : base(InvalidValue, 8)
        {
        }

        public MMReg(RegCode code)
            : this((int)code)
        {
        }

        public MMReg(int code)
            : base(code, 8)
        {
        }
    }
}