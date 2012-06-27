namespace AsmJitNet
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class JitException : Exception
    {
        public JitException()
        {
        }

        public JitException(string message)
            : base(message)
        {
        }

        public JitException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        protected JitException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
