namespace AsmJitNet.Contracts
{
    using System;
    using System.Diagnostics.Contracts;

    [ContractClassFor(typeof(IIntrinsicSupport))]
    public abstract class IIntrinsicSupportContracts : IIntrinsicSupport
    {
        private IIntrinsicSupportContracts()
        {
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code)
        {
            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Operand operand0)
        {
            Contract.Requires(operand0 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Operand operand0, Operand operand1)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);
            Contract.Requires(operand2 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitJcc(InstructionCode code, Label label, Hint hint)
        {
            Contract.Requires(label != null);
            Contract.Requires(hint == Hint.None || hint == Hint.NotTaken || hint == Hint.Taken);

            throw new NotImplementedException();
        }
    }
}
