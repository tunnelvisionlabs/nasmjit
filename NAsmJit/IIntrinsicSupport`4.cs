namespace NAsmJit
{
    public interface IIntrinsicSupport<TGP, TX87, TMM, TXMM> : IIntrinsicSupport
        where TGP : IGpOperand
        where TX87 : IX87Operand
        where TMM : IMmOperand
        where TXMM : IXmmOperand
    {
    }
}
