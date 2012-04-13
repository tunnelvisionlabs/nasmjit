﻿namespace AsmJitNet
{
    public enum RegCode
    {
        // --------------------------------------------------------------------------
        // [8-bit Registers]
        // --------------------------------------------------------------------------

        AL = (int)RegType.GPB_LO,
        CL,
        DL,
        BL,
        SPL,
        BPL,
        SIL,
        DIL,

        R8B,
        R9B,
        R10B,
        R11B,
        R12B,
        R13B,
        R14B,
        R15B,

        AH = (int)RegType.GPB_HI,
        CH,
        DH,
        BH,

        // --------------------------------------------------------------------------
        // [16-bit Registers]
        // --------------------------------------------------------------------------

        AX = (int)RegType.GPW,
        CX,
        DX,
        BX,
        SP,
        BP,
        SI,
        DI,

        R8W,
        R9W,
        R10W,
        R11W,
        R12W,
        R13W,
        R14W,
        R15W,

        // --------------------------------------------------------------------------
        // [32-bit Registers]
        // --------------------------------------------------------------------------

        EAX = (int)RegType.GPD,
        ECX,
        EDX,
        EBX,
        ESP,
        EBP,
        ESI,
        EDI,

        R8D,
        R9D,
        R10D,
        R11D,
        R12D,
        R13D,
        R14D,
        R15D,

        // --------------------------------------------------------------------------
        // [64-bit Registers]
        // --------------------------------------------------------------------------

        RAX = (int)RegType.GPQ,
        RCX,
        RDX,
        RBX,
        RSP,
        RBP,
        RSI,
        RDI,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15,

        // --------------------------------------------------------------------------
        // [MM Registers]
        // --------------------------------------------------------------------------

        MM0 = (int)RegType.MM,
        MM1,
        MM2,
        MM3,
        MM4,
        MM5,
        MM6,
        MM7,

        // --------------------------------------------------------------------------
        // [XMM Registers]
        // --------------------------------------------------------------------------

        XMM0 = RegType.XMM,
        XMM1,
        XMM2,
        XMM3,
        XMM4,
        XMM5,
        XMM6,
        XMM7,

        XMM8,
        XMM9,
        XMM10,
        XMM11,
        XMM12,
        XMM13,
        XMM14,
        XMM15,

        //// --------------------------------------------------------------------------
        //// [Native registers (depends on 32-bit or 64-bit mode)]
        //// --------------------------------------------------------------------------

        //NAX = (int)RegType.GPN,
        //NCX,
        //NDX,
        //NBX,
        //NSP,
        //NBP,
        //NSI,
        //NDI,

        // --------------------------------------------------------------------------
        // [Segment registers]
        // --------------------------------------------------------------------------

        /// <summary>ES segment register.</summary>
        ES = RegType.Segment,
        /// <summary>CS segment register.</summary>
        CS,
        /// <summary>SS segment register.</summary>
        SS,
        /// <summary>DS segment register.</summary>
        DS,
        /// <summary>FS segment register.</summary>
        FS,
        /// <summary>GS segment register.</summary>
        GS
    }
}
