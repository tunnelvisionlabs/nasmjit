namespace AsmJitNet2
{
    public enum VariableType
    {
        Invalid = -1,
        GPD = 0,
        GPQ = 1,
        GPN = GPD,
        PTR = GPN,
        X87 = 2,
        X87_1F = 3,
        X87_1D = 4,
        MM = 5,
        XMM = 6,
        XMM_1F = 7,
        XMM_4F = 8,
        XMM_1D = 9,
        XMM_2D = 10,
        FLOAT = X87_1F,
        DOUBLE = X87_1D
    }
}
