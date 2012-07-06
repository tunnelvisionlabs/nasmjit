namespace NAsmJit.Contracts
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

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Mem operand0)
        {
            Contract.Requires(operand0 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Imm operand0)
        {
            Contract.Requires(operand0 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Label operand0)
        {
            Contract.Requires(operand0 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Mem operand0, Imm operand1)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, Mem operand0, SegmentReg operand1)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);

            throw new NotImplementedException();
        }

        void IIntrinsicSupport.EmitInstruction(InstructionCode code, SegmentReg operand0, Mem operand1)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);

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
