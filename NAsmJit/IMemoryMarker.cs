namespace NAsmJit
{
    using System;

    /// <summary>
    /// Virtual memory marker interface.
    /// </summary>
    public interface IMemoryMarker
    {
        void Mark(IntPtr ptr, IntPtr size);
    }
}
