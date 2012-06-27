namespace NAsmJit
{
    public static class HintByteValue
    {
        /// <summary>
        /// Condition is likely to be taken
        /// </summary>
        public const byte Taken = 0x3E;

        /// <summary>
        /// Condition is unlikely to be taken
        /// </summary>
        public const byte NotTaken = 0x2E;
    }
}
