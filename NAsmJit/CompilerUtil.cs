namespace AsmJitNet
{
    using System;

    public static class CompilerUtil
    {
        public static bool IsStack16ByteAligned
        {
            get
            {
                bool result = IntPtr.Size == 8;

                return result;
            }
        }
    }
}
