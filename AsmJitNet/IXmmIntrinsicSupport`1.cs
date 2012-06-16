namespace AsmJitNet
{
    public interface IXmmIntrinsicSupport<TXMM> : IIntrinsicSupport
        where TXMM : IXmmOperand
    {
    }
}
