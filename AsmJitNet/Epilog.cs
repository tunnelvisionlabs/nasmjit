namespace AsmJitNet
{
    public sealed class Epilog : Emittable
    {
        private readonly Function _function;

        public Epilog(Compiler compiler, Function function)
            : base(compiler)
        {
            _function = function;
        }

        public Function Function
        {
            get
            {
                return _function;
            }
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Epilog;
            }
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
        }

        protected override void TranslateImpl(CompilerContext cc)
        {
        }
    }
}
