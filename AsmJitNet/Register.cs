namespace AsmJitNet
{
    internal static class Register
    {
        public static GPReg gpb_lo(RegIndex index)
        {
            return new GPReg((int)index | (int)RegType.GPB_LO);
        }

        public static GPReg gpb_hi(RegIndex index)
        {
            return new GPReg((int)index | (int)RegType.GPB_HI);
        }

        public static GPReg gpw(RegIndex index)
        {
            return new GPReg((int)index | (int)RegType.GPW);
        }

        public static GPReg gpd(RegIndex index)
        {
            return new GPReg((int)index | (int)RegType.GPD);
        }

#if ASMJIT_X64
        public static GPReg gpq(RegIndex index)
        {
            return new GPReg((int)index | (int)RegType.GPN);
        }
#endif

        public static GPReg gpn(RegIndex index)
        {
            return new GPReg((int)index | (int)RegType.GPN);
        }

        public static MMReg mm(RegIndex index)
        {
            return new MMReg((int)index | (int)RegType.MM);
        }

        public static XMMReg xmm(RegIndex index)
        {
            return new XMMReg((int)index | (int)RegType.XMM);
        }
    }
}
