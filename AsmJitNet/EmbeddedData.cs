namespace AsmJitNet2
{
    using System;

    public class EmbeddedData : Emittable
    {
        private readonly byte[] _data;

        public EmbeddedData(Compiler compiler, byte[] data)
            : base(compiler)
        {
            if (data == null)
                throw new ArgumentNullException("data");

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

        public override void Emit(Assembler a)
        {
            a.Embed(_data);
        }
    }
}
