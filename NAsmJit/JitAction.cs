namespace NAsmJit
{
    using System.Collections.ObjectModel;

    public class JitAction : JitMethodBase
    {
        private static readonly ReadOnlyCollection<VariableType> _argumentTypes = new ReadOnlyCollection<VariableType>(new VariableType[0]);
        private static readonly VariableType _returnType = Compiler.TypeToId(typeof(void));

        public JitAction(
            CallingConvention callingConvention = CallingConvention.Default,
            FunctionHints hints = FunctionHints.None,
            CodeGenerator codeGenerator = null)
            : base(callingConvention, hints, codeGenerator)
        {
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
