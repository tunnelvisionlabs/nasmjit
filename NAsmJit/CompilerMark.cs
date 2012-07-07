namespace NAsmJit
{
    public sealed class CompilerMark : CompilerItem
    {
        public CompilerMark(Compiler compiler)
            : base(compiler)
        {
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.Mark;
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
