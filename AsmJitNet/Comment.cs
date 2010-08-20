namespace AsmJitNet2
{
    public sealed class Comment : Emittable
    {
        public Comment(Compiler compiler, string comment)
            : base(compiler)
        {
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

        public override void Emit(Assembler a)
        {
            if (a.Logger != null && a.Logger.IsUsed)
                a.Logger.LogString(Comment);
        }
    }
}
