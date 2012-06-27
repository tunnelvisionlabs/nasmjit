namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class EmbeddedData : Emittable
    {
        private readonly byte[] _data;

        public EmbeddedData(Compiler compiler, byte[] data)
            : base(compiler)
        {
            if (data == null)
                throw new ArgumentNullException("data");
            Contract.Requires(compiler != null);
            Contract.EndContractBlock();

            _data = data;
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.EmbeddedData;
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
