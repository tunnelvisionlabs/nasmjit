namespace AsmJitNet.Contracts
{
    using System;
    using System.Collections.ObjectModel;
    using System.Diagnostics.Contracts;

    [ContractClassFor(typeof(JitMethodBase))]
    public abstract class JitMethodBaseContracts : JitMethodBase
    {
        private JitMethodBaseContracts(CallingConvention callingConvention, FunctionHints hints, CodeGenerator codeGenerator)
            : base(callingConvention, hints, codeGenerator)
        {
        }

        public override ReadOnlyCollection<VariableType> ArgumentTypes
        {
            get
            {
                Contract.Ensures(Contract.Result<ReadOnlyCollection<VariableType>>() != null);

                throw new NotImplementedException();
            }
        }

        public override VariableType ReturnType
        {
            get
            {
                throw new NotImplementedException();
            }
        }
    }
}
