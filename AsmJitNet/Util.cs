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

        internal static bool IsUInt16(long value)
        {
            return value >= ushort.MinValue && value <= ushort.MaxValue;
        }

        internal static bool IsInt32(long value)
        {
            return value >= int.MinValue && value <= int.MaxValue;
        }

        internal static void myhex(StringBuilder buf, IList<byte> binaryData)
        {
            throw new NotImplementedException();
        }
    }
}
