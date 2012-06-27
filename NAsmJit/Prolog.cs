namespace NAsmJit
{
    using System.Diagnostics.Contracts;

    public sealed class Prolog : Emittable
    {
        private readonly Function _function;

        public Prolog(Compiler compiler, Function function)
            : base(compiler)
        {
            Contract.Requires(compiler != null);

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

        protected override Emittable TranslateImpl(CompilerContext cc)
        {
            _function.AllocVariables(cc);
            return Next;
        }
    }
}
