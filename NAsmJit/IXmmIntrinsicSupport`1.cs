namespace NAsmJit
{
    public interface IXmmIntrinsicSupport<TXMM> : IIntrinsicSupport
        where TXMM : IXmmOperand
    {
    }
}
