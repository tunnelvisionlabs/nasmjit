namespace AsmJitNet2
{
    using System;
    using Debug = System.Diagnostics.Debug;
    using System.Runtime.InteropServices;

    public static class VirtualMemory
    {
        private static readonly VirtualMemoryLocal _vm = new VirtualMemoryLocal();

        public static int Alignment
        {
            get
            {
                return _vm.Alignment;
            }
        }

        public static int PageSize
        {
            get
            {
                return _vm.PageSize;
            }
        }

        public static IntPtr Alloc(int length, out int allocated, bool canExecute)
        {
            allocated = 0;

            // VirtualAlloc rounds allocated size to page size automatically.
            int msize = RoundUp(length, _vm.PageSize);

            // Windows XP SP2 / Vista allows Data Excution Prevention (DEP).
            UnsafeNativeMethods.MemoryProtect protect = canExecute ? UnsafeNativeMethods.MemoryProtect.ExecuteReadwrite : UnsafeNativeMethods.MemoryProtect.Readwrite;
            IntPtr mbase = UnsafeNativeMethods.VirtualAlloc(IntPtr.Zero, (UIntPtr)msize, UnsafeNativeMethods.AllocationType.Commit | UnsafeNativeMethods.AllocationType.Reserve, protect);
            if (mbase == IntPtr.Zero)
                return IntPtr.Zero;

            Debug.Assert(IsAligned((long)mbase, _vm.Alignment));
            allocated = msize;
            return mbase;
        }

        public static void Free(IntPtr address, int length)
        {
            UnsafeNativeMethods.VirtualFree(address, UIntPtr.Zero, UnsafeNativeMethods.FreeType.Release);
        }

        private static bool IsAligned(long @base, int alignment)
        {
            return @base % alignment == 0;
        }

        private static int RoundUp(int @base, int pageSize)
        {
            int over = @base % pageSize;
            return @base + (over > 0 ? pageSize - over : 0);
        }

        private static int RoundUpToPowerOf2(int value)
        {
            return (int)RoundUpToPowerOf2((uint)value);
        }

        private static uint RoundUpToPowerOf2(uint value)
        {
            value -= 1;

            value = value | (value >> 1);
            value = value | (value >> 2);
            value = value | (value >> 4);
            value = value | (value >> 8);
            value = value | (value >> 16);

            return value + 1;
        }

        private static class UnsafeNativeMethods
        {
            [DllImport("kernel32.dll")]
            internal static extern void GetNativeSystemInfo(out SYSTEM_INFO info);

            [DllImport("kernel32.dll")]
            internal static extern IntPtr VirtualAlloc(IntPtr address, UIntPtr size, AllocationType allocationType, MemoryProtect protect);

            [DllImport("kernel32.dll")]
            internal static extern bool VirtualFree(IntPtr address, UIntPtr size, FreeType freeType);

            [Flags]
            public enum AllocationType : uint
            {
                Commit = 0x1000,
                Reserve = 0x2000,
                Reset = 0x80000,
                LargePages = 0x20000000,
                Physical = 0x400000,
                TopDown = 0x100000,
                WriteWatch = 0x200000
            }

            [Flags]
            public enum FreeType : uint
            {
                Decommit = 0x4000,
                Release = 0x8000,
            }

            [Flags]
            public enum MemoryProtect : uint
            {
                Execute = 0x10,
                ExecuteRead = 0x20,
                ExecuteReadwrite = 0x40,
                ExecuteWritecopy = 0x80,
                Noaccess = 0x01,
                Readonly = 0x02,
                Readwrite = 0x04,
                Writecopy = 0x08,
                GuardModifierflag = 0x100,
                NocacheModifierflag = 0x200,
                WritecombineModifierflag = 0x400
            }

            [StructLayout(LayoutKind.Sequential, Pack = 2)]
            public struct SYSTEM_INFO
            {
                public ushort wProcessorArchitecture;
                public ushort wReserved;
                public uint dwPageSize;
                public IntPtr lpMinimumApplicationAddress;
                public IntPtr lpMaximumApplicationAddress;
                public IntPtr dwActiveProcessorMask;
                public uint dwNumberOfProcessors;
                public uint dwProcessorType;
                public uint dwAllocationGranularity;
                public ushort wProcessorLevel;
                public ushort wProcessorRevision;
            }
        }

        private sealed class VirtualMemoryLocal
        {
            public VirtualMemoryLocal()
            {
                UnsafeNativeMethods.SYSTEM_INFO info;
                UnsafeNativeMethods.GetNativeSystemInfo(out info);

                Alignment = (int)info.dwAllocationGranularity;
                PageSize = RoundUpToPowerOf2((int)info.dwPageSize);
            }

            public int Alignment
            {
                get;
                private set;
            }

            public int PageSize
            {
                get;
                private set;
            }
        }
    }
}
