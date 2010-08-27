namespace AsmJitNet
{
    public enum RegType
    {
        MASK = 0xFF00,

        /// <summary>
        /// 8-bit general purpose register (low byte)
        /// </summary>
        GPB_LO = 0x0100,

        /// <summary>
        /// 8-bit general purpose register (high byte)
        /// </summary>
        GPB_HI = 0x0300,

        /// <summary>
        /// 16-bit general purpose register
        /// </summary>
        GPW = 0x1000,

        /// <summary>
        /// 32-bit general purpose register
        /// </summary>
        GPD = 0x2000,

        /// <summary>
        /// 64-bit general purpose register
        /// </summary>
        GPQ = 0x3000,

        /// <summary>
        /// Native-size (platform specific) general purpose register
        /// </summary>
#if ASMJIT_X86
        GPN = GPD,
#elif ASMJIT_X64
        GPN = GPQ,
#endif

        /// <summary>
        /// X87 (FPU) register
        /// </summary>
        X87 = 0x5000,

        /// <summary>
        /// 64-bit MMX register
        /// </summary>
        MM = 0x6000,

        /// <summary>
        /// 128-bit SSE register
        /// </summary>
        XMM = 0x7000,
    }
}
