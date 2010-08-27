namespace AsmJitNet
{
    public static class RegNum
    {
        public static readonly int Base = Util.IsX86 ? 8 : (Util.IsX64 ? 16 : 0);
        public static readonly int GP = Base;
        public static readonly int MM = 8;
        public static readonly int FPU = 8;
        public static readonly int XMM = Base;
    }
}
