namespace AsmJitNet
{
    public sealed class Prolog : Emittable
    {
        private readonly Function _function;

        public Prolog(Compiler compiler, Function function)
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
                return EmittableType.Prolog;
            }
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
            _function.PrepareVariables(this);
        }

        protected override void TranslateImpl(CompilerContext cc)
        {
            _function.AllocVariables(cc);
        }
    }
}
