namespace NAsmJit
{
    public interface IXmmIntrinsicSupport<TXMM> : IIntrinsicSupport
        where TXMM : IXmmOperand
    {
        void EmitInstruction(InstructionCode code, TXMM operand0);

        void EmitInstruction(InstructionCode code, TXMM operand0, Imm operand1);

        void EmitInstruction(InstructionCode code, TXMM operand0, Mem operand1);

        void EmitInstruction(InstructionCode code, Mem operand0, TXMM operand1);

        void EmitInstruction(InstructionCode code, TXMM operand0, TXMM operand1);

        void EmitInstruction(InstructionCode code, TXMM operand0, Mem operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, Mem operand0, TXMM operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, TXMM operand0, TXMM operand1, Imm operand2);
    }
}
