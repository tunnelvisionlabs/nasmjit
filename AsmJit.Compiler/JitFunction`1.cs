namespace AsmJitNet
{
    using System;
    using System.Collections.ObjectModel;

    public class JitFunction<TResult> : JitMethodBase
    {
        private static readonly ReadOnlyCollection<VariableType> _argumentTypes = new ReadOnlyCollection<VariableType>(new VariableType[0]);
        private static readonly VariableType _returnType = Compiler.TypeToId(typeof(TResult));

        public JitFunction(
            CallingConvention callingConvention = CallingConvention.Default,
            FunctionHints hints = FunctionHints.None,
            CodeGenerator codeGenerator = null)
            : base(callingConvention, hints, codeGenerator)
        {
            if (_returnType == VariableType.Invalid)
                throw new ArgumentException("The specified return type is not supported by the JIT compiler.");
        }

        public override ReadOnlyCollection<VariableType> ArgumentTypes
        {
            get
            {
                return _argumentTypes;
            }
        }

        public override VariableType ReturnType
        {
            get
            {
                return _returnType;
            }
        }
    }
}
