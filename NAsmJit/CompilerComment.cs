namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class CompilerComment : CompilerItem
    {
        public CompilerComment(Compiler compiler, string comment)
            : base(compiler)
        {
            Contract.Requires(compiler != null);

            Comment = comment;
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.Comment;
            }
        }

        public override int MaxSize
        {
            get
            {
                return 0;
            }
        }

        protected override void EmitImpl(Assembler a)
        {
            if (a.Logger != null)
                a.Logger.LogFormat("; {0}" + Environment.NewLine, Comment);
        }
    }
}
