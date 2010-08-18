namespace AsmJitNet2
{
    public enum RegType
    {
        MASK = 0xFF00,

        GPB_LO = 0x0100,
        GPB_HI = 0x0300,
        GPW = 0x1000,
        GPD = 0x2000,
        GPQ = 0x3000,

        GPN = GPD,

        X87 = 0x5000,
        MM = 0x6000,
        XMM = 0x7000,
    }
}
