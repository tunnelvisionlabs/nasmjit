namespace AsmJitNet
{
    public sealed class Dummy : Emittable
    {
        public Dummy(Compiler compiler)
            : base(compiler)
        {
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Dummy;
            }
        }

        public override int MaxSize
        {
            get
            {
                return 0;
            }
        }
    }
}
