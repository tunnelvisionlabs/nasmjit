namespace AsmJitNet2
{
    using System;
    using System.Diagnostics;
    using System.Linq;
    using Marshal = System.Runtime.InteropServices.Marshal;

    public sealed class CpuInfo
    {
        private static readonly object _lock = new object();
        private static readonly Lazy<CpuInfo> _instance = new Lazy<CpuInfo>(() => new CpuInfo());

        private string _vendor;

        private CpuVendor _vendorId;
        private int _family;
        private int _model;
        private int _stepping;
        private int _numberOfProcessors;
        private CpuFeatures _features;
        private CpuBugs _bugs;
#if ASMJIT_X86 || ASMJIT_X64
        private X86ExtendedInfo _x86ExtendedInfo;
#endif

        private CpuInfo()
        {
            DetectCpuInfo();
        }

        [DebuggerBrowsable(DebuggerBrowsableState.Never)]
        public static CpuInfo Instance
        {
            get
            {
                return _instance.Value;
            }
        }

        public string Vendor
        {
            get
            {
                return _vendor;
            }
        }

        public CpuVendor VendorId
        {
            get
            {
                return _vendorId;
            }
        }

        public int Family
        {
            get
            {
                return _family;
            }
        }

        public int Model
        {
            get
            {
                return _model;
            }
        }

        public int Stepping
        {
            get
            {
                return _stepping;
            }
        }

        public int NumberOfProcessors
        {
            get
            {
                return _numberOfProcessors;
            }
        }

        public CpuFeatures Features
        {
            get
            {
                return _features;
            }
        }

        public CpuBugs Bugs
        {
            get
            {
                return _bugs;
            }
        }

#if ASMJIT_X86 || ASMJIT_X64
        public X86ExtendedInfo ExtendedInfo
        {
            get
            {
                return _x86ExtendedInfo;
            }
        }

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void CpuIdMethod(ref uint eax, out uint ebx, out uint ecx, out uint edx);

        private static Lazy<CpuIdMethod> _cpuId = new Lazy<CpuIdMethod>(BuildCpuIdFunction);

        [DebuggerBrowsable(DebuggerBrowsableState.Never)]
        private static CpuIdMethod CpuId
        {
            get
            {
                return _cpuId.Value;
            }
        }
#endif

        private static readonly VendorInfo[] _vendorInfo =
            {
                new VendorInfo( CpuVendor.Intel, new char[] { 'G', 'e', 'n', 'u', 'i', 'n', 'e', 'I', 'n', 't', 'e', 'l' } ),
                new VendorInfo( CpuVendor.Amd  , new char[] { 'A', 'M', 'D', 'i', 's', 'b', 'e', 't', 't', 'e', 'r', '!' } ),
                new VendorInfo( CpuVendor.Amd  , new char[] { 'A', 'u', 't', 'h', 'e', 'n', 't', 'i', 'c', 'A', 'M', 'D' } ),
                new VendorInfo( CpuVendor.Via  , new char[] { 'V', 'I', 'A', '\0','V', 'I', 'A', '\0','V', 'I', 'A', '\0'} ),
            };

        private void DetectCpuInfo()
        {
            _vendor = "Unknown";
            _numberOfProcessors = Environment.ProcessorCount;

            byte[] vendor = new byte[12];

#if ASMJIT_X86 || ASMJIT_X64
            uint a;

            uint eax;
            uint ebx;
            uint ecx;
            uint edx;

            // Get vendor string
            eax = 0;
            CpuIdMethod cpuId = CpuId;
            cpuId(ref eax, out ebx, out ecx, out edx);

            BitConverter.GetBytes(ebx).CopyTo(vendor, 0);
            BitConverter.GetBytes(edx).CopyTo(vendor, 4);
            BitConverter.GetBytes(ecx).CopyTo(vendor, 8);

            for (a = 0; a < 3; a++)
            {
                //if (memcmp(vendor, _vendorInfo[a].text, 12) == 0)
                if (vendor.SequenceEqual(_vendorInfo[a].Text.Select(i => (byte)i)))
                {
                    _vendorId = _vendorInfo[a].Vendor;
                    _vendor = new string(_vendorInfo[a].Text.ToArray());
                    break;
                }
            }

            // get feature flags in ecx/edx, and family/model in eax
            eax = 1;
            CpuId(ref eax, out ebx, out ecx, out edx);

            // family and model fields
            _family = (int)(eax >> 8) & 0x0F;
            _model = (int)(eax >> 4) & 0x0F;
            _stepping = (int)(eax) & 0x0F;

            // use extended family and model fields
            if (_family == 0x0F)
            {
                _family += (int)((eax >> 20) & 0xFF);
                _model += (int)(((eax >> 16) & 0x0F) << 4);
            }

            _x86ExtendedInfo.ProcessorType = (int)((eax >> 12) & 0x03);
            _x86ExtendedInfo.BrandIndex = (int)((ebx) & 0xFF);
            _x86ExtendedInfo.FlushCacheLineSize = (int)((ebx >> 8) & 0xFF) * 8;
            _x86ExtendedInfo.MaxLogicalProcessors = (int)((ebx >> 16) & 0xFF);
            _x86ExtendedInfo.ApicPhysicalId = (int)((ebx >> 24) & 0xFF);

            if ((ecx & 0x00000001U) != 0)
                _features |= CpuFeatures.SSE3;
            if ((ecx & 0x00000008U) != 0)
                _features |= CpuFeatures.MONITOR_MWAIT;
            if ((ecx & 0x00000200U) != 0)
                _features |= CpuFeatures.SSSE3;
            if ((ecx & 0x00002000U) != 0)
                _features |= CpuFeatures.CMPXCHG16B;
            if ((ecx & 0x00080000U) != 0)
                _features |= CpuFeatures.SSE4_1;
            if ((ecx & 0x00100000U) != 0)
                _features |= CpuFeatures.SSE4_2;
            if ((ecx & 0x00400000U) != 0)
                _features |= CpuFeatures.MOVBE;
            if ((ecx & 0x00800000U) != 0)
                _features |= CpuFeatures.POPCNT;

            if ((edx & 0x00000010U) != 0)
                _features |= CpuFeatures.RDTSC;
            if ((edx & 0x00000100U) != 0)
                _features |= CpuFeatures.CMPXCHG8B;
            if ((edx & 0x00008000U) != 0)
                _features |= CpuFeatures.CMOV;
            if ((edx & 0x00800000U) != 0)
                _features |= CpuFeatures.MMX;
            if ((edx & 0x01000000U) != 0)
                _features |= CpuFeatures.FXSR;
            if ((edx & 0x02000000U) != 0)
                _features |= CpuFeatures.SSE | CpuFeatures.MMX_EXT;
            if ((edx & 0x04000000U) != 0)
                _features |= CpuFeatures.SSE | CpuFeatures.SSE2;
            if ((edx & 0x10000000U) != 0)
                _features |= CpuFeatures.MULTI_THREADING;

            if (_vendorId == CpuVendor.Amd && (edx & 0x10000000U) != 0)
            {
                // AMD sets Multithreading to ON if it has more cores.
                if (_numberOfProcessors == 1)
                    _numberOfProcessors = 2;
            }

            // This comment comes from V8 and I think that its important:
            //
            // Opteron Rev E has a bug in which on very rare occasions a locked
            // instruction doesn't act as a read-acquire barrier if followed by a
            // non-locked read-modify-write instruction.  Rev F has this bug in 
            // pre-release versions, but not in versions released to customers,
            // so we test only for Rev E, which is family 15, model 32..63 inclusive.

            if (_vendorId == CpuVendor.Amd && _family == 15 && _model >= 32 && _model <= 63)
            {
                _bugs |= CpuBugs.AmdLockMb;
            }

            // Calling cpuid with 0x80000000 as the in argument
            // gets the number of valid extended IDs.

            eax = 0x80000000;
            CpuId(ref eax, out ebx, out ecx, out edx);

            uint exIds = eax;

            for (a = 0x80000001; a < exIds && a <= (0x80000001); a++)
            {
                eax = a;
                CpuId(ref eax, out ebx, out ecx, out edx);

                switch (a)
                {
                case 0x80000001:
                    if ((ecx & 0x00000001U) != 0)
                        _features |= CpuFeatures.LAHF_SAHF;
                    if ((ecx & 0x00000020U) != 0)
                        _features |= CpuFeatures.LZCNT;
                    if ((ecx & 0x00000040U) != 0)
                        _features |= CpuFeatures.SSE4_A;
                    if ((ecx & 0x00000080U) != 0)
                        _features |= CpuFeatures.MSSE;
                    if ((ecx & 0x00000100U) != 0)
                        _features |= CpuFeatures.PREFETCH;
                    if ((ecx & 0x00000800U) != 0)
                        _features |= CpuFeatures.SSE5;

                    if ((edx & 0x00100000U) != 0)
                        _features |= CpuFeatures.EXECUTE_DISABLE_BIT;
                    if ((edx & 0x00200000U) != 0)
                        _features |= CpuFeatures.FFXSR;
                    if ((edx & 0x00400000U) != 0)
                        _features |= CpuFeatures.MMX_EXT;
                    if ((edx & 0x08000000U) != 0)
                        _features |= CpuFeatures.RDTSCP;
                    if ((edx & 0x20000000U) != 0)
                        _features |= CpuFeatures.X64_BIT;
                    if ((edx & 0x40000000U) != 0)
                        _features |= CpuFeatures.AMD3DNOW_EXT | CpuFeatures.MMX_EXT;
                    if ((edx & 0x80000000U) != 0)
                        _features |= CpuFeatures.AMD3DNOW;

                    break;

                // Additional features can be detected in the future.
                }
            }
#endif // ASMJIT_X86 || ASMJIT_X64
        }

#if ASMJIT_X86 || ASMJIT_X64
        private static CpuIdMethod BuildCpuIdFunction()
        {
            Compiler compiler = new Compiler();

#if ASMJIT_X86
            compiler.NewFunction(CallingConvention.Cdecl, typeof(Action<IntPtr, IntPtr, IntPtr, IntPtr>));
#elif ASMJIT_X64
            compiler.NewFunction(CallingConvention.X64W, typeof(Action<IntPtr, IntPtr, IntPtr, IntPtr>));
#endif
            compiler.Function.SetHint(FunctionHints.Naked, true);

            GPVar eaxPtr = compiler.ArgGP(0);
            GPVar ebxPtr = compiler.ArgGP(1);
            GPVar ecxPtr = compiler.ArgGP(2);
            GPVar edxPtr = compiler.ArgGP(3);

            GPVar eax = compiler.NewGP();
            GPVar ebx = compiler.NewGP();
            GPVar ecx = compiler.NewGP();
            GPVar edx = compiler.NewGP();

            compiler.Mov(eax, Mem.dword_ptr(eaxPtr));
            compiler.Cpuid(eax, ebx, ecx, edx);
            compiler.Mov(Mem.dword_ptr(eaxPtr), eax);
            compiler.Mov(Mem.dword_ptr(ebxPtr), ebx);
            compiler.Mov(Mem.dword_ptr(ecxPtr), ecx);
            compiler.Mov(Mem.dword_ptr(edxPtr), edx);

            compiler.Ret();
            compiler.EndFunction();
            CpuIdMethod cpuId = (CpuIdMethod)Marshal.GetDelegateForFunctionPointer(compiler.Make(), typeof(CpuIdMethod));

            return cpuId;
        }

        public struct X86ExtendedInfo
        {
            public int ProcessorType;
            public int BrandIndex;
            public int FlushCacheLineSize;
            public int MaxLogicalProcessors;
            public int ApicPhysicalId;
        }
#endif
    }
}
