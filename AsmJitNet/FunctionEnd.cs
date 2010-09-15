namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public sealed class FunctionEnd : Dummy
    {
        public FunctionEnd(Compiler compiler)
            : base(compiler)
        {
            Contract.Requires(compiler != null);
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.FunctionEnd;
            }
        }

        protected override Emittable TranslateImpl(CompilerContext cc)
        {
            return null;
        }
    }
}
