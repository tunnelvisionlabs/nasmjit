namespace AsmJitNet2
{
    using System.Collections.ObjectModel;

    public class VendorInfo
    {
        private readonly CpuVendor _vendorId;
        private readonly char[] _text;

        public VendorInfo(CpuVendor vendorId, char[] text)
        {
            _vendorId = vendorId;
            _text = text;
        }

        public CpuVendor Vendor
        {
            get
            {
                return _vendorId;
            }
        }

        public ReadOnlyCollection<char> Text
        {
            get
            {
                return new ReadOnlyCollection<char>(_text);
            }
        }
    }
}
