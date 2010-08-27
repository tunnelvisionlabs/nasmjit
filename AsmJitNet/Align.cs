namespace AsmJitNet
{
    using System;

    public class Align : Emittable
    {
        private readonly int _size;

        public Align(Compiler compiler, int size = 0)
            : base(compiler)
        {
            if (size < 0)
                throw new ArgumentOutOfRangeException("size");

            _size = size;
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Align;
            }
        }

        public override int MaxSize
        {
            get
            {
                return (_size > 0) ? _size - 1 : 0;
            }
        }

        public int Size
        {
            get
            {
                return _size;
            }
        }

        public override void Emit(Assembler a)
        {
            a.Align(_size);
        }
    }
}
