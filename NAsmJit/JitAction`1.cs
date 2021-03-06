﻿namespace NAsmJit
{
    using System;
    using System.Collections.ObjectModel;

    public class JitAction<T> : JitMethodBase
    {
        private static readonly ReadOnlyCollection<VariableType> _argumentTypes =
            new ReadOnlyCollection<VariableType>(
                new VariableType[]
                {
                    Compiler.TypeToId(typeof(T)),
                });

        private static readonly VariableType _returnType = Compiler.TypeToId(typeof(void));

        private static readonly bool _hasInvalidType =
            (_argumentTypes.Contains(VariableType.Invalid))
            || (_returnType == VariableType.Invalid);

        public JitAction(
            CallingConvention callingConvention = CallingConvention.Default,
            FunctionHints hints = FunctionHints.None,
            CodeGenerator codeGenerator = null)
            : base(callingConvention, hints, codeGenerator)
        {
            if (_hasInvalidType)
            {
                if (_argumentTypes.Contains(VariableType.Invalid))
                    throw new ArgumentException("The specified argument type is not supported by the JIT compiler.");
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
