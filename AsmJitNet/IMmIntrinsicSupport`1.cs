namespace AsmJitNet
{
    public interface IMmIntrinsicSupport<TMM> : IIntrinsicSupport
        where TMM : IMmOperand
    {
    }
}
