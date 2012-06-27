namespace AsmJitNet
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class AssemblerException : JitException
    {
        public AssemblerException()
        {
        }

        public AssemblerException(string message)
            : base(message)
        {
        }

        public AssemblerException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        protected AssemblerException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
