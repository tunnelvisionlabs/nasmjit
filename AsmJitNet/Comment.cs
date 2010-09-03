namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class Comment : Emittable
    {
        public Comment(Compiler compiler, string comment)
            : base(compiler)
        {
            Contract.Requires(compiler != null);

            Comment = comment;
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Comment;
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
            if (a.Logger != null && a.Logger.IsUsed)
                a.Logger.LogFormat("; {0}" + Environment.NewLine, Comment);
        }
    }
}
