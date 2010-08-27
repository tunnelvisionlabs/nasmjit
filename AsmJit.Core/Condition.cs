namespace AsmJitNet
{
    public enum Condition
    {
        //! @brief No condition code.
        None = -1,

        // Condition codes from processor manuals.
        A = 0x7,
        AE = 0x3,
        B = 0x2,
        BE = 0x6,
        C = 0x2,
        E = 0x4,
        G = 0xF,
        GE = 0xD,
        L = 0xC,
        LE = 0xE,
        NA = 0x6,
        NAE = 0x2,
        NB = 0x3,
        NBE = 0x7,
        NC = 0x3,
        NE = 0x5,
        NG = 0xE,
        NGE = 0xC,
        NL = 0xD,
        NLE = 0xF,
        NO = 0x1,
        NP = 0xB,
        NS = 0x9,
        NZ = 0x5,
        O = 0x0,
        P = 0xA,
        PE = 0xA,
        PO = 0xB,
        S = 0x8,
        Z = 0x4,

        // Simplified condition codes
        OVERFLOW = 0x0,
        NO_OVERFLOW = 0x1,
        BELOW = 0x2,
        ABOVE_EQUAL = 0x3,
        EQUAL = 0x4,
        NOT_EQUAL = 0x5,
        BELOW_EQUAL = 0x6,
        ABOVE = 0x7,
        SIGN = 0x8,
        NOT_SIGN = 0x9,
        PARITY_EVEN = 0xA,
        PARITY_ODD = 0xB,
        LESS = 0xC,
        GREATER_EQUAL = 0xD,
        LESS_EQUAL = 0xE,
        GREATER = 0xF,

        // aliases
        ZERO = 0x4,
        NOT_ZERO = 0x5,
        NEGATIVE = 0x8,
        POSITIVE = 0x9,

        // x87 floating point only
        FP_UNORDERED = 16,
        FP_NOT_UNORDERED = 17
    }
}
