namespace AsmJitNet
{
    public enum EmittableType
    {
        None,
        Dummy,
        Comment,
        EmbeddedData,
        Align,
        VariableHint,
        Instruction,
        Block,
        Function,
        Prolog,
        Epilog,
        Target,
        JumpTable,
        Call,
        Return
    }
}
