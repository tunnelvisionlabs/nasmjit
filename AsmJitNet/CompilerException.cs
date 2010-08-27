namespace AsmJitNet
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class CompilerException : JitException
    {
        public CompilerException()
        {
        }

        public CompilerException(string message)
            : base(message)
        {
        }

        public CompilerException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        protected CompilerException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
