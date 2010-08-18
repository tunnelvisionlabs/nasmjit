namespace AsmJitNet2
{
    using System;
    using System.Diagnostics;

    internal class CpuInfo
    {
        private static readonly Lazy<CpuInfo> _instance = new Lazy<CpuInfo>(() => new CpuInfo());

        private string _vendor;

        private CpuVendor _vendorId;
        //private int _family;
        //private int _model;
        //private int _stepping;
        private int _numberOfProcessors;
        //private int _features;
        //private int _bugs;

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

        public CpuVendor VendorId
        {
            get
            {
                return _vendorId;
            }
        }

        protected virtual void DetectCpuInfo()
        {
            _vendor = "Unknown";
            _numberOfProcessors = DetectNumberOfProcessors();
            _vendorId = CpuVendor.Unknown;
        }

        private static int DetectNumberOfProcessors()
        {
            return Environment.ProcessorCount;
        }
    }
}
