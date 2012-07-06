namespace NAsmJit
{
    public interface IX86IntrinsicSupport<TGP> : IIntrinsicSupport
        where TGP : IGpOperand
    {
        void EmitInstruction(InstructionCode code, TGP operand0);

        void EmitInstruction(InstructionCode code, TGP operand0, Imm operand1);

        void EmitInstruction(InstructionCode code, TGP operand0, SegmentReg operand1);

        void EmitInstruction(InstructionCode code, TGP operand0, Mem operand1);

        void EmitInstruction(InstructionCode code, Mem operand0, TGP operand1);

        void EmitInstruction(InstructionCode code, SegmentReg operand0, TGP operand1);

        void EmitInstruction(InstructionCode code, TGP operand0, TGP operand1);

        void EmitInstruction(InstructionCode code, TGP operand0, Mem operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, Mem operand0, TGP operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, TGP operand0, TGP operand1, Imm operand2);
    }
}
