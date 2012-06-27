namespace NAsmJit
{
    using System.Collections.ObjectModel;
    using System.Diagnostics.Contracts;

    public class VendorInfo
    {
        private readonly CpuVendor _vendorId;
        private readonly char[] _text;

        public VendorInfo(CpuVendor vendorId, char[] text)
        {
            Contract.Requires(text != null);

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
                Contract.Ensures(Contract.Result<ReadOnlyCollection<char>>() != null);

                return new ReadOnlyCollection<char>(_text);
            }
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_text != null);
        }
    }
}
