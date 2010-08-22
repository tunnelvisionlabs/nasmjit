namespace AsmJitNet
{
    public sealed class Epilog : Emittable
    {
        private Function _function;

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

        public override void Prepare(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
        }

        public override void Translate(CompilerContext cc)
        {
        }
    }
}
