namespace AsmJitNet
{
    public enum Condition
    {
        /// <summary>No condition code.</summary>
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
        OVERFLOW = O,
        NO_OVERFLOW = NO,
        BELOW = B,
        ABOVE_EQUAL = AE,
        EQUAL = E,
        NOT_EQUAL = NE,
        BELOW_EQUAL = BE,
        ABOVE = A,
        SIGN = S,
        NOT_SIGN = NS,
        PARITY_EVEN = PE,
        PARITY_ODD = PO,
        LESS = L,
        GREATER_EQUAL = GE,
        LESS_EQUAL = LE,
        GREATER = G,

        // aliases
        ZERO = Z,
        NOT_ZERO = NZ,
        NEGATIVE = S,
        POSITIVE = NS,

        // x87 floating point only
        FP_UNORDERED = 16,
        FP_NOT_UNORDERED = 17
    }
}
