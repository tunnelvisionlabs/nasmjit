namespace NAsmJit
{
    using System.Diagnostics.Contracts;

    [ContractClass(typeof(Contracts.IIntrinsicSupportContracts))]
    public interface IIntrinsicSupport
    {
        void EmitInstruction(InstructionCode code);

        void EmitInstruction(InstructionCode code, Mem operand0);

        void EmitInstruction(InstructionCode code, Imm operand0);

        void EmitInstruction(InstructionCode code, Label operand0);

        void EmitInstruction(InstructionCode code, Mem operand0, Imm operand1);

        void EmitInstruction(InstructionCode code, Mem operand0, SegmentReg operand1);

        void EmitInstruction(InstructionCode code, SegmentReg operand0, Mem operand1);

        void EmitJcc(InstructionCode code, Label label, Hint hint);
    }
}
