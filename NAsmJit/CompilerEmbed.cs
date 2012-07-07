namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class CompilerEmbed : CompilerItem
    {
        private readonly byte[] _data;

        public CompilerEmbed(Compiler compiler, byte[] data)
            : base(compiler)
        {
            if (data == null)
                throw new ArgumentNullException("data");
            Contract.Requires(compiler != null);
            Contract.EndContractBlock();

            _data = data;
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.EmbeddedData;
            }
        }

        public override int MaxSize
        {
            get
            {
                return _data.Length;
            }
        }

        public byte[] Data
        {
            get
            {
                return _data;
            }
        }

        protected override void EmitImpl(Assembler a)
        {
            a.Embed(_data);
        }
    }
}
