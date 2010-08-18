namespace AsmJitNet2
{
    using System;

    internal sealed class Buffer
    {
        private byte[] _data;

        private int _cur;

        private int _max;

        private int _capacity;

        private int _growThreshold;

        public Buffer(int growThreshold = 16)
        {
            _growThreshold = growThreshold;
        }

        public byte[] Data
        {
            get
            {
                return _data;
            }
        }

        public int Offset
        {
            get
            {
                return _cur;
            }
        }

        public int Capacity
        {
            get
            {
                return _capacity;
            }
        }

        public bool EnsureSpace()
        {
            return (_cur >= _max) ? Grow() : true;
        }

        public void EmitByte(byte x)
        {
            _data[_cur] = x;
            _cur++;
        }

        public void EmitWord(ushort x)
        {
            unsafe
            {
                fixed (byte* data = _data)
                {
                    ushort* cast = (ushort*)(data + _cur);
                    *cast = x;
                    _cur += 2;
                }
            }
        }

        public void EmitDWord(uint x)
        {
            unsafe
            {
                fixed (byte* data = _data)
                {
                    uint* cast = (uint*)(data + _cur);
                    *cast = x;
                    _cur += 4;
                }
            }
        }

        public void EmitQWord(ulong x)
        {
            unsafe
            {
                fixed (byte* data = _data)
                {
                    ulong* cast = (ulong*)(data + _cur);
                    *cast = x;
                    _cur += 8;
                }
            }
        }

        public void EmitSysInt(IntPtr x)
        {
            unsafe
            {
                fixed (byte* data = _data)
                {
                    IntPtr* cast = (IntPtr*)(data + _cur);
                    *cast = x;
                    _cur += IntPtr.Size;
                }
            }
        }

        public void EmitSysUInt(UIntPtr x)
        {
            unsafe
            {
                fixed (byte* data = _data)
                {
                    UIntPtr* cast = (UIntPtr*)(data + _cur);
                    *cast = x;
                    _cur += UIntPtr.Size;
                }
            }
        }

        public byte GetByteAt(int position)
        {
            return _data[position];
        }

        public void SetByteAt(int position, byte value)
        {
            _data[position] = value;
        }

        public void SetDWordAt(int position, uint value)
        {
            unsafe
            {
                fixed (byte* data = _data)
                {
                    uint* cast = (uint*)(data + position);
                    *cast = value;
                }
            }
        }

        private bool Grow()
        {
            int to = _capacity;

            if (to < 512)
                to = 1024;
            else if (to > 65536)
                to += 65536;
            else
                to <<= 1;

            return Realloc(to);
        }

        private bool Realloc(int to)
        {
            if (Capacity < to)
            {
                int len = Offset;

                if (_data != null)
                    Array.Resize(ref _data, to);
                else
                    _data = new byte[to];

                _max = to;
                _max -= (to >= _growThreshold) ? _growThreshold : to;

                _capacity = to;
            }

            return true;
        }
    }
}
