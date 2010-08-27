namespace AsmJitNet
{
    public interface IIntrinsicSupport<TGP, TX87, TMM, TXMM> : IIntrinsicSupport
        where TGP : Operand
        where TX87 : Operand
        where TMM : Operand
        where TXMM : Operand
    {
    }
}
