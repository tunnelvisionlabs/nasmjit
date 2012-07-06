namespace NAsmJit
{
    public interface IMmIntrinsicSupport<TMM> : IIntrinsicSupport
        where TMM : IMmOperand
    {
        void EmitInstruction(InstructionCode code, TMM operand0);

        void EmitInstruction(InstructionCode code, TMM operand0, Imm operand1);

        void EmitInstruction(InstructionCode code, TMM operand0, Mem operand1);

        void EmitInstruction(InstructionCode code, Mem operand0, TMM operand1);

        void EmitInstruction(InstructionCode code, TMM operand0, TMM operand1);

        void EmitInstruction(InstructionCode code, TMM operand0, Mem operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, TMM operand0, TMM operand1, Imm operand2);
    }
}
