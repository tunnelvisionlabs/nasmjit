namespace NAsmJit
{
    using System;

    /// <summary>
    /// Instruction operand flags
    /// </summary>
    [Flags]
    public enum OperandFlags : short
    {
        None = 0,

        // X86, MM, XMM

        /// <summary>
        /// 1-byte general purpose register.
        /// </summary>
        GB = 0x0001,

        /// <summary>
        /// 2-byte general purpose register.
        /// </summary>
        GW = 0x0002,

        /// <summary>
        /// 4-byte general purpose register.
        /// </summary>
        GD = 0x0004,

        /// <summary>
        /// 8-byte general purpose register.
        /// </summary>
        GQ = 0x0008,

        /// <summary>
        /// MMX register.
        /// </summary>
        MM = 0x0010,

        /// <summary>
        /// XMM register.
        /// </summary>
        XMM = 0x0020,
        MEM = 0x0040,
        IMM = 0x0080,


        GB_MEM = GB | MEM,
        GW_MEM = GW | MEM,
        GD_MEM = GD | MEM,
        GQ_MEM = GQ | MEM,

        GQDWB = GQ | GD | GW | GB,
        GQDW = GQ | GD | GW,
        GQD = GQ | GD,
        GWB = GW | GB,

        GQDWB_MEM = GQDWB | MEM,
        GQDW_MEM = GQDW | MEM,
        GQD_MEM = GQD | MEM,
        GWB_MEM = GWB | MEM,

        MM_MEM = MM | MEM,
        XMM_MEM = XMM | MEM,
        MM_XMM = MM | XMM,
        MM_XMM_MEM = MM | XMM | MEM,

        // X87
        FM_2 = MEM | 0x0100,
        FM_4 = MEM | 0x0200,
        FM_8 = MEM | 0x0400,
        FM_10 = MEM | 0x0800,

        FM_2_4 = FM_2 | FM_4,
        FM_2_4_8 = FM_2 | FM_4 | FM_8,
        FM_4_8 = FM_4 | FM_8,
        FM_4_8_10 = FM_4 | FM_8 | FM_10,

        // Don't emit REX prefix.
        NOREX = 0x2000
    }
}
