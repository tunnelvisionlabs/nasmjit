namespace AsmJitNet
{
    public static class RegNum
    {
        public static readonly int Base = Util.IsX86 ? 8 : (Util.IsX64 ? 16 : 0);
        public static readonly int GP = Base;
        public static readonly int MM = 8;
        public static readonly int FPU = 8;
        public static readonly int XMM = Base;

        /// <summary>Count of segment registers, including no segment (AsmJit specific).</summary>
        /// <remarks>
        /// There are 6 segment registers, but AsmJit uses 0 as no segment, and
        /// 1...6 as segment registers, this means that there are 7 segment registers
        /// in AsmJit API, but only 6 can be used through @c Assembler or @c Compiler
        /// API.
        /// </remarks>
        public static readonly int Segment = 7;
    }
}
