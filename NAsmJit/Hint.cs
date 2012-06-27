namespace NAsmJit
{
    public enum Hint
    {
        /// <summary>
        /// No hint
        /// </summary>
        None = 0x00,

        /// <summary>
        /// Condition is likely to be taken
        /// </summary>
        Taken = 0x01,

        /// <summary>
        /// Condition is unlikely to be taken
        /// </summary>
        NotTaken = 0x02,
    }
}
