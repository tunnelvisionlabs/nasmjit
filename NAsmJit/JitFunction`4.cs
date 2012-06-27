namespace NAsmJit
{
    using System;
    using System.Collections.ObjectModel;

    public class JitFunction<T1, T2, T3, TResult> : JitMethodBase
    {
        private static readonly ReadOnlyCollection<VariableType> _argumentTypes =
            new ReadOnlyCollection<VariableType>(
                new VariableType[]
                {
                    Compiler.TypeToId(typeof(T1)),
                    Compiler.TypeToId(typeof(T2)),
                    Compiler.TypeToId(typeof(T3)),
                });

        private static readonly VariableType _returnType = Compiler.TypeToId(typeof(TResult));

        private static readonly bool _hasInvalidType =
            (_argumentTypes.Contains(VariableType.Invalid))
            || (_returnType == VariableType.Invalid);

        public JitFunction(
            CallingConvention callingConvention = CallingConvention.Default,
            FunctionHints hints = FunctionHints.None,
            CodeGenerator codeGenerator = null)
            : base(callingConvention, hints, codeGenerator)
        {
            if (_hasInvalidType)
            {
                if (_argumentTypes.Contains(VariableType.Invalid))
                    throw new ArgumentException("The specified argument type is not supported by the JIT compiler.");
                if (_returnType == VariableType.Invalid)
                    throw new ArgumentException("The specified return type is not supported by the JIT compiler.");
            }
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
