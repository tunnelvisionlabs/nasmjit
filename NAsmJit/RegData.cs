namespace AsmJitNet
{
    public sealed class RegData
    {
        private readonly byte _op;
        private readonly byte _size;
        private readonly int _id;
        private readonly int _code;

        public RegData(OperandType type, int size, int id, int code)
        {
            _op = checked((byte)type);
            _size = checked((byte)size);
            _id = id;
            _code = code;
        }

        public OperandType Type
        {
            get
            {
                return (OperandType)_op;
            }
        }

        public int Size
        {
            get
            {
                return _size;
            }
        }

        public int Id
        {
            get
            {
                return _id;
            }
        }

        public int Code
        {
            get
            {
                return _code;
            }
        }
    }
}
