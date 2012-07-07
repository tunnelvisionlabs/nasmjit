namespace NAsmJit
{
    using System;

    [Flags]
    public enum VariableFlags
    {
        SinglePrecision = 0x10,
        DoublePrecision = 0x20,
        Packed = 0x40,
    }
}
