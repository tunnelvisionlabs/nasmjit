namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    internal static class Util
    {
        internal static bool IsX86
        {
            get
            {
                return IntPtr.Size == 4;
            }
        }

        internal static bool IsX64
        {
            get
            {
                return IntPtr.Size == 8;
            }
        }

        internal static bool IsInt8(long value)
        {
            return value >= sbyte.MinValue && value <= sbyte.MaxValue;
        }

        internal static bool IsUInt8(long value)
        {
            return value >= byte.MinValue && value <= byte.MaxValue;
        }

        internal static bool IsInt16(long value)
        {
            return value >= short.MinValue && value <= short.MaxValue;
        }

        internal static bool IsUInt16(long value)
        {
            return value >= ushort.MinValue && value <= ushort.MaxValue;
        }

        internal static bool IsInt32(long value)
        {
            return value >= int.MinValue && value <= int.MaxValue;
        }

        internal static bool IsUInt32(long value)
        {
            return value >= uint.MinValue && value <= uint.MaxValue;
        }

        internal static uint MaskFromIndex(RegIndex index)
        {
            return 1U << (int)index;
        }

        internal static uint MaskUpToIndex(int x)
        {
            if (x >= 32)
                return 0xFFFFFFFF;
            else
                return (1U << x) - 1;
        }

        internal static int BitCount(int x)
        {
            return BitCount((uint)x);
        }

        internal static int BitCount(uint x)
        {
            x = x - ((x >> 1) & 0x55555555);
            x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
            return (int)(((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24);
        }

        internal static uint FindFirstBit(uint mask)
        {
            for (uint i = 0, bit = 1; i < sizeof(uint); i++, bit <<= 1)
            {
                if ((mask & bit) != 0)
                    return i;
            }
            return 0xFFFFFFFFU;
        }

        internal static int DeltaTo16(int x)
        {
            return AlignTo16(x) - x;
        }

        internal static int AlignTo16(int x)
        {
            return (x + 15) & ~15;
        }

        internal static void myhex(StringBuilder buf, IList<byte> binaryData)
        {
            throw new NotImplementedException();
        }
    }
}
