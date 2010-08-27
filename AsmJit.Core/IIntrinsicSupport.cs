namespace AsmJitNet
{
    public interface IIntrinsicSupport
    {
        void EmitInstruction(InstructionCode code);

        void EmitInstruction(InstructionCode code, Operand operand0);

        void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1);

        void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2);

        void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3);

        void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3, Operand operand4);

        void EmitJcc(InstructionCode code, Label label, Hint hint);
    }
}
