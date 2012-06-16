namespace AsmJitNet
{
    public enum PrefetchHint
    {
        /// <summary>
        /// Non-temporal data with respect to all cache levels
        /// </summary>
        NTA = 0,

        /// <summary>
        /// Temporal data
        /// </summary>
        T0 = 1,

        /// <summary>
        /// Temporal data with respect to the first level cache
        /// </summary>
        T1 = 2,

        /// <summary>
        /// Temporal data with respect to the second level cache
        /// </summary>
        T2 = 3
    }
}
