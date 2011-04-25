namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public struct RegisterMask : IEquatable<RegisterMask>
    {
        public static readonly RegisterMask All = new RegisterMask(~0);
        public static readonly RegisterMask Zero = new RegisterMask(0);

        private readonly int _mask;

        public RegisterMask(int mask)
        {
            _mask = mask;
        }

        public int RegisterCount
        {
            get
            {
                return Util.BitCount(_mask);
            }
        }

        public RegIndex FirstRegister
        {
            get
            {
                return (RegIndex)Util.FindFirstBit(_mask);
            }
        }

        public static RegisterMask FromIndex(RegIndex index)
        {
            return new RegisterMask(1 << (int)index);
        }

        public static RegisterMask MaskToIndex(int index)
        {
            if (index >= 32)
                return new RegisterMask(~0);
            else
                return new RegisterMask((1 << index) - 1);
        }

        public static bool operator ==(RegisterMask a, RegisterMask b)
        {
            return a._mask == b._mask;
        }

        public static bool operator !=(RegisterMask a, RegisterMask b)
        {
            return a._mask != b._mask;
        }

        public static RegisterMask operator |(RegisterMask a, RegisterMask b)
        {
            return new RegisterMask(a._mask | b._mask);
        }

        public static RegisterMask operator &(RegisterMask a, RegisterMask b)
        {
            return new RegisterMask(a._mask & b._mask);
        }

        public static RegisterMask operator ~(RegisterMask a)
        {
            return new RegisterMask(~a._mask);
        }

        public bool Equals(RegisterMask other)
        {
            return _mask == other._mask;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is RegisterMask))
                return false;

            return Equals((RegisterMask)obj);
        }

        public override int GetHashCode()
        {
            return _mask.GetHashCode();
        }
    }
}
