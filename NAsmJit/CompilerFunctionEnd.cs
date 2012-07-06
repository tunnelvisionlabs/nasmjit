namespace NAsmJit
{
    using System.Diagnostics.Contracts;

    public sealed class CompilerFunctionEnd : CompilerItem
    {
        private readonly CompilerFunction _function;

        public CompilerFunctionEnd(Compiler compiler, CompilerFunction function)
            : base(compiler)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(function != null);
            _function = function;
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.FunctionEnd;
            }
        }

        public override int MaxSize
        {
            get
            {
                return 0;
            }
        }

        public CompilerFunction Function
        {
            get
            {
                Contract.Ensures(Contract.Result<CompilerFunction>() != null);
                return _function;
            }
        }

        protected override CompilerItem TranslateImpl(CompilerContext cc)
        {
            return null;
        }
    }
}
