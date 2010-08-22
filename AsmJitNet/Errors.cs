namespace AsmJitNet
{
    internal static class Errors
    {
        public static readonly int None = 0;
        public static readonly int NoHeapMemory = 1;
        public static readonly int NoVirtualMemory = 2;
        public static readonly int UnknownInstruction = 3;
        public static readonly int IllegalInstruction = 4;
        public static readonly int IllegalAddressing = 5;
        public static readonly int IllegalShortJump = 6;
        public static readonly int NoFunction = 7;
        public static readonly int NotEnoughRegisters = 8;
        public static readonly int RegistersOverlap = 9;
        public static readonly int IncompatibleArgument = 10;
        public static readonly int IncompatibleReturnValue = 11;


        private static readonly string[] _errorMessages =
            {
                "No error",
                "No heap memory",
                "No virtual memory",
                "Unknown instruction",
                "Illegal instruction",
                "Illegal addressing",
                "Illegal short jump",
                "No function defined",
                "Not enough registers",
                "Registers overlap",
                "Incompatible argument",
                "Incompatible return value",

                "Unknown error"
            };

        public static string GetErrorCodeAsString(int error)
        {
            if (error < 0 || error >= _errorMessages.Length)
                error = _errorMessages.Length - 1;

            return _errorMessages[error];
        }
    }
}
