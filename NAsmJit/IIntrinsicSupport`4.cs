namespace NAsmJit
{
    public interface IIntrinsicSupport<TGP, TX87, TMM, TXMM> : IX86IntrinsicSupport<TGP>, IX87IntrinsicSupport<TX87>, IMmIntrinsicSupport<TMM>, IXmmIntrinsicSupport<TXMM>, IIntrinsicSupport
        where TGP : IGpOperand
        where TX87 : IX87Operand
        where TMM : IMmOperand
        where TXMM : IXmmOperand
    {
        void EmitInstruction(InstructionCode code, TGP operand0, TMM operand1);

        void EmitInstruction(InstructionCode code, TMM operand0, TGP operand1);

        void EmitInstruction(InstructionCode code, TGP operand0, TXMM operand1);

        void EmitInstruction(InstructionCode code, TXMM operand0, TGP operand1);

        void EmitInstruction(InstructionCode code, TMM operand0, TXMM operand1);

        void EmitInstruction(InstructionCode code, TXMM operand0, TMM operand1);

        void EmitInstruction(InstructionCode code, TGP operand0, TMM operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, TGP operand0, TMM operand1, TMM operand2);

        void EmitInstruction(InstructionCode code, TGP operand0, TXMM operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, TGP operand0, TXMM operand1, TXMM operand2);

        void EmitInstruction(InstructionCode code, TMM operand0, TGP operand1, Imm operand2);

        void EmitInstruction(InstructionCode code, TXMM operand0, TGP operand1, Imm operand2);
    }
}
