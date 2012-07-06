namespace NAsmJit
{
    public interface IX87IntrinsicSupport<TX87> : IIntrinsicSupport
        where TX87 : IX87Operand
    {
        void EmitInstruction(InstructionCode code, TX87 operand0);

        void EmitInstruction(InstructionCode code, TX87 operand0, Mem operand1);

        void EmitInstruction(InstructionCode code, Mem operand0, TX87 operand1);

        void EmitInstruction(InstructionCode code, TX87 operand0, TX87 operand1);
    }
}
